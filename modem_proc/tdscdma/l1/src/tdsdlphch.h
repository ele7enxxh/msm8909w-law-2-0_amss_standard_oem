#ifndef TDSDLPHCH_H
#define TDSDLPHCH_H

/*===========================================================================
                 TDSCDMA L1 DOWNLINK PHYSICAL CHANNEL CONFIGURATION

GENERAL DESCRIPTION
  This file contains the code for handling validation, configuration and
  management of downlink physical channels 
  (PCCPCH, SCCPCH,  DPCH, AICH, PICH and MICH).

EXTERNALIZED FUNCTIONS

INTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS


Copyright (c) 2007 - 2009 by Qualcomm Technologies Incorporated.  All Rights Reserved.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/l1/vcs/dlcmd.c_v   1.109   02 Jul 2002 18:11:42   cemani  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/tdscdma/l1/src/tdsdlphch.h#1 $ $DateTime: 2016/12/13 07:58:19 $ $Author: mplcsds1 $

when       who      what, where, why
--------   ---      --------------------------------------------------------
11/22/10   yongh    Revise this file from UMTS to TDSCDMA.

===========================================================================*/


/* -----------------------------------------------------------------------
** Include Files
** ----------------------------------------------------------------------- */
#include "tdscdma_variation.h"
#include "comdef.h"
#include "customer.h"

#include "tdsl1rrcif.h"
#include "tdsl1const.h"
#include "tdsl1def.h"
#include "mcaltds_dl.h"
/*+TDSDLPHCH Development-----------------------------------------------------------------------*/
/*+Yongh development, remove later *********************************************************/
#define TDSDLPHCH_INVALID_RL_ID NO_RL_IDX

#ifdef FEATURE_TDSCDMA_L1_LOGGING_ENABLED
/*there two type for phch cfg log, one is cphy setup and other is cell_trans*/
extern boolean tdsdlphch_cfg_log_pkt_setup_state;
#endif
/*-Yongh development, remove later *********************************************************/
/* ==========================================================================
** Macro const Definitions
** ========================================================================*/
#define TDSDLPHCH_INVALID_CFG_HANDLE 0xFF
#define TDSDLPHCH_INVALID_SLOT_FORMAT 0xFF
#define TDSDLPHCH_INVALID_HWCH_ID 0xFF
/* TDSDLPHCH module max cfg table is 6.
 * For S or N pccpch and sccpch(pich and pch),
 * the number of table is 3, if channel is recfged,
 * the number of table is 6, so max number of table is 6.
 */
#define TDSDLPHCH_MAX_NUM_CFG 6
#define TDSDLPHCH_INVALID_TS_NUM 0xFF
/* ==========================================================================
** Macro condition Definitions
** ========================================================================*/
#define TDSDLPHCH_IS_PHCH_CFG_DB_IDX_INVALID(phch_cfg_db_idx)    \
    (phch_cfg_db_idx >= TDSDLPHCH_MAX_NUM_CFG)

/* ==========================================================================
** Enum Definitions
** ========================================================================*/
typedef enum
{
  TDSDLPHCH_STATE_INACTIVE,
  TDSDLPHCH_STATE_SUSPEND,
  TDSDLPHCH_STATE_ACTIVE
} tdsdlphch_state_enum_type;

typedef enum
{
  TDSDLPHCH_NOTIFY_NONE,
  TDSDLPHCH_NOTIFY_ADD,
  TDSDLPHCH_NOTIFY_RECFG,
  TDSDLPHCH_NOTIFY_DROP,
  TDSDLPHCH_NOTIFY_SUSPEND,
  TDSDLPHCH_NOTIFY_RESUME,
  TDSDLPHCH_NOTIFY_RECFG_SPECIAL
}tdsdlphch_notify_enum_type;

#ifdef FEATURE_TDSCDMA_MCPM_POWER_REPORTING
typedef enum
{
  TDSDLPHCH_EQ,
  TDSDLPHCH_MUD,
  TDSDLPHCH_NODEMOD
}tdsdlphch_demod_mode_type;
#endif

/* ==========================================================================
** Data structure Definitions
** ========================================================================*/
typedef uint8 tdsdlphch_cfg_handle_type;

