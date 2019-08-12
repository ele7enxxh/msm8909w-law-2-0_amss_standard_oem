#ifndef WL1TRII_H
#define WL1TRII_H

/*===========================================================================
                           WL1TRII . H

GENERAL DESCRIPTION
  This file contains internal variables and functions of triage module. This file will not contains any exported data to other
  module

EXTERNALIZED FUNCTIONS

INTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2000-2013 by Qualcomm Technologies, Inc.  All Rights Reserved.

===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/wcdma/l1/offline/src/wl1trii.h#1 $
$DateTime: 2016/12/13 08:00:12 $
$Author: mplcsds1 $

when        who     what, where, why
--------    ---    --------------------------------------------------------
03/17/14    mit    Changes to accommodate dynamic cell_db_idx allocation
03/18/14    mit    F3 cleanup, enabling via NV's
05/13/14    ymu    Feature name FEATURE_WCDMA_DC_HSUPA_FRAME replacement with FEATURE_WCDMA_DC_HSUPA 
04/02/14    pr     Fix for KW errors.
04/10/14    sr     Changes to cleanup "antenna" from finger database
02/28/14    sad    SCCH order optimization check in
02/05/14    pr     Added support for DCHSUPA Finger Switching Ops.
10/17/13    bs     Preliminary 3C Support
05/21/13    sad    Added DC HSUPA code changes under FEATURE_WCDMA_DC_HSUPA 
11/06/12    kcm    Reduction of debug f3s in idle mode.
12/28/12    sr     Exclude the cell under configuration from triage operation.
11/12/12    dm     Changes to register a callback with triage if any module 
                   wants to do finger related processing while triage is running.
08/01/12    sr     Reduction in Debug F3s
07/24/12    geg    Upmerge WCDMA.MPSS.1.0 to WCDMA.MPSS.2.0
06/06/12    kcm    Optimization of bounding conditions for standoff/sidelobe algorithms 
01/29/12    vs     Feature cleanup
07/07/11    hk     Up-merge from offline/main/latest
03/16/11    ks     Added bmsk in internal db to track reassignable fingers
12/21/10    ks     Added a macro to query config status of an active RL  
12/20/10    ks     Added explicit check for cell status in DL_TRI_RL_ACTIVE macro
08/17/10    ks     Constants for delay triage clients
05/17/10    ks     Changed fing mask from 16->32 bit for rxd operations
02/26/10    rvs    Fixes for Mustang.
02/27/10    hk     Added new macros for number of fingers on primary and 
                   secondary carriers
02/10/10    hk     Featurized the Dual carrier specific code
02/09/10    hk     Added max_paths to the carrier specific info structure
01/22/09    rgn    SSID to be used for fingers and antenna id to be used for paths
07/16/09    hk     Reorganized the code to remove INC/SRC violations
06/04/09    ks     Added RxD operations enum type
06/03/09    ks     Changes related to mob requirements, moved some structs
                   to wl1tri.h.
04/14/09    ks     Added RxD op related elements in internal db. Fixed compiler
                   warnings 
02/09/09    vb     Added defines to access triage rl info state
01/20/09    ks     Modified WL1_TRI_MSG to be driven by global
01/12/09    ks     Changed WL1_TRI_MSG to MSG_LOW
12/22/08    ks     Mainlining FEATURE_WCDMA_OFFLINE_DEMOD_SUPPORT
12/10/08    ks     Mainlining FEATURE_WCDMA_DL_ENHANCED
11/11/2008  ks     Added reaserved pool
10/24/2008  hk     Lint Fixes
10/16/2008  ks     Removed antenna enum type
10/02/2008  ks     Added assn/deassn mask in internal db
10/01/2008  ks     Added ref_fing_cfg_cell_idx_bmsk in internal db
09/29/2008  ks     Added pool_type in dltri_fing_pool_struct_type
09/26/2008  vb     Modified pool management data structures
08/27/2008  ks     added strucutre member fing_deassign_in_prog_bmsk to
                   mark fingers that are in deassign in progress for given RL.
                   mostly used in case of cell drop
07/16/2008  ks     first cut of triage with new design

===========================================================================*/


