#ifndef MCALWCDMA_DLI_H
#define MCALWCDMA_DLI_H

/*============================================================================*/
/** @file
 * This module has definition and declaration related to MCAL UMTS downlink
 * channel setup module APIs
 */
/*============================================================================*/

/*============================================================================
Copyright (c) 2007 - 2013 Qualcomm Technologies, Inc.  All Rights Reserved.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government. Diversion contrary to U.S. law prohibited.
============================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/wcdma/l1/offline/src/mcalwcdma_dli.h#1 $
$DateTime: 2016/12/13 08:00:12 $
$Author: mplcsds1 $

when        who     what, where, why
--------    ---     --------------------------------------------------------
09/18/15    kcm     Add 10bpg to the DPCH cfg evt time if it falls too close to the frame boundary
05/25/15    sl      Revert Fix for CR812954 & disable SFN/CFN gen tl event while slamming to new counter
02/11/15    pr      Add 10bpg to the DPCH cfg evt time if it falls too close to the frame boundary
05/13/14    ymu     Feature name FEATURE_WCDMA_DC_HSUPA_FRAME replacement with FEATURE_WCDMA_DC_HSUPA 
11/08/13    pr      Increment config CFN for ASAP add cases when current CFN 
                    is close to the frame boundary.
10/14/13    sr      Corrected combiner to phch mapping
10/14/13    pr/sr   Channel config optimizations
05/21/13    sad     Added DC HSUPA code changes under FEATURE_WCDMA_DC_HSUPA 
01/03/13    dm      When we miss the config window narrowly, set up combiner config 
                    event at the middle of config window.
07/24/12    geg     Upmerge WCDMA.MPSS.1.0 to WCDMA.MPSS.2.0
06/11/12    vs      MBMS feature cleanup.
06/08/12    gv      Feature HS RACH Initial code check in
02/12/12    hk      Nikel feature cleanup
01/06/11    vb      Modified cfg window end tick to be 125 for genesis
09/24/10    hk      Corrected the changes to perform the config window check in mdsp pre cb
09/17/10    hk      Changes to perform the config window check in mdsp pre cb
08/11/10    hk      Changing the cofing window boundary end to 11th slot for Genesis
08/03/10    hk      Invalidate the contents of phychan config FW interface data
                    structure after allocating the interface index
04/30/10    ks      Increased ch config window size for genesis (40-130th tick)                    
05/25/09    hk      Fixed compiler warnings
03/10/09    rgn     Removed chan_cfg_time_diff from mcal phychan db
02/19/09    rgn     Added channel config time diff to prevent combiner config for
                    for the same combiner in two consecutive frames
10/08/08    rgn     Moved the cfg window to start of fourth slot
06/17/08    vsr     Moved mdsp_phychan_enum_type to MCAL DL from mdspsync
06/16/08    vsr     Offline modem MCAL DL code cleanup
05/06/08    vsr     Added use of stmr int3 under FEATURE_WCDMA_STMR_INT3_SUPPORT
03/24/08    vsr     Included variable to hold wcb start address for each 
                    hw channel
03/24/08    vsr     Defined macro for Config Event firing time
01/31/08    vb      modified feature name for MBMS
01/30/08    vp      added recfg to active state
01/25/08    vsr     Checking in MCAL files onto mainline
07/05/07    vsr     Merged data structures and modified combiner evt mapping 
                    functions to support MCAL-EDL
06/08/07    vsr     MCAL_DL code cleanup changes
05/23/07    vsr     Added support for combiner log packet
04/21/07    vsr     Lint Fix
26/01/07    vb      added code to integrate mcalwcdma evt module

===========================================================================*/

/* ==========================================================================
** Includes and Public Data Declarations
** ========================================================================*/
/* -----------------------------------------------------------------------
** Include Files
** ----------------------------------------------------------------------- */
#include "wcdma_variation.h"
#include "comdef.h"
#include "mcalwcdma_dl.h"
#include "tlm.h"

