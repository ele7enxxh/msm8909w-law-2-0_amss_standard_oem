#ifndef DS_EPC_PDN_CTL_H
#define DS_EPC_PDN_CTL_H
/*===========================================================================

                      DS_EPC_PDN_CTL.H

DESCRIPTION


EXTERNALIZED FUNCTIONS

 Copyright (c) 2011-2015 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/
/*===========================================================================

                            EDIT HISTORY FOR FILE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp2/epchandoff/inc/ds_epc_pdn_ctl.h#1 $
  $Author: mplcsds1 $ 

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

when        who    what, where, why
--------    ---    ----------------------------------------------------------
12/07/11    vpk    Add 5 PDN support
09/01/11    fjia    Created Module
===========================================================================*/
#include "datamodem_variation.h"
#ifdef FEATURE_EPC_HANDOFF
#include "comdef.h"
#include "customer.h"
#include "ps_iface.h"
#include "ps_iface_ioctl.h"
#include "ps_acl.h"
#include "sys.h"
#include "rex.h"
#include "stm2.h"
#include "ds_profile.h"
#include "ds_profile_3gpp.h"
#include "ds_profile_3gpp2.h"
#include "ds_epc_sm_defs.h"
#include "ps_sys_event.h"
#include "ds_epc_ds_cmd_hdlr.h"
#include "ds3geventmgr.h"
#define FEATURE_DS_EPC_LOGGING

/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

                           PRIVATE DATA STRUCTURES

=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/
#define DS_MAX_EPC_INSTS          16

/*---------------------------------------------------------------------------
  Critical section used in EPC module
---------------------------------------------------------------------------*/
extern rex_crit_sect_type              ds_epc_critical_section;

#define DS_EPC_CTXT_BLK_CAUSE_NONE_MASK                      0x00
#define DS_EPC_CTXT_BLK_CAUSE_HANDOFF_ABORT_MASK             0x01
#define DS_EPC_CTXT_BLK_CAUSE_TRAT_NON_RCVRBL_MASK           0x02
#define DS_EPC_CTXT_BLK_CAUSE_IP_ADDR_MASK                   0x04

#define DS_EPC_CTXT_UNBLK_PREREG_MASK                        0x06
//DS_EPC_CTXT_BLK_CAUSE_TRAT_NON_RCVRBL_MASK | DS_EPC_CTXT_BLK_CAUSE_IP_ADDR_MASK //0x06

#define DS_EPC_CTXT_SRAT_TIMER_EXP_MASK                      0x08

#define DS_EPC_CTXT_TRAT_GOING_DOWN_MASK                     0x10

#define DS_EPC_CTXT_ATTACH_PDN_HO_MASK                       0x20

#ifdef FEATURE_DATA_WLAN_MAPCON
#define DS_EPC_CTXT_HO_GIVEUP_TIMER_EXP_MASK                 0x40
#define DS_EPC_CTXT_HO_PENDING_HO_THROTTLE_MASK              0x80

#define DS_EPC_PDN_SET_LOCAL_ABORT(user_data_ptr) \
 { \
   memset(&srat_tear_down_info, 0, sizeof(srat_tear_down_info)); \
   srat_tear_down_info.validity_mask |= \
                                     PS_CLIENT_DATA_INFO_LOCAL_ABORT_MASK; \
   srat_tear_down_info.data_info.local_abort = TRUE; \
   user_data_ptr = (void*)&srat_tear_down_info; \
 }

#endif /*FEATURE_DATA_WLAN_MAPCON*/

#define DS_EPC_CTXT_UNBLK_ALL_MASK                           0xFF
#define DS_EPC_CTXT_BLK_CAUSE_MAX_MASK                       0xFF

typedef enum
{
  DS_EPC_PDN_RAT_MIN =        0,
  DS_EPC_PDN_RAT_NONE =       DS_EPC_PDN_RAT_MIN,
  DS_EPC_PDN_RAT_LTE =        1,
  DS_EPC_PDN_RAT_EHRPD =      2,
  DS_EPC_PDN_RAT_HRPD =       3,
  DS_EPC_PDN_RAT_1X =         4,
  DS_EPC_PDN_RAT_GSM =        5,
  DS_EPC_PDN_RAT_WCDMA =      6,
  DS_EPC_PDN_RAT_TDSCDMA =    7,
  DS_EPC_PDN_RAT_OTHER =      8,
  DS_EPC_PDN_RAT_IWLAN =      9,
  DS_EPC_PDN_RAT_MAX =        DS_EPC_PDN_RAT_IWLAN
} ds_epc_pdn_rat_type;

