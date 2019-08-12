#ifndef _DS_RMNET_SM_EXT_H
#define _DS_RMNET_SM_EXT_H
/*===========================================================================

                      D S _ R M N E T _ S M _ E X T . H
DESCRIPTION

  The Data Services WWAN Rm State Machine header file.

  This state machine manages the Rm network interface and coordinates
  wireless WAN connectivity between the Rm and Um interfaces over all
  supported radio technologies.

EXTERNALIZED FUNCTIONS

  rmnet_sm_init()
    Allocate and initialize all data required for the WWAN Rm State Machine.

  rmnet_sm_process_cmd()
    Process a cmd posted to Rm State Machine.

Copyright (c) 2004-2011 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/
/*===========================================================================

                            EDIT HISTORY FOR FILE
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/rmifacectls/inc/ds_rmnet_sm_ext.h#1 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
09/27/13    wc     Support dynamic port configuration
03/09/11    kk     Removing DCC command buffer dependency from RmNet.
02/19/09    am     DS Task De-coupling effort and introduction of DCC task.
11/29/06    jd     Added DCTM support to RmNet
11/13/06    ks     Added rmnet_sm_config_net_params().
10/09/06    ks     Added rmnet_sm_process_cmd to process DS rmnet cmds.
08/23/06    ks     Changes to support multiple rmnet instances.
09/13/05    jd/ks  added rmnet_sm_process_dtr_changed_cmd().
05/31/05    jd     Code review updates
03/15/05    ks     corrected featurization
03/07/05    ks     Moved RMNET module internal definitions to ds_rmnet_smi.h
10/27/04    jd     Define AUTO_CALL_CONNECT to start call on cable connect.
                   Added rmnet_sm_interface() API to return ps_iface in use.
05/12/04    jd     Added call startup result notification callback
                   Added rm_dev_end_pkt_event handlign (same as rm_link_down)
                   Renamed  rmnet_sm_is_packet_call() as rmnet_sm_in_call()
04/01/04    jd     Added rmnet_sm_is_packet_call() prototype
02/06/04    jd     Initial implementation
===========================================================================*/

/*===========================================================================

                          INCLUDE FILES FOR MODULE

===========================================================================*/

#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"

#include "dcc_task_defs.h"
#include "ps_iface.h"


/*===========================================================================

                                DATA TYPES

===========================================================================*/
typedef enum
{
  RMNET_CALL_TYPE_LAPTOP   = 0x00,
  RMNET_CALL_TYPE_EMBEDDED = 0x01,
  RMNET_CALL_TYPE_MAX
} rmnet_meta_sm_call_e_type;

#define RMNET_SMI_GET_CMD_BUF(cmd_id) \
        rmnet_smi_get_cmd_buf(cmd_id,__FILENAME__,__LINE__,TRUE)

#define RMNET_SMI_GET_CMD_BUF_ASSERT(cmd_id) \
        rmnet_smi_get_cmd_buf(cmd_id,__FILENAME__,__LINE__,FALSE)


/*===========================================================================

                        PUBLIC FUNCTION DECLARATIONS

===========================================================================*/

/*===========================================================================
  FUNCTION RMNET_SM_CONFIG_NET_PARAMS()

  DESCRIPTION
    Configure the network params before bring up.
    Today used only to correctly configure auth credentials.

  PARAMETERS
    net_params - ptr to network params structure

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
extern void rmnet_sm_config_net_params
(
  network_params_info_type *  net_params,
  rmnet_meta_sm_call_e_type   call_type
);


/*===========================================================================
FUNCTION RMNET_SM_EXT_CALL_FAILURE()

DESCRIPTION
  Callback executed when call failure occurs during RmNet call setup

PARAMETERS
  None.

RETURN VALUE
  None.

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void rmnet_sm_ext_call_failure
(
  void
);


#ifdef FEATURE_DATA_RM_NET

/*===========================================================================
  FUNCTION RMNET_SM_INIT()

  DESCRIPTION
    This function initializes the Network Interface Rm state machine.
                                                                                        
  PARAMETERS
    None

  RETURN VALUE
    TRUE on success
    FALSE on failure

  DEPENDENCIES
    Should be called once at powerup

  SIDE EFFECTS
    None
===========================================================================*/
boolean rmnet_sm_init
(
  void
);


/*===========================================================================
  FUNCTION RMNET_SM_PROCESS_EV_CMD()

  DESCRIPTION
    This function processes a RmNet RmSM event cmd.

    It is called by the serving task main loop and will dispatch the 
    associated event handler function.

  PARAMETERS
    cmd          :  RmNet command to be processed
    user_data_ptr:  private data buffer containing the Rmnet command
                    information.

  RETURN VALUE
    None

  DEPENDENCIES
    RM State Machine should have already been initialized

  SIDE EFFECTS
    None
===========================================================================*/
void rmnet_sm_process_ev_cmd
(
  dcc_cmd_enum_type cmd,
  void              *user_data_ptr
);

/*===========================================================================
  FUNCTION RMNET_SM_PROCESS_CMD()

  DESCRIPTION
    This function processes a RmNet RmSM cmd.

  PARAMETERS
    cmd          :  RmNet command to be processed
    user_data_ptr:  private data buffer containing the Rmnet command
                    information.

  RETURN VALUE
    None

  DEPENDENCIES
    RM State Machine should have already been initialized

  SIDE EFFECTS
    None
===========================================================================*/
void rmnet_sm_process_cmd
(
  dcc_cmd_enum_type cmd,
  void              *user_data_ptr
);

#endif /* FEATURE_DATA_RM_NET */
#endif /* _DS_RMNET_SM_EXT_H */