typedef void TDSDLPHCH_CFG_DONE_CB_FUNC_TYPE(
  uint8 cctrch_idx,
  tdsl1_dl_phychan_enum_type phch_type,
  tdsdlphch_cfg_handle_type phch_handle,
  boolean status);

typedef void (*TDSDLPHCH_CFG_DONE_CB_FUNC_PTR_TYPE)( uint8 cctrch_idx, tdsl1_dl_phychan_enum_type phch_type, tdsdlphch_cfg_handle_type phch_handle,  boolean status);

typedef  void TDSDLPHCH_CFG_SUSPEND_RESUME_FUNC_TYPE(
  tdsdlphch_cfg_handle_type phch_cfg_db_idx,
  boolean fast_cfg,
  TDSDLPHCH_CFG_DONE_CB_FUNC_PTR_TYPE phch_suspend_done_cb_ptr);

/*--------------------------------------------------------------------------
                  STRUCTURE: DL_PHYCHAN_ADD_TYPE

This structure contains the parameters for the dl_phychan_add().
--------------------------------------------------------------------------*/
typedef struct {
/*+ Legacy for TDSDLPHCH dev, to be deleted later ******************************/

  /* frequency on which config goes on */
  uint16 rf_chan;

  /*Cell parameters ID (see 25.223:7.3)*/
  uint8  cell_parm_id;

  /* align type for this channel */
  //tdsseq_align_type align_type;
/*- Legacy for TDSDLPHCH dev, to be deleted later ******************************/

  /* the physical channel */
  tdsl1_dl_phychan_enum_type phch;

  /* channel activation  time . It is the start sfn of the bitmask: chanPresentMask.
   * Invalid value for channel drop: 0xffff
   */
  uint16 activation_time; 

  uint8 notify_cfg;

  /* the CCTrCH table index chosen by dl_dec for this channel */
  uint8 cctrch_index;

  /* pccpch detection algorithm
   * 00 - No pccpch detection needed (TTI Boundary is known).
   * 01 - Phase based detection.
   * 10 - Blind detection.
   * 11 - Not used.
   */
  tdsl1_dl_phycfg_pccpch_tti_detection_enum_type tti_detection_algorithm;

  /*0 - One time, 1- Continuous.*/
  /*Yong comment out TDM mode for new interface of SW and FW*/
  /*tdsl1_dl_phycfg_dl_tdm_mode_enum_type tdm_mode;*/
  /*Yong comment out TDM mode for new interface of SW and FW*/
  
  /* Channel Present repetition period. Valid regardless of TDM mode. 
	  Valid values: 2,4,8,16,32,64 frames */
  uint8  chanOnRepPeriod;

  /* Channel Present on-off bit mask to be applied at action time.  
  Bit[0..(chanOnRepPeriod-1)] with each bit indicates if Physical channel is present in
  the corresponding frame. 1: phy channel is present. 0: not present. Bit[chanOnRepPeriod..63] 
  shall be set to 0 when chanOnRepPeriod is less than 64.*/
  uint64  chanPresentMask;

  /*! \brief insert Dob For Zero TB, pch is one, other is 0 */
  uint8 insertDobForZeroTB ;
#ifdef FEATURE_TDSCDMA_PCH_FET
  uint8 pch_fet_enable;
#endif

#ifdef FEATURE_TDSCDMA_LVA_DECODE
  /*for LVA using 1: enable, 0 disable*/
  uint8 call_type_mask;
#endif

  /*! \brief SFN number within TTI at the action time when config is applied  Valid :0..255.*/
  //uint8  ttiSfn;
  /* pointer to the dl phychan ordered config database */
  tdsl1_dl_phycfg_phychan_db_struct_type *phychan_db;
} tdsdlphch_add_type;

typedef tdsdlphch_add_type tdsdlphch_recfg_type;
/*--------------------------------------------------------------------------
                  STRUCTURE: DL_PHYCHAN_ADD_TYPE

This structure contains the parameters for the dl_phychan_add().
--------------------------------------------------------------------------*/