#ifdef FEATURE_DATA_WLAN_MAPCON
/*---------------------------------------------------------------------------
  Define the prefer system for an EPC instance.
  OTHER means EPC does not take any actions on that system, including: GSM,
  WCDMA, TDSCDMA
---------------------------------------------------------------------------*/
typedef enum
{
  DS_EPC_PDN_SYS_MIN =              0,
  DS_EPC_PDN_SYS_INIT =             DS_EPC_PDN_SYS_MIN,
  DS_EPC_PDN_SYS_LTE =              1,
  DS_EPC_PDN_SYS_EHRPD =            2,
  DS_EPC_PDN_SYS_HRPD =             3,
  DS_EPC_PDN_SYS_1X =               4,
  DS_EPC_PDN_SYS_GSM =              5,
  DS_EPC_PDN_SYS_WCDMA =            6,
  DS_EPC_PDN_SYS_TDSCDMA =          7,
  DS_EPC_PDN_SYS_OTHER =            8,
  DS_EPC_PDN_SYS_LTE_TUNNELING =    9,
  DS_EPC_PDN_SYS_IWLAN =            10,
  DS_EPC_PDN_SYS_WLAN_LB =          11,
  DS_EPC_PDN_SYS_LTE_ATTACHING =    12,
  DS_EPC_PDN_SYS_MAX =              DS_EPC_PDN_SYS_LTE_ATTACHING
} ds_epc_pdn_pref_sys_type;

typedef struct
{
  ds_epc_pdn_pref_sys_type              pref_sys;
  boolean                               sys_status_active;
}ds_epc_pdn_pref_sys_status;
#endif /*FEATURE_DATA_WLAN_MAPCON*/

typedef enum
{
  DS_EPC_PDN_TECH_MIN  = 0,
  DS_EPC_PDN_TECH_NONE = DS_EPC_PDN_TECH_MIN,
  DS_EPC_PDN_TECH_3GPP,
  DS_EPC_PDN_TECH_3GPP2,
  DS_EPC_PDN_TECH_IWLAN,
  DS_EPC_PDN_TECH_OTHER,
  DS_EPC_PDN_TECH_MAX = DS_EPC_PDN_TECH_OTHER
} ds_epc_pdn_tech_type;

/*---------------------------------------------------------------------------
  DS EPC function return status
---------------------------------------------------------------------------*/
typedef enum
{
  DS_EPC_SUCCESS                      = 0,
  DS_EPC_INVALID,
  DS_EPC_UNREGISTERED_IFACE,
  DS_EPC_PROFILES_NOT_COMPATIBLE,
  DS_EPC_SRAT_CONTEXT_INVALID,
  DS_EPC_NOROUTE_HANDOFF_FAILURE,
  DS_EPC_IPV6_IID_TRANS_FAILURE,
  DS_EPC_SAME_RAT_HANDOFF_FAILURE,
  DS_EPC_TRAT_GOING_DOWN_IN_HANDOFF,
  DS_EPC_TRAT_UP_WHILE_EPC_IN_DOWN,
  DS_EPC_TRAT_UP_PRIMARY_IP_ADDR_FAILURE,
  DS_EPC_TRAT_UP_SWAPPING_FAILURE,
  DS_EPC_TRAT_DOWN_NOT_RECOVERABLE,
  DS_EPC_TRAT_DOWN_RECOVERABLE,
  DS_EPC_TRAT_DOWN_HO_THROTTLE,
  DS_EPC_RESYNC_FAILURE,
  DS_EPC_SRAT_TIMER_STARTED,
  DS_EPC_HO_ABORT_REJECT,
  DS_EPC_TRAT_RESOLVED_APN_NO_MATCH
} ds_epc_status_type;

/*---------------------------------------------------------------------------
Reason to update EPC PDN rat
---------------------------------------------------------------------------*/
typedef enum
{
  DS_EPC_PDN_RAT_UPDATE_IFACE_COMING_UP    = 0,
  DS_EPC_PDN_RAT_UPDATE_IFACE_UP,
  DS_EPC_PDN_RAT_UPDATE_IFACE_HANDOFF,
  DS_EPC_REASON_MAX
}ds_epc_pdn_rat_update_enum_type;

/*---------------------------------------------------------------------------
 Iface event identifier in ds cmd
---------------------------------------------------------------------------*/
typedef enum
{
  DS_EPC_DS_CMD_IFACE_EV_ID_EPC_UP           = 0,
  DS_EPC_DS_CMD_IFACE_EV_ID_EPC_DOWN,
  DS_EPC_DS_CMD_IFACE_EV_ID_TRAT_UP,
  DS_EPC_DS_CMD_IFACE_EV_ID_TRAT_DOWN,
  DS_EPC_DS_CMD_IFACE_EV_ID_TRAT_CONFIG,
  DS_EPC_DS_CMD_IFACE_EV_ID_SRAT_DOWN,
  DS_EPC_DS_CMD_IFACE_EV_ID_ATTACH_UP,
  DS_EPC_DS_CMD_IFACE_EV_ID_ATTACH_DOWN,
  DS_EPC_DS_CMD_IFACE_EV_ID_COMP_TRAT_UP,
  DS_EPC_DS_CMD_IFACE_EV_ID_COMP_TRAT_DOWN, 
  DS_EPC_DS_CMD_IFACE_EV_ID_MAX
} ds_epc_ds_cmd_iface_ev_id_type;

