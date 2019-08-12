#ifndef DLCELLCFG_H
#define DLCELLCFG_H
/*===========================================================================
                 WCDMA L1 DL CELL DATA BASE

GENERAL DESCRIPTION
  This file contains the code for allocate/deallocate/provide cell db Index 
  and maintaines necessary cell info.

EXTERNALIZED FUNCTIONS

INTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS


Copyright (c) 2007 - 2013 by Qualcomm Technologies, Inc.  All Rights Reserved.


*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/wcdma/l1/offline/src/dlcellcfg.h#1 $ $DateTime: 2016/12/13 08:00:12 $ $Author: mplcsds1 $

when        who     what, where, why
--------    ---     --------------------------------------------------------- 
02/24/15    ap      Initial check-in for WL1 QSH framework 
01/20/15    kcm     Call MCVS for shadow cell update in task context
11/7/14     sks     Redesign of MCVS algo.
07/31/14    sr      Fixed low level warnings
07/03/14    sks     Changes for FCDC
04/30/14    pr      Issue a Demod status dump every frame if TxD detection 
                    is in progress for any cell
03/17/14    mit     Changes to accommodate dynamic cell_db_idx allocation
03/18/14    mit     F3 cleanup, enabling via NV's
02/05/14    pr      Added support for DCHSUPA Finger Switching Ops.
10/29/13    sad     Inital check in for Secondary ASET update changes
10/17/13    bs      Preliminary 3C Support
09/02/13    hk      Changed the TD detection done cell bit mask to 16-bit
07/10/13    kcm     Use mcal macros for number of cells on primary and secondary 
                    carrier instead of FW macros
05/20/13    pr      SW support for FW Shadow Cell
04/25/13    pr      Fix for TD dirty bit being overwritten by cell operation.
04/23/13    pr      Fixed Incorrect clearing of the TD Dirty Bit which results in 
                    wrong SW assumption of TD ongoing in FW
03/27/13    pr      SW Changes for TxD command done from FW.
02/22/13    dm      R99 SCHIC changes with MVCS.
02/13/13    kcm     Added cell update reason field in dl_cell_info and exported
                    an API to return cell cfgd/recfg due to ref fing change
                    status
10/22/12    kcm     Reducing F3 prints in Idle Mode scenarios
01/09/13    pr      Reverted Change of Cell Idx start for C1 from Idx8 back to Idx7.
11/13/12    kcm     Signal DRX only when the whole fast cell deconfig procedure is done
12/28/12    sr      Exclude the cell under configuration from triage operation.
09/17/12    ksr     Externs for new APIs for fast cell/fing decfg opt.
11/20/12    pr      Change Cell Idx start for C1 from Idx8.
11/19/12    pr      Mapped macro to ensure that SW is in sync with FW.
08/13/12    sr      Sync up the reference finger with the data base 
                    during ASET update.
07/24/12    geg     Upmerge WCDMA.MPSS.1.0 to WCDMA.MPSS.2.0
03/20/12    zr      Adding DL_CELL_DB_IDX_SEC_CARRIER_END macro
10/25/11    hk      Commit the pending demod config after removing the
                    concurrency with cell config
09/14/11    kcm     Corrected the computation of end cell idx
09/13/11    kcm     Initializing the DL_CELL_DB_IDX_SEC_CARRIER_START to 7
09/02/11    kcm     External API that returns the start cell idx and end cell
                    idx per carrier and added some MACROS for getting Max num 
                    cells per carrier. 
08/17/11    rgn     Increased the array size of cfg/decfg structure 
08/09/11    rgn     Calculate the cell start idx based on carrier idx
07/07/11    hk      Up-merge from offline/main/latest
06/17/11    ks      Added function to provide cell's state in mutex protected way
06/06/11    sv      Added API prototype to print cells in triage set.
05/06/11    amj     Increased the TD Detection timeout if DRX is active.
01/13/11    vsr     Added API to return available DC cells
01/07/11    sv      Externed API that returns cell state.
01/07/11    sv      Externed API that returns TD settings for a configured cell.
12/21/10    ks      Added prototype of an API to export cell config progress status
09/02/10    sv      Make the mutex lock for triage set pointer the same as srch 
                    cell mutex lock.
06/30/10    hk      Changes to update the triage set in the cell config done ISR
06/07/10    ms      Moved declarations within #ifndef DLCELLCFG_H
05/21/10    hk      Make sure cell is present in triage set before 
                    invoking client callback
03/23/10    rgn     Added processed cell list to the client cfg db 
02/26/10    rvs     Fixes for Mustang.
02/08/10    rgn     Syncup to mustang tip 
02/05/10    rgn     Removed enabling/disabling triage 
                    Expanded scope of mutex during notify clients
                    Added wrapper to provide dc cells 
02/02/10    hk      Use cell db idx to remove cell from triage set
11/11/09    rvs     Add function dl_cell_get_sch_invert().
07/20/09    rgn     Removed redundant scpich query function
07/15/09    rgn     Enhanced existing s-cpich support  
07/07/09    sv      Added externs for S-CPICH support.
05/08/09    uk      Externed API dl_cell_is_cell_under_decfg
04/17/09    rgn     Fixed compiler warnings
03/31/09    rvs     Remove DL_CELL_UPDATE_DUE_TO_EQUALISER from the
                    dl_cell_update_reason_enum_type type.
02/08/09    rgn     Added API to get DPCH SSC
01/28/09    vc      Added a new interface to get cell ptr based on the cell db index.
01/13/09    vsr     Changes to check FW status multiple times during TD 
                    determination phase
12/22/08    sv      Added boolean to indicate if SEQ CB registered.
12/03/08    rgn     Added support for Tx Diversity detection procedure 
11/10/08    uk      Changed fn name to dl_cell_is_cell_going_inactive
                    from dl_cell_is_cell_under_deconfig
11/10/08    uk      Externed dl_cell_is_cell_under_deconfig
11/10/08    uk      Defined DL_CELL_INVALID_CELL_DB_ID as SRCH_INVALID_CELL_DB_IDX
12/20/07    rgn     Initial version

===========================================================================*/

/* -----------------------------------------------------------------------
** Include Files
** ----------------------------------------------------------------------- */

#include "wcdma_variation.h"
#include "comdef.h"
#include "customer.h"
#include "mcalwcdma_demod.h"
#include "l1const.h"
#include "srchset.h"
#include "whl1mcalif.h"
#include "dltri.h"

/* -----------------------------------------------------------------------
** Constant / Define Declarations
** ----------------------------------------------------------------------- */
#define DL_CELL_INVALID_CELL_DB_ID SRCH_INVALID_CELL_DB_IDX