/*--------------------------------------------------------------------------
                      Included Header Files
--------------------------------------------------------------------------*/
#include "wcdma_variation.h"
#include "comdef.h"
#include "dltriparm.h"
#include "msm.h"
#include "srchset.h"
#include "whl1mcalif.h"
#include "wl1tri.h"

/* Macros internal to triage */

#define DL_TRI_INVALID_CARR_IDX (0xFF)

#define PN_POS_DIFF(x, y)  \
   ((labs((int32)((int32)(x) - (int32)(y))) < CHIPX4_PER_FRAME ) ? labs((int32)((int32)x-(int32)y)) : (CHIPX8_PER_FRAME-labs((int32)((int32)x-(int32)y))))

/* return finger's antenna type PRIM/DIV */
#ifdef FEATURE_WCDMA_DC_HSDPA
#define FING_ANTENNA_TYPE(f) ((demod_fing_status_dbase[(f)].ssid) & FING_ANT_BIT_POS_MASK)
#else
#define FING_ANTENNA_TYPE(f) (demod_fing_status_dbase[(f)].ssid)
#endif

#define DL_TRI_RL_ACTIVE(cell_db_idx) ((dltri_rl_info[(cell_db_idx)].enabled) && \
                                      (dltri_cell_deactivation_on_progress[(cell_db_idx)] == FALSE) && \
                                      ((dl_cell_get_cell_configured_status(cell_db_idx) == TRUE)))

#define DL_TRI_RL_ACTIVE_IN_PROGRESS(cell_db_idx) ((dltri_rl_info[(cell_db_idx)].enabled) && \
                                                  (dltri_cell_deactivation_on_progress[(cell_db_idx)] == FALSE) && \
                                                  (dl_cell_get_cell_config_on_prog_status(cell_db_idx) == TRUE))

#define DL_TRI_RL_ACTIVE_IN_PROGRESS_FOR_TRIAGE(cell_db_idx) ((dltri_rl_info[(cell_db_idx)].enabled) && \
                                                              (dltri_cell_deactivation_on_progress[(cell_db_idx)] == FALSE) && \
                                                              (dl_cell_get_cell_config_on_prog_for_triage(cell_db_idx) == TRUE))

#define DL_TRI_RL_CONFIGURED(cell_db_idx) (dltri_rl_info[(cell_db_idx)].enabled)

#define DL_TRI_IS_RL_ACTIVE(cell_db_idx) ((dltri_rl_info[(cell_db_idx)].enabled) && \
                                         (dltri_cell_deactivation_on_progress[(cell_db_idx)] == FALSE) && \
                                         (dl_cell_get_cell_config_status(cell_db_idx) == TRUE))

#define DLTRI_INVALID_FING_ID (0xFF)

#define DLTRI_IS_FING_OUT_OF_BOUNDS(fing) ((fing >= MSMHW_MAX_NUM_FINGERS) ? TRUE: FALSE)

/* to check if a finger is part of given mask */
#define DLTRI_IS_FING_IN_MASK(mask, fing) (((mask & (1<<fing)) == 0x0) ? FALSE : TRUE)


/* Maximum number of samples ever contained in the above sum.  Sampling at
** 40 msec (triage rate), this is 2 seconds worth of data. */
#define DL_TRI_MAX_FREQ_ERROR_SAMPLES  50

/*Invalid antenna ID*/
#define NO_ANTENNA (0xFF)

/* Macro for invalid PSC */
#define DLTRI_NO_PSC 0x0FFF

//#define DLTRI_PATH_ENG(path) (dl_tri_paths[path].eng)

#define DLTRI_PATH_ENG(carr_idx, path) (dl_tri_carr_info[(carr_idx)].path_info[(path)].eng)

//#define DLTRI_PATH_CELL_ID(path) (dl_tri_paths[path].cell_db_idx)