/* -----------------------------------------------------------------------
** Constant / Define Declarations
** ----------------------------------------------------------------------- */

/* Macro to convert TTO which is in 256 chip units to TLM base units whioch is currently in cx8 resolution */ 
#define MCALWCDMA_GET_TTO_IN_TLM_UNITS(tto)  \
    (MCALWCDMA_256CHIPS_TO_CX8(tto))


/* The start and end of channel config window are placed at 20th and 120th ticks
 * respectively */
#define MCALWCDMA_DL_CFG_WINDOW_START (40 << CHIPX8_TO_BPG_RSHIFT)
/* Modified cfg window end tick to be 125 as cfg window check now happens at mdsp sync cmd pre-callback */
#define MCALWCDMA_DL_CFG_WINDOW_END (125 << CHIPX8_TO_BPG_RSHIFT)
#define MCALWCDMA_DL_CFG_EVT_FIRING_START_TIME (41 << CHIPX8_TO_BPG_RSHIFT)

#define MCALWCDMA_DL_CFG_EVT_FIRING_BUFFER_TIME (5 << CHIPX8_TO_BPG_RSHIFT)

/* the cfg time minimum limit is set to 1. What this means is that if a combiner
 * config is sent at frame x, next config for the same combiner can go only at
 * frame x + 2*/
#define MCALWCDMA_DL_CFG_TIME_MINIMUM_LIMIT 1

#define MCALWCDMA_DL_CFG_WINDOW_START_FOR_RESLAM (10 << CHIPX8_TO_BPG_RSHIFT)

#define MCALWCDMA_DL_CFG_WINDOW_END_FOR_RESLAM (140 << CHIPX8_TO_BPG_RSHIFT)

#define MCALWCDMA_DL_CFG_WINDOW_START_FOR_DROP (0 << CHIPX8_TO_BPG_RSHIFT)
#define MCALWCDMA_DL_CFG_WINDOW_END_FOR_DROP (150 << CHIPX8_TO_BPG_RSHIFT)

#define MCALWCDMA_DL_CFG_WINDOW_END_FOR_AUTO_PCH (135 << CHIPX8_TO_BPG_RSHIFT)

#define MCALWCDMA_DL_INTERFACE_BUF_ID_MAX WFW_DEMOD_NUM_MAX_PHCHS

#define MCALWCDMA_DL_ADV_CFG_EVT_FIRING_TIME_MIN (145 << CHIPX8_TO_BPG_RSHIFT)
#define MCALWCDMA_DL_ADV_CFG_EVT_FIRING_TIME_MAX (150 << CHIPX8_TO_BPG_RSHIFT)

uint32 mcalwcdma_dl_phch_cfg_upd_bmsk;
uint32 mcalwcdma_dl_phch_cfg_sent_bmsk;
/* -----------------------------------------------------------------------
** Type Declarations
** ----------------------------------------------------------------------- */

typedef struct{
/* if this evt info is in use */
boolean in_use;

/* for what combine config this evt id is in use */
uint8 comb_id;

/** evt handler */
MCALWCDMA_DL_EVENT_CB_TYPE evt_cb_func;
}mcalwcdma_dl_evt_id_comb_chan_mapping_info_struct_type;

/* Call back function type declaration */
/* ----------------------------------- */

/* Enum type declaration */
/* --------------------- */

/** This enumerates the various FW physical channel types */
typedef enum {
  MDSP_PCCPCH = WFW_DEMOD_PHCH_TYPE_PCCPCH,
  MDSP_SCCPCH = WFW_DEMOD_PHCH_TYPE_SCCPCH,
  MDSP_DPCH = WFW_DEMOD_PHCH_TYPE_DPCH,
  MDSP_HS_RACH_FDPCH = WFW_DEMOD_PHCH_TYPE_FDPCH,
  MDSP_PICH = WFW_DEMOD_PHCH_TYPE_PICH,
  MDSP_AICH = WFW_DEMOD_PHCH_TYPE_AICH,
  MDSP_HS_RACH_AICH = WFW_DEMOD_PHCH_TYPE_AICH,
  MDSP_FDPCH = WFW_DEMOD_PHCH_TYPE_FDPCH,
#ifdef FEATURE_WCDMA_DC_HSUPA 
  MDSP_SEC_FDPCH = WFW_DEMOD_PHCH_TYPE_FDPCH,
#endif
  NUM_MDSP_PHCH_TYPES = WFW_DEMOD_PHCH_NUM_TYPE
} mdsp_phychan_enum_type;