/**22 comes from 6 (dpch) + 9 (sccpch) + 1
(mich) + 1 (agch) + 1 (rgch) + 1 (hich)*/
#define DL_CELL_MAX_NUM_CB_CLIENTS 32

#define DL_CELL_MAX_EQ_IDX_VAL 3

#define DL_CELL_INIT_ALL_CELLS 0xFF

/** Init td determindation age */
#define DL_CELL_INVALID_TD_DET_AGE 0xFF

/** Invalid ssc */
#define DL_CELL_INVALID_SEC_SCR_CODE_IDX 0xFF

/** Maximum td determination age in frames, after this a
 *  status dump is performed to find the td detection status
 *  for a cell */
#define DL_CELL_MAX_TD_DET_AGE 0x9
/** if DRX is enabled and RX is off then we need to increase
 *  the TD detection maximum CFN to allow FW to collect the
 *  required number of samples */
#define DL_CELL_MAX_TD_DET_AGE_FW_GATING_FACTOR 8

/** check for validity of cell */
#define DL_CELL_CFG_IS_CELL_VALID(cell_db_idx) \
  (cell_db_idx < DL_MAX_NUM_CELLS)

/** check for validity of cell */
#define DL_CELL_CFG_IS_CELL_INVALID(cell_db_idx) \
  ((cell_db_idx == DL_CELL_INVALID_CELL_DB_ID) || \
   (cell_db_idx >= DL_MAX_NUM_CELLS))

/** check for validity of cell update reason */
#define DL_CELL_CFG_IS_CELL_UPDATE_REASON_VALID(cell_update_reason) \
  ((cell_update_reason & (DL_CELL_UPDATE_DUE_TO_CELL_RECFG | \
                          DL_CELL_UPDATE_DUE_TO_TXD_DET | DL_CELL_UPDATE_DUE_TO_SEC_CPICH_CHANGE | \
                          DL_CELL_UPDATE_DUE_TO_PRIMARY_REF_FING_CHANGE | \
                          DL_CELL_UPDATE_DUE_TO_SEC_REF_FING_CHANGE)))

#ifdef FEATURE_WCDMA_DC_HSDPA
/**Max num cells on Pri Carrier**/
#define DL_CELL_NUM_MAX_CELLS_PRI_CARRIER MCALWCDMA_MAX_NUM_CELLS_PRI_CARRIER

/**Max num cells on Sec Carrier**/ 
#define DL_CELL_NUM_MAX_CELLS_SEC_CARRIER MCALWCDMA_MAX_NUM_CELLS_SEC_CARRIER

/**Max num cells on ter carrier **/
#define DL_CELL_NUM_MAX_CELLS_TER_CARRIER MCALWCDMA_MAX_NUM_CELLS_TER_CARRIER

#else
#define DL_CELL_NUM_MAX_CELLS_PRI_CARRIER WFW_DEMOD_NUM_MAX_CELLS
#endif

#define DL_CELL_GET_ALLOCATED_CELL_IDX_BMASK(carr_idx) \
    dl_cell_info.carr_cell_resource_info[(carr_idx)].alloc_cell_idx_bmask

#define DL_CELL_FAST_CELL_DECFG_DONE DRX_PRL_RQ_DL_WRKLP_DONE_SIG

#define DL_CELL_GET_CARR_IDX_FRM_CELL_DB_IDX(cell_idx) \
               dl_cell_info.cell_info[cell_idx].cell_cfg_info.carrierIdx

#define DL_CELL_MSG(str, a, b, c) \
if (dl_cell_cfg_debug_msg_nv_enable) \
{ \
  MSG_HIGH(str, a, b, c); \
}\
else \
{ \
  MSG_LOW(str, a, b, c); \
}

#define DL_CELL_CONN_MSG(str, a, b, c) \
if (dl_cell_cfg_debug_msg_enable) \
{ \
 MSG_HIGH(str, a, b, c); \
}\
else \
{ \
  MSG_LOW(str, a, b, c); \
}

#define DL_CELL_CONN_MSG_E(str, a, b, c) \
if (dl_cell_cfg_debug_msg_enable) \
{ \
 MSG_ERROR(str, a, b, c); \
}

#define DL_CELL_CONN_MSG_6(str, a, b, c, d, e, f) \
if (dl_cell_cfg_debug_msg_enable) \
{ \
  MSG_6(MSG_SSID_DFLT, MSG_LEGACY_HIGH, str, a, b, c, d, e, f);\
}\
else \
{ \
  MSG_6(MSG_SSID_DFLT, MSG_LEGACY_LOW, str, a, b, c, d, e, f);\
}

#define DL_CELL_CONN_MSG_7(str, a, b, c, d, e, f,g) \
if (dl_cell_cfg_debug_msg_enable) \
{ \
  MSG_7(MSG_SSID_DFLT, MSG_LEGACY_HIGH, str, a, b, c, d, e, f, g);\
}\
else \
{ \
  MSG_7(MSG_SSID_DFLT, MSG_LEGACY_LOW, str, a, b, c, d, e, f, g);\
}

/** cell state enum types */
typedef enum
{
  /** cell is inactive   */
  DL_CELL_INACTIVE,
  /** cell in allocated state  */
  DL_CELL_ALLOCATED,
  /** cell configuration in progress   */
  DL_CELL_CONFIGURATION_IN_PROGRESS,
  /** cell is in configured state   */
  DL_CELL_CONFIGURED,
  /** cell deconfiguration in progress   */
  DL_CELL_DECONFIGURATION_IN_PROGRESS,
  /* total number of states*/
  DL_CELL_MAX_STATES,
  /** invalid cell state */
  DL_CELL_INVALID_CELL_STATE = DL_CELL_MAX_STATES
}dl_cell_state_enum_type;

/** cell update enum type */
typedef enum
{
  /** searcher is updating */
  DL_CELL_UPDATE_DUE_TO_CELL_RECFG = 0x1,
  /** Update for TXD determination */
  DL_CELL_UPDATE_DUE_TO_TXD_DET = 0x2,
  /** update due to change in secondary cpich params */
  DL_CELL_UPDATE_DUE_TO_SEC_CPICH_CHANGE = 0x4,
  /** primary ref finger change */
  DL_CELL_UPDATE_DUE_TO_PRIMARY_REF_FING_CHANGE = 0x8,
  /** secondary ref finger change */
  DL_CELL_UPDATE_DUE_TO_SEC_REF_FING_CHANGE = 0x10,
  /** update due to new cell */
  DL_CELL_UPDATE_DUE_TO_NEW_CELL = (DL_CELL_UPDATE_DUE_TO_CELL_RECFG |
                                    DL_CELL_UPDATE_DUE_TO_TXD_DET | DL_CELL_UPDATE_DUE_TO_SEC_CPICH_CHANGE |
                                    DL_CELL_UPDATE_DUE_TO_PRIMARY_REF_FING_CHANGE | DL_CELL_UPDATE_DUE_TO_SEC_REF_FING_CHANGE)
      

}dl_cell_update_reason_enum_type;