#define DLTRI_PATH_CELL_ID(carr_idx, path) \
  (dl_tri_carr_info[(carr_idx)].path_info[(path)].cell_db_idx)

#define DLTRI_FING_PATH(fing) (demod_fing_status_dbase[fing].path_idx)

/* TRUE if fing on path else FALSE */
//#define DLTRI_IS_FING_ON_PATH(path) (dl_tri_paths[path].fing != NO_FINGER) ? TRUE : FALSE

#define DLTRI_IS_FING_ON_PATH(carr_idx, path) \
  (dl_tri_carr_info[(carr_idx)].path_info[(path)].fing != NO_FINGER) ? TRUE : FALSE

#define DL_TRI_GET_CARR_IDX_FROM_CELL_IDX(cell_idx) \
  dl_cell_get_carr_idx_from_cell_db_idx(cell_idx)

#define WL1_TRI_MSG(str, a, b, c) \
if (dltri_debug_msg_enable) \
{ \
 MSG_ERROR(str, a, b, c); \
}\
else \
{ \
  MSG_LOW(str, a, b, c); \
}

#define WL1_TRI_CONN_MSG(str, a, b, c) \
if (dltri_dbg_msgs_drx_state) \
{ \
 MSG_HIGH(str, a, b, c); \
}\
else \
{ \
  MSG_LOW(str, a, b, c); \
}


#define WL1_TRI_MSG_4(str, a, b, c, d) \
if (dltri_debug_msg_enable) \
{ \
    MSG_4(MSG_SSID_DFLT, MSG_LEGACY_HIGH, str, a, b, c, d);\
}\
else \
{ \
  MSG_4(MSG_SSID_DFLT, MSG_LEGACY_LOW, str, a, b, c, d);\
}

#define WL1_TRI_MSG_5(str, a, b, c, d, e) \
if (dltri_debug_msg_enable) \
{ \
    MSG_5(MSG_SSID_DFLT, MSG_LEGACY_HIGH, str, a, b, c, d, e);\
}\
else \
{ \
  MSG_5(MSG_SSID_DFLT, MSG_LEGACY_LOW, str, a, b, c, d, e);\
}

#define WL1_TRI_MSG_6(str, a, b, c, d, e, f) \
if (dltri_debug_msg_enable) \
{ \
  MSG_6(MSG_SSID_DFLT, MSG_LEGACY_HIGH, str, a, b, c, d, e, f);\
}\
else \
{ \
  MSG_6(MSG_SSID_DFLT, MSG_LEGACY_LOW, str, a, b, c, d, e, f);\
}

#define WL1_TRI_MSG_8(str, a, b, c, d, e, f, g, h) \
if (dltri_debug_msg_enable) \
{ \
  MSG_8(MSG_SSID_DFLT, MSG_LEGACY_HIGH, str, a, b, c, d, e, f, g, h);\
}\
else \
{ \
  MSG_8(MSG_SSID_DFLT, MSG_LEGACY_LOW, str, a, b, c, d, e, f, g, h);\
}

#define DL_TRI_IS_CELL_DB_IDX_INVALID(cell_db_idx)    \
    (cell_db_idx >= DL_MAX_NUM_CELLS)

/* constants for delay triage clients */
#define DL_TRI_DELAY      0x1
#define DL_TRI_DELAY_HS   0x8
#define DL_TRI_DELAY_EDL  0x10
/* Structures internal to triage */

/* enum type for rxd state */
typedef enum {
  /* RxD ON in progress */
  DLTRI_RXD_ON,
  /* RxD OFF in proegress  */  
  DLTRI_RXD_OFF,
  
  DLTRI_RXD_MAX_TRANS_STATE
} dltri_rxd_xition_enum_type;

typedef struct
{
  dl_tri_fing_pool_mgr_enum_type pool_type;
  uint8 num_pool_fing;
  uint32 pool_mask;
}dltri_fing_pool_struct_type;