/** This enumerates the various possible states of a downlink physical channel */
typedef enum
{
  /** The free state of channel, when it ready for any kind of addition */
  MCALWCDMA_DL_PHYCHAN_INACTIVE,
  /** This is when the channel is active in software and MDSP sense */
  MCALWCDMA_DL_PHYCHAN_ACTIVE,
  /** This is when channel is under configuration for addition */
  MCALWCDMA_DL_PHYCHAN_CFG_TO_ACTIVE,
  /** This is when channel is under re-configuration*/
  MCALWCDMA_DL_PHYCHAN_RECFG_TO_ACTIVE,
  /** This is when channel is under configuration for drop */
  MCALWCDMA_DL_PHYCHAN_CFG_TO_INACTIVE,
  /** Number of physical channel types */
  MCALWCDMA_NUM_DL_PHYCHAN_STATES
} mcalwcdma_dl_phychan_state_enum_type;

/* Structure type declaration */
/* -------------------------- */

/** This structure is used to contain the parameters for the downlink
  * physical channel config command to the mDSP.  Also included are
  * masks for the valid bits for each field */
typedef struct
{
  /** Current state for the channel */
  mcalwcdma_dl_phychan_state_enum_type dl_hwch_state;

  /** TRUE if it is reserved */
  boolean rsvd;

  /** for which physical channel type this hwch is reserved */  
  mcalwcdma_dl_phychan_enum_type rsvd_for_phch;

  /** ID of the event used by the MCAL_EVT for channel configuration purposes */
  uint8 config_evt_id;

  /** Timing information of the channel at the configuration time */
  tlm_ch_cfn_info_struct_type cfn_info; 
  
  /** DL physical channel information as passed on by the user client */
  mcalwcdma_dl_combiner_cfg_parms_struct_type *comb_chan_info_ptr;

  /** Number of Radio links that are linked to the combiner */
  uint8 phychan_set_cnt;


  /** Pointer to array of physical channel configuration parameters for the combiner */
  mcalwcdma_dl_rl_phychan_parms_struct_type* phychan_set_ptr[MCALWCDMA_DEMOD_MAX_RL_SET];
} mcalwcdma_dl_phychan_db_entry_struct_type;

/*===========================================================================
FUNCTION        mcalwcdma_dl_get_evt_id_for_comb_chan

DESCRIPTION     This function returns evt id for the given combiner.

DEPENDENCIES    None

RETURN VALUE    evt id

SIDE EFFECTS    None
===========================================================================*/
uint8 mcalwcdma_dl_get_evt_id_for_comb_chan(uint8 comb_id);

/* -----------------------------------------------------------------------
** Global Data Declarations
** ----------------------------------------------------------------------- */

/* =======================================================================
**                        Function Declarations
** ======================================================================= */

/*============================================================================*/
/**
  * This validates the rl set physical channel with specified parameters.
  * Returns result of validation
  */
/*============================================================================*/

boolean mcalwcdma_dl_rl_phychan_parms_validate(
  /** Parameters for setting up the downlink physical channel */
  mcalwcdma_dl_rl_phychan_parms_struct_type **parms_set, mcalwcdma_dl_phychan_enum_type phychan_type,
  uint8 cnt);


/*============================================================================*/
/**
  * This validates the addition of a physical channel with specified parameters.
  * Returns result of validation
  */
/*============================================================================*/