/*---------------------------------------------------------------------------
  Handoff Type identifier
---------------------------------------------------------------------------*/
typedef enum
{
  DS_EPC_PDN_FULL_HO            = 0,
  DS_EPC_PDN_SWAP_ONLY_HO
}ds_epc_pdn_ho_type;

typedef struct
{
  ps_iface_type                  *this_iface_ptr;
  ps_iface_event_enum_type        event;
  ps_iface_event_info_u_type      event_info;
  void                           *user_data;
} ds_epc_iface_ev_data_type;

typedef struct
{
  ds_epc_ds_cmd_iface_ev_id_type  iface_ev_cmd_id;
  ds_epc_iface_ev_data_type       iface_ev_data;
} epc_iface_ev_cmd_params_type;

typedef struct
{
  rex_timer_type              timer;
  boolean 	              running_flag;
} ds_epc_timer_type;

#ifdef FEATURE_DATA_WLAN_MAPCON
typedef struct
{
  uint8                       failure_count;    /** < throttle failure count */
  boolean                     comp_recv_throttle; /** < If comp inst's throttle has been recieved */
  ds_epc_timer_type           timer; /** < epc timer */
} ds_epc_throttle_timer_type;
#endif
/*---------------------------------------------------------------------------
  Structure for srat timer. The srat timer starts when handoff fails with
  recoverable failures.
  start_count: records how many times the timer has started for a given srat
               to trat HO.
  timer:       The real timer.
  trat_name:   Indicate on which trat HO was failed.
  trat_type:   Indicate which rat_type the HO was failed. In order to
               distinguish UMTS and LTE radio.
--------------------------------------------------------------------------*/
typedef struct
{
  ds_epc_timer_type           timer;
  uint8                       start_count;
  ps_iface_name_enum_type     trat_name;
  ds_epc_pdn_rat_type         trat_type;
} ds_epc_srat_timer_type;

typedef struct __ds_epc_cb_s ds_epc_cb_type;

struct __ds_epc_cb_s
{
  ps_iface_type             local_iface;
  acl_type                  local_out_acl;
  acl_type                  local_in_acl;

  uint32                     epc_pdn_instance_id;

  /* Associated PDN meta and HO SM pointer*/
  stm_state_machine_t       * pdn_meta_sm;
  stm_state_machine_t       * pdn_ho_sm;
  /*-------------------------------------------------------------------------
  this is to restrict the handover across only a subset of physical ifaces.
  This is populated during power up.

  Different logic can also determine whether the IP session continuity
  is possible across technologies.
  -------------------------------------------------------------------------*/
  void                    * client_data_ptr;
  ds_epc_cb_type          * this_epc_ptr;
  void                    * iface_up_buf;
  void                    * iface_down_buf;
  void                    * trat_iface_config_ev_buf;
  void                    * srat_iface_prefix_update_ev_buf;
  /*-------------------------------------------------------------------------
   Registers the SRAT down event after Handoff started in case of SRAT went
   down before TRAT came up.
  -------------------------------------------------------------------------*/
  void                    * srat_iface_down_ev_buf;
  ps_iface_type           * waiting_on_reg_srat_iface_ptr;
  void                    * epc_up_buf;
  void                    * epc_down_buf;
  ps_iface_type           * waiting_on_reg_evnt_iface_ptr;

  /*-------------------------------------------------------------------------
  flag to prevent repeated prereg retry in the same prereg zone in the
  following cases
    - alc policy misconfiguration including apn mismatch, pdp type mismatch
    - srat conetxt ioctl failure
    - handoff no route failure
    - ipv6 apply failure
    - ip addr comparison failure
  The flag is reset when another S101 prereg enable is received
  -------------------------------------------------------------------------*/
  uint8                           ctxt_flag_mask;

  /*-------------------------------------------------------------------------
  Timer per EPC
  -------------------------------------------------------------------------*/
  ds_epc_srat_timer_type       epc_data_srat_timer;
  ds_epc_timer_type            epc_data_retry_timer;
#ifdef FEATURE_DATA_WLAN_MAPCON
  ds_epc_timer_type            epc_data_ho_giveup_timer;
#endif /*FEATURE_DATA_WLAN_MAPCON*/
  /*-------------------------------------------------------------------------
  Store the acl_policy information
  -------------------------------------------------------------------------*/
  struct
  {
    boolean                    acl_info_valid;
    acl_policy_info_type       any_acl_policy_info;
    ds_profile_info_type       profile_3gpp_apn_name;
    ps_iface_bearer_ip_type    profile_3gpp_pdn_type_value;
    ds_profile_info_type       profile_3gpp2_apn_name;
    ps_iface_bearer_ip_type    profile_3gpp2_pdn_type_value;
  }acl_info;