typedef struct
{
  tdsl1def_l1_local_cmd_hdr_type hdr;

  uint8 cfg_id;

  /* cfn at which configuration is applied */
  uint8 ccfn;
  
  /* status of the configuration */
  boolean status;
}tdsdlphch_cfg_done_cmd_type;

/*--------------------------------------------------------------------------
                 STRUCTURE: DL_PHCH_CFG_DB_STRUCT_TYPE

This structure contains all of the various parameters for physical channel
configuration process
--------------------------------------------------------------------------*/

typedef struct 
{
  /* state of the physical channel */
  tdsdlphch_state_enum_type state;

  /* configuration cfn at which the configuration is activated/deactivated */
  uint8 ccfn;

  /* Actions before notification */
  /*
   * Bit0 allocate cfg db
   * Bit1 disable cctrch
   * Bit2 mcal add
   * Bit3 mcal drop
   * Bit4 mcal recfg
   * Bit5 mcal drop old cfg
   * Bit6 enable cctrch
   * Bit7 free cctrch
   * Bit8 deallocate cfg db
   * Bit9 free old cctrch and deallocate old cfg db
   */
  uint32 actions_bmsk;

  /*yongh remove this variable*/
  //tdsl1_tpc_step_size_enum_type tpc_step_size;

  /* index points to the old config of tdsdlphch_cfg_db array */
  tdsdlphch_cfg_handle_type old_phch_cfg_idx;

  /* handles notification type */
  tdsdlphch_notify_enum_type notify_type;

  /* notification phychan type */
  tdsl1_dl_phychan_enum_type notify_phychan_type;

  /* call back function pointer to indicate the clients  
   * once after config operation is done 
   */
  TDSDLPHCH_CFG_DONE_CB_FUNC_TYPE *cfg_done_cb;

  /* physical channel config parameters to MCAL DL Module */
   mcaltds_dl_phch_cfg_parms_struct_type phch_cfg_parms;   
}tdsdlphch_cfg_db_struct_type;

typedef struct
{
  /*activation time for FW cfg*/
  uint16 activation_time;

  /* notify flag */
  uint8 notify_cfg;

  /*channel present period*/
  uint8  chanOnRepPeriod;

  /*channel present bitmask*/
  uint64  chanPresentMask;

  /*! \brief insert Dob For Zero TB, pch is one, other is 0 */
  uint8 insertDobForZeroTB;
#ifdef FEATURE_TDSCDMA_PCH_FET
  uint8 pch_fet_enable;
#endif
  /*! \brief SFN number within TTI at the action time when config is applied  Valid :0..255.*/
  //uint8  ttiSfn;
}tdsdlphch_update_parms_struct_type;

/* ==========================================================================
** Function Definitions
** ========================================================================*/

/*===========================================================================
FUNCTION tdsdlphch_init

DESCRIPTION
  None

DEPENDENCIES
  None

RETURN VALUE
  None
  
SIDE EFFECTS
  None
===========================================================================*/
extern void tdsdlphch_init(void);


/*--------------------------------------------------------------------------
                 Operation Handler Functions
                 1. tdsdlphch_add
                 2. tdsdlphch_recfg
                 3. tdsdlphch_drop
                 4. tdsdlphch_suspend
                 5. tdsdlphch_resume
                 6. dl_phch_aset_upd
--------------------------------------------------------------------------*/

/*===========================================================================
FUNCTION tdsdlphch_add

DESCRIPTION
  Adds a physical channel to put it in either suspended state or active state as specified in next state. Once after 
  physical channel is added call back function call will be executed

DEPENDENCIES
  None

RETURN VALUE
  physical channel configuration handle, which should be used for all 
  future communication for reconfig/suspend/resume/drop operations. Returns
  Invalid physical channeld handle if operation fails
  
SIDE EFFECTS
  None
===========================================================================*/

extern tdsdlphch_cfg_handle_type tdsdlphch_add(
  /* next state after this configuration */
  tdsdlphch_state_enum_type phch_next_state,

  /* add type phychan confguration */
  tdsdlphch_add_type *phychan_add_ptr,

  /* call back function pointer after add operation is done */
  TDSDLPHCH_CFG_DONE_CB_FUNC_TYPE *phch_add_done_cb_ptr);

