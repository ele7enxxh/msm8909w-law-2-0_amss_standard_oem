#ifndef RLCFC_H
#define RLCFC_H
/*===========================================================================

          U P L I N K   R L C : S U B T A S K   H E A D E R   F I L E


GENERAL DESCRIPTION

  This module contains the declaration of the functions and data types
  defined in rlcfc.c.

Copyright (c) 1992-2005 by Qualcomm Technologies, Inc.  All Rights Reserved.
Copyright (c) 2007-2009 by Qualcomm Technologies, Inc.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/wcdma/rlc/inc/rlcfc.h#1 $ $DateTime: 2016/12/13 08:00:12 $ $Author: mplcsds1 $


when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
01/08/15   aa      Changes for disabling ULC when FC is active
07/01/14   aa      CFCM changes for RLC
01/04/13   kc      Function modularization related changes
02/01/12   grk     Feature cleanup
01/23/12   as      Support for Free Floating Task changes
09/22/11   grk     Support for thermal mitigation through NPA inteface
07/08/10   av      Fixing compilation error 
05/06/10   pj      Added function prototypes 
03/30/10   grk     Changes for RLC QTF.
10/23/09   av      Temperature FC   
10/07/09   av      Multi-client flow control algorithm 
05/11/09   ssg     Updated Copyright Information
05/04/09   kvk     Added support to send the FC command for WINDOW size update 
                   rather than directly updating the WINDOW size from timer CB function
04/29/09   av      Fixed compiler warnings in L2 
04/25/08   ar      Added support for decoupling UL and DL cpu threshold 
                   values
03/31/08   ar      Added declaration for rlc_fc_register_f
03/25/08   ar      Fixed lint warnings
03/17/08   ar      Added appropriate featurization:FEATURE_DL_CPU_BASED_FC
                   or FEATURE_UL_CPU_BASED_FC for flow control functions, 
                   required for separating UL and DL fc functionality.
03/14/07   ttl     Provide RLC FC registration/de-registration functions.
01/02/00   dvp     Created file.

===========================================================================*/
#include "wcdma_variation.h"
#include "uecomdef.h"
#include "rlcul.h"
#include "rlci.h"
#include "cfcm.h"

typedef enum {
  RLC_FC_UPLINK = 0x00,
  RLC_FC_DOWNLINK = 0x01
}rlc_fc_direction_e_type;

typedef enum {
  RLC_FC_TASK_CMD = 0,
  RLC_FC_TIMER_CMD
}rlc_fc_src_cmd_e_type;

typedef enum {
  WS_NO_OP = 0,
  WS_STEP_DOWN,
  WS_STEP_UP,
  WS_FREEZE,
  WS_MAX_OP
} rlci_fc_ws_step_dir_e_type;

typedef struct {
  /* Timer for which RLC would act on FC messages perodically. */
  uint16  rlc_fc_status_check_duration;

  /* Intial window size, at the start of flow control. */
  uint16  rlc_fc_init_ws;

  /* Number of steps taken to reach window size. */
  uint16  rlc_fc_ws_stp;

  /* Window size is increased in steps of (WS/RLC_FC_INC_WS_STP).
   This variable holds the current WS step applied on all RLC entity. */
  uint16  rlc_fc_ws_inc;

  /* Either to increase or decrease Peer WS  */
  rlci_fc_ws_step_dir_e_type rlc_fc_ws_step_dir;

  /* Window size stepping timer */
  rex_timer_type  *rlc_fc_step_timer;

  /* TRUE indicates the FC is active in RLC. */
  boolean        rlc_fc_status;

  /* Register FC flag */
  boolean rlc_fc_register_f;

} rlc_fc_type;

#ifdef FEATURE_WCDMA_FREE_FLOATING_TASK
#define RLC_ACQ_FC_LOCK()          REX_ISR_LOCK(&rlc_fc_sem)
#define RLC_ACQ_FC_UNLOCK()        REX_ISR_UNLOCK(&rlc_fc_sem)
#else
#define RLC_ACQ_FC_LOCK()
#define RLC_ACQ_FC_UNLOCK()
#endif