/** cell config notifier */
typedef enum
{
  /**Notify only when cell(s) are deactivated   */
  DL_CELL_NOTIFY_ON_DEACTIVATION,
  /**Notify when cell(s) go active */
  DL_CELL_NOTIFY_ON_ACTIVATION,
  /**Notify when cell(s) td detection is completed */
  DL_CELL_NOTIFY_ON_TD_DETECTION
}dl_cell_notify_action_enum_type;

typedef enum
{
	/** SCPICH r99 mode */
  DL_CELL_SCPICH_R99_MODE = WFW_DEMOD_CELL_SCPICH_NONMIMO_MODE,
	/** SCPICH mimo mode */
  DL_CELL_SCPICH_MIMO_MODE = WFW_DEMOD_CELL_SCPICH_MIMO_MODE,
}dl_cell_scpich_mimo_mode;

/** phch function callback ptr */
typedef void (*DL_CELL_CFG_CLIENT_FUNC_CB_PTR)(
  /** pointer to transaction notification id */
  uint8 transaction_notify_id,
  /** status of the cell config command */
  boolean status);

/** this struct contains the following information
 * 
 *  that needs to be passed to the client for the client to
 *  resume after a cell config is done
 * 
 *  That tells cell config module when to call the client cb ptr */
typedef struct
{
  /** dl phch config resume parameters   */
  /** phch config db idx */
  uint8 transaction_notify_id;
  /** active cell bmsk */
  uint16 active_cell_bmsk;
  /** original wait bmsk */
  uint16 orig_wait_cell_bmsk;
  /** processed cell mask */
  uint16 processed_cell_bmsk;
  /** asap bit */
  boolean asap;
  /** notify on activation/deactivation */
  dl_cell_notify_action_enum_type notify_action_cause;
  /**  cell cfg done callback pointer   */
  DL_CELL_CFG_CLIENT_FUNC_CB_PTR cell_cfg_client_cb_ptr;
}dl_cell_cfg_client_input_struct_type;

/** this struct contains the cell configuration details   */
typedef struct
{
  /** boolean to indicate this cell entry has been updated */
  boolean updated;
  /** cell index   */
  uint8 cell_db_idx;
  /** cell current state   */
  dl_cell_state_enum_type cell_current_state;
  /** cell update reason   */
  dl_cell_update_reason_enum_type cell_update_reason;
  /** detected td value */
  uint8 detected_td_value;
  /**uarfcn   */
  uint16 frequency;
  /** cell configuration parameters */
  dl_cell_config_info_struct_type cell_cfg_info;
  /** search cell pointer for this cell */
  srch_cell_struct_type *srch_cell_ptr;
  /** boolean to indicate if fast cell deconfig is in progress*/
  boolean dl_cell_fast_cell_decfg_in_progress;
}dl_cell_config_struct_type;


/** This struct contins the max num cells and cell_idx
 *  details per carrier**/
typedef struct{
    uint8 max_num_cells;
/**current numer of cells allocated per carrier*   */
    uint8 alloc_num_cells;
/** bit mask for allocated cell db idx's**/
    uint16 alloc_cell_idx_bmask;
}dl_cell_per_carr_cell_resource_struct_type;

/** this struct contains the parameters needed for clients to
 *  resume back after a cell configuration is done. It also
 *  configuration contains information for all cells */
typedef struct
{
  /** struct containing the per carrier cell idx information */
   dl_cell_per_carr_cell_resource_struct_type carr_cell_resource_info[WL1_MAX_NUM_CARRIER_IDX];
  /**struct containing the client info   */
  dl_cell_cfg_client_input_struct_type cell_cfg_client_resume_param[DL_CELL_MAX_NUM_CB_CLIENTS];
  /**struct containing the cell configuration   */
  dl_cell_config_struct_type cell_info[DL_MAX_NUM_CELLS];
  /*Variable holds the shadow cell count that FW is maintaining*/
  uint8 shadow_cell_cnt_pri_carr;
}dl_cell_cfg_info_struct_type;

typedef struct
{
  /**header enum   */
  l1_local_cmd_hdr_type hdr;
  /* cell_db_idx bmsk*/
  uint16 cell_bmsk;
}dl_cell_cfg_post_cmd_type;

typedef struct
{
  /**header enum   */
  l1_local_cmd_hdr_type hdr;
  /** detected td val */
  uint16 detected_td_val_bmsk;
  /* nothing for now*/
}dl_cell_cfg_td_det_notify_cmd_type;

extern dl_cell_cfg_info_struct_type dl_cell_info;

#define DL_TRIAGE_SET_LOCK()    srchsetutil_get_intlock()
#define DL_TRIAGE_SET_UNLOCK()  srchsetutil_get_intfree()

/*===========================================================================
FUNCTION dl_get_psc_for_cell_db_idx

DESCRIPTION
  This function returns PSC for the given cell db idx

DEPENDENCIES
  None

RETURN VALUE
  PSC
  
SIDE EFFECTS
  cell db is changed
===========================================================================*/
extern uint16 dl_get_psc_for_cell_db_idx(uint8 cell_db_idx);

/*===========================================================================
FUNCTION dl_get_freq_for_cell_db_idx

DESCRIPTION
  This function returns PSC for the given cell db idx

DEPENDENCIES
  None

RETURN VALUE
  freq

SIDE EFFECTS
  cell db is changed
===========================================================================*/
extern uint16 dl_get_freq_for_cell_db_idx(uint8 cell_db_idx);

/*===========================================================================
FUNCTION dl_init_cell_info

DESCRIPTION
  Initialises all entries in the dl_cell_info_set to unused. Also sets up the 
  client callback pointers to null

  1. loop through all cells and set the values to be invalid values
  2. Set all client callback pointers to null

  Input range: cell_db_idx E [0,DL_CELL_MAX_NUM_CELLS) U DL_CELL_INIT_ALL_CELLS
DEPENDENCIES
  None

RETURN VALUE
  None
  
SIDE EFFECTS
  None
===========================================================================*/
extern void dl_init_cell_info(void);

