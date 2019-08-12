#ifndef WL1HSEDRXENTITY_H
#define WL1HSEDRXENTITY_H

/*===========================================================================
                 WCDMA L1 HS EDRX entity

GENERAL DESCRIPTION
  This file contains the code for implementing the HS state machine under EDRX

EXTERNALIZED FUNCTIONS

INTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS


Copyright (c)  2011 by Qualcomm Technologies, Inc.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/wcdma/l1/offline/src/wl1edrxhsentity.h#1 $
$DateTime: 2016/12/13 08:00:12 $
$Author: mplcsds1 $


when       who    what, where, why
--------   ---    ---------------------------------------------------------
08/18/14   sr     Changes to wakeup HS entity 60 bpg before RxBurst and waking up EDRX at 10th bpg after FS
07/16/14   sr     EDRX code cleanup
02/11/13   gsk/sr EDRX Full Sleep initial check in.
09/10/12   stk    Added support for Cell reselection optimizations with HSFACH/HSRACH
09/05/12   sr     Changes to trigger HS wakeup upon valid AICH decode
08/19/12   sj     Added support for QICE receiver in EDRX mode
06/14/12   hk     E-DRX support for HS-RACH
05/15/12   hk     Added a new API to get the HS configured status 
04/30/12   hk     Increased the HS entity wakeup margin to 35 BPGs
03/27/12   hk     Added support for FEATURE_WCDMA_HS_FACH_DRX
11/28/11   nd     Added some logging related restructuring of code.
11/22/11   nd     Fixed compiler warning.
11/09/11   nd     Added support for the EDRX log packets.
04/11/11   nd     Initial revision.

===========================================================================*/

/* -----------------------------------------------------------------------
** Include Files
** ----------------------------------------------------------------------- */
#include "wcdma_variation.h"
#include "comdef.h"
#include "customer.h"

#ifdef FEATURE_WCDMA_HS_FACH_DRX
#include "l1rrcif.h"
#include "log.h"
#include "log_codes_wcdma_int.h"
#include "wl1m.h"
#include "wl1dldrxctrl.h"

/* -----------------------------------------------------------------------
** Type Declarations
** ----------------------------------------------------------------------- */

/** enum state describing the entity's awake state */
typedef enum
{
  /** entity is inactive */
  WL1_EDRX_HS_ENTITY_STATE_INACTIVE,
  
  /** entity is configured but scheduling hasnt started */
  WL1_EDRX_HS_ENTITY_STATE_CONFIGURED,
  
  /** entity is actively demodulating data */
  WL1_EDRX_HS_ENTITY_STATE_ACTIVE,
  
  /** entity is demodulating but in inactive monitoring T321 */
  WL1_EDRX_HS_ENTITY_STATE_RX_ACTIVE_MONITORING_T321,

  /* entity is not following a DRX pattern */
  WL1_EDRX_HS_ENTITY_STATE_RX_ACTIVE_T321_FROZEN,
  
  /** entity is not demodulating and is currently asleep */
  WL1_EDRX_HS_ENTITY_STATE_RX_INACTIVE,
  
  /** EDRX entity is HS channel down */
  WL1_EDRX_HS_ENTITY_STATE_SLEEP,
  
  /** max num entity states */
  WL1_EDRX_HS_ENTITY_MAX_NUM_STATES
}wl1_edrx_hs_entity_state_enum_type;

typedef enum
{
  WL1_EDRX_HS_ENTITY_EXT_CMD_CONFIG,
  WL1_EDRX_HS_ENTITY_EXT_CMD_START, 
  WL1_EDRX_HS_ENTITY_EXT_CMD_DECONFIG,
  WL1_EDRX_HS_ENTITY_EXT_CMD_FREEZE,
  WL1_EDRX_HS_ENTITY_EXT_CMD_UNFREEZE,
  WL1_EDRX_HS_ENTITY_EXT_CMD_MAX
}wl1_edrx_hs_entity_cmd_enum_type;

typedef enum
{
  WL1_EDRX_HS_SCHED_CTRL_REASON_INVALID,
  WL1_EDRX_HS_SCHED_CTRL_REASON_EXT_CMD_PROC,
  WL1_EDRX_HS_SCHED_CTRL_REASON_EDCH,
  WL1_EDRX_HS_SCHED_CTRL_REASON_ETWS,
  WL1_EDRX_HS_SCHED_CTRL_REASON_MAX
}wl1_edrx_hs_entity_sched_ctrl_reason_enum_type;