  /*-------------------------------------------------------------------------
  Store the APN name
  -------------------------------------------------------------------------*/
  ds_profile_info_type         resolved_apn_name;

  /*-------------------------------------------------------------------------
  Store the rat type for an EPC instance
  -------------------------------------------------------------------------*/
  ds_epc_pdn_rat_type          pdn_rat;

#ifdef FEATURE_DATA_WLAN_MAPCON
  /*-------------------------------------------------------------------------
  Store the current prefer system for an EPC instance
  -------------------------------------------------------------------------*/
  ds_epc_pdn_pref_sys_status   pdn_pref_sys;
#endif /*FEATURE_DATA_WLAN_MAPCON*/

  uint32                       handoff_trat_down_reason; 
  boolean                      is_disable; 
#ifdef FEATURE_DATA_WLAN_MAPCON
  /*-------------------------------------------------------------------------
  Companion iface for an EPC instance
  -------------------------------------------------------------------------*/
  ds_epc_cb_type          * companion_epc_ptr; 
  /*-------------------------------------------------------------------------
   Comp TRAT UP/DOWN buffer
  -------------------------------------------------------------------------*/
  void                    * comp_iface_up_buf;
  void                    * comp_iface_down_buf;
  /*-------------------------------------------------------------------------
   Registers comp inst TRAT UP/TRAT DOWN event
  -------------------------------------------------------------------------*/
  ps_iface_type           * waiting_on_reg_comp_evnt_iface_ptr;
  /*-------------------------------------------------------------------------
   Cache for ho info: SRAT and TRAT will be recorded 
  -------------------------------------------------------------------------*/
  ps_iface_handoff_event_info_type  ho_info;
#endif /*FEATURE_DATA_WLAN_MAPCON*/
};

/*---------------------------------------------------------------------------
EPC teardown cmd parameter type
---------------------------------------------------------------------------*/
typedef struct
{
  ds_epc_cb_type                                   *instance_ptr;
  boolean                                           local_abort;
} epc_tear_down_cmd_params_type;

/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
                            FORWARD DECLARATIONS
=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/
extern ds_epc_cb_type       * ds_epc_instance_ptrs[DS_MAX_EPC_INSTS];
extern uint32  ds_epc_disable_flag;


/*===========================================================================
FUNCTION      DS_EPC_PDN_CREATE

DESCRIPTION

DEPENDENCIES  None

RETURN VALUE  -1 on failure
               0 on success

SIDE EFFECTS  None
===========================================================================*/
int ds_epc_create
(
  ds_epc_cb_type        *instance_ptr,      /* Ptr to EPC Iface instance*/
  void                  *client_data_ptr,
  acl_fptr_type          acl_f_ptr
);

/*===========================================================================
FUNCTION      DS_EPC_PDN_INIT

DESCRIPTION   Called at initialization.
              Creates the iface

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void ds_epc_init(void);

/*===========================================================================
FUNCTION      DS_EPC_IFACE_INIT

DESCRIPTION   Called once at mobile power-up.  Registers the appropriate
              iface unctionality with PS IFACE.

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None
===========================================================================*/
void ds_epc_iface_init
(
  ps_iface_type             * iface_ptr,       /* ptr to 1x iface          */
  acl_type                  * my_out_acl_ptr,  /* ptr to acl               */
  acl_type                  * my_in_acl_ptr   /* ptr to acl               */
);

/*===========================================================================
FUNCTION      DS_EPC_BRIDGE_PROC_CMD_HDLR

DESCRIPTION   Calls the associated bridge function pointer.

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
int ds_epc_bridge_proc_cmd_hdlr
(
  ps_iface_type *this_iface_ptr,
  ps_iface_type *target_iface_ptr,
  void *client_data
);

/*===========================================================================
FUNCTION      DS_EPC_IFACE_RX_CMD

DESCRIPTION

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None

===========================================================================*/
int ds_epc_iface_rx_cmd
(
  ps_iface_type        *  this_iface_ptr,
  dsm_item_type        ** pkt_chain_ptr,
  ps_rx_meta_info_type *  meta_info_ptr
);

/*===========================================================================
FUNCTION      DS_EPC_GET_EPC_CB

DESCRIPTION   This is utility function and gives EPC CB ptr for associated
              EPC iface ptr

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
ds_epc_cb_type  * ds_epc_get_epc_cb
(
  ps_iface_type             * this_iface_ptr
);

/*===========================================================================
FUNCTION      DS_EPC_GET_EPC_CB_BY_PDN_META_SM

DESCRIPTION   This is utility function and gives EPC CB ptr for associated PDN
              Meta SM

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================
ds_epc_cb_type  * ds_epc_get_epc_cb_by_pdn_meta_sm
(
  stm_state_machine_t *sm
);*/