/*===========================================================================
FUNCTION dl_clear_cell_info

DESCRIPTION
  Clears entries for a particular cell db index  

  Input range: cell_db_idx E [0,DL_CELL_MAX_NUM_CELLS)
DEPENDENCIES
  None

RETURN VALUE
  None
  
SIDE EFFECTS
  None
===========================================================================*/
extern void dl_clear_cell_info(
  /** cell db index */
  uint8 cell_db_index);

/*===========================================================================
FUNCTION dl_get_cell_db_index

DESCRIPTION
  This function returns an index from the cell db for a given psc and frequency
  for a fresh cell it returns the first unused entry in the cell db and for a 
  cell that is already allocated an entry in the cell db it returns the index

DEPENDENCIES
  None

RETURN VALUE
  index
  
SIDE EFFECTS
  cell db is changed
===========================================================================*/
extern uint8 dl_get_cell_db_index(
  /**primary scrambling code   */
  uint16 psc,
  /**frequency   */
  uint16 frequency);

/*===========================================================================
FUNCTION dl_cell_allocate_cell_id

DESCRIPTION
  This function uses the psc and frequency to index into the cell db to retunr
  the index that a cell is allocated if it has already been allocated
  (unexpected condition) or returns a new index for a fresh cell

  1. Query the db to find if this cell exists
  2. if it doesnt exist loop through the db and provide a cell db
     index
  Input range : psc E [0,MAX_PRI_SCR_CODE) 
                frequency should be a valid frequency

DEPENDENCIES
  None

RETURN VALUE
  cell db index
  
SIDE EFFECTS
  cell is added to the cell db if it is a new cell
===========================================================================*/
extern uint8 dl_cell_allocate_cell_id(
  /**primary scrambling code   */
  uint16 psc,
  /**frequency of the cell   */
  uint16 frequency
  /**carrier idx for which cell id is being requested */ 
  ,uint8 carr_idx
);
/*===========================================================================
FUNCTION dl_cell_configure_cells

DESCRIPTION
  Takes in a cell idx bmsk. for each of the cells
  Checks if the cell state is CELL_CONFIGURED or 
  CELL_CFG_IN_PROGRESS. If it is then checks whether any parameters have changed 
  if yes updates mcal so that cell configuration command can be sent. If the 
  cell is in CELL_ALLOCATED state doesn't check for change in parameters and 
  goes ahead and updates the parameters. also commits the configuration if the
  commit_needed flag is set to TRUE

  Input range: cell_db_idx_mask  
               srch cell pointers should be valid

DEPENDENCIES
  Cell id should have been allocated already

RETURN VALUE
  True or False
  
SIDE EFFECTS
  cell is added to the cell db if it is a new cell and its parameters are 
  updated
===========================================================================*/
extern boolean dl_cell_configure_cells(
  /**cell db index   */
  uint16 cell_db_idx_mask,
  /** srch set struct ptr */
  srch_cell_struct_type **srch_cell_ptr,
  /** commit needed */
  boolean commit_needed,
    /** cell cfg client input struct */
  dl_cell_cfg_client_input_struct_type *cell_cfg_client_input);

/*===========================================================================
FUNCTION dl_cell_configure

DESCRIPTION
  Checks if the cell state is CELL_CONFIGURED or 
  CELL_CFG_IN_PROGRESS. If it is then checks whether any parameters have changed 
  if yes updates mcal so that cell configuration command can be sent. If the 
  cell is in CELL_ALLOCATED state doesn't check for change in parameters and 
  goes ahead and updates the parameters.

  Input range: cell_db_idx E [0,DL_MAX_NUM_CELLS) 
               srch cell pointer should be valid

DEPENDENCIES
  TRUE or FALSE based upon the state of the cell

RETURN VALUE
  cell db index
  
SIDE EFFECTS
  cell is added to the cell db if it is a new cell and its parameters are 
  uodated
===========================================================================*/
extern boolean dl_cell_configure(
  /**cell db index   */
  uint16 cell_db_idx,
  /** srch set struct ptr */
  srch_cell_struct_type *srch_cell_ptr);

/*===========================================================================
FUNCTION dl_cell_configure_cell_ref_fing_cb

DESCRIPTION
  This function uses the ref finger provided by finger to proceed with cell config

DEPENDENCIES
  Cell should be in config in progress state

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void dl_cell_configure_cell_ref_fing_cb(
  /** cell db idx for which fing is available */
  uint8 cell_db_idx);

/*===========================================================================
FUNCTION dl_cell_deallocate_cell_id

DESCRIPTION
  Deallocates a cell id from the db and sets the psc and the freqeuncy to invalid
  values

  cell db idx E [0,DL_MAX_NUM_CELLS)

DEPENDENCIES
  Input should be a valid cell id

RETURN VALUE
  TRUE or FALSE based on the deallocation process
  
SIDE EFFECTS
  
===========================================================================*/
extern boolean dl_cell_deallocate_cell_id(
  /*cell db index*/
  uint8 cell_db_idx);

/*===========================================================================
FUNCTION dl_cell_deconfigure_cells

DESCRIPTION
  Takes a cell idx bmsk and for each of the cells 
  Checks if the cell state is deact in progress. If it is then triage is 
  instructed to drop fingers on this cell. Following this cell config module 
  updates MCAL db
  Input range:  cell_db_idx E [0,8)

DEPENDENCIES
  TRUE or FALSE based upon the state of the cell

RETURN VALUE
  cell db index
  
SIDE EFFECTS
  cell is added to the cell db if it is a new cell and its parameters are 
  uodated
===========================================================================*/
extern boolean dl_cell_deconfigure_cells(
  /**cell db index   */
  uint16 cell_db_idx_mask,
  /** cell cfg client input struct */
  dl_cell_cfg_client_input_struct_type *cell_cfg_client_input);

/*===========================================================================
FUNCTION dl_cell_update_cell_config

DESCRIPTION
  This function commits the cell config to MCAL. Following are the sequence of 
  steps
  1. goes through the linked list to find the cell.
  2. Calls triage with a callback for a finger  

DEPENDENCIES
  Input should be a valid cell id, Valid cell pointer and a valid update reason

RETURN VALUE
  Cell state enum type
  
SIDE EFFECTS
  None
===========================================================================*/
extern boolean dl_cell_update_cell_config(
  /** cell_db index */
  uint16 cell_db_idx, 
	/** fing idx */
	uint16 ref_fing_idx);

/*===========================================================================
FUNCTION dl_cell_notify_on_cfg_done

DESCRIPTION
  This function takes the function pointer that needs to be called so that the client
  can be notified on completion of the cell configuration process 

DEPENDENCIES
  Input should be a valid cell id, and valid cb ptrs

RETURN VALUE
  Cell state enum type
  
SIDE EFFECTS
  None
===========================================================================*/
extern boolean dl_cell_notify_on_cfg_done(
  /**function pointer to be called when cell config is
   * completed */
  dl_cell_cfg_client_input_struct_type *cell_cfg_client_input);