typedef struct
{
  wl1_edrx_hs_entity_cmd_enum_type cmd_type;
  wl1_edrx_hs_entity_sched_ctrl_reason_enum_type sched_ctrl_reason_code;
}wl1_edrx_hs_entity_cmd_struct_type;

typedef enum
{
  WL1_EDRXHS_DRXCTRL_CMD_REGISTER,
  WL1_EDRXHS_DRXCTRL_CMD_DEREGISTER,
  WL1_EDRXHS_DRXCTRL_CMD_SLEEP,
  WL1_EDRXHS_DRXCTRL_CMD_START,
  WL1_EDRXHS_DRXCTRL_CMD_MAX
 
}wl1_edrxhs_drxctrl_cmd_enum_type;

#ifdef FEATURE_L1_LOGGING_ENABLED
typedef PACKED struct PACKED_POST
{
  uint16 dhrnti;
  uint16 drx_cycle_len_fr;
  uint16 rx_burst_fr;
  uint16 t321_drx_interruption;
  uint16 ul_cfg;
  uint32 s_params_1;
  uint32 s_params_2;
}wl1_edrx_hs_entity_cfg_info_struct_type;

typedef PACKED struct PACKED_POST
{
  /*uint16 sfn_subfr;
  uint16 hs_activity;
  uint16 ul_activity;
  uint16 sleep_info; 
  uint32 srch_activity;*/
 uint16 data[6];
 
}wl1_edrx_hs_state_info_struct_type;

#define WL1_EDRX_HS_MAX_NUM_SAMPLES 50
#define WL1_EDRX_HS_STATE_MACHINE_NUM_BUF 2

/*-----------------------------------------------------------*/
/*          SFN and SUBFRAME                                                          */
/* Bits 0:11 SFN (0 to 4095 inclusive)                                               */
/* Bits 14:12 Subframe number (0 to 4 inclusive)                                */
/* Bits 15 Sleep indicator 0 - FALSE, 1 - TRUE                                   */
/*-----------------------------------------------------------*/
#define EDRX_STATE_SFN_BMSK        0x0FFF
#define EDRX_STATE_SFN_REG         0
#define EDRX_STATE_SFN_SHFT        0

#define EDRX_STATE_SUBFR_BMSK      0x7000
#define EDRX_STATE_SUBFR_REG       0
#define EDRX_STATE_SUBFR_SHFT      12

#define EDRX_STATE_SLEEP_BMSK      0x8000
#define EDRX_STATE_SLEEP_REG       0
#define EDRX_STATE_SLEEP_SHFT      15
/*-----------------------------------------------------------*/
/*         HS ACTIVITY                                                                   */
/* Bit 0 HS subframe monitor flag 0 - FALSE (blank), 1 - TRUE (Y)         */
/* Bit 1 HS subframe SCCH status 0 - INVALID (blank), 1 - VALID (V)     */ 
/* Bit 2 RxBurst flag 0 - FALSE (blank), 1 - TRUE (Y)                           */
/* Bit 14:3 Number of subframes left to monitor SCCH - T321/T325 Timer */
/* Bit 15 T325 indicator flag 0 - FALSE (blank), 1 - TRUE (Y)                */
/*-----------------------------------------------------------*/
#define EDRX_STATE_HS_MON_BMSK     0x0001
#define EDRX_STATE_HS_MON_REG      1
#define EDRX_STATE_HS_MON_SHFT     0

#define EDRX_STATE_DEC_STAT_BMSK   0x0002
#define EDRX_STATE_DEC_STAT_REG    1
#define EDRX_STATE_DEC_STAT_SHFT   1

#define EDRX_STATE_RXBURST_BMSK    0x0004
#define EDRX_STATE_RXBURST_REG     1
#define EDRX_STATE_RXBURST_SHFT    2

#define EDRX_STATE_T321_BMSK       0x7FF8
#define EDRX_STATE_T321_REG        1
#define EDRX_STATE_T321_SHFT       3

#define EDRX_STATE_T325_IND_BMSK   0x8000
#define EDRX_STATE_T325_IND_REG    1
#define EDRX_STATE_T325_IND_SHFT   15