/*===========================================================================
FUNCTION      DS_EPC_RESET_ACL_INFO

DESCRIPTION   This function resets the ACL information: Handoff flag

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds_epc_reset_acl_info
(
  ds_epc_cb_type       * epc_iface_ctrl_ptr
);

/*===========================================================================
FUNCTION      DS_EPC_PDN_CTL_GET_INSTANCE_IFACE_PTR

DESCRIPTION:  Get the EPC iface ptr from inst number.

DEPENDENCIES  None

RETURN VALUE  EPC iface ptr

SIDE EFFECTS  None
===========================================================================*/
ps_iface_type *ds_epc_pdn_ctl_get_instance_iface_ptr
(
  ds_epc_cb_type  *instance_ptr
);

#if 0
/*===========================================================================
FUNCTION      DS_EPC_PDN_META_EDCT_TIMER_EXP_CMD_PROC

DESCRIPTION:  Function processes EPC data context timer expiry on DS task
              context.

DEPENDENCIES  None

RETURN VALUE  TRUE/FALSE

SIDE EFFECTS  None
===========================================================================*/
void ds_epc_pdn_meta_edct_timer_exp_cmd_proc(void);
#endif

/*===========================================================================
FUNCTION      DS_EPC_PDN_CTL_REPROCESS_SYSTEM

DESCRIPTION:  Re-process the current UE system for the PDN

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds_epc_pdn_ctl_reprocess_system
(
  ds_epc_cb_type  *instance_ptr
);

/*===========================================================================
FUNCTION      DS_EPC_PDN_CTL_UPDATE_INSTANCE_RAT

DESCRIPTION:  Set the pdn rat with the current rat.

PARAMETERS    epc_iface_ctrl_ptr   - Ptr to the EPC iface ctrl block

DEPENDENCIES  None

RETURN VALUE  TRUE/FALSE: success/failure

SIDE EFFECTS  None
===========================================================================*/
void ds_epc_pdn_ctl_update_instance_rat
(
  ds_epc_cb_type                  * epc_iface_ctrl_ptr,
  ds_epc_pdn_rat_update_enum_type   rat_update_type
);

/*===========================================================================
FUNCTION      DS_EPC_PDN_CTL_RESET_INSTANCE_RAT

DESCRIPTION:  Reset the pdn rat.

PARAMETERS    epc_iface_ctrl_ptr   - Ptr to the EPC iface ctrl block

DEPENDENCIES  None

RETURN VALUE  TRUE/FALSE: success/failure

SIDE EFFECTS  None
===========================================================================*/
void ds_epc_pdn_ctl_reset_instance_rat
(
  ds_epc_cb_type                  * epc_iface_ctrl_ptr
);

/*===========================================================================
FUNCTION      DS_EPC_PDN_CTL_GET_INSTANCE_RAT

DESCRIPTION:  Get pdn_rat in EPC ctrl.block.

PARAMETERS    epc_iface_ctrl_ptr   - Ptr to the EPC iface control block.

DEPENDENCIES  None

RETURN VALUE: instance rat

SIDE EFFECTS  None
===========================================================================*/
ds_epc_pdn_rat_type ds_epc_pdn_ctl_get_instance_rat
(
  ds_epc_cb_type                  * epc_iface_ctrl_ptr
);

/*===========================================================================
FUNCTION      DS_EPC_PDN_CTL_POST_DS_CMD_FOR_IFACE_EV

DESCRIPTION:  Post DS command for processing an iface event.

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds_epc_pdn_ctl_post_ds_cmd_for_iface_ev
(
  ds_epc_ds_cmd_iface_ev_id_type   iface_ev_cmd_id,
  ps_iface_type                  * this_iface_ptr,
  ps_iface_event_enum_type         event,
  ps_iface_event_info_u_type     * event_info_ptr,
  void                           * user_data
);

/*===========================================================================
FUNCTION      DS_EPC_PROCESS_IFACE_EV_CMD

DESCRIPTION:  Process DS command for iface event.

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds_epc_process_iface_ev_cmd
(
  epc_iface_ev_cmd_params_type  * iface_ev_cmd_info_ptr
);

/*===========================================================================
FUNCTION      DS_EPC_PDN_CTL_STORE_IP_CONTEXT_FROM_SRAT

DESCRIPTION: Get the ip context from srat and set it to epc ctrl.

DEPENDENCIES:

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
boolean ds_epc_pdn_ctl_store_ip_context_from_srat
(
  ds_epc_cb_type       * epc_iface_ctrl_ptr
);

/*===========================================================================
FUNCTION      DS_EPC_RESET_PDN_RAT

DESCRIPTION:  Resetet pdn_rat in EPC ctrl.block.

PARAMETERS    epc_iface_ctrl_ptr   - Ptr to the EPC iface control block.

DEPENDENCIES  None

RETURN VALUE  TRUE/FALSE: success/failure

SIDE EFFECTS  None
===========================================================================
void ds_epc_reset_pdn_rat
(
  ds_epc_cb_type                  * epc_iface_ctrl_ptr
);*/