/*===========================================================================
FUNCTION dl_cell_cfg_done_isr_handler

DESCRIPTION
  This function handles the cell config update following a demod done isr from 
  MCAL. This is called in ISR context. It does the following
  1. finds out the cells whose states have changed. Switches their states and 
     other control variables for a cell
  2. Posts local command so that cell config module can call the callbacks in
     task context.


DEPENDENCIES
  Input should be a non zero cell bmsk

RETURN VALUE
  None
  
SIDE EFFECTS
  None
===========================================================================*/
extern void dl_cell_cfg_done_isr_handler(
  /** cell bitmask */
  uint16 cell_bmsk,
  /** status of the command */
  int8 status);

/*===========================================================================
FUNCTION dl_cell_make_cell_triage_capable

DESCRIPTION
  This function is called on completion of a cell cfg to add it to the list 
  of srch cell ptrs that can be used for triage for triaging fingers

DEPENDENCIES
  Input should be a valid cell db idx

RETURN VALUE
  None
  
SIDE EFFECTS
  None
===========================================================================*/
extern boolean dl_cell_make_cell_triage_capable(
  /* cell db idx*/
  uint8 cell_db_idx);

/*===========================================================================
FUNCTION dl_cell_handle_ref_finger_change

DESCRIPTION
  This function is called as a result of a reference finger change from fing manager
  The cell if it is in configuration in progress or configured state is now 
  switched back to config in progress. Cell config module populates mcal parameters
  but doesnt commit the configuration. Commit in this case is done by finger manager

  Input range: ref_fing_phase_ind e{DL_FING_PHASE_REF_P_CPICH, DL_FING_PHASE_REF_S_CPICH}
               cell_db_idx E [0,DL_MAX_NUM_CELLS)
               new_ref_finger E [0, dl_max_num_fingers)
DEPENDENCIES
  Input should be a non zero cell bmsk

RETURN VALUE
  Cell state enum type
  
SIDE EFFECTS
  None
===========================================================================*/
extern boolean dl_cell_handle_ref_finger_change(
  /** triage enum type specifying p-cpich ref fing chnage or
   *  s-cpich */
  dl_tri_ref_fing_phase_ind *ref_fing_phase_ind,
  /** cell db index */
  uint16 cell_db_idx_mask,
  /** new reference finger */
  uint8 *new_ref_finger);

/*===========================================================================
FUNCTION dl_cell_handle_txdiv_determination

DESCRIPTION
  This function is called by searcher for tx diversity determination. This function
  does the following.
  1. Updates parameters for the cell that needs tx diversity determination.
  2. GEts a finger for this purpose
  3. sends cell configuration with a callback

  Input range: cell_db_idx E [0,DL_MAX_NUM_CELLS)
               srch cell ptr should be valid
DEPENDENCIES
  Input should be a vaid cell id and srch cell ptr

RETURN VALUE
  true or false
  
SIDE EFFECTS
  None
===========================================================================*/
extern boolean dl_cell_handle_txdiv_determination(
  /** cell db index */
  uint16 cell_db_idx_mask,
  /** search cell pointer  */
  srch_cell_struct_type **srch_cell_ptr,
  /** cell cfg client input struct */
  dl_cell_cfg_client_input_struct_type *cell_cfg_client_input);

/*===========================================================================
FUNCTION dl_cell_get_active_cell_mask

DESCRIPTION
  This function a mask that represents the cell idx that are active

DEPENDENCIES
  None

RETURN VALUE
  active cell idx mask
  
SIDE EFFECTS
  None
===========================================================================*/
extern uint16 dl_cell_get_active_cell_mask(void);

/*===========================================================================
FUNCTION dl_cell_get_cell_config_on_prog_status

DESCRIPTION
  This function returns TRUE if cell is under configutation

DEPENDENCIES
  None

RETURN VALUE
  TRUE: If the cell state is DL_CELL_CONFIGURATION_IN_PROGRESS
  
SIDE EFFECTS
  None
===========================================================================*/
extern boolean dl_cell_get_cell_config_on_prog_status(uint16 cell_db_idx);

/*===========================================================================
FUNCTION dl_cell_get_cell_config_on_prog_for_triage

DESCRIPTION
  This function returns TRUE if cell under configutation and 
  is waiting a for finger

DEPENDENCIES
  None

RETURN VALUE
  boolean
  
SIDE EFFECTS
  None
===========================================================================*/
extern boolean dl_cell_get_cell_config_on_prog_for_triage(uint16 cell_db_idx);

/*===========================================================================
FUNCTION dl_cell_get_cell_configured_status

DESCRIPTION
  This function returns TRUE or FALSE when a cell is configured

DEPENDENCIES
  None

RETURN VALUE
  active cell idx mask
  
SIDE EFFECTS
  None
===========================================================================*/
extern uint16 dl_cell_get_cell_configured_status(uint16 cell_db_idx);

/*===========================================================================
FUNCTION dl_cell_get_cell_configured_status

DESCRIPTION
  This function returns TRUE when a cell is either configured OR config in progress
  FALSE otherwise
 
DEPENDENCIES
  None

RETURN VALUE
  Cell config state
  
SIDE EFFECTS
  None
===========================================================================*/
extern uint16 dl_cell_get_cell_config_status(uint16 cell_db_idx);


/*===========================================================================
FUNCTION dl_cell_is_any_cell_under_config

DESCRIPTION
  This function returns TRUE or FALSE for a cell bmsk that it takes as input,
  if any of the cells in that mask is undergoing configuration.

DEPENDENCIES
  None

RETURN VALUE
  TRUE or FALSE
  
SIDE EFFECTS
  None
===========================================================================*/
extern boolean dl_cell_is_any_cell_under_config(uint16 cell_idx_bmsk);

/*===========================================================================
FUNCTION dl_cell_is_cell_triage_capable

DESCRIPTION
  This function is called by seearcher to find out if a cell is triage'able.
  if the cell is not in configured state then this cell cannot have fingers
  configured,

DEPENDENCIES
  Input should be a valid cell db idx

RETURN VALUE
  None
  
SIDE EFFECTS
  None
===========================================================================*/
extern boolean dl_cell_is_cell_triage_capable(
  /* cell db idx*/
  uint8 cell_db_idx);

