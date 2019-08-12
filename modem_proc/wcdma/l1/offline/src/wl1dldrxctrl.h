#ifndef WL1DLDRXCTRL_H
#define WL1DLDRXCTRL_H
/*===========================================================================
                 WCDMA L1 DOWNLINK CPC DRX CONTROL MODULE

GENERAL DESCRIPTION
  This file contains the code for carrying out CPC DRX operations

EXTERNALIZED FUNCTIONS

INTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS


Copyright (c) 2007 - 2014 by Qualcomm Technologies, Inc.  All Rights Reserved.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order. 
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/wcdma/l1/offline/src/wl1dldrxctrl.h#1 $ $DateTime: 2016/12/13 08:00:12 $ $Author: mplcsds1 $   


when       who      what, where, why
--------   ---      -------------------------------------------------------- 
06/17/16   sl       Add 40 bpg delay for RF warm up while freezing EDRX scheduling
12/01/15   sg       KW error fix
10/05/15   at       Added state check to unblock EUL ISR if CPC lite sleep or wakeup is in transition.
09/06/15   sr/cjz   try sche_seq lock and call ent_sche_update in task context
12/15/14   sr       Changes to vote to DLDRCCTRL for RF to be on during LTE driver cleanup.03/08/14   nd       Update 0x423A log packet for Rx Blanking
11/18/14   gp       Fix for incorrect datatype for edch inactivity timer that may lead to truncation
03/08/14   nd       Update 0x423A log packet for Rx Blanking
10/22/14   sr       Changes to inform CPC clients when CPC is activated
10/07/14   mit      fixing compiler warning
08/21/14   mit      Fixed compiler warnings
08/18/14   sr       Changes to wakeup HS entity 60 bpg before RxBurst and waking up EDRX at 10th bpg after FS
08/07/14   sr       Changes to handle freeze/unfreeze for CPC SCCH order in local command
02/18/13   sr       Initial checkin for CPC Lite sleep
07/22/14   sr       Bring RF out of LPM when waking up from EDRX FS
07/16/14   sr       EDRX cleanup
06/07/14   nd       Deregister cb for log submission while HS reconfig.
05/11/14   pr       MCPM Opts for unification of HSDPA/HSUA Clk requests.
03/06/14   nd       Set RF state while handling QTA start/stop to handle CPC-DRX with
                    DSDS.
05/15/14   sr       FR 20740 - EDRX timeline optimization initial checkin05/09/14   sr       Fixed 0x423A logging issue
05/09/14   sr       Changes to introduce new client RF voting for Sec PCCPCH 
04/11/14   sr       Changes to introduce voting mechanism to ignore CPC SCCH order
03/31/14   sad      Change to skip cpc DRX setup after implicit HS drop if only 
                    EDRX was active before
03/26/14   nd       Made scch inactivity counter uint16 to take care of values
                    greater than 255
07/17/13   sm       Adding RxD as a client for RF vote as part of RxDPM with CPC-DRX
07/23/13   vs       Added ASDiv support in the enum wl1_dl_drxctrl_rf_ctrl_client_enum_type
07/11/13   sr       Added NV support to disable CPC-DRX when 
                    DRX length is 4 or 5 subframes
07/05/13   sad      Changes to DRX order handling
02/11/13   gsk/sr   EDRX Full Sleep initial check in.
08/31/12   hk       Changes to handle freeze/un-freeze T321 based on E-DCH resource
08/19/12   sj       Added support to Suspend/Unsuspend CME state machine based
                    on E-DRX state during HS-FACH
07/24/12   geg      Upmerge WCDMA.MPSS.1.0 to WCDMA.MPSS.2.0
06/27/12   hk       Added support to vote for RF ON during SIB decode
05/01/12   pr       Changes to remember SCCH Order for CPC-DRX
04/27/12   pr       Disable RF Toggle during TxD Detection for CPC-DRX
04/04/12   sj       Registers CME callback for ARD v2 support
04/04/12   hk       Changes to skip RF PLL toggle for sleep duration of
                    less than 2 subframes
03/27/12   hk       Added support for FEATURE_WCDMA_HS_FACH_DRX
02/14/12   hk       Implicit CPC-DRX STOP/START during SC->DC transition
01/24/12   vsr      ARD updates
01/18/12   hk       Generalized the handling of DRX START/STOP mechanism
07/07/11   hk       Up-merge from offline/main/latest
04/28/11   sj       upmerge from offline/main
04/25/11   geg      Replaced *.h with *_v.h
04/25/11   amj      Added grant monitoring enabled field to CPC DRX CFG log
                    Changed function signature to take a pointer to log packet
04/19/11   rgn      Made changes to DRX logging algorithm
04/08/11   rgn      Made some changes for logging
03/31/11   amj      Added code for CPC DRX CFG log packet.
03/23/11   rgn      Reorganized logging operations 
09/27/10   dm       Added support for CM state in 0x423A log packet.
09/22/10   rgn      Fixed a compilation error 
09/21/10   rgn      Changed pre wakeup margin to fixed value
09/15/10   dm       Fixed lint errors.
09/16/10   rgn      Lowered gotosleep and wakeup f3 levels 
09/08/10   dm       Added changes for 0x423A log packet.
09/07/10   dm       Added optimization which dynamically calculates wakeup 
                    pre margin.
09/02/10   rgn      Added CM support for CPC-DRX 
08/20/10   dm       Fixed compilation warnings.
08/20/10   dm       Fixed KW error.
08/16/10   dm       Added support for logging CPC DRX parameters.
08/08/10   dm       Added macro to check if scheduling is frozen
08/04/10   oh       ADded support for ns-rgch in cpc drx mode 
03/05/10   rgn      First cut
===========================================================================*/
#include "wcdma_variation.h"
#include "comdef.h"
#include "customer.h"
#if defined(FEATURE_WCDMA_CPC_DRX)|| defined(FEATURE_WCDMA_HS_FACH_DRX)
#include "wl1m.h"
#ifdef WL1_DL_DRX_CTRL_UNIT_TEST
#include "test_main.h"
#else
#include "seq_v.h"
#include "tlm.h"
#endif

/** Maximum number of drx entities to be monitored */
#define WL1_DL_DRXCTRL_MAX_NUM_ENTITIES 5
#define WL1_DL_DRXCTRL_INVALID_ENTITY_IDX 0xFF

/** time after which scch order should be applied */
#define WL1_DL_DRXCTRL_SCCH_ORDER_APPLY_TIME_IN_TICKS 120


#define WL1_DL_DRXCTRL_MSG_HIGH MSG_HIGH

#define WL1_DL_DRXCTRL_MSG_ERROR MSG_ERROR

/*Size of the log packet, in number of samples*/
#define WL1_DL_DRXCTRL_MAX_FRAMES_PER_LOG_BUF 20

/*WL1_DL_DRXCTRL_MAX_FRAMES_PER_LOG_BUF * 5*/
#define WL1_DL_DRXCTRL_MAX_LOG_BUF_IDX 100

#define WL1_DL_DRXCTRL_NUM_MDSP_DRX_EVT_LOG_SAMPLES 15

#define WL1_DL_DRXCTRL_NUM_MDSP_DRX_INACT_CNTR_SAMPLES 5

/* Size of the array of processor speed values */
#define WL1_DL_DRXCTRL_MAX_NUM_SPEED_VALUES 8

#define WL1_DL_DRXCTRL_LOGGING_ARR_SIZE 2

extern boolean disable_tcxo_sleep;
extern boolean wl1_edrx_pccpch_active;

/** function pointer */
typedef void (*WL1_DL_DRXCTRL_ENTITY_OPS_HANDLER)(void);
typedef void wl1_dl_drxctrl_ard_cb_func_type(boolean);
typedef void wl1_dl_drxctrl_cme_cb_func_type(boolean, boolean);