/*-----------------------------------------------------------*/
/*          UL ACTIVITY                                                                   */
/* Bit 1:0 UL activity 0 - nothing, 1 - R99 UL, 2 - PRE, 3 - ENH UL        */
/*            Note: Display string: 0 - "", 1 - "R99", 2 - "PRE", 3 - "ENH"    */
/* Bit 15:2 Reserved                                                                      */
/*-----------------------------------------------------------*/
#define EDRX_STATE_UL_STATE_BMSK   0x0003
#define EDRX_STATE_UL_STATE_REG    2
#define EDRX_STATE_UL_STATE_SHFT   0

/*-----------------------------------------------------------*/
/*          SRCH ACTIVITY_1                                                            */
/* Bit 7:0 Squal (-127 to +127, with -128 reserved as blank)                */
/* Bit 15:8 Srxlev (-127 to +127, with -128 reserved as blank)             */
/*-----------------------------------------------------------*/
#define EDRX_STATE_SQUAL_BMSK      0x00FF
#define EDRX_STATE_SQUAL_REG       3
#define EDRX_STATE_SQUAL_SHFT      0

#define EDRX_STATE_SRXLEV_BMSK     0xFF00
#define EDRX_STATE_SRXLEV_REG      3
#define EDRX_STATE_SRXLEV_SHFT     8

/*-----------------------------------------------------------*/
/*          SRCH ACTIVITY_2                                                            */
/* Bit 16 Intrafreq search enabled flag 0 - FALSE (blank), 1 - TRUE (Y)  */
/* Bit 17 Interfreq search enabled flag 0 - FALSE (blank), 1 - TRUE (Y)  */
/* Bit 18 InterRAT search enabled flag 0 - FALSE (blank), 1 - TRUE (Y)  */
/* Bit 23:19 Reserved                                                                   */
/* Bit 27:24 Step search 0 - nothing, 1 - Step 1 search, 2 - Step 2 search/
            NAS-TT search, 3 - Step 3 search, 4 - List search, 5 - Intra-Other,
            8 - Inter-Freq, 9 - Inter-RAT GSM, 10 - Inter-RAT LTE,
            11 - Inter-Other, {6, 7, 12-16} - Reserved 
            Note: Display string: 0 - "", 1 - "Step1", 2 - "Step2", 3 - "Step3"
            4 - "List", 5 - "intra", 8 - "IFREQ", 9 - "GSM", 10 - "LTE",
            11 - "inter", others = "n/a"                                                  */
/* Bit 31:28 Reserved                                                                    */
/*-----------------------------------------------------------*/
#define EDRX_STATE_INTRAF_EN_BMSK  0x0001
#define EDRX_STATE_INTRAF_EN_REG   4
#define EDRX_STATE_INTRAF_EN_SHFT  0

#define EDRX_STATE_INTERF_EN_BMSK  0x0002
#define EDRX_STATE_INTERF_EN_REG   4
#define EDRX_STATE_INTERF_EN_SHFT  1

#define EDRX_STATE_IRAT_EN_BMSK    0x0004
#define EDRX_STATE_IRAT_EN_REG     4
#define EDRX_STATE_IRAT_EN_SHFT    2

#define EDRX_STATE_SRCH_TYPE_BMSK  0x0F00
#define EDRX_STATE_SRCH_TYPE_REG   4
#define EDRX_STATE_SRCH_TYPE_SHFT  8

/*-----------------------------------------------------------*/
/* SLEEP INFO                                                                              */
/*Bit 1:0 Sleep Type 0 - Nothing, 1 - CPC-DRX, 2 - PCH-DRX, 3 - Other */
/*        Note: Display string: 0 - "", 1 - "CPC", 2 - "PCH", 3 - "n/a"        */
/*Bit 13:2 Wakeup SFN (0 to 4095 inclusive)                                      */
/*Bit 15:14 Reserved                                                                     */
/*-----------------------------------------------------------*/
#define EDRX_STATE_SLEEP_TYPE_BMSK 0x0003
#define EDRX_STATE_SLEEP_TYPE_REG  5
#define EDRX_STATE_SLEEP_TYPE_SHFT 0

#define EDRX_STATE_WKUP_CFN_BMSK   0x3FFC
#define EDRX_STATE_WKUP_CFN_REG    5
#define EDRX_STATE_WKUP_CFN_SHFT   2

#define WL1_EDRX_HS_GET_WAKEUP_MARGIN 55