/*===========================================================================
FUNCTION      DS_EPC_CTL_GET_CURR_RAT

DESCRIPTION:  Get current bearer tech.

PARAMETERS    epc_iface_ctrl_ptr   - Ptr to the EPC iface ctrl block

DEPENDENCIES  None

RETURN VALUE  TRUE/FALSE: success/failure

SIDE EFFECTS  None
===========================================================================*/
ds_epc_pdn_rat_type ds_epc_ctl_get_curr_rat(void);

/*===========================================================================
FUNCTION      DS_EPC_UPDATE_PDN_RAT

DESCRIPTION:  Set the pdn rat with the current rat.

PARAMETERS    epc_iface_ctrl_ptr   - Ptr to the EPC iface ctrl block

DEPENDENCIES  None

RETURN VALUE  TRUE/FALSE: success/failure

SIDE EFFECTS  None
===========================================================================
void ds_epc_update_pdn_rat
(
  ds_epc_cb_type                  * epc_iface_ctrl_ptr
);*/

/*===========================================================================
FUNCTION      DS_EPC_IS_HANDOFF_ALLOWED

DESCRIPTION   Called by physical MHs (LTE and eHRPD) to query if handoff is
              possible.

DEPENDENCIES  None.

RETURN VALUE

SIDE EFFECTS  None.
===========================================================================*/
boolean ds_epc_pdn_ctl_is_handoff_allowed(
  ps_iface_type *iface_ptr
);

/*===========================================================================
FUNCTION      ds_epc_pdn_ctl_is_epc_pdn_throttled

DESCRIPTION   Check if an epc instance is throttled for pdn.

PARAMETERS

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
boolean ds_epc_pdn_ctl_is_epc_pdn_throttled
(
  ds_epc_cb_type  * instance_ptr
);

/*===========================================================================
FUNCTION      DS_EPC_PROCESS_IFACE_TEARDOWN_CMD

DESCRIPTION:  Process DS_EPC_IFACE_TEARDOWN_CMD from ds_task

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds_epc_process_iface_teardown_cmd
(
  epc_tear_down_cmd_params_type              *tear_down_info_ptr
);

#ifdef FEATURE_DATA_WLAN_MAPCON
/*===========================================================================
FUNCTION      DS_EPC_PROCESS_HO_GIVEUP_TIMER_EXPIRE_CMD

DESCRIPTION:  Process DS_EPC_HO_GIVEUP_TIMER_EXP_PROC_CMD from ds_task

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds_epc_process_ho_giveup_timer_expire_cmd
(
  ds_epc_cb_type *instance_ptr
);
#endif /*FEATURE_DATA_WLAN_MAPCON*/

/*===========================================================================
FUNCTION      DS_EPC_PROCESS_SRAT_TIMER_EXPIRE_CMD

DESCRIPTION:  Process DS_EPC_SRAT_TIMER_EXP_PROC_CMD from ds_task

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds_epc_process_srat_timer_expire_cmd
(
  ds_epc_cb_type *instance_ptr
);

/*===========================================================================
FUNCTION      DS_EPC_PROCESS_RETRY_TIMER_EXPIRE_CMD

DESCRIPTION:  Process DS_EPC_RETRY_TIMER_EXP_PROC_CMD from ds_task

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds_epc_process_retry_timer_expire_cmd
(
  ds_epc_cb_type *instance_ptr
);

/*===========================================================================
FUNCTION      DS_EPC_PDN_CTL_SSM_EV_HDLR

DESCRIPTION:  Handle System State Machine input events for all PDNs

DEPENDENCIES  None

RETURN VALUE  TRUE/FALSE

SIDE EFFECTS  None
===========================================================================*/
boolean ds_epc_pdn_ctl_ssm_ev_hdlr
(
  epc_pdn_meta_sm_input_event_type event,
  void *info
);

/*===========================================================================
FUNCTION      DS_EPC_PDN_CTL_PREREG_COMPLETE_REP_CMD_PROC

DESCRIPTION   Process function for DS_EPC_eHRPD_PREREG_COMPLETE_RSP_CMD
              from DS task.

DEPENDENCIES  None.

RETURN VALUE

SIDE EFFECTS  None.
===========================================================================*/
void ds_epc_pdn_ctl_prereg_complete_rsp_cmd_proc
(
  boolean   prereg_handoff_success,
  uint32    subs_id
);