boolean mcalwcdma_dl_phychan_add_validate(
  /** Parameters for setting up the downlink physical channel */
  mcalwcdma_dl_phch_cfg_parms_struct_type *phychan_cfg_parms);


/*============================================================================*/
/**
  * This validates the reconfiguration of an existing physical channel on the
  * combiner channel with specified ID. Returns result of validation
  */
/*============================================================================*/

boolean mcalwcdma_dl_phychan_reconfigure_validate(
  /** Combiner Id for the channel to be reconfigured */
  uint8 comb_id,
  /** Parameters for setting up the downlink physical channel */
  mcalwcdma_dl_phch_cfg_parms_struct_type *phychan_cfg_parms);


/*============================================================================*/
/**
  * This validates if dropping the physical channel on the combiner channel
  * with specified ID is feasible. Returns result of validation
  */
/*============================================================================*/

boolean mcalwcdma_dl_phychan_drop_validate(
  /** Parameters for the downlink physical channel */
  mcalwcdma_dl_phch_cfg_parms_struct_type *phychan_cfg_parms,
  /** Combiner Id for the channel to be dropped */
  uint8 comb_id);


/*============================================================================*/
/**
 * Function for update database. This function will update pointers that are provided by user modules and also it does 
 * allocate event id for the configuration if it is necessary.
 *
 * @return TRUE if allocation of event id is succesful else FALSE
 */
/*============================================================================*/

boolean mcalwcdma_dl_phychan_update_db(uint8 comb_id,
  /** Parameters for setting up the downlink physical channel */
  mcalwcdma_dl_phch_cfg_parms_struct_type *phch_cfg_parms_ptr);

/*===========================================================================
FUNCTION         MCALWCDMA_DL_MDSP_PRE_CB

DESCRIPTION
  Callback function that mDSP calls before acting on the command sent.
  This function gets the combiner index based on the current mDSP command
  and invokes mcalwcdma_dl_compute_config_cfn to calculate the config CFN

DEPENDENCIES
  None
  
RETURN VALUE 
  None

SIDE EFFECTS
  None
  
===========================================================================*/
void mcalwcdma_dl_mdsp_pre_cb(void);

/*===========================================================================
FUNCTION         MCALWCDMA_DL_COMPUTE_CONFIG_CFN

DESCRIPTION
  This function does the below actions,
  1) Calculates the config CFN
  2) Checks the config window boundary
  3) Updates the config CFN in the phychan interface structure if in config window (Imp)
  4) If not in config window, sets up the config event in the next frame

DEPENDENCIES
  None
  
RETURN VALUE 
  None

SIDE EFFECTS
  None
  
===========================================================================*/
void mcalwcdma_dl_compute_config_cfn(uint8 ch_idx);

/*============================================================================*/
/**
  * This function is the callback handler for channel's config event to
  * trigger sending the configuration to MDSP
  */
/*============================================================================*/
void mcalwcdma_dl_phychan_cfg_evt_cb(
/** combiner id */
  uint8 db_idx);

/*============================================================================*/
/**
  * This function finds the associated MCAL physical channel index for the mDSP phychan type
  * 
  */
/*============================================================================*/
mcalwcdma_dl_phychan_enum_type mcalwcdma_dl_get_mcal_phch_for_mdsp_phch(
  mdsp_phychan_enum_type mdsp_phch);

/*============================================================================*/
/** This function is the interrupt handler for R99 Channel Configuration done notification */
/*============================================================================*/
void mcalwcdma_dl_r99_chan_drop_done_interrupt_handler(void);

/*============================================================================*/
/**
 * This function finds the combiner channel that is being used for the given physical channel.
 *
 * @return DL channel Id that is reserved for the channel. If no combiner channel
 * is available it will return invalid ch id
 *
 * @see MCALWCDMA_DL_INVALID_HWCH_ID
 */
/*============================================================================*/
uint8 mcalwcdma_dl_get_comb_id_for_interface_buf(uint32 interface_buf_id);

#endif /* MCALWCDMA_DLI_H */