/*--------------------------------------------------------------------------

--------------------------------------------------------------------------*/
#define EDRX_STATE_MACHINE_DATA(page_idx,samp_idx,field, val) \
  (wl1_edrx_hs_state_info_log_pkt.page[page_idx].state[samp_idx].data[field##_REG] = \
  (wl1_edrx_hs_state_info_log_pkt.page[page_idx].state[samp_idx].data[field##_REG] & ~field##_BMSK) | \
  (((val) << field##_SHFT) & (field##_BMSK)))


LOG_RECORD_DEFINE(WL1_EDRX_HS_ENTITY_CFG_LOG_PKT)
/** version num of the log packet */
uint8 version;
/** number of samples in the logpacket */
uint8 fach_drx_status;
/**start cfn drx   */
uint16 sfn;
/** drx log info */
wl1_edrx_hs_entity_cfg_info_struct_type info;
LOG_RECORD_END


LOG_RECORD_DEFINE(WL1_EDRX_HS_ENTITY_STATE_MACHINE_LOG_PKT)
/** version num of the log packet */
uint8 version;
/** number of samples in the logpacket */
uint8 num_sample;
/**start cfn drx   */
uint16 start_sfn_subfr;
/** drx log info */
wl1_edrx_hs_state_info_struct_type state_info[WL1_EDRX_HS_MAX_NUM_SAMPLES];
LOG_RECORD_END

typedef enum
{
  WL1_EDRX_EVENT_ACTION_NO_ACTION = 0,
  WL1_EDRX_EVENT_ACTION_RRC_CONFIG = 1,
  WL1_EDRX_EVENT_ACTION_L1_CONFIG = 2,
  WL1_EDRX_EVENT_ACTION_L1_START_PATTERN = 3,
  WL1_EDRX_EVENT_ACTION_L1_STOP = 4,
  WL1_EDRX_EVENT_ACTION_MAX = 5
}wl1_edrx_event_action_enum_type;

typedef enum
{
  WL1_EDRX_LOG_NONE =0x0 ,
  WL1_EDRX_LOG_CONFIG_PKT = 0x1,
  WL1_EDRX_LOG_STATE_MACHINE_PKT = 0x2,
  WL1_EDRX_LOG_EDRX_EVENT = 0x3,
  WL1_EDRX_LOG_MAX = 0x4
}wl1_edrx_log_pkt_type;
#endif /* FEATURE_L1_LOGGING_ENABLED*/
/* =======================================================================
**                        Function Declarations
** ======================================================================= */

/*===========================================================================

FUNCTION        wl1_hsdrx_entity_init

DESCRIPTION     This function initialises the hs drx entity database

DEPENDENCIES    None

RETURN VALUE    None

SIDE EFFECTS    None

===========================================================================*/
extern void wl1_edrx_hs_entity_init(void);

/*===========================================================================

FUNCTION        wl1_hsdrx_validate_hsdrx_params

DESCRIPTION     This function validates hsdrx params received from RRC

DEPENDENCIES    None

RETURN VALUE    True/False based upon validation success/failure

SIDE EFFECTS    None

===========================================================================*/
extern boolean wl1_edrx_hs_validate_edrx_params(l1_setup_cmd_type *setup_cmd);

/*===========================================================================

FUNCTION        WL1_EDRX_HS_CMD_PROCESSOR

DESCRIPTION     This function handles all the HS EDRX cmds

DEPENDENCIES    None

RETURN VALUE    None

SIDE EFFECTS    None

===========================================================================*/
extern boolean wl1_edrx_hs_cmd_processor(wl1_edrx_hs_entity_cmd_struct_type *cmd);

/*===========================================================================

FUNCTION        wl1_edrx_hs_is_subfn_within_rx_active

DESCRIPTION     Can be used by external modules to check if a PDSCH block is inside
                       the Rx active period of EDRX state machine. 

DEPENDENCIES    None

RETURN VALUE    FALSE - outside Rx active period
                        TRUE - inside Rx active period

SIDE EFFECTS    None

===========================================================================*/
extern boolean wl1_edrx_hs_is_subfn_within_rx_active(uint16 scch_dec_subfr);

/*===========================================================================

FUNCTION        wl1_edrx_hs_entity_query_edrx_params

DESCRIPTION     This API for external world to get the eDRX parameters 

DEPENDENCIES    None

RETURN VALUE    None

SIDE EFFECTS    None

===========================================================================*/
extern boolean wl1_edrx_hs_entity_query_edrx_params(uint8 *drx_cycle_len_fr,uint16 *rx_burst_fr);

/*===========================================================================

FUNCTION        WL1_EDRX_HS_FLUSH_LOG_PKT

DESCRIPTION     Flush the EDRX state machine log packet when one of the page given by
                       log_flush_idx is full;

DEPENDENCIES    None

RETURN VALUE    none

SIDE EFFECTS    None

===========================================================================*/
extern void wl1_edrx_hs_flush_log_pkt(wl1_edrx_log_submit_cmd_type *log_cmd);

/*===========================================================================

FUNCTION WL1_EDRX_CME_TRIGGER_HANDLER

DESCRIPTION This function will handle the cme enable or disable
  

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void wl1_edrx_cme_trigger_handler(wl1_edrx_cme_action_local_cmd_type *cme_cmd);

/*===========================================================================

FUNCTION        wl1_edrx_hs_is_edrx_configued

DESCRIPTION     Returns the E-DRX configured status

DEPENDENCIES    None

RETURN VALUE    FALSE - Not configured
                        TRUE - E-DRX HS state machine initiated

SIDE EFFECTS    None

===========================================================================*/
extern boolean wl1_edrx_hs_is_edrx_configued(void);

/*===========================================================================

FUNCTION WL1_EDRX_HS_NOTIFY_EDCH_RESOURCE_STATE

DESCRIPTION This function informs the EDCH common resource state to EDRX
  This will help to enable/disable continuous HS monitoring

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void wl1_edrx_hs_notify_edch_resource_state(boolean edch_resource_state);

/*===========================================================================

FUNCTION WL1_EDRX_HS_TRIGGER_HS_ENTITY_WAKEUP

DESCRIPTION This function is used to trigger HS entity wakeup procedure unconditionally
  irrespective of Rx Burst

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void wl1_edrx_hs_trigger_hs_entity_wakeup(void);

/*===========================================================================

FUNCTION WL1_EDRX_IS_T321_ACTIVE

DESCRIPTION This function will return TRUE is T321 is either active or frozen
  

DEPENDENCIES
  None.

RETURN VALUE
  TRUE: If entity state is WL1_EDRX_HS_ENTITY_STATE_RX_ACTIVE_MONITORING_T321
  (or) WL1_EDRX_HS_ENTITY_STATE_RX_ACTIVE_T321_FROZEN
  FALSE: Otherwise

SIDE EFFECTS
  None.

===========================================================================*/
extern boolean wl1_edrx_is_t321_active(void);

/*===========================================================================

FUNCTION        wl1_edrx_hs_suspend_hs

DESCRIPTION     This API for external world to get the eDRX parameters 

DEPENDENCIES    None

RETURN VALUE    None

SIDE EFFECTS    None

===========================================================================*/
extern void wl1_edrx_hs_suspend_hs(void);

/*===========================================================================

FUNCTION        wl1_edrx_hs_resume_hs

DESCRIPTION     This API for external world to get the eDRX parameters 

DEPENDENCIES    None

RETURN VALUE    None

SIDE EFFECTS    None

===========================================================================*/
extern void wl1_edrx_hs_resume_hs(void);

/*===========================================================================

FUNCTION        wl1_edrx_hs_resume_done_cb

DESCRIPTION     This API for external world to get the eDRX parameters 

DEPENDENCIES    None

RETURN VALUE    None

SIDE EFFECTS    None

===========================================================================*/
extern void wl1_edrx_hs_resume_done_cb(boolean status);

/*===========================================================================

FUNCTION WL1_EDRX_GET_DRX_CYCLE_LENGTH

DESCRIPTION This function return the current DRX cycle length
  

DEPENDENCIES
  None.

RETURN VALUE
  Drx cycle length.

SIDE EFFECTS
  None.

===========================================================================*/
extern int16 wl1_edrx_get_drx_cycle_length(void);

/*===========================================================================

FUNCTION WL1_EDRX_IS_HS_ENTITY_ACTIVE

DESCRIPTION This function return TRUE if HS entity state is not not inactive or Configured
  

DEPENDENCIES
  None.

RETURN VALUE
  Boolean

SIDE EFFECTS
  None.

===========================================================================*/
extern boolean wl1_edrx_is_hs_entity_active(void);

#endif /* FEATURE_WCDMA_HS_FACH_DRX */

#endif