/*===========================================================================
FUNCTION      DS_EPC_PDN_CTL_EHRPD_PDN_THROTTLE_PS_CB

DESCRIPTION   Callback function for PS event
              PS_SYS_EVENT_PDN_THROTTLE_INFO

DEPENDENCIES  None.

RETURN VALUE

SIDE EFFECTS  None.
===========================================================================*/
void ds_epc_pdn_ctl_ehrpd_pdn_throttle_ps_cb
(
  ps_sys_tech_enum_type                          tech_type,
  ps_sys_subscription_enum_type                  subscription_id,
  ps_sys_event_enum_type                         event_name,
  void                                         * event_info_ptr,
  void                                         * user_data_ptr
);

/*===========================================================================
FUNCTION      DS_EPC_PDN_CTL_EHRPD_PDN_THROTTLE_DS3G_CB

DESCRIPTION   Callback function for DS3G event
              DS3GEVENTMGR_PDN_THROTTLE_INFO_EV

DEPENDENCIES  None.

RETURN VALUE

SIDE EFFECTS  None.
===========================================================================*/
void ds_epc_pdn_ctl_ehrpd_pdn_throttle_ds3g_cb
(
  ps_sys_tech_enum_type                          tech_type,
  ps_sys_subscription_enum_type                  subscription_id,
  ds3geventmgr_event_type                        event_name,
  void                                         * event_info_ptr,
  void                                         * user_data_ptr
);

/*===========================================================================
FUNCTION      DS_EPC_PDN_CTL_PREREG_COMPLETE_REP_CB_EX

DESCRIPTION   Callback function for DS3G event 
              DS3GEVENTMGR_EHRPD_PREREG_HANDOFF_COMPLETE_RESP_EV

DEPENDENCIES  None.

RETURN VALUE

SIDE EFFECTS  None.
===========================================================================*/
void ds_epc_pdn_ctl_prereg_complete_rsp_cb_ex
(
  ps_sys_subscription_enum_type                  subscription_id,
  ps_sys_tech_enum_type                          tech_type,
  ds3geventmgr_event_type                        event_name,
  void                                         * event_info_ptr,
  void                                         * user_data_ptr
);

/*===========================================================================
FUNCTION      DS_EPC_PDN_CTL_PREREG_COMPLETE_REP_DS3G_CB

DESCRIPTION   Callback function for DS3G event 
              DS3GEVENTMGR_EHRPD_PREREG_HANDOFF_COMPLETE_RESP_EV

DEPENDENCIES  None.

RETURN VALUE

SIDE EFFECTS  None.
===========================================================================*/
void ds_epc_pdn_ctl_prereg_complete_rsp_ds3g_cb
(
  ps_sys_subscription_enum_type                  subscription_id,
  ps_sys_tech_enum_type                          tech_type,
  ds3geventmgr_event_type                        event_name,
  void                                         * event_info_ptr,
  void                                         * user_data_ptr
);

/*===========================================================================
FUNCTION      DS_EPC_PDN_CTL_EHRPD_PDN_THROTTLE_CMD_PROC

DESCRIPTION   Process function for DS_EPC_EHRPD_PDN_THROTTLE_PROC_CMD
              from DS task.

DEPENDENCIES  None.

RETURN VALUE

SIDE EFFECTS  None.
===========================================================================*/
void ds_epc_pdn_ctl_ehrpd_pdn_throttle_cmd_proc
(
  list_type               * list_info,
  uint32                    subs_id
);

/*===========================================================================
FUNCTION      DS_EPC_PDN_CTL_SEND_HO_COMPLETE_IOCTL

DESCRIPTION:  Send Handoff_COMPLETE IOCTL to ds707 MH and indicate if all PDNs
              are finished pre-reg on LTE if handoff needed.
     
              Only call ioctl when AT LEAST ONE CONTEXT IS PREREGED.
 
DEPENDENCIES  None

RETURN VALUE  TRUE/FALSE

SIDE EFFECTS  None
===========================================================================*/
void ds_epc_pdn_ctl_send_ho_complete_ioctl
(
  uint32 subs_id
);

/*===========================================================================
FUNCTION      DS_EPC_PDN_CTL_CB_INIT_CMD_PROC

DESCRIPTION   Process function for DS_EPC_CB_INIT_CMD from DS task. The cmd 
              is put after EPC routing is done. 

DEPENDENCIES  None.

RETURN VALUE

SIDE EFFECTS  None.
===========================================================================*/
void ds_epc_pdn_ctl_cb_init_cmd_proc
(
  epc_cb_init_cmd_params_type                 *info_ptr
);