typedef struct
{
  /*bitmask of modules waiting while triage is running*/
  uint8 modules_waiting_bmsk;
  /*call back to be posted once triage is done*/
  WL1_TRI_CFG_REQ_POSTCB post_cb[MAX_MODULES];
} dl_tri_register_post_cb_struct_type;

typedef struct
{
  /* set bit in this bitmask represents fingers don't need deassignment in given triage cycle */
  uint32 dltri_fing_exempt_from_deassign_mask;

  /* count of total mandatory paths. Paths from index 0..(mandatory_path_count-1) in dl_tri_paths 
      represents mandatory paths and they need finger in a given triage cycle */
  uint8 mandatory_path_count;

  /* number of fingers needed for mandatory paths */
  uint8 mandatory_fing_need_count;

  /* number of fingers that can be re-assigned for one triage session */
  uint16 num_reassignable_fings;

  /* index to sorted fing array that can point to available finger for de-assignment */
  uint8 sorted_arr_idx_for_available_fing;

  /* fing bmsk of reassignable fingers */
  uint32 reassign_fing_list_mask;

  /* this is our final path mask which need finger assignment. All set bit should get a finger in given triage cycle */
  uint64 path_need_fing_mask;

  dltri_fing_pool_struct_type pool_info[DL_FING_NUM_POOL];

  /* finger assigned to THE strongest path*/
  uint8 fing_to_strongest_path; 

 /* cells that need finger from triage module requested by cell cfg module */
  uint16 cell_cfg_cell_need_fing_mask;

  /* cell id for which ref fing got updated */
  uint16 ref_fing_cfg_cell_idx_bmsk;

  uint32 assn_mask_for_commit;

  uint32 deassn_mask_for_commit;
  
  /* to flag rxd on/off operation. TRUE: rxd getting disabled  FALSE: rxd getting enabled. Don't rely on this flag to
     check rxd on/off state instead use dltri_rxd_enabled */
  boolean tri_rxd_state_xition_to_disable;

  /* TRUE if RxD OFF in progress, false otherwise */
  dltri_rxd_xition_enum_type rxd_xition_cmd;

  /* this mask contains fingers which are getting reconfigured due to rxd state xition */
  uint32 rxd_op_fing_reconf_mask;

  /* this mask contains fingers which are getting deassigned due to rxd state xition */
  uint32 rxd_op_fing_deassn_mask;
  
  /*Num tri cycles combiner rssi threshold was satisfied*/
  uint32 num_tri_cyc_comb_rssi_thresh_satisfied;
  
  /*modules waiting while triage is running*/
  dl_tri_register_post_cb_struct_type dl_tri_register_post_cb;

  /*Flag to track the triage operations*/
  boolean triage_ops_in_progress;

  /* Flag to track when to skip primary triage for scch order activation */
  boolean skip_triage_this_cycle;
}dltri_internal_struct_type;

typedef struct {

  uint8 fing_start;

  uint8 fing_end;

  uint8 max_fings;

  uint8 max_usable_fing_idx;

  dltri_internal_struct_type *int_db_ptr;

  dl_tri_path_struct_type *path_info;

  uint8 *num_paths;

  uint8 max_paths;

  uint8 *reassign_fing_list_ptr;

  dl_tri_rl_set_struct_type *rl_sets_ptr;
} dl_tri_carr_info_struct_type;

/*
 -----------     ---
|                 |      ^
|----------        |
|                 |   mandatory paths
-----------        |
|                 |      V
|----------  <--mandatory_path_count
|                 |      ^
.----------        |
.                 .   remainign paths which qualifies for finger assignment 
.                 .       |
.                 .       V
|---------- <--last_path_need_fing_index       
|                |
- ----------
dl_tri_paths[]

*/

#define DL_TRI_GET_FING_MASK_FOR_POOL(carr_idx, pool) \
  (dl_tri_carr_info[(carr_idx)].int_db_ptr->pool_info[(pool)].pool_mask)
//  (dltri_internal_db.pool_info[(pool)].pool_mask)