/*===========================================================================
FUNCTION tdsdlphch_recfg

DESCRIPTION
  Reconfigures a physical channel to put it in either suspended state or active state as specified in next state. Once after 
  physical channel is reconfigured call back function call will be executed

DEPENDENCIES
  None

RETURN VALUE
  physical channel configuration handle, which should be used for all 
  future communication for reconfig/suspend/resume/drop operations. Returns
  Invalid physical channeld handle if operation fails
  
SIDE EFFECTS
  None
===========================================================================*/

extern tdsdlphch_cfg_handle_type tdsdlphch_recfg(
  /* phch cfg handle that is to be reconfigured */
  tdsdlphch_cfg_handle_type phch_cfg_db_idx,

  /* next state after this configuration */
  tdsdlphch_state_enum_type phch_next_state,

  /* add type phychan confguration */
  tdsdlphch_recfg_type *phychan_recfg,

  /* call back function pointer after reconfigure operation is done */
  TDSDLPHCH_CFG_DONE_CB_FUNC_TYPE *phch_recfg_done_cb_ptr);
/*===========================================================================
FUNCTION tdsdlphch_recfg_special

DESCRIPTION
  None

DEPENDENCIES
  None

RETURN VALUE
  None
  
SIDE EFFECTS
  pending command not yet handled
===========================================================================*/
#if 0
tdsdlphch_cfg_handle_type tdsdlphch_recfg_special(
  /* phch cfg handle that is to be reconfigured */
  tdsdlphch_cfg_handle_type old_phch_cfg_db_idx,

  tdsdlphch_update_parms_struct_type *phch_parms_ptr,

  /* call back function pointer after reconfigure operation is done */
  TDSDLPHCH_CFG_DONE_CB_FUNC_TYPE *phch_recfg_done_cb_ptr);
#endif
/*===========================================================================
FUNCTION tdsdlphch_drop

DESCRIPTION
  Drops a physical channel to put it in inactive state. Once after 
  physical channel is dropped call back function call will be executed

DEPENDENCIES
  None

RETURN VALUE
  Returns Boolean to specify if drop operation successful or not. 
  
SIDE EFFECTS
  None
===========================================================================*/

extern boolean tdsdlphch_drop(
  /* phch cfg handle that is to be dropped */
  tdsdlphch_cfg_handle_type phch_cfg_db_idx,

  /* activation time */
  uint16 activation_time,
  
  /* notify type */
  uint8 notify_cfg,

  /* call back function pointer after drop operation is done */
  TDSDLPHCH_CFG_DONE_CB_FUNC_TYPE *phch_drop_done_cb_ptr);

/*===========================================================================
FUNCTION tdsdlphch_suspend

DESCRIPTION
  Suspends a physical channel to put it in suspended state. Once after 
  physical channel is suspended call back function call will be executed

DEPENDENCIES
  None

RETURN VALUE
  physical channel configuration handle, which should be used for all 
  future communication for reconfig/resume/drop operations. Returns
  Invalid physical channeld handle if operation fails
  
SIDE EFFECTS
  None
===========================================================================*/

extern boolean tdsdlphch_suspend(
  /* phch cfg handle that is to be suspended */
  tdsdlphch_cfg_handle_type phch_cfg_db_idx,

  uint16 activation_time,

  /* notify type */
  uint8 notify_cfg,

  /* call back function pointer after suspend operation is done */
  TDSDLPHCH_CFG_DONE_CB_FUNC_TYPE *phch_suspend_done_cb_ptr);

/*===========================================================================
FUNCTION tdsdlphch_resume

DESCRIPTION
  Resumes a physical channel to put it in active state. Once after 
  physical channel is resumed call back function call will be executed

DEPENDENCIES
  None

RETURN VALUE
  physical channel configuration handle, which should be used for all 
  future communication for reconfig/suspend/drop operations. Returns
  Invalid physical channeld handle if operation fails
  
SIDE EFFECTS
  None
===========================================================================*/