/** enum describing states in the DRX control state machine */
typedef enum
{
  /** inactive state */
  WL1_DL_DRX_MODE_INACTIVE = 0,
  /** drx mode configured but not active */
  WL1_DL_DRX_MODE_CONFIGURED = 1,
  /** drx mode active rx is active */
  WL1_DL_DRX_MODE_RX_ACTIVE = 2,
  /** drx mode rx is warming up */
  WL1_DL_DRX_MODE_RX_WARMINGUP = 3,
  /** drx mode active and rx is inactive */
  WL1_DL_DRX_MODE_RX_INACTIVE = 4,
  /** drx mode in SUSPEND state as MIMO */
  WL1_DL_DRX_MODE_DRX_SUSPENDED = 5,

  #ifdef FEATURE_WCDMA_HS_FACH_DRX
  /* sleep has been initiated with edrx -channel teardown*/
  WL1_DL_DRX_MODE_INITIATE_SLEEP = 6,
  /** drx mode active and rx is in TCXO SLEEP */
  WL1_DL_DRX_MODE_RX_ASLEEP = 7,
  /* wkup has been initiated with edrx -channel setup*/
  WL1_DL_DRX_MODE_INITIATE_WKUP = 8,
  #endif
  /* CPC lite sleep state */
  WL1_DL_DRX_MODE_CPC_LITE_SLEEP = 9,

  WL1_DL_DRX_MODE_MAX_NUM_STATES = 10
}wl1_dl_drxctrl_state_enum_type;

/** enum describing states in the DRX control state in FW */
typedef enum
{
  /** inactive state */
  WL1_DL_DRX_FW_STATE_INACTIVE = 0,
  /** drx mode configured but not active */
  WL1_DL_DRX_FW_STATE_ACTIVE = 1,
  /** drx mode active rx is active */
  WL1_DL_DRX_FW_STATE_SUSPENDED = 2,
}wl1_dl_drxctrl_drx_fw_state_enum_type;

/** enum describing the entity state */
typedef enum 
{
  /** entity awake state not configured */
  WL1_DL_DRX_ENTITY_AWAKE_STATE_INACTIVE = 0,
  /** entity to stay awake indefinitely */
  WL1_DL_DRX_ENTITY_STAY_AWAKE_INDEFINITELY = 1,
  /** entity to stay awake for a fixed duration */
  WL1_DL_DRX_ENTITY_FIXED_WAKEUP_DURATION = 2,
  WL1_DL_DRX_ENTITY_NUM_WAKEUP_TYPES = 3
}wl1_dl_drxctrl_entity_wakeup_enum_type;

/** enum describing the sleep state of an entity */
typedef enum 
{
  /** entity awake state not configured */
  WL1_DL_DRX_ENTITY_SLEEP_STATE_INACTIVE = 0,
  /** entity to sleep indefinitely */
  WL1_DL_DRX_ENTITY_SLEEP_INDEFINITELY = 1,
  /** entity to sleep for a fixed duration */
  WL1_DL_DRX_ENTITY_SLEEP_FIXED_DURATION = 2,
  WL1_DL_DRX_ENTITY_NUM_SLEEP_TYPES = 3
}wl1_dl_drxctrl_entity_sleep_enum_type;

/** enum describing the request type from an entity */
typedef enum
{
  /** entity registration */
  WL1_DL_DRXCTRL_ENTITY_REGISTER,
  /** entity deregistration */
  WL1_DL_DRXCTRL_ENTITY_DEREGISTER,
  /** entity notifies ready to sleep */
  WL1_DL_DRXCTRL_ENTITY_READY_TO_SLEEP,
  /** control event */
  WL1_DL_DRXCTRL_CTRL_EVENT_OCCURENCE,
  /** completion of enabling delay */
  WL1_DL_DRXCTRL_START_DRX_SCHEDULING
}wl1_dl_drxctrl_entity_ops_request_enum_type;

/** entity state */
typedef enum
{
  /** this entity is inactive */
  WL1_DL_DRXCTRL_ENTITY_STATE_INACTIVE,
  /** this entity is awake */
  WL1_DL_DRXCTRL_ENTITY_STATE_AWAKE,
  /** this entity is waking up */
  WL1_DL_DRXCTRL_ENTITY_STATE_WAKINGUP,
  /** this entity is asleep */
  WL1_DL_DRXCTRL_ENTITY_STATE_ASLEEP,
  /** this entity is ready to sleep */
  WL1_DL_DRXCTRL_ENTITY_STATE_READY_TO_SLEEP
}wl1_dl_drxctrl_entity_state_enum_type;

typedef enum
{
  /** rf turn off val */
  WL1_DL_DRXCTRL_RF_OFF_VAL = -1,
  WL1_DL_DRXCTRL_RF_NO_PENDING_VAL = 0,
  /** rf turn on val */
  WL1_DL_DRXCTRL_RF_ON_VAL = 1,
  WL1_DL_DRXCTRL_RF_CTL_MAX_VAL
}wl1_dl_drxctrl_rf_ops_ctrl_enum_type;

/** enum describing the action type to DRX controller module */
typedef enum
{
  WL1_DL_DRXCTRL_CPHY_SETUP_REQ,
  WL1_DL_DRXCTRL_CELL_CFG_CHANGE,
  WL1_DL_DRXCTRL_CPHY_SETUP_CNF
}wl1_dl_drxctrl_setup_req_enum_type;

/** averaging length used while dynamically calculating wakeup pre margin  */
#define WL1_DL_DRXCTRL_WAKEUP_MARGIN_AVG_LEN 8

/*Indicates that no value is associated with a certain event*/
#define WL1_DL_DRXCTRL_LOG_VAL_INVALID  0

/*Bitmask for each of the logging events. Any module calls the API for notifying triggering 
of an event using this bitmask*/
#define WL1_DL_DRXCTRL_LOG_DRX_ACTIVE        0x00000001
#define WL1_DL_DRXCTRL_LOG_DRX_INACTIVE      0x00000002
#define WL1_DL_DRXCTRL_LOG_RX_ACTIVE         0x00000004
#define WL1_DL_DRXCTRL_LOG_RX_INACTIVE       0x00000008
#define WL1_DL_DRXCTRL_LOG_EDCH_TYPE         0x00000010
#define WL1_DL_DRXCTRL_LOG_EDCH_INACT_TMR    0x00000020

#define WL1_DL_DRXCTRL_LOG_SCCH_RX_PATTERN   0x00000040
#define WL1_DL_DRXCTRL_LOG_SCCH_TYPE         0x00000080
#define WL1_DL_DRXCTRL_LOG_SCCH_INACT_TMR    0x00000100

#define WL1_DL_DRXCTRL_LOG_UL_BUFFER         0x00000200
#define WL1_DL_DRXCTRL_LOG_AGCH_ACTIVE       0x00000400
#define WL1_DL_DRXCTRL_LOG_AGCH_INACTIVE     0x00000800
#define WL1_DL_DRXCTRL_LOG_RGCH_SRV_ACTIVE   0x00001000
#define WL1_DL_DRXCTRL_LOG_RGCH_SRV_INACTIVE 0x00002000
#define WL1_DL_DRXCTRL_LOG_RGCH_RLS_ACTIVE   0x00004000
#define WL1_DL_DRXCTRL_LOG_RGCH_RLS_INACTIVE 0x00008000
#define WL1_DL_DRXCTRL_LOG_RGCH_AS_ACTIVE    0x00010000
#define WL1_DL_DRXCTRL_LOG_RGCH_AS_INACTIVE  0x00020000

#define WL1_DL_DRXCTRL_LOG_HS_ACTIVE         0x00040000
#define WL1_DL_DRXCTRL_LOG_FDPCH_ACTIVE      0x00080000
#define WL1_DL_DRXCTRL_LOG_DL_STATE_ACTIVE   0x00100000
#define WL1_DL_DRXCTRL_LOG_SRCH_START        0x00200000
#define WL1_DL_DRXCTRL_LOG_SRCH_END          0x00400000
#define WL1_DL_DRXCTRL_LOG_CM_ACTIVE         0x00800000

#define WL1_DL_DRXCTRL_LOG_SMALL_GAP         0x01000000
#define WL1_DL_DRXCTRL_LOG_DEBUG             0x02000000

#define WL1_DL_DRXCTRL_LOG_RX_WARMUP         0x04000000
#define WL1_DL_DRXCTRL_LOG_HS_SCCH_PATTERN_IND   0x08000000
#define WL1_DL_DRXCTRL_LOG_DRX_STATE_MACHINE_CONFIGURED_STATE   0x10000000
#define WL1_DL_DRXCTRL_LOG_RX_BLANKING       0x20000000