#ifdef FEATURE_DATA_WLAN_MAPCON
/*===========================================================================
FUNCTION      DS_EPC_CTL_PDN_PREF_SYS_INIT

DESCRIPTION:  Initialize the pref system for a EPC instance. It is called 
              during EPC routing. 

PARAMETERS    epc_iface_ctrl_ptr   - Ptr to the EPC iface ctrl block

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds_epc_ctl_pdn_pref_sys_init
(
  ds_epc_cb_type                  *epc_inst_ptr
);

/*===========================================================================
FUNCTION      DS_EPC_CTL_PDN_GET_PREF_SYS

DESCRIPTION:  Get the pref sys and its status of a EPC instance

PARAMETERS    epc_iface_ctrl_ptr   - Ptr to the EPC iface ctrl block

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
ds_epc_pdn_pref_sys_status*   ds_epc_ctl_pdn_get_pref_sys
(
  ds_epc_cb_type                  *epc_inst_ptr
);

/*===========================================================================
FUNCTION      DS_EPC_CTL_PDN_PREF_SYS_RESET

DESCRIPTION:  Reset the pref system status for an EPC instance

PARAMETERS    epc_iface_ctrl_ptr   - Ptr to the EPC iface ctrl block

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds_epc_ctl_pdn_pref_sys_reset
(
  ds_epc_cb_type                  *epc_inst_ptr
);

/*===========================================================================
FUNCTION      DS_EPC_CTL_GET_CURR_PDN_RAT

DESCRIPTION:  Get current rat of a PDN.It is computed by its current prefer 
              system. 

PARAMETERS    epc_iface_ctrl_ptr   - Ptr to the EPC iface ctrl block

DEPENDENCIES  None

RETURN VALUE  TRUE/FALSE: success/failure

SIDE EFFECTS  None
===========================================================================*/
ds_epc_pdn_rat_type ds_epc_ctl_get_curr_pdn_rat
(
  ds_epc_cb_type                  *epc_inst_ptr
);

/*===========================================================================
FUNCTION      DS_EPC_PDN_CTL_ADD_COMP_INST

DESCRIPTION   Add the companion instance ptr to current EPC instance

PARAMETERS    Current EPC instance ptr

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds_epc_pdn_ctl_add_comp_inst
(
  ds_epc_cb_type *instance_ptr
);

/*===========================================================================
FUNCTION      DS_EPC_PDN_CTL_REMOVE_COMP_INST

DESCRIPTION   Reset Companion instance pointer for current EPC instance to NULL

PARAMETERS    Current EPC instance ptr

DEPENDENCIES  None

RETURN VALUE  TRUE: successfully remove comp inst
              FALSE: failed to remove comp inst 

SIDE EFFECTS  None
===========================================================================*/
boolean ds_epc_pdn_ctl_remove_comp_inst
(
  ds_epc_cb_type *instance_ptr
);

/*===========================================================================
FUNCTION      DS_EPC_PDN_CTL_TEAR_DOWN_DANGLING_TRAT

DESCRIPTION   Tear down the LTE PDNs which are not in use, if the same 
              apn handoff success/fail to other EPC rat.  

DEPENDENCIES  None.

RETURN VALUE

SIDE EFFECTS  None
===========================================================================*/
void ds_epc_pdn_ctl_tear_down_stale_srat
(
  ds_epc_cb_type             *instance_ptr
);

#endif /*FEATURE_DATA_WLAN_MAPCON*/

/*===========================================================================
FUNCTION      DS_EPC_PDN_CTL_UPDATE_APN_INFO

DESCRIPTION   Query the actual APN info from underlying SRAT iface. If EPC 
              was brought up on NULL profile, and network resolved to a APN1,
              EPC overwrite the cached APN name to APN1 and update to
              DS EPC NULL APN MGR.

DEPENDENCIES  None.

RETURN VALUE

SIDE EFFECTS  None.
===========================================================================*/
void ds_epc_pdn_ctl_update_apn_info
(
  ds_epc_cb_type                            * instance_ptr
);

/*===========================================================================
FUNCTION      DS_EPC_PDN_CTL_UPDATE_INSTANCE_RAT_BY_PDN_PREF_SYS

DESCRIPTION:  Set the pdn rat based on the current PDN prefer system.

PARAMETERS    epc_iface_ctrl_ptr   - Ptr to the EPC iface ctrl block. 
              rat_update_type - identify when the rat get updated.  

DEPENDENCIES  None

RETURN VALUE  TRUE/FALSE: success/failure

SIDE EFFECTS  None
===========================================================================*/
void ds_epc_pdn_ctl_update_instance_rat_by_pdn_pref_sys 
(
  ds_epc_cb_type                  * epc_iface_ctrl_ptr,
  ds_epc_pdn_rat_update_enum_type   rat_update_type
);

//for testing
#ifdef TEST_FRAMEWORK
#error code not present
#endif /* TEST_FRAMEWORK */

#endif /*FEATURE_EPC_HANDOFF*/
#endif /*DS_EPC_PDN_CTL_H*/