/*===========================================================================
FUNCTION dl_cell_resume_cell_commit

DESCRIPTION
  This function resumes the cell config commit operation to MCAL following the 
  availability of a finger for cell config. It commits all cell config done until
  now

DEPENDENCIES
  None

RETURN VALUE
  boolean whether commit went through or not
  
SIDE EFFECTS
  None
===========================================================================*/
extern boolean dl_cell_resume_cell_commit(void);

/*===========================================================================
FUNCTION dl_cell_inform_triage_of_cell_deactivation

DESCRIPTION
  This function informs finger manager of completion of cell config. Finger 
  manager then changes the states of the fingers.

  Input range:  cell_db_idx E [0, DL_CELL_MAX_NUM_CELLS)
DEPENDENCIES
  Input should be a valid cell db idx

RETURN VALUE
  None
  
SIDE EFFECTS
  None
===========================================================================*/
extern void dl_cell_inform_triage_of_cell_deactivation(
  /* cell db idx*/
  uint8 cell_db_idx);

/*===========================================================================
FUNCTION dl_cell_notify_clients_of_cell_cfg

DESCRIPTION
  This function is called as a result of a local command posted by whl1 when
  it gets the demod cfg done isr from mcal 

DEPENDENCIES
  Input should be a non zero cell bmsk

RETURN VALUE
  Cell state enum type
  
SIDE EFFECTS
  None
===========================================================================*/
extern void dl_cell_notify_clients_of_cell_cfg(uint16 cell_bmsk);

/*===========================================================================
FUNCTION dl_cell_check_tddet_status

DESCRIPTION
  This function checks for the tddet status of all cells that are undergoing td detection.
  This is done every cfn until all cells complete td detection

  Input range:  CFN E [0,256)
DEPENDENCIES
  Input should be a valid cfn

RETURN VALUE
  None
  
SIDE EFFECTS
  None
===========================================================================*/
extern void dl_cell_check_tddet_status(
  /** cfn */
  uint8 cfn);

/*===========================================================================
FUNCTION dl_cell_handle_tddet_complete

DESCRIPTION
  This function is the local command handler for the td detection complete. 
  This function calls all clients that are waiting for the cell cfg to complete

  Input range:  8 bit bit mask
DEPENDENCIES
  None
RETURN VALUE
  None
  
SIDE EFFECTS
  None
===========================================================================*/
extern void dl_cell_handle_tddet_complete(uint16 detected_td_bmsk);

/*===========================================================================
FUNCTION dl_cell_get_detected_td_val

DESCRIPTION
  This function returns the detected td value for a cell idx

  Input range:  valid cell idx[0,7)
DEPENDENCIES
  None
RETURN VALUE
  TRUE or FALSE
  
SIDE EFFECTS
  None
===========================================================================*/
extern uint8 dl_cell_get_detected_td_val(uint8 cell_idx);

/*===========================================================================
FUNCTION dl_cell_is_cell_under_deconfig

DESCRIPTION
  This function returns TRUE is the current cell indicated by cell_idx
  in undergoing decfg currently

DEPENDENCIES
  
RETURN VALUE
  None
  
SIDE EFFECTS
  None
===========================================================================*/
extern boolean dl_cell_is_cell_going_inactive(uint8 cell_idx);

/*===========================================================================
FUNCTION dl_cell_is_cell_under_decfg

DESCRIPTION
  This function returns TRUE is the current cell indicated by cell_idx
  in undergoing decfg currently

DEPENDENCIES
  
RETURN VALUE
  None
  
SIDE EFFECTS
  None
===========================================================================*/
extern boolean dl_cell_is_cell_under_decfg(uint8 cell_idx);

/*===========================================================================
FUNCTION dl_cell_check_for_ongoing_td_detection

DESCRIPTION
  This function checks if there is any ongoing td detection for any cell, cell
  that needs to be checked for td detection has a bit set for a position, which
  is equal to its cell db idx

  Input range:  8 bit bit mask
DEPENDENCIES
  None
RETURN VALUE
  None
  
SIDE EFFECTS
  None
===========================================================================*/
extern uint32 dl_cell_check_for_ongoing_td_detection(uint16 cell_bmsk);

/*===========================================================================
FUNCTION dl_get_cell_ptr_for_cell_db_idx

DESCRIPTION
  This function returns pointer to the cell structure based on the cell_db_idx
  value.

DEPENDENCIES
  
RETURN VALUE
  None
  
SIDE EFFECTS
  None
===========================================================================*/
extern srch_cell_struct_type* dl_get_cell_ptr_for_cell_db_idx(uint8 cell_db_idx);

/*===========================================================================
FUNCTION dl_cell_get_ssc_status

DESCRIPTION
  This function returns the secondary scrambling code if any on the cell.
  
DEPENDENCIES
  None
RETURN VALUE
  0xFF signifies ssc is not enabled on this cell
  
SIDE EFFECTS
  None
===========================================================================*/
extern uint32 dl_cell_get_ssc_status(uint8 cell_db_idx);

/*===========================================================================
FUNCTION dl_cell_get_phase_ref_type

DESCRIPTION
  This function phase reference type based on the sec-cpich info of a cell

DEPENDENCIES
  None

RETURN VALUE
  phase ref type
  
SIDE EFFECTS
  None
===========================================================================*/
extern dl_tri_ref_fing_phase_ind dl_cell_get_phase_ref_type(
		/** search cell	pointer */
		srch_cell_struct_type *cell_ptr);

/*===========================================================================
FUNCTION dl_cell_get_scpich_status

DESCRIPTION
  This function returns the status of a sec-cpich on a cell
  
DEPENDENCIES
  None
RETURN VALUE
  0xFF signifies ssc is not enabled on this cell
  
SIDE EFFECTS
  None
===========================================================================*/
extern uint32 dl_cell_get_scpich_status(
  /** cell db idx */
  uint8 cell_db_idx, 
  /** mimo mode */
  dl_cell_scpich_mimo_mode mode);

/*===========================================================================
FUNCTION dl_cell_get_cell_state

DESCRIPTION
  This accessor routine returns the configuration state of a cell given its 
  cell db index

DEPENDENCIES
  Input should be a valid cell id

RETURN VALUE
  Cell state enum type
  
SIDE EFFECTS
  None
===========================================================================*/
extern dl_cell_state_enum_type dl_cell_get_cell_state(
  /** cell db index */
  uint16 cell_db_idx);

/*===========================================================================

FUNCTION     dl_cell_add_to_triage_set

DESCRIPTION
    This function adds cells to the TRIAGE SET. Only cells that need finger
    assignment are moved to TRIAGE SET. 

DEPENDENCIES
  None.

PARAMETERS
    cell_ptr is the pointer to cell in search_cells DB.
 
RETURN VALUE
   none

SIDE EFFECTS
    When adding cells to triage set, we have to make sure that only unique 
    cells exist.
===========================================================================*/
extern void dl_cell_add_to_triage_set(srch_cell_struct_type *cell_ptr);