#define WL1_DL_DRXCTRL_SCPICH_ON_IND_MASK    0x0001
#define WL1_DL_DRXCTRL_SCPICH_OFF_IND_MASK   0x0010
#define WL1_DL_DRXCTRL_SKIP_CPC_DRX_CONFIG_ON_MASK      0x0002
#define WL1_DL_DRXCTRL_SKIP_CPC_DRX_CONFIG_OFF_MASK     0x0020
#define WL1_DL_DRXCTRL_DC_ON_IND_MASK        0x0004
#define WL1_DL_DRXCTRL_DC_OFF_IND_MASK       0x0040
#define WL1_DL_DRXCTRL_DRX_ON_IND_MASK       0x1000

#define WL1_DL_DRXCTRL_GET_SUSPEND_MASK(suspend_mask)  \
  ((suspend_mask) >> (4))


typedef tlm_cfn_info_in_cfn_tick wl1_dl_drxctrl_time_var_struct_type;

/** wakeup/sleep align info */
typedef struct
{
  /** flag to indicate whether align info should be used */
  boolean use_align_info;
  /** align entity type */
  uint8 align_entity_id;
  /** align offset */
  wl1_dl_drxctrl_time_var_struct_type align_delta_offset;
}wl1_dl_drxctrl_align_info_struct_type;

/** entity wakeup struct */
typedef struct
{
  /** wakeup type for entity */
  wl1_dl_drxctrl_entity_wakeup_enum_type wakeup_type;
  /** align info */
  wl1_dl_drxctrl_align_info_struct_type align_info;
  /** timing info for sleep */
  wl1_dl_drxctrl_time_var_struct_type sleep_time_val;
}wl1_dl_drxctrl_wakeup_struct_type;

/** entity sleep  struct   */
typedef struct
{
  /** sleep type for entity */
  wl1_dl_drxctrl_entity_sleep_enum_type sleep_type;
  /** align info */
  wl1_dl_drxctrl_align_info_struct_type align_info;
  /** timing info for sleep */
  wl1_dl_drxctrl_time_var_struct_type wakeup_time_val;
}wl1_dl_drxctrl_sleep_struct_type;

/** entity information for external interfaces */
typedef struct
{
  /** wakeup handler for entity */
  WL1_DL_DRXCTRL_ENTITY_OPS_HANDLER drx_entity_wakeup_handler;
  /** sleep handler for entity */
  WL1_DL_DRXCTRL_ENTITY_OPS_HANDLER drx_entity_gotosleep_handler;
}wl1_dl_drxctrl_entity_intf_info_struct_type;

/** entity information to let state machine know of a new
 *  entity registration/deregistration */
typedef struct
{
  uint8 entity_id;
}wl1_dl_drxctrl_entity_registration_notify_struct_type;

typedef wl1_dl_drxctrl_entity_registration_notify_struct_type wl1_dl_drxctrl_entity_wakeup_notify_struct_type;
typedef wl1_dl_drxctrl_entity_registration_notify_struct_type wl1_dl_drxctrl_entity_sleep_notify_struct_type;

/** consolidated entity */
typedef struct
{
  /** the entity entry is in use */
  boolean in_use;
  /** updated flag */
  boolean updated_control_info;
  /** wakeup time */
  wl1_dl_drxctrl_wakeup_struct_type wakeup_time;
  /** sleep time for entity */
  wl1_dl_drxctrl_sleep_struct_type sleep_time;
  /** entity info */
  wl1_dl_drxctrl_entity_intf_info_struct_type entity_info;
}wl1_dl_drxctrl_entity_struct_type;

/** control event info */
typedef struct
{
  /** boolean active */
  boolean active_indicator;
  /** curr time */
  wl1_dl_drxctrl_time_var_struct_type ctrl_event_time;
}wl1_dl_drxctrl_ctrl_event_struct_type;

/** consolidated entity db */
typedef struct
{
  /** mask containing active entities*/
  uint8 active_entity_mask;
  /** entities databas */
  wl1_dl_drxctrl_entity_struct_type entities[WL1_DL_DRXCTRL_MAX_NUM_ENTITIES];
}wl1_dl_drxctrl_entity_db_struct_type;

/** event programming struct */
typedef struct
{
  /** all timing information */
  wl1_dl_drxctrl_time_var_struct_type ctrl_event_ops;
  /** programmed wakeup time */
  wl1_dl_drxctrl_time_var_struct_type sched_ctrl_event;
  /** seq notify cfn update input */
  l1_cfn_update_notify_struct_type cfn_notify_input_cfg;
}wl1_dl_drxctrl_event_prog_input_struct_type;

typedef struct
{
  /** no of cfns before dtx is enabled */
  uint8 enabling_delay;
  /** no of subframes following completion of enabling delay
   *  after which drx can start */
  uint8 dtx_drx_offset;
}wl1_dl_drxctrl_start_info_struct_type;

/** dl drx control database */
typedef struct
{
  /** all scheduling is frozen */
  boolean scheduling_needed;
  /** logging enabled */
  boolean logging_enabled;
  /** enabling delay/dtx drx offset done control var */
  //boolean dtx_drx_offset_passed;
  /** ctrl event id */
  uint8 ctrl_event_id;
  /** Keeps track of the entities stopping DRX (like MIMO) */
  uint8 curr_suspend_cfg_status;
  /** state to resume after suspended */
  wl1_dl_drxctrl_state_enum_type drxctrl_after_suspend;
  /** Keeps track of the FW state */
  wl1_dl_drxctrl_drx_fw_state_enum_type drx_fw_state;
  /** current state of the dl drx control state machine   */
  wl1_dl_drxctrl_state_enum_type drxctrl_curr_state;
  /** drx control timing struct */
  wl1_dl_drxctrl_start_info_struct_type start_time_info;
  /** entity db */
  wl1_dl_drxctrl_entity_db_struct_type entity_db;
  /** entity to be woken up */
  wl1_dl_drxctrl_entity_struct_type *configured_wakeup_entities;
  /** entity that has sleep configured */
  wl1_dl_drxctrl_entity_struct_type *configured_sleep_entity;
  /** current SCCH order state
  Current state is set to FALSE when CPC-DRX Scheduling
  is active and TRUE when it is in inactive state*/
  boolean current_scch_order;
  /** Bitmask to control RF state */
  uint32 rf_state_ctrl;
}wl1_dl_drxctrl_db_struct_type;


typedef union
{
  /** entity registration info   */
  wl1_dl_drxctrl_entity_registration_notify_struct_type registration_info;
  /** wakeup info */
  wl1_dl_drxctrl_wakeup_struct_type wakeup_info;
  /** sleep info */
  wl1_dl_drxctrl_sleep_struct_type sleep_info;
  /** control event struct type */
  wl1_dl_drxctrl_ctrl_event_struct_type ctrl_event_info;
}wl1_dl_drxctrl_entity_request_info_union_type;


typedef struct
{
  /** entity id */
  uint8 entity_id;
  /** entity operation request type   */
  wl1_dl_drxctrl_entity_ops_request_enum_type req_type;
  /** input for the above operation */
  wl1_dl_drxctrl_entity_request_info_union_type req_info;
}wl1_dl_drxctrl_entity_ops_request_struct_type;


typedef struct
{
  /** cpc drx event pending */
  uint8 *cpc_drx_event_pending;
  /** tick at which the above event goes active */
  int32 *cpc_drx_event_tick;
  /** current state of rf */
  int32 *cpc_drx_curr_rf_state;
  /** ssbi transaction register to turn on rf */
  int32 *cpc_drx_rx_pll_on_reg;
  /** ssbi transaction to turn off rf  */
  int32 *cpc_drx_rx_pll_off_reg;
}wl1_dl_drxctrl_fw_async_var_struct_type;

#ifdef FEATURE_WCDMA_HS_FACH_DRX
typedef struct
{
  /** EDRX Rx event pending */
  int16 *e_drx_rx_event_pending;
  /** subframe number at which the above event goes active */
  int16 *e_drx_rx_event_start;
  /** EDRX HS event pending */
  int16 *e_drx_hs_event_pending;
  /** subframe number at which the above event goes active */
  int16 *e_drx_hs_event_start;
}wl1_dl_drxctrl_edrx_fw_async_var_struct_type;

extern wl1_dl_drxctrl_edrx_fw_async_var_struct_type               wl1_dl_drxctrl_edrx_fw_async_buf_ptr;

typedef struct
{
  uint8 drxctrl_entity_id;
  boolean sleep_aborted;
  drx_entity_handle_type drx_handle;
  wl1_dl_drxctrl_time_var_struct_type tcxo_wkup_time;
  wl1_dl_drxctrl_time_var_struct_type drxctrl_wkup_time;
}wl1_dl_drxctrl_tcxo_wkup_struct_type;