#define RX_WINDOW_MIN_SIZE 1
#define RX_WINDOW_SHUTDOWN_SIZE 0

typedef struct
{
  q_link_type link;
  cfcm_cmd_e cmd;
  uint32 step_timer;
  uint32 monitors_active;
}rlc_cfcm_cmd_out_type;

#define RLC_GET_FC_ARB_PTR(dir, ctrl_blk) (dir) ? &(ctrl_blk->ctrl.am_ctrl.multiclient_dl_fc_arbitarator) : &(ctrl_blk->ctrl.am_ctrl.multiclient_ul_fc_arbitarator)
#define RLC_GET_CFCM_ENTITY(dir) (dir) ? RLC_DL_FC_ENTITY_CFCM : RLC_UL_FC_ENTITY_CFCM

/*===========================================================================

FUNCTION RLC_FC_CMD_HDLR

DESCRIPTION
  Command handler for commands from FC Module.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void rlc_fc_cmd_hdlr
(
  rlc_fc_direction_e_type fc_dir
);

/*===========================================================================

FUNCTION RLC_FC_TIMER_CMD_HDLR

DESCRIPTION
  Command handler for commands from CFCM timer.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void rlc_fc_timer_cmd_hdlr
(
  rlc_fc_direction_e_type fc_dir
);


/*===========================================================================

FUNCTION RLCI_FC_NEW_WIN_SIZE

DESCRIPTION
  The function calculates the window size under the current cpu load.

DEPENDENCIES
  None.

RETURN VALUE
  Determined window size under current CPU load.

SIDE EFFECTS
  None.

===========================================================================*/
uint32 rlci_fc_new_win_size
(
  uint16 conf_win_size,
  rlc_fc_direction_e_type fc_dir
);

/*===========================================================================

FUNCTION RLCI_FC_CTL_BLK_RESET_REPORT

DESCRIPTION
  One of the User plane AM hits RESET. Should have no need for flow control
  after RESET. Resume every one in the User Plane to their full config'd
  window size.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void rlci_fc_ctl_blk_reset_report
(
  rlc_lc_id_type lc_id,
  rlc_fc_direction_e_type fc_dir
);

/*===========================================================================

FUNCTION RLC_DL_FC_INIT

DESCRIPTION
  Initiliazes DL flow control states in rlc.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void rlc_dl_fc_init(void);

/*===========================================================================

FUNCTION RLC_GET_FC_PTR

DESCRIPTION
  Returns pointer to RLC FC global(UL/DL)

DEPENDENCIES
  None.

RETURN VALUE
  Pointer to FC global.

SIDE EFFECTS
  None.

===========================================================================*/
rlc_fc_type *rlc_get_fc_ptr(rlc_fc_direction_e_type direction);

/*===========================================================================

FUNCTION RLC_FC_RESET

DESCRIPTION
  Clears UL/DL flow control states in rlc.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void rlc_fc_reset(rlc_fc_type *rlc_fc_ptr, rlc_fc_direction_e_type direction);

/*===========================================================================

FUNCTION RLC_DL_FC_TX_NEW_WIN_SUFI

DESCRIPTION
  New window size is calculated based on last FC command and tx. new window
  SUFI to that effect.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void rlci_dl_fc_tx_new_win_sufi
(
  rlci_ul_ctrl_blk_type *ul_ctrl_blk_ptr /* Logical channel id that has to be flow controled.*/
);