/*===========================================================================

FUNCTION     dl_cell_remove_from_triage_set

DESCRIPTION
  This function removes cells from the TRIAGE SET. 

DEPENDENCIES
  None.

INPUT
  cell_db_idx for cell to remove
 
RETURN VALUE
  none
SIDE EFFECTS
   scr_code, freq are not set in this function
===========================================================================*/
extern void dl_cell_remove_from_triage_set(uint8 cell_db_idx);

/*===========================================================================
FUNCTION     dl_cell_clear_triage_set

DESCRIPTION
  This function clears the TRIAGE SET. 

DEPENDENCIES
  None.

PARAMETERS
 None.
 
RETURN VALUE
  none

SIDE EFFECTS
   None
===========================================================================*/
extern void dl_cell_clear_triage_set( dl_tri_carr_id_enum_type carr_idx);

/*===========================================================================
FUNCTION     dl_cell_restore_triage_set

DESCRIPTION
  This function restores the TRIAGE SET with intra-F cells that need finger
  assignment. 

DEPENDENCIES
  None.

PARAMETERS
 None.
 
RETURN VALUE
  none
SIDE EFFECTS
   None
===========================================================================*/
extern void dl_cell_restore_triage_set(dl_tri_carr_id_enum_type carr_idx);

/*===========================================================================
FUNCTION     dl_cell_find_cell_in_triage_set

DESCRIPTION
  This function finds the cell in the TRIAGE SET. 

DEPENDENCIES
  None.

PARAMETERS
  frequency
  scrambling code
 
RETURN VALUE
  none
SIDE EFFECTS
   None
===========================================================================*/
extern srch_cell_struct_type* dl_cell_find_cell_in_triage_set
(
  uint16 freq,       /* find cell with this cdma frequency */
  uint16 scr_code    /* and  this scrambling code */
);

/*===========================================================================
FUNCTION     dl_cell_print_cells_in_triage_set

DESCRIPTION
  Given a particular carrier, this function prints relevant info for cells in 
  TRIAGE SET under that carrier. 

DEPENDENCIES
  None.

PARAMETERS
  carrier index
 
RETURN VALUE
  none
SIDE EFFECTS
   None
===========================================================================*/
extern void dl_cell_print_cells_in_triage_set(dl_tri_carr_id_enum_type carr_idx);

/*===========================================================================
FUNCTION     dl_cell_update_triage_set_with_interf_cell

DESCRIPTION
  This function the cell pointer registered with DL cell module with the
  interfrequency cell pointer. 

DEPENDENCIES
  None.

PARAMETERS
  inter frequency cell pointer
 
RETURN VALUE
  none
SIDE EFFECTS
   Updates the cell pointer registered with DL cell module.
===========================================================================*/
extern void dl_cell_update_triage_set_with_interf_cell(srch_cell_struct_type *cell_ptr);

/*===========================================================================
FUNCTION dl_cell_get_sch_invert

DESCRIPTION
  This function returns the status of SCH invert on a cell
  
DEPENDENCIES
  None

RETURN VALUE
  TRUE if SCH invert is present.
  
SIDE EFFECTS
  None
===========================================================================*/
extern boolean dl_cell_get_sch_invert(
  /** cell db idx */
  uint8 cell_db_idx);

/*===========================================================================
FUNCTION dl_cell_get_td_val_for_cell

DESCRIPTION
  This function returns the TD settings on a configured cell given a cell db index
  
DEPENDENCIES
  None

RETURN VALUE
  TRUE if TD is ON.
  FALSE, if TD is OFF or cell_db_idx INVALID

SIDE EFFECTS
  None
===========================================================================*/
extern boolean dl_cell_get_td_val_for_cell(
  /** cell db idx */
  uint8 cell_db_idx);

#ifdef FEATURE_WCDMA_DC_HSDPA

/*===========================================================================
FUNCTION dl_cell_is_any_available_non_primary_empty_cell

DESCRIPTION
  This function returns available non primary cells
  
DEPENDENCIES
  None

RETURN VALUE
  Valid cell db idx mask if the cell is present, else 0xFF
  
SIDE EFFECTS
  None
===========================================================================*/
extern boolean dl_cell_is_any_available_non_primary_empty_cell(void);

#endif /* FEATURE_WCDMA_DC_HSDPA */

/*===========================================================================
FUNCTION dl_cell_control_cell_config_concurrency

DESCRIPTION
  This function is used to control the cell config concurrency

  grab_mutex = TRUE -> Grab the mutex
  grab_mutex = FALSE -> Release the mutex

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void dl_cell_control_cell_config_concurrency(boolean grab_mutex);

 /*===========================================================================

FUNCTION dl_cell_get_ref_fing_from_cell_dbase

DESCRIPTION
  This function returns the reference finger from cell database

DEPENDENCIES
  None.

RETURN VALUE
  Reference finger

SIDE EFFECTS
  None
 ===========================================================================*/
extern uint8 dl_cell_get_ref_fing_from_cell_dbase(uint8 cell_db_idx);

/*===========================================================================
FUNCTION dl_cell_get_cell_update_reason

DESCRIPTION
  This function returns the cell update reason
  
DEPENDENCIES
  None

RETURN VALUE
  Cell update reason type

SIDE EFFECTS
  None
===========================================================================*/
extern dl_cell_update_reason_enum_type dl_cell_get_cell_update_reason(uint16 cell_db_idx);

/*===========================================================================
FUNCTION dl_cell_get_cellcfgd_cellrecfg_with_ref_fing_change_status

DESCRIPTION
  This function returns the status of the cell whether it already 
  configured or it's in recfg due to reference fing change
  
DEPENDENCIES
  None

RETURN VALUE
  Boolean

SIDE EFFECTS
  None
===========================================================================*/
extern boolean dl_cell_get_cellcfgd_cellrecfg_with_ref_fing_change_status(uint16 cell_db_idx);

/*===========================================================================
FUNCTION dl_cell_check_fast_decfg_in_progress

DESCRIPTION
  This function checks if fast cell decfg is in progress for any on the cells.

DEPENDENCIES
  None

RETURN VALUE
  Boolean

SIDE EFFECTS
  None
===========================================================================*/


extern boolean dl_cell_check_fast_decfg_in_progress(uint16 cell_cfg_done_bmsk);