extern wl1_dl_drxctrl_tcxo_wkup_struct_type wl1_dl_drxctrl_tcxo_wkup_info;
#endif /* FEATURE_WCDMA_HS_FACH_DRX */

typedef PACKED struct PACKED_POST
{
  /** contains rx state, edch state, edch inactivity */
  uint16 word1;
  /** contains hs drx info */
  uint16 word2;
  /** contains detailed eul info */
  uint16 word3;
  /** contains hs, f-dpch, r99dl, srch info */
  uint16 word4;
  /** contains cm present indicator, gap override */
  uint16 word5;
  /** contains processor config */
  uint16 word6;
}wl1_dl_drxctrl_log_info_struct_type;

LOG_RECORD_DEFINE(WL1_DL_DRXCTRL_LOG_PKT)
/** version num of the log packet */
uint8 version;
/** number of samples in the logpacket */
uint8 num_samples;
/**start cfn drx   */
uint16 start_cfn_drx;
/** drx log info */
wl1_dl_drxctrl_log_info_struct_type info[WL1_DL_DRXCTRL_MAX_LOG_BUF_IDX];
LOG_RECORD_END

LOG_RECORD_DEFINE(WL1_DL_DRXCTRL_CFG_LOG_PKT)
/** version number of the log packet */
uint8 version;
/** input CPC DRX request mask */
uint8 input_cpc_drx_req_mask;
/** HS SCCH Reception pattern length */
uint8 hs_scch_rec_patt_length;
/** Flag for whether grant monitoring is enabled/disabled */
boolean grant_monitoring_en;
/** HS SCCH inactivity threshold - max value 512*/
uint16 hs_scch_inact_thresh;
/** Grant monitor inactivity threshold - max value 512 */
uint16 grant_monitor_inact_thresh;
/** Enabling delay - max value 128 */
uint16 enabling_delay;
/** DTX DRX offset - max value 159 */
uint16 dtx_drx_offset;
/** CFN DRX offset */
uint16 cfn_drx_offset;
/** tau_dpch - max value 159 */
uint16 tau_dpch;
/** Processor speed values, uint32 array of 8 elements */
uint32 proc_speed_values[WL1_DL_DRXCTRL_MAX_NUM_SPEED_VALUES];
/** Bus speed values, uint32 array of 8 elements */
uint32 bus_speed_values[WL1_DL_DRXCTRL_MAX_NUM_SPEED_VALUES];
/** MDSP speed values, uint32 array of 8 elements */
uint32 mdsp_speed_values[WL1_DL_DRXCTRL_MAX_NUM_SPEED_VALUES];
LOG_RECORD_END

/** Value associated with certain logging events**/
typedef struct
{
  uint8 rx_active;
/** EDCH type **/
  uint8 edch_type;
/** current EDCH inactivity timer value */
  uint16 edch_inact_tmr;
/** SCCH type   */
  uint8 scch_type;
  uint8 scch_pattern_ind;
/** current SCCH inactivity timer value */
  uint16 scch_inact_tmr;
  uint8 agch_mask;
  uint8 rgch_srv_mask;
  uint8 rgch_rls_mask;
  uint8 rgch_as_mask;
  uint8 srch_step;
  uint8 hs_active;
  uint8 fdpch_active;
  uint8 cm_active;
  uint8 small_gap;
  uint8 debug_ind;
  uint8 rx_warmup_ind;
  uint8 drx_state_machine_in_configured_state;
  uint8 rx_blanking;
}wl1_dl_drxctrl_logging_input_val_struct_type;

/** Logging info for each subframe**/
typedef struct
{
/** Bitmask containing info of all events which got triggered
    in a particular subframe**/
  uint32 event_bitmask;
/** Value associated with certain events which got triggered in 
    a particular subframe**/
  wl1_dl_drxctrl_logging_input_val_struct_type input_val;
}wl1_dl_drxctrl_logging_event_info_struct_type;

/**Raw logging structure**/
typedef struct
{
  /** Active buffer index   */
  uint8 active_buffer_idx;
  /** buf idx waiting to be flushed */
  uint8 buf_idx_waiting_to_be_flushed;
  /** start cfn stored here before logging is flushed */
  uint8 start_cfn_to_be_flushed;
  /** start subframe stored here before logging is flushed */
  uint8 start_subfr_to_be_flushed;
  /** The current count of the CFN within a raw log buffer of N
     CFNs, N being the max size of the buffer in CFNs **/
  uint8 cfn_count;
  /** start cfn for idx 0*/
  uint8 start_cfn;
  /** start subfr for idx 0   */
  uint8 start_subfr;
  /** drx logging cmd_pending**/
  boolean drx_logging_cmd_pending[2];
  /** Buffer info of all events which got triggered in N CFNs.* */
  wl1_dl_drxctrl_logging_event_info_struct_type wl1_dl_drxctrl_raw_log_arr[2][WL1_DL_DRXCTRL_MAX_LOG_BUF_IDX];   
}wl1_dl_drxctrl_raw_logging_struct_type;   

/** Structure for storing the previous wakeup margins */
typedef struct
{
  /** Current buffer index   */
  uint8 curr_buffer_idx;
  /** Index of the current element in the buffer  */
  uint8 curr_data_idx;
  /** Buffer storing the previous wakeup margins  */
  uint8 data[2][WL1_DL_DRXCTRL_WAKEUP_MARGIN_AVG_LEN];
}wl1_dl_drxctrl_previous_wakeup_margin_struct_type;

/** Structure maintaining info of the previous pre wakeup margins
 and average pre wakeup margin */
typedef struct
{
  /** Average wakeup margin   */
  uint8 avg_margin;
  /** First wakeup margin   */
  uint8 first_margin;
  /** Total wakeup margin   */
  uint8 total_margin;
  /** Structure for storing the previous wakeup margins */
  wl1_dl_drxctrl_previous_wakeup_margin_struct_type previous_margin;
}wl1_dl_drxctrl_wakeup_margin_info_struct_type;

/** Structure maintaining the type of the command to the DRX controller */
typedef union
{
  /** setup cmd */
  l1_setup_cmd_type *setup_cmd;
  /** sttd state toggled */
  uint16 cell_cfg_change_mask;
}wl1_dl_drxctrl_setup_param_struct_type;

/** Structure maintaining parameters to the DRX controller module */
typedef struct
{
  /** setup type */
  wl1_dl_drxctrl_setup_req_enum_type drx_setup_type;
  wl1_dl_drxctrl_setup_param_struct_type setup_params;
  boolean done_rsp_needed;
}wl1_dl_drxctrl_setup_struct_type;

/** enum describing different clients who vote for RF state */
typedef enum
{
  /** Client for TxD Detection */
  WL1_DL_DRXCTRL_RF_CTRL_FOR_TXD_DET,
  /** Client for Serving SIB Read */
  WL1_DL_DRXCTRL_RF_CTRL_FOR_S_SIB_READ,
  #ifdef FEATURE_WCDMA_ANT_SWITCH_SUPPORT
  /* Client for Antenna Switching */
  WL1_DL_DRXCTRL_RF_CTRL_FOR_ANT_SWITCH,
  #endif
    /** Client for Neighbor SIB Read or SFN_WRC decode */
  WL1_DL_DRXCTRL_RF_CTRL_FOR_N_SIB_READ,
  /** Client for Sec carrier Neighbor SIB Read or SFN_WRC decode */
  WL1_DL_DRXCTRL_RF_CTRL_FOR_N_SIB_READ_SEC,
  /** Client for RxD */
  WL1_DL_DRXCTRL_RF_CTRL_FOR_RXD,
  /* Client for QTA */
  WL1_DL_DRXCTRL_RF_CTRL_FOR_QTA,
  /* Client for EDRX HS channels */
  WL1_DL_DRXCTRL_RF_CTRL_FOR_EDRX_HS,
  /* Client for LTE cleanup after W2L CM */
  WL1_DL_DRXCTRL_RF_W2L_CM_CLEANUP
}wl1_dl_drxctrl_rf_ctrl_client_enum_type;


extern wl1_dl_drxctrl_raw_logging_struct_type wl1_dl_drxctrl_raw_log;

/*Timing info needed to start DTX/DRX at L1 for CPC*/
extern l1_dtx_drx_timing_info_struct_type wl1_dl_drxctrl_saved_dtx_drx_timing_info;