#define DL_TRI_GET_FING_CNT_FOR_POOL(carr_idx, pool) \
  (dl_tri_carr_info[(carr_idx)].int_db_ptr->pool_info[(pool)].num_pool_fing)
//  (dltri_internal_db.pool_info[(pool)].num_pool_fing)

#define DL_TRI_GET_POOL_FING_MASK_FOR_FING_STATE(carr_idx, state) \
  (dl_tri_carr_info[(carr_idx)].int_db_ptr->pool_info[dltri_fing_state_to_pool_mapping_tbl[(state)]].pool_mask)
//  (dltri_internal_db.pool_info[dltri_fing_state_to_pool_mapping_tbl[(state)]].pool_mask)

#define DL_TRI_GET_POOL_FING_CNT_FOR_FING_STATE(carr_idx, state) \
  (dl_tri_carr_info[(carr_idx)].int_db_ptr->pool_info[dltri_fing_state_to_pool_mapping_tbl[(state)]].num_pool_fing)
//  (dltri_internal_db.pool_info[dltri_fing_state_to_pool_mapping_tbl[(state)]].num_pool_fing)


#define DL_TRI_GET_FING_START_IDX_FOR_CARR(carr_idx) \
  (dl_tri_carr_info[(carr_idx)].fing_start)

#define DL_TRI_GET_FING_END_IDX_FOR_CARR(carr_idx) \
  (dl_tri_carr_info[(carr_idx)].fing_end)

#define DL_TRI_GET_MAX_FINGS_FOR_CARR(carr_idx) \
  (dl_tri_carr_info[(carr_idx)].max_fings)

#define DL_TRI_GET_MAX_USABLE_FING_IDX_FOR_CARR(carr_idx) \
  (dl_tri_carr_info[(carr_idx)].max_usable_fing_idx)

#define DL_TRI_GET_INT_DB_PTR_FOR_CARR(carr_idx) \
  (dl_tri_carr_info[(carr_idx)].int_db_ptr)

#define DL_TRI_GET_PATHS_PTR_FOR_CARR(carr_idx) \
  (dl_tri_carr_info[(carr_idx)].path_info)

#define DL_TRI_GET_RL_SETS_PTR_FOR_CARR(carr_idx) \
  (dl_tri_carr_info[(carr_idx)].rl_sets_ptr)

#define DL_TRI_GET_NUM_PATHS_FOR_CARR(carr_idx) \
  (dl_tri_carr_info[(carr_idx)].num_paths)

#define DL_TRI_GET_MAX_PATHS_FOR_CARR(carr_idx) \
  (dl_tri_carr_info[(carr_idx)].max_paths)

#define DL_TRI_GET_REASSIGN_FING_LIST_PTR_FOR_CARR(carr_idx) \
  (dl_tri_carr_info[(carr_idx)].reassign_fing_list_ptr)

/* returns TRUE if fing id is within limit else returns FALSE */
#define DLTRI_FING_ID_CHECK(carr_idx, fing_id) \
  ((fing_id > DL_TRI_GET_MAX_USABLE_FING_IDX_FOR_CARR(carr_idx)) ? FALSE :\
  (fing_id >= DL_TRI_GET_FING_START_IDX_FOR_CARR(carr_idx)) ? TRUE : FALSE)

#define DLTRI_FING_ID_MAX_CHECK(carr_idx, fing_id) \
  ((fing_id > DL_TRI_GET_FING_END_IDX_FOR_CARR(carr_idx)) ? FALSE :\
  (fing_id >= DL_TRI_GET_FING_START_IDX_FOR_CARR(carr_idx)) ? TRUE : FALSE)

#define DLTRI_CELL_ID_CHECK(carr_idx, cell_idx) \
  ((dl_cell_info.carr_cell_resource_info[carr_idx].alloc_cell_idx_bmask & (1<<cell_idx)) == 0) ? FALSE : TRUE



/******************************************************************************
*******************************************************************************
                             EXTERNAL VARIABLES
*******************************************************************************
******************************************************************************/


#endif /* WL1TRII_H */