/*===========================================================================
FUNCTION dl_cell_get_fast_cell_decfg_status

DESCRIPTION
  This function checks if fast cell decfg is in progress for a particular 
  cell db idx.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

extern boolean dl_cell_get_fast_cell_decfg_status(uint8 cell_db_idx);

/*===========================================================================
FUNCTION dl_cell_fast_cell_decfg_post_ops

DESCRIPTION
  This function handles the post cell deconfig done ops.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

extern void dl_cell_fast_cell_decfg_post_ops(uint16 cell_done_bmsk);

/*===========================================================================
FUNCTION dl_cell_fast_cell_disable

DESCRIPTION
  This function is used to initiate a fast cell disable

  grab_mutex = TRUE -> Grab the mutex
  grab_mutex = FALSE -> Release the mutex

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern boolean dl_cell_fast_cell_disable(uint8 cell_db_idx);

/*===========================================================================
FUNCTION dl_cell_process_fast_cell_fing_deconfig_ops

DESCRIPTION
  This function handles the ops for fast cell decfg cases.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

extern void dl_cell_process_fast_cell_fing_deconfig_ops(uint8 cell_db_idx);

/*===========================================================================
FUNCTION dl_cell_sanity_check_bef_sleep

DESCRIPTION
  This function sanity checks the cell status before UE is put to sleep

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

extern void dl_cell_sanity_check_bef_sleep(void);

/*===========================================================================
FUNCTION dl_cell_enable_dbg_msgs

DESCRIPTION
  This function controls the enabling/disabling of debug f3s

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

extern void dl_cell_enable_dbg_msgs(boolean enable);

 /*===========================================================================

FUNCTION dl_set_dl_cell_cfg_nv_values

DESCRIPTION
  Function controls enabling of debug F3s for dl_cell_cfg

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None
 ===========================================================================*/
extern void dl_set_dl_cell_cfg_nv_values(void);

/*=========================================================================

 FUNCTION     :dl_cell_get_combined_txd_status
 
 DESCRIPTION  : This function returns TRUE if TxD is active on any cell for a given carrier..
              
 DEPENDENCIES : 
 
 RETURN VALUE : 
       
 SIDE EFFECTS : 
==========================================================================*/
extern boolean dl_cell_get_combined_txd_status(uint8 carr_idx);

/*=========================================================================

 FUNCTION     :dl_cell_check_tddet_status_interrupt_handler
 
 DESCRIPTION  : This is the interrupt handler for TxD Determination done from FW.
              
 DEPENDENCIES : 
 
 RETURN VALUE : 
       
 SIDE EFFECTS : 
==========================================================================*/
extern void dl_cell_check_tddet_status_interrupt_handler(void);

/*===========================================================================
FUNCTION        dl_cell_update_shadow_cell_interrupt_handler

DESCRIPTION     This function stores the shadow cell information provided by FW and
                requests MVCS accordingly

DEPENDENCIES    None 

RETURN VALUE    None

SIDE EFFECTS    None
===========================================================================*/

extern void dl_cell_update_shadow_cell_interrupt_handler(void);

/*=========================================================================
 FUNCTION     : dl_cell_get_aset_cell_cnt
 
 DESCRIPTION  : This function returns the total cell count which will be the SW cell count
                and the shadow cell cell count that FW maintains.
 DEPENDENCIES : 
 
 RETURN VALUE : 
       
 SIDE EFFECTS : 
==========================================================================*/

extern uint8 dl_cell_get_aset_cell_cnt(uint8 car_cnt);

/*===========================================================================
FUNCTION        dl_cell_get_carr_idx_from_fing

DESCRIPTION     This function returns the carrier index associated with a specific finger on a configured cell

DEPENDENCIES    None 

RETURN VALUE    Carrier idx

SIDE EFFECTS    None
===========================================================================*/
extern uint8 dl_cell_get_carr_idx_from_fing(uint8 fing_idx);

/*===========================================================================
FUNCTION        dl_cell_get_carr_idx_from_cell_db_idx

DESCRIPTION     This function returns the carrier index associated with a 
                specific configured cell associated with given cell_db_idx

DEPENDENCIES    None 

RETURN VALUE    Carrier idx

SIDE EFFECTS    None
===========================================================================*/
extern dl_tri_carr_id_enum_type dl_cell_get_carr_idx_from_cell_db_idx(uint8 cell_db_idx);

/*===========================================================================
FUNCTION        dl_cell_cfg_perform_dsd_for_txd_det

DESCRIPTION     This function posts the demod status dump for TxD Detection that is ongoing.

DEPENDENCIES    None 

RETURN VALUE    None

SIDE EFFECTS    None
===========================================================================*/
extern void dl_cell_cfg_perform_dsd_for_txd_det(void);

/*===========================================================================
FUNCTION        dl_cell_handle_status_dump_done_for_ongoing_txd_det

DESCRIPTION     This function serves as the demod status dump cb handler when called by the dch sync state machine
                          when TxD detection is ongoing.

DEPENDENCIES    None 

RETURN VALUE    Carrier idx

SIDE EFFECTS    None
===========================================================================*/
extern void dl_cell_handle_status_dump_done_for_ongoing_txd_det(void);

/*===========================================================================
FUNCTION dl_cell_fast_deconfigure_all_cells

DESCRIPTION
  This function will actually clear all the SW DL databases, which is like deconfig all
  But will not send any command to FW
  Input range:  none

DEPENDENCIES


RETURN VALUE
  none
  
SIDE EFFECTS

===========================================================================*/
extern void  dl_cell_fast_deconfigure_all_cells(void);
/*=========================================================================
 FUNCTION     : dl_cell_get_total_cell_cnt
 
 DESCRIPTION  : This function returns the total cell count which will be the total number of cells that are not inactive
 DEPENDENCIES : 
 
 RETURN VALUE : total num cells.
       
 SIDE EFFECTS : 
==========================================================================*/
extern uint8 dl_cell_get_total_cell_cnt(void);

/*===========================================================================
FUNCTION        dl_cell_update_shadow_cell_info

DESCRIPTION     This function stores the shadow cell information provided by FW and
                requests MVCS accordingly

DEPENDENCIES    None 

RETURN VALUE    None

SIDE EFFECTS    None
===========================================================================*/
extern void dl_cell_update_shadow_cell_info(void);

#ifdef FEATURE_QSH_DUMP
/*===========================================================================
FUNCTION dl_cell_qsh_populate_cell_info

DESCRIPTION
  This function will populate cell info for QSH

DEPENDENCIES


RETURN VALUE


SIDE EFFECTS

===========================================================================*/
void dl_cell_qsh_populate_cell_info(wl1_dl_mini_struct_type *demod_dump_ptr);
#endif /* FEATURE_QSH_DUMP */

#endif /* DLCELLCFG_H */