/*All the info needed for DTX operation at L1 for CPC*/
extern l1_dtx_info_struct_type wl1_dl_drxctrl_saved_dtx_info;

/*All the info needed for DRX operation at L1 for CPC*/
extern l1_drx_info_struct_type wl1_dl_drxctrl_saved_drx_info;

/* pointer to a DRX act/deact callback function registered by 
   the CME Module */
wl1_dl_drxctrl_cme_cb_func_type *wl1_dl_drxctrl_cme_act_deact_cb_func;

extern boolean wl1dldrxctrl_handle_sleep_abort;
/* MACROs to access elements of various data structures*/
#define WL1_DL_DRXCTRL_GET_DRX_STATE() \
        wl1_dl_drxctrl_db.drxctrl_curr_state

#define WL1_DL_DRXCTRL_SET_DRX_STATE(curr_state) \
        MSG_HIGH("WL1DLDRXCTRL: Setting state %d curr state %d", curr_state, wl1_dl_drxctrl_db.drxctrl_curr_state, 0); \
        wl1_dl_drxctrl_db.drxctrl_curr_state = curr_state

#define WL1_DL_DRXCTRL_FREEZE_SCHEDULING_NOW() \
        MSG_HIGH("WL1DLDRXCTRL: Freezing scheduling", 0, 0, 0); \
        wl1_dl_drxctrl_db.scheduling_needed = FALSE

#define WL1_DL_DRXCTRL_UNFREEZE_SCHEDULING_NOW() \
        MSG_HIGH("WL1DLDRXCTRL: Unfreezing scheduling", 0, 0, 0); \
        wl1_dl_drxctrl_db.scheduling_needed = TRUE

#define WL1_DL_DRXCTRL_IS_SCHEDULING_NEEDED() \
        wl1_dl_drxctrl_db.scheduling_needed

#define WL1_DL_DRXCTRL_FREEZE_LOGGING_NOW() \
        MSG_HIGH("WL1DLDRXCTRL: Freezing logging", 0, 0, 0); \
        wl1_dl_drxctrl_db.logging_enabled = FALSE

#define WL1_DL_DRXCTRL_UNFREEZE_LOGGING_NOW() \
        MSG_HIGH("WL1DLDRXCTRL: Unfreezing logging", 0, 0, 0); \
        wl1_dl_drxctrl_db.logging_enabled = TRUE

#define WL1_DL_DRXCTRL_IS_LOGGING_ENABLED() \
        wl1_dl_drxctrl_db.logging_enabled


#define WL1_DL_DRXCTRL_IS_ENTITY_ID_VALID(entity_id) \
        (entity_id < WL1_DL_DRXCTRL_MAX_NUM_ENTITIES)

#define WL1_DL_DRXCTRL_GET_ACTIVE_ENTITY_MASK() \
        wl1_dl_drxctrl_db.entity_db.active_entity_mask

#define WL1_DL_DRXCTRL_SET_ENTITY_ACTIVE(entity_id) \
        MSG_HIGH("WL1DLDRXCTRL: Setting entity %d active curr_active_entities 0x%x", entity_id, wl1_dl_drxctrl_db.entity_db.active_entity_mask, 0); \
        wl1_dl_drxctrl_db.entity_db.entities[entity_id].in_use = TRUE; \
        wl1_dl_drxctrl_db.entity_db.active_entity_mask |= (1 << entity_id)

#define WL1_DL_DRXCTRL_SET_ENTITY_FREE(entity_id) \
        MSG_HIGH("WL1DLDRXCTRL: Freeing entity %d active curr_active_entities 0x%x", entity_id, wl1_dl_drxctrl_db.entity_db.active_entity_mask, 0); \
        wl1_dl_drxctrl_db.entity_db.entities[entity_id].in_use = FALSE; \
        wl1_dl_drxctrl_db.entity_db.active_entity_mask &= ~(1 << entity_id);

#define WL1_DL_DRXCTRL_INVOKE_ENTITY_WAKEUP_HANDLER(entity_id, tick_cnt) \
        if(wl1_dl_drxctrl_db.entity_db.entities[entity_id].entity_info.drx_entity_wakeup_handler)           \
        {                                                                                                   \
          MSG_4(MSG_SSID_DFLT, MSG_LEGACY_HIGH, "WL1DLDRXCTRL: Waking up entity %d handler 0x%x wakeup cfn %d tick %d",                                        \
                entity_id, wl1_dl_drxctrl_db.entity_db.entities[entity_id].entity_info.drx_entity_wakeup_handler, \
                wl1_dl_drxctrl_db.entity_db.entities[entity_id].sleep_time.wakeup_time_val.occasion_cfn, tick_cnt);\
          wl1_dl_drxctrl_db.entity_db.entities[entity_id].entity_info.drx_entity_wakeup_handler();          \
        }

#define WL1_DL_DRXCTRL_INVOKE_ENTITY_SLEEP_HANDLER(entity_id) \
        if(wl1_dl_drxctrl_db.entity_db.entities[entity_id].entity_info.drx_entity_gotosleep_handler)           \
        {                                                                                                      \
          MSG_MED("WL1DLDRXCTRL: Sending entity %d to sleep handler 0x%x",                                    \
                   entity_id, wl1_dl_drxctrl_db.entity_db.entities[entity_id].entity_info.drx_entity_gotosleep_handler, 0);\
          wl1_dl_drxctrl_db.entity_db.entities[entity_id].entity_info.drx_entity_gotosleep_handler();          \
        }

#define WL1_DL_DRXCTRL_GET_LOG_SUBFN_COUNT(subfr_num, offset, cfn_count) (subfr_num + (5 * cfn_count) + offset)

#define GET_SLOT_IDX_WITHIN_SUBFN(tick) (math_floor((tick - math_floor(tick,30)*30),10))

#define WL1_DL_DRXCTRL_CONV_DRX_HLOPS_TYPE_TO_MDSP_OPS_TYPE(ops_type) \
        (ops_type & 0x3)
#define WL1_DL_DRXCTRL_WAKEUP_PRE_MARGIN 15
/** rf warmup time */
#define WL1_DL_DRXCTRL_RF_WARMUP_TIME 20
/** total time needed if rf needs to be toggled */
#define WL1_DL_DRXCTRL_GET_WAKEUP_PRE_MARGIN() \
        WL1_DL_DRXCTRL_WAKEUP_PRE_MARGIN

#define WL1_DL_DRXCTRL_GET_WAKEUP_MARGIN() \
        (WL1_DL_DRXCTRL_WAKEUP_PRE_MARGIN + WL1_DL_DRXCTRL_RF_WARMUP_TIME)