/*===========================================================================

FUNCTION RLCI_DL_FC_STATUS

DESCRIPTION
  Returns RLC-DL flow control status.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
boolean rlci_dl_fc_status(void);

/*===========================================================================

FUNCTION RLC_DL_SET_FC_CONFIG

DESCRIPTION
  Downlink configuraton set by QxDM.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void rlc_dl_set_fc_config
(
  uint16 ws_step_period,
  uint16 ws_step_stage,
  uint16 init_ws
);

/*===========================================================================

FUNCTION RLC_UL_FC_INIT

DESCRIPTION
  Initiliazes UL flow control states in rlc.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void rlc_ul_fc_init(void);

/*===========================================================================

FUNCTION RLCI_UL_FC_CALC_NEW_TX_WIN_SIZE

DESCRIPTION
  This function computes the tx window size that needs to be applied.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void rlci_ul_fc_calc_new_tx_win_size
(
  rlci_ul_ctrl_blk_type *ul_ctrl_blk_ptr  /* Logical channel id that has to be flow controled.*/
);

/*===========================================================================

FUNCTION RLCI_UL_FC_STATUS

DESCRIPTION
  Returns RLC-UL flow control status.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
boolean rlci_ul_fc_status(void);

/*===========================================================================

FUNCTION RLC_UL_SET_FC_CONFIG

DESCRIPTION
  Uplink configuraton set by QxDM.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void rlc_ul_set_fc_config
(
  uint16 ws_step_period,
  uint16 ws_step_stage,
  uint16 init_ws
);

/*===========================================================================

FUNCTION MULTI_CLIENT_FC_INIT

DESCRIPTION
  Initialization of the multiclient fc 

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void multi_client_fc_init (
  multiclient_fc_arbitarator_type *fc_arb_ptr,
  uint16 num_flow_control_entity,
  uint16 configured_max_flow,
  output_flow_to_output_trigger_function_type output_fn,
  void *fc_obj
  );

/*===========================================================================

FUNCTION MULTI_CLIENT_FC_REGISTER_CLIENT

DESCRIPTION
  Registration of induvidual client into muliclient FC

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void multi_client_fc_register_client (
  multiclient_fc_arbitarator_type *fc_arb_ptr,
  uint16 fc_entity,
  uint16 init_state,
  state_to_entity_flow_function_type state_to_entity_flow_function);

/*===========================================================================

FUNCTION RLC_MULTI_CLIENT_FC_ARBITRATE

DESCRIPTION
  Multiclient flow control central arbitarator.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void multi_client_fc_arbitrate(
  multiclient_fc_arbitarator_type *fc_arb_ptr,
  uint16 fc_entity,
  uint16 new_observed_state,
  boolean lock_fc_semaphore);

/*===========================================================================

FUNCTION MULTI_CLIENT_FC_MODIFY_MAX_FLOW

DESCRIPTION
  configuration of max flow

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void multi_client_fc_modify_max_flow (
  multiclient_fc_arbitarator_type *fc_arb_ptr,
  uint16 new_max_flow,
  boolean output_action );
/*===========================================================================

FUNCTION RLC_DL_MULTI_CLIENT_FC_INIT

DESCRIPTION
  rlc fc initialization.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void rlc_dl_multi_client_fc_init(
  rlci_ul_am_ctrl_type *ctrl_blk);

/*===========================================================================

FUNCTION RLC_DL_MULTI_CLIENT_FC_OUTPUT_TRIGGER

DESCRIPTION
  The output action function for rlc multiclient fc

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void rlc_dl_multi_client_fc_output_trigger(
  uint16 output_flow,
  void *fc_obj);

/*===========================================================================

FUNCTION RLC_MULTI_CLIENT_FC_PS_WM_ENTITY_FLOW_FUNCTION

DESCRIPTION
  State to output flow mapping for PS WM

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
uint16 rlc_dl_multi_client_fc_ps_wm_entitiy_flow_function(
  uint16 state, 
  uint16 configured_max_flow);

/*===========================================================================

FUNCTION RLC_MULTI_CLIENT_FC_CFCM_ENTITY_FLOW_FUNCTION

DESCRIPTION
  State to output flow mapping function for CFCM

DEPENDENCIES
  None.

RETURN VALUE
  New Window Size to be set

SIDE EFFECTS
  None.

===========================================================================*/
uint16 rlc_multi_client_fc_cfcm_entity_flow_function(
  uint16 state, 
  uint16 configured_max_flow);