extern boolean tdsdlphch_resume(
  /* phch cfg handle that is to be resumed */
  tdsdlphch_cfg_handle_type phch_cfg_db_idx,

  tdsdlphch_update_parms_struct_type *phch_parms_ptr,

  /* call back function pointer after resume operation is done */
  TDSDLPHCH_CFG_DONE_CB_FUNC_TYPE *phch_resume_done_cb_ptr);

/*--------------------------------------------------------------------------
                 DL PHCH MCAL DL CFG Handler Call Back Functions defined in MCAL DL module
                 1. mcalwcdma_dl_phychan_cfg_sent_to_dsp_ind 
                 2. mcalwcdma_dl_phychan_done_ind
--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------
                 DL PHCH CFG Handler Functions
                 1. tdsdlphch_cfg_done_handler
--------------------------------------------------------------------------*/

/*===========================================================================
FUNCTION tdsdlphch_cfg_done_handler

DESCRIPTION
  None

DEPENDENCIES
  None

RETURN VALUE
  None
  
SIDE EFFECTS
  None
===========================================================================*/
extern void tdsdlphch_cfg_done_handler(
  tdsdlphch_cfg_done_cmd_type* cmd);

/*===========================================================================
FUNCTION tdsdlphch_get_first_ts_num_for_bho

DESCRIPTION
  find the first downlink time slot used for DPCH channel for Baton handover.

DEPENDENCIES
  None

RETURN VALUE
  None
  
SIDE EFFECTS
  None
===========================================================================*/
extern uint8 tdsdlphch_get_first_ts_num_for_bho(void);
/*===========================================================================
FUNCTION tdsdlphch_is_phch_suspended

DESCRIPTION
  None

DEPENDENCIES
  None

RETURN VALUE
  None
  
SIDE EFFECTS
  None
===========================================================================*/
extern boolean tdsdlphch_is_phch_suspended(tdsdlphch_cfg_handle_type phch_hdle);

/*===========================================================================
FUNCTION tdsdlphch_is_phch_suspended

DESCRIPTION
  None

DEPENDENCIES
  None

RETURN VALUE
  None
  
SIDE EFFECTS
  None
===========================================================================*/
extern boolean tdsdlphch_is_phch_inactive(tdsdlphch_cfg_handle_type phch_hdle);

/*===========================================================================
FUNCTION tdsdlphch_is_phch_suspended

DESCRIPTION
  None

DEPENDENCIES
  None

RETURN VALUE
  None
  
SIDE EFFECTS
  None
===========================================================================*/
extern boolean tdsdlphch_is_phch_active(tdsdlphch_cfg_handle_type phch_hdle);

/*-TDSDLPHCH Development-----------------------------------------------------------------------*/





/*--------------------------------------------------------------------------
                 STRUCTURE: DL_PHCH_CELL_DB_STRUCT_TYPE

This structure contains all of the various parameters for RL physical channel
configuration process
--------------------------------------------------------------------------*/
extern boolean tdsdlphch_pccpch_setup_in_dpch_state;

/*===========================================================================
FUNCTION tdsdlphch_get_channel_slot_infor

DESCRIPTION
  None

DEPENDENCIES
  None

RETURN VALUE
  None
  
SIDE EFFECTS
  None
===========================================================================*/
extern uint8 tdsdlphch_get_channel_slot_infor(tdsl1_dl_phychan_enum_type phch_type);

#ifdef FEATURE_TDSCDMA_MCPM_POWER_REPORTING
/*===========================================================================
FUNCTION tdsdlphch_update_demode_mode_for_power_report

DESCRIPTION
  None

DEPENDENCIES
  None

RETURN VALUE
  None
  
SIDE EFFECTS
  None
===========================================================================*/
void tdsdlphch_update_demode_mode(tdsdlphch_cfg_handle_type phch_cfg_db_idx, tdsdlphch_notify_enum_type operation);

/*===========================================================================
FUNCTION tdsdlphch_get_demode_mode_for_power_report

DESCRIPTION
  None

DEPENDENCIES
  None

RETURN VALUE
  None
  
SIDE EFFECTS
  None
===========================================================================*/
extern uint8 tdsdlphch_get_demode_mode(void);
#endif

#endif /* DLPHCH_H */