#ifdef __GNUC__
#define WL1_DL_DRXCTRL_UPDATE(idx,word,fieldname,val) \
        wl1_dl_drxctrl_diag_log_pkt.info[idx].word |= (val << WL1_DL_DRXCTRL_##fieldname##_SHFT) & WL1_DL_DRXCTRL_##fieldname##_BMSK
#else
#define WL1_DL_DRXCTRL_UPDATE(idx,word,fieldname,val) \
        wl1_dl_drxctrl_diag_log_pkt.info[idx].##word |= (val << WL1_DL_DRXCTRL_##fieldname##_SHFT) & WL1_DL_DRXCTRL_##fieldname##_BMSK
#endif
#define WL1_DL_DRXCTRL_GET_CURR_SUBFM() \
        (stmr_get_current_time(MCALWCDMA_EVT_RX_TL_INTR)/HS_SUB_FRAME_LEN_UNIT_256_CHIP)

#define WL1_DL_DRXCTRL_IS_CPC_DRX_ACTIVE() \
        (wl1_dl_drxctrl_get_drx_state() != WL1_DL_DRX_MODE_INACTIVE)

/** Bit mask for each parameter in the log packet*  */
#define WL1_DL_DRXCTRL_EDCH_INACT_TMR_BMSK         0x7FC0
#define WL1_DL_DRXCTRL_EDCH_INACT_TMR_SHFT         6
#define WL1_DL_DRXCTRL_EDCH_TYPE_BMSK              0x0030
#define WL1_DL_DRXCTRL_EDCH_TYPE_SHFT              4
#define WL1_DL_DRXCTRL_RX_ACTIVE_BMSK              0x000E
#define WL1_DL_DRXCTRL_RX_ACTIVE_SHFT              1
#define WL1_DL_DRXCTRL_DRX_ACTIVE_BMSK             0x0001
#define WL1_DL_DRXCTRL_DRX_ACTIVE_SHFT             0

#define WL1_DL_DRXCTRL_SCCH_INACT_TMR_BMSK         0x1FF8
#define WL1_DL_DRXCTRL_SCCH_INACT_TMR_SHFT         3
#define WL1_DL_DRXCTRL_SCCH_TYPE_BMSK              0x0006
#define WL1_DL_DRXCTRL_SCCH_TYPE_SHFT              1
#define WL1_DL_DRXCTRL_SCCH_RECEP_PATTERN_IND_BMSK 0x0001
#define WL1_DL_DRXCTRL_SCCH_RECEP_PATTERN_IND_SHFT 0

#define WL1_DL_DRXCTRL_RGCH_AS_ACTIVE_BMSK         0x1C00
#define WL1_DL_DRXCTRL_RGCH_AS_ACTIVE_SHFT         10
#define WL1_DL_DRXCTRL_RGCH_HICH_RLS_ACTIVE_BMSK   0x0380
#define WL1_DL_DRXCTRL_RGCH_HICH_RLS_ACTIVE_SHFT   7
#define WL1_DL_DRXCTRL_RGCH_HICH_SERV_BMSK         0x0070
#define WL1_DL_DRXCTRL_RGCH_HICH_SERV_SHFT         4
#define WL1_DL_DRXCTRL_AGCH_SERVING_BMSK           0x000E
#define WL1_DL_DRXCTRL_AGCH_SERVING_SHFT           1
#define WL1_DL_DRXCTRL_UL_BUFFER_BMSK              0x0001
#define WL1_DL_DRXCTRL_UL_BUFFER_SHFT              0

#define WL1_DL_DRXCTRL_SRCHR_ACTIVE_BMSK           0x0E00
#define WL1_DL_DRXCTRL_SRCHR_ACTIVE_SHFT           9
#define WL1_DL_DRXCTRL_DL_STATE_ACTIVE_BMSK        0x01C0
#define WL1_DL_DRXCTRL_DL_STATE_ACTIVE_SHFT        6
#define WL1_DL_DRXCTRL_FDPCH_ACTIVE_BMSK           0x0038
#define WL1_DL_DRXCTRL_FDPCH_ACTIVE_SHFT           3
#define WL1_DL_DRXCTRL_HSDPA_ACTIVE_BMSK           0x0007
#define WL1_DL_DRXCTRL_HSDPA_ACTIVE_SHFT           0

#define WL1_DL_DRXCTRL_RX_WARMUP_IND_BMSK          0x0E00
#define WL1_DL_DRXCTRL_RX_WARMUP_IND_SHFT          9
#define WL1_DL_DRXCTRL_DEBUG_IND_BMSK              0x01C0
#define WL1_DL_DRXCTRL_DEBUG_IND_SHFT              6
#define WL1_DL_DRXCTRL_SMALLGAP_IND_BMSK           0x0038
#define WL1_DL_DRXCTRL_SMALLGAP_IND_SHFT           3
#define WL1_DL_DRXCTRL_CM_IND_BMSK                 0x0007
#define WL1_DL_DRXCTRL_CM_IND_SHFT                 0

#define WL1_DL_DRXCTRL_MDSP_SPEED_INDEX_BMSK       0x01C0
#define WL1_DL_DRXCTRL_MDSP_SPEED_INDEX_SHFT       6
#define WL1_DL_DRXCTRL_MODEM_BUS_SPEED_INDEX_BMSK  0x0038
#define WL1_DL_DRXCTRL_MODEM_BUS_SPEED_INDEX_SHFT  3
#define WL1_DL_DRXCTRL_RX_BLANKING_BMSK            0x0007
#define WL1_DL_DRXCTRL_RX_BLANKING_SHFT            0

#ifdef FEATURE_WCDMA_HS_FACH_DRX
#define WL1_DL_DRXCTRL_EDRX_FULL_SLEEP_WAKEUP_TICK 10
#endif /* FEATURE_WCDMA_HS_FACH_DRX */

/*===========================================================================

FUNCTION        wl1_dl_drxctrl_init

DESCRIPTION     This function initialises the the drx ctrlr databases

DEPENDENCIES    None

RETURN VALUE    None

SIDE EFFECTS    None

===========================================================================*/
extern void wl1_dl_drxctrl_init(void);

/*===========================================================================

FUNCTION        wl1_dl_drxctrl_register_entity

DESCRIPTION     This function initialises the carrcfg info databases and also 
                resets the state

DEPENDENCIES    None

RETURN VALUE    None

SIDE EFFECTS    None

===========================================================================*/
extern uint8 wl1_dl_drxctrl_register_entity(
  /** input entity info */
  wl1_dl_drxctrl_entity_intf_info_struct_type *input_entity_info);

/*===========================================================================

FUNCTION        wl1_dl_drxctrl_deregister_entity

DESCRIPTION     This function initialises the carrcfg info databases and also 
                resets the state

DEPENDENCIES    None

RETURN VALUE    None

SIDE EFFECTS    None

===========================================================================*/
extern boolean wl1_dl_drxctrl_deregister_entity(
  /** entity id */
  uint8 entity_id);

/*===========================================================================

FUNCTION        wl1_dl_drxctrl_entity_schedule_update

DESCRIPTION     This function initialises the carrcfg info databases and also 
                resets the state

DEPENDENCIES    None

RETURN VALUE    None

SIDE EFFECTS    None

===========================================================================*/
extern void wl1_dl_drxctrl_entity_schedule_update(
  /** control input from entities */
  wl1_dl_drxctrl_entity_ops_request_struct_type *input_control_info);

/*===========================================================================

FUNCTION        wl1_dl_drxctrl_entity_schedule_update_command

DESCRIPTION     This function is a dummy handler to call 
                wl1_dl_drxctrl_entity_schedule_update from task context

DEPENDENCIES    None

RETURN VALUE    None

SIDE EFFECTS    None

===========================================================================*/
extern void wl1_dl_drxctrl_entity_schedule_update_command(
  uint8 entity_id,
  uint8 req_type,
  uint8 sleep_type,
  uint8 occasion_cfn,
  uint8 tick_cnt
);

/*===========================================================================

FUNCTION        wl1_dl_drxctrl_validate_drx_params

DESCRIPTION     This function validates drx ops type. on validation of the drx   
                ops it calls other drx submodules for specific validation 

DEPENDENCIES    None

RETURN VALUE    None

SIDE EFFECTS    None

===========================================================================*/
extern boolean wl1_dl_drxctrl_validate_drx_params(
  /** ops type */
  uint8 ops_type, 
  /** input drx info */
  l1_drx_info_struct_type *inp_drx_info);

/*===========================================================================

FUNCTION        wl1_dl_drxctrl_configure_drx_mode

DESCRIPTION     This function initialises the carrcfg info databases and also 
                resets the state

DEPENDENCIES    None

RETURN VALUE    None

SIDE EFFECTS    None

===========================================================================*/
extern void wl1_dl_drxctrl_configure_drx_mode(
  /** DRX controller setup pakcet */
  wl1_dl_drxctrl_setup_struct_type* setup_packet);

/*===========================================================================

FUNCTION        wl1_dl_drxctrl_evt_handler

DESCRIPTION     This function initialises the carrcfg info databases and also 
                resets the state

DEPENDENCIES    None

RETURN VALUE    None

SIDE EFFECTS    None

===========================================================================*/
extern void wl1_dl_drxctrl_evt_handler(
  /** generic event type */
  seq_generic_event_enum_type cpc_gen_evt_type);

/*===========================================================================

FUNCTION        wl1_dl_drxctrl_notify_cpc_status_to_clients

DESCRIPTION     This function notifies the clients registered to dldrxctrl about 
                start/stop drx operation

DEPENDENCIES    None

RETURN VALUE    None

SIDE EFFECTS    None

===========================================================================*/
extern void wl1_dl_drxctrl_notify_cpc_status_to_clients(boolean status);

/*===========================================================================

FUNCTION        wl1_dl_drxctrl_start_drx

DESCRIPTION     This function starts drx mode of operation

DEPENDENCIES    None

RETURN VALUE    None

SIDE EFFECTS    None

===========================================================================*/
extern void wl1_dl_drxctrl_start_drx(void);

/*===========================================================================

FUNCTION        wl1_dl_drxctrl_unfreeze_scheduling

DESCRIPTION     This function initialises the carrcfg info databases and also 
                resets the state

DEPENDENCIES    None

RETURN VALUE    None

SIDE EFFECTS    None

===========================================================================*/
extern void wl1_dl_drxctrl_unfreeze_scheduling(void);

/*===========================================================================

FUNCTION        wl1_dl_drxctrl_freeze_scheduling

DESCRIPTION     This function initialises the carrcfg info databases and also 
                resets the state

DEPENDENCIES    None

RETURN VALUE    FALSE if CPHY_ needs to be suspended
                TRUE otherwise

SIDE EFFECTS    None

===========================================================================*/
boolean wl1_dl_drxctrl_freeze_scheduling();

/*===========================================================================

FUNCTION        wl1_dl_drxctrl_cleanup

DESCRIPTION     This function initialises the carrcfg info databases and also 
                resets the state

DEPENDENCIES    None

RETURN VALUE    None

SIDE EFFECTS    None

===========================================================================*/
extern void wl1_dl_drxctrl_cleanup(void);

/*===========================================================================

FUNCTION        wl1_dl_drxctrl_dtx_drx_offset_complete_notify

DESCRIPTION     This function signals completion of dtx drx offset

DEPENDENCIES    None

RETURN VALUE    None

SIDE EFFECTS    None

===========================================================================*/
extern void wl1_dl_drxctrl_dtx_drx_offset_complete_notify(
  /** notify cfn */
  uint8 trigger_cfn);


/*===========================================================================

FUNCTION        wl1_dl_drxctrl_cfn_notify_func_cb

DESCRIPTION     This function initialises the carrcfg info databases and also 
                resets the state

DEPENDENCIES    None

RETURN VALUE    None

SIDE EFFECTS    None

===========================================================================*/
extern void wl1_dl_drxctrl_cfn_notify_func_cb(
  /** notify cfn */
  uint8 trigger_cfn);

/*===========================================================================

FUNCTION        wl1_dl_drxctrl_start_drx_time

DESCRIPTION     This function starts drx procedures   
                at a particular cfn and dtx offset

DEPENDENCIES    None

RETURN VALUE    None

SIDE EFFECTS    None

===========================================================================*/
extern void wl1_dl_drxctrl_start_drx_time(
  /** enabling cfn */
  uint8 enabling_cfn, 
  /** dtx<->drx offset   */
  uint8 dtx_drx_offset);

/*===========================================================================

FUNCTION        wl1_dl_drxctrl_notify_scch_order_reception

DESCRIPTION     This function handles scch orders

DEPENDENCIES    None

RETURN VALUE    None

SIDE EFFECTS    None

===========================================================================*/
extern void wl1_dl_drxctrl_notify_scch_order_reception(
  /** time elapesed since scch order was received */
  uint16 elapsed_time
);

/*===========================================================================

FUNCTION        wl1_dl_drxctrl_cpc_drx_cfg_done

DESCRIPTION     This function is interrupt handler for CPC cfg done

DEPENDENCIES    None

RETURN VALUE    None

SIDE EFFECTS    None

===========================================================================*/
extern void wl1_dl_drxctrl_cpc_drx_cfg_done(void);

/*===========================================================================

FUNCTION        wl1_dl_drxctrl_freeze_unfreeze_for_CPC_scch_order

DESCRIPTION     This function handles freezing/unfreezing DLDRXCTRL for SCCH orders

DEPENDENCIES    None

RETURN VALUE    None

SIDE EFFECTS    None

===========================================================================*/
extern void wl1_dl_drxctrl_freeze_unfreeze_for_CPC_scch_order(boolean freeze);

/*===========================================================================

FUNCTION        wl1_dl_drxctrl_log_activity_times

DESCRIPTION     This function is the API called to notify the start/end of an event. The input parameters passed are 
the subframe when the event was triggered, the event name and any parameter associated with the event. This info
is copied to a raw logging buffer.

DEPENDENCIES    None

RETURN VALUE    None

SIDE EFFECTS    None

===========================================================================*/
extern void wl1_dl_drxctrl_log_activity_times(uint8 subfm, uint32 input_key, uint16 input_val);
/*===========================================================================

FUNCTION        wl1_dl_drxctrl_post_log_cmd

DESCRIPTION     This function is called at the start of every CFN. The raw 
log is processed and commited only every N
(WL1_DL_DRXCTRL_MAX_LOG_BUF_IDX) samples, N being a multiple of 5. So every 
CFN, it checks whether the number
of samples has reached this limit. If yes, it posts the logging cmd and 
initializes the raw logging buffer. Else, it increments
the CFN count.

DEPENDENCIES    None

RETURN VALUE    None

SIDE EFFECTS    None

===========================================================================*/
extern void wl1_dl_drxctrl_process_mdsp_event_log(wl1_dldrx_process_mdsp_evt_log_type *mdsp_evt_log);

/*===========================================================================

FUNCTION        wl1_dl_drxctrl_cfg_process_log_cmd

DESCRIPTION     This function processes the CPC DRX CFG Log packet. It populates
                the fields of the log packet.

DEPENDENCIES    None
  
RETURN VALUE    None
  
SIDE EFFECTS    None

===========================================================================*/
extern void wl1_dl_drxctrl_cfg_process_log_cmd(wl1_dldrx_cfg_log_cmd_type *cpcdrx_cfg_logging);

/*===========================================================================

FUNCTION        wl1_dl_drxctrl_process_log_cmd

DESCRIPTION     This function processes the raw log buffer. It takes note of the start and end time of each event and then 
populates the log packet with the right bitmask for all subframes for which the event is active. Certain events also have a 
parameter attached. These values are also populated in the log packet with the corresponding bitmask.

DEPENDENCIES    None
  
RETURN VALUE    None
  
SIDE EFFECTS    None

===========================================================================*/
extern void wl1_dl_drxctrl_process_log_cmd(uint8 buff_idx);

/*===========================================================================

FUNCTION        wl1_dl_drxctrl_vote_rf_state

DESCRIPTION     This function is called by various modules to inform the
                          requirement of RF state

DEPENDENCIES    None

RETURN VALUE    None

SIDE EFFECTS    None

===========================================================================*/
extern void wl1_dl_drxctrl_vote_rf_state(wl1_dl_drxctrl_rf_ctrl_client_enum_type client_id, boolean rf_state);

extern uint8 wl1_dl_drxctrl_get_sleep_log_mask(uint8 tick);

extern uint8 wl1_dl_drxctrl_get_wakeup_log_mask(uint8 tick);

/*===========================================================================

FUNCTION        wl1_dl_drxctrl_is_drx_active

DESCRIPTION     This function is called by dl cell config module to check if
                DRX is in Active state or not

DEPENDENCIES    None

RETURN VALUE    TRUE if DRX is active, FALSE otherwise

SIDE EFFECTS    None

===========================================================================*/
extern boolean wl1_dl_drxctrl_is_cpc_drx_active(void);

/*===========================================================================

FUNCTION     WL1_DL_DRXCTRL_CONTROL_SCHEDULING_CONCURRENCY

DESCRIPTION
  This function is used to control the DRX scheduler handling
  grab_mutex = TRUE -> Grab the mutex
  grab_mutex = FALSE -> Release the mutex

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
extern void wl1_dl_drxctrl_control_scheduling_concurrency(boolean grab_mutex);

/*===========================================================================

FUNCTION     WL1_DL_DRXCTRL_CONFIGURE_DRX_PRE_SETUP

DESCRIPTION
  This function performs the pre-configuration of CPC-DRX before L1M tries
    to configure based on the cphy_setup contents.
  Basically, it does the job of stopping CPC-DRX before HS reconfig or stop

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
extern void wl1_dl_drxctrl_configure_drx_pre_setup(hsdpa_setup_ops_enum_type hs_ops);

/*===========================================================================

FUNCTION     WL1_DL_DRXCTRL_REGISTER_ACT_DEACT_ARD_CB_FUNC

DESCRIPTION
  This function is used by RxD to register the callback that needs to be 
  called when DRX is being enabled/disabled

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
extern void wl1_dl_drxctrl_register_act_deact_ard_cb_func(wl1_dl_drxctrl_ard_cb_func_type *ard_cb);

/*===========================================================================

FUNCTION     WL1_DL_DRXCTRL_REGISTER_ACT_DEACT_CME_CB_FUNC

DESCRIPTION
  This function is used by CME to register the callback that needs to be 
  called when DRX is being enabled/disabled

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
extern void wl1_dl_drxctrl_register_act_deact_cme_cb_func(wl1_dl_drxctrl_cme_cb_func_type *cme_cb);

#ifdef FEATURE_WCDMA_HS_FACH_DRX
/*===========================================================================

FUNCTION        wl1_dl_drxctrl_configure_edrx_mode

DESCRIPTION     This function triggers the configuration of EDRX mode

DEPENDENCIES    None

RETURN VALUE    None

SIDE EFFECTS    None

===========================================================================*/
void wl1_dl_drxctrl_configure_edrx_mode(
  /** l1 setup cmd ptr */
  l1_setup_cmd_type *setup_cmd);

/*===========================================================================

FUNCTION        wl1_dl_drxctrl_delta_time_from_ref_time

DESCRIPTION     This function calculates the difference between a reference time   
                and an event time 

DEPENDENCIES    None

RETURN VALUE    None

SIDE EFFECTS    None

===========================================================================*/
extern int32 wl1_dl_drxctrl_delta_time_from_ref_time(
  /** reference time */
  wl1_dl_drxctrl_time_var_struct_type *ref_time,
  /** event time */
  wl1_dl_drxctrl_time_var_struct_type *evt_time);

/*===========================================================================

FUNCTION        wl1_dl_drxctrl_start_drx

DESCRIPTION     This function starts drx mode of operation

DEPENDENCIES    None

RETURN VALUE    None

SIDE EFFECTS    None

===========================================================================*/
extern void wl1_dl_drxctrl_start_edrx(void);
/*===========================================================================

FUNCTION        wl1_dl_drxctrl_get_active_entity_mask

DESCRIPTION     This function return the active entity bmask

DEPENDENCIES    None

RETURN VALUE    uint8 -bit mask of the active entities.

SIDE EFFECTS    None

===========================================================================*/
extern uint8 wl1_dl_drxctrl_get_active_entity_mask(void);

/*===========================================================================

FUNCTION        wl1_dl_drxctrl_get_next_wkup_sfn

DESCRIPTION     This function aborts the sleep whenever there is an external cmd.

DEPENDENCIES    None

RETURN VALUE    None

SIDE EFFECTS    None

===========================================================================*/
extern uint16 wl1_dl_drxctrl_get_next_wkup_sfn(void);

#endif /* FEATURE_WCDMA_HS_FACH_DRX */

/*===========================================================================

FUNCTION        WL1_DL_DRXCTRL_CHECK_TCXO_SLEEP_POSSIBLE

DESCRIPTION     This function checks if tcxo shutdown possible or not by looking
                       into available time to sleep

DEPENDENCIES    None

RETURN VALUE    boolean. TRUE -tcxo sleep possible, FALSE-TCXO sleep not possible

SIDE EFFECTS    None

===========================================================================*/
LOCAL boolean wl1_dl_drxctrl_check_tcxo_sleep_possible
(
  wl1_dl_drxctrl_entity_ops_request_struct_type* prog_input_ptr
);

/*===========================================================================

FUNCTION        WL1_DL_DRXCTRL_IS_SCHEDULING_ENABLED

DESCRIPTION     This function retruns the current scheduling status

DEPENDENCIES    None

RETURN VALUE    Scheduling status

SIDE EFFECTS    None

===========================================================================*/
extern boolean wl1_dl_drxctrl_is_scheduling_enabled(void);

#ifdef FEATURE_WCDMA_HS_FACH_DRX 
/*===========================================================================

FUNCTION        wl1_dl_drxctrl_wakeup_cb

DESCRIPTION     This function is the call back handler registered to DRX manager
                to inform about wakeup.

DEPENDENCIES    None

RETURN VALUE    None

SIDE EFFECTS    None

===========================================================================*/
extern void wl1_dl_drxctrl_wakeup_cb(drx_entity_handle_type entity_handle);

/*===========================================================================

FUNCTION        wl1_dl_drxctrl_wakeup_cb

DESCRIPTION     This function is the call back handler registered to DRX manager
                to prepare the E-DRX entities for full sleep

DEPENDENCIES    None

RETURN VALUE    None

SIDE EFFECTS    None

===========================================================================*/
extern void wl1_dl_drxctrl_sleep_cb(drx_entity_handle_type entity_handle);

/*===========================================================================
FUNCTION   wl1_dl_drxctrl_next_frame_wanted_cb

DESCRIPTION
  DRX Manager invokes this callback routine to find out how long a E-DRX wants
  to be active or sleeping, whichever the case may be.  So this function
  effectively implements the scheduling of E-DRX.

DEPENDENCIES
  None.

RETURN VALUE
  boolean - TRUE on success (always returns TRUE).

SIDE EFFECTS
  None.
===========================================================================*/
extern boolean wl1_dl_drxctrl_next_frame_wanted_cb
(
  drx_entity_handle_type          entity_handle,
  drx_manager_sleep_struct_type  *sleep_info_ptr
);

/*===========================================================================

FUNCTION wl1_dl_drxctrl_wakeup_edrx_for_slp_abort

DESCRIPTION This function returns the current scheduling status

DEPENDENCIES None

RETURN VALUE Scheduling status

SIDE EFFECTS None

===========================================================================*/
void wl1_dl_drxctrl_wakeup_edrx_for_slp_abort(void);

/*===========================================================================

FUNCTION wl1_dl_drxctrl_is_edrx_hs_awake

DESCRIPTION Returns the state of the edrx hs module

DEPENDENCIES None

RETURN VALUE Boolean

SIDE EFFECTS None

===========================================================================*/
extern boolean wl1_dl_drxctrl_is_edrx_hs_awake(void);

/*===========================================================================

FUNCTION        wl1_edrx_get_edrx_configured_status

DESCRIPTION     This function can be used to find out the E-DRX configured status

DEPENDENCIES    None

RETURN VALUE    None

SIDE EFFECTS    None

===========================================================================*/
extern boolean wl1_edrx_get_edrx_configured_status(void);

#endif /* FEATURE_WCDMA_HS_FACH_DRX */

/*===========================================================================

FUNCTION        wl1_dl_drxctrl_query_edrx_active

DESCRIPTION     This function returns TRUE if DLDRCCTRL is active in FACH state

DEPENDENCIES    None

RETURN VALUE    Boolean

SIDE EFFECTS    None

===========================================================================*/
extern boolean wl1_dl_drxctrl_query_edrx_active(void);

/*===========================================================================

FUNCTION        wl1_dl_drxctrl_get_drx_state

DESCRIPTION     This function returns the current DRX state.

DEPENDENCIES    None

RETURN VALUE    Current DRX state: wl1_dl_drxctrl_state_enum_type

SIDE EFFECTS    None

===========================================================================*/
extern wl1_dl_drxctrl_state_enum_type wl1_dl_drxctrl_get_drx_state(void);

/*===========================================================================
FUNCTION        wl1_dl_drxctrl_lock_unlock_mutex

DESCRIPTION     This function is used to acquire/release controller mutex lock
                          
DEPENDENCIES    None

RETURN VALUE    Boolean
 
SIDE EFFECTS    None
==========================================================================*/
extern boolean wl1_dl_drxctrl_lock_unlock_mutex(boolean lock, boolean try_lock);

/*===========================================================================
FUNCTION        wl1_dl_drxctrl_is_lite_sleep_trans_in_prog

DESCRIPTION     This function is returns TRUE when UE is in transient states
                of CPC lite

DEPENDENCIES    None

RETURN VALUE    Boolean

SIDE EFFECTS    None
==========================================================================*/
extern boolean wl1_dl_drxctrl_is_lite_sleep_trans_in_prog(void);
#endif /*defined(FEATURE_WCDMA_CPC_DRX)|| defined(FEATURE_WCDMA_HS_FACH_DRX)*/
/*===========================================================================
FUNCTION        WL1_DL_DRXCTRL_RESUME_CPHY

DESCRIPTION     This function ensures WL1DLDRXCTRL is complete performing suspend periodic operations
                          
DEPENDENCIES    Below is a call back function hence an event on gen tl has to be already programmed to invoke the below function

RETURN VALUE    None
 
SIDE EFFECTS    None
==========================================================================*/
extern void wl1_dl_drxctrl_resume_cphy(uint8 evt);
#endif /*WL1DLDRXCTRL_H*/