/*===========================================================================

FUNCTION RLC_DL_MULTI_CLIENT_FC_GET_WIN_SIZE

DESCRIPTION
  Mapping from output flow to window size.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
uint16 rlci_dl_multi_client_fc_get_win_size(
  rlci_ul_am_ctrl_type *ctrl_blk);

/*===========================================================================

FUNCTION RLC_DL_MULTI_CLIENT_FC_GET_WIN_SIZE

DESCRIPTION
  Mapping from output flow to window size.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
uint16 rlc_dl_multi_client_fc_get_win_size(
  uint16 rlc_id);

/*===========================================================================

FUNCTION RLC_MULTI_CLIENT_FC_MODIFY_MAX_FLOW

DESCRIPTION
  Mapping from output flow to window size.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void rlc_dl_multi_client_fc_modify_max_flow(
  uint16 rlc_id,
  uint16 new_max_flow,
  boolean output_action
  );

/*===========================================================================

FUNCTION RLC_UL_MULTI_CLIENT_FC_INIT

DESCRIPTION
  rlc fc initialization.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void rlc_ul_multi_client_fc_init(
  rlci_ul_am_ctrl_type *ctrl_blk);

/*===========================================================================

FUNCTION RLC_UL_MULTI_CLIENT_FC_OUTPUT_TRIGGER

DESCRIPTION
  The output action function for rlc multiclient fc

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void rlc_ul_multi_client_fc_output_trigger(
  uint16 output_flow,
  void *fc_obj);
/*===========================================================================

FUNCTION RLCI_UL_MULTI_CLIENT_FC_GET_WIN_SIZE

DESCRIPTION
  Mapping from output flow to window size.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

uint16 rlci_ul_multi_client_fc_get_win_size(
rlci_ul_am_ctrl_type *am_ctl_blk_ptr);

/*===========================================================================

FUNCTION RLC_UL_MULTI_CLIENT_FC_MODIFY_MAX_FLOW

DESCRIPTION
  rlc ul wrapper function to modify max flow.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void rlc_ul_multi_client_fc_modify_max_flow(
  uint16 rlc_id,
  uint16 new_max_flow,
  boolean output_action
  );

/*===========================================================================

FUNCTION RLC_CFCM_REGISTER

DESCRIPTION
  Register UL and DL with the CFCM module

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void rlc_cfcm_register(void);

/*===========================================================================

FUNCTION RLC_CFCM_DEREGISTER

DESCRIPTION
  Deregister UL and DL with the CFCM module

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void rlc_cfcm_deregister(void);

/*===========================================================================

FUNCTION RLC_UL_CFCM_CB

DESCRIPTION
  CFCM Callback Function for UL client

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void rlc_ul_cfcm_cb(cfcm_cmd_type_s *cfcm_cmd);

/*===========================================================================

FUNCTION RLC_DL_CFCM_CB

DESCRIPTION
  CFCM Callback Function for DL client

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void rlc_dl_cfcm_cb(cfcm_cmd_type_s *cfcm_cmd);

/*===========================================================================

FUNCTION RLC_CFCM_PROC_SHUT_DOWN_COMMAND

DESCRIPTION
  Processes SHUT DOWN command received from CFCM module

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void rlc_cfcm_proc_shut_down_command(rlc_fc_direction_e_type dir, uint32 monitors_active);
 
/*===========================================================================

FUNCTION RLC_PROC_CFCM_COMMAND

DESCRIPTION
  Sets the UL/DL window size to the value passed as win_size

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void rlc_proc_cfcm_command(rlc_fc_direction_e_type dir, uint8 win_size);

#endif  /* RLCFC_H */
