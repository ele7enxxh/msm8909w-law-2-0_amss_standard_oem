#ifndef SRCHCRGSM_H
#define SRCHCRGSM_H
/*===========================================================================
             S R C H   C E L L   R E S E L E C T I O N   G S M
                     D E C L A R A T I O N S

DESCRIPTION
  This header file contains the constant definitions and function
  declarations that are necessary for performing GSM measurements
  on neighboring GSM cells during Cell Reselection.

EXTERNALIZED FUNCTIONS

INTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2002-2012 by Qualcomm Technologies, Inc.
All Rights Reserved.
===========================================================================*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/wcdma/l1/offline/inc/srchcrgsm.h#1 $DateTime: $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/24/15   ap      Initial check-in for WL1 QSH framework 
07/14/15   db      Reschedule EDRX FMO scheduler if GSM search is ongoing
06/19/14   cc      IRAT DR-DSDS check in
04/07/14   cc      Protecting shared global variables
03/10/14   ks      Bolt FED API checkin
01/29/14   mm      W2G cleanup done function prototype changed
10/28/13   ar      Moving GSM module cleanup to L1M
10/14/13   cc      Fix wrong cell into RECONF list issue
10/03/13   cc      Fix the cleanup decision for RSSI in SCH
08/08/13   cc      Fixed compilation errors
08/01/13   cc      Add new cb system to avoid FACH cleanup race condition
12/17/12   mm      Interface changes for passing AS ID from RRC into GL1 functions
08/28/12   cc      Maintain a list of cmd seq to a handler for dealing with the critical sec in sending a cmd to FW  
08/17/12   cc      Support the possibility of the negative FCCH tone time offset in idle
07/24/12   geg     Upmerge WCDMA.MPSS.1.0 to WCDMA.MPSS.2.0
07/02/12   cc      Fix a typo on the function prototype
04/02/12   cc      Increased the number of array buffer in the rssi result struct from 6 to 10
                   Prototype changed on some functions to reduce unnecessary parameters 
02/03/12   dp      Get RSSI Threshold from common external #define
01/30/12   vs      Feature cleanup.
01/20/12   zr      Mainlined FEATURE_WCDMA_GENESIS_SW 
12/27/12   uk      Chk for GFW suspend before proceeding with state cleanup
05/26/11   dp      Added code so W2X_IDLE_CFG gets cleanup buf idx
04/28/11   yh      Added RxLM support for FMO
1/7/11     mm      Setting macro SRCHCRGSM_FACH_MAX_PWR_MEAS_BURST_PER_GSM_FR 
12/08/10   yh      Added GSM cleanup done callback
06/15/09   uk      Support for sending START_FACH_MEAS cmd to fw before 
                   starting a FMO gap for GSM meas
05/08/09   sup      Changing the includes for the renaming of header files  dec.h, enci.h, l1m.h, srch.h, srchi.h, 
                    srchlog.h to wdec.h, wenci.h, wl1m.h, wsrch.h, wsrchi.h, wsrchlog.h
04/24/09   stk     Compiler warning fixes
12/09/08   uk      Mainlining FEATURE_ENHANCED_CELL_RESELECTION
10/26/07   sk      Checking in MBMS branch code into mainline with MBMS feature disabled
08/03/06   sk      Changes for forbidden tones and correction of delta frame offset.
05/02/05   gs      Corrected array size of tags in struct
                   srchcrgsm_fach_rssi_cmd_result_type
04/28/05   bbd     Added support for passing GL1 HW tags parameters during
                   sending of GSM commands and retrieval of results.
12/03/04   yiz     Add FACH meas oksn info and inter-RAT meas log packets.
                   Save T_identify_GSM and T_reconfirm_GSM to FACH GSM globals.
05/25/04   sh      Added function proto srchcrgsm_cell_in_nbr_list() and
                   srchcrgsm_update_gsm_info_to_cell().
02/27/04   yiz/src Brought over field changes for FACH-GSM cleanup.
02/18/04   yiz     Add srchcrgsm_fach_cleanup_pending_cmds() to cleanup FACH
                   GSM meas cmds already sent to GL1 by CM driver.
                   Change FACH GSM meas cmds to return FALSE if oksn skipped.
01/21/04   yiz     Increase SRCHCRGSM_FACH_MAX_PWR_MEAS_BURST_PER_GSM_FR per
                   gl1_hw.h constants.
12/10/03   yiz     Add FEATURE_INTER_RAT_CELL_FACH_RE_SELECTION
12/01/03   jc      Increase num of gsm rssi measured per gsm frame.
09/20/03   sh      Changed number of cells to be reconfirmed to 1 to save
                   awake time in drx, under feature flag
                   FEATURE_MINI_BSIC_RECONF_IN_DRX.
08/15/03   gs      Added lna_state to srchcrgsm_rssi_meas_result_struct_type
07/24/03   sh      Changed SRCHCRGSM_PWR_SCAN_CMD_MAX to be calculated based
                   on SRCHCRGSM_MAX_PWR_MEAS_BURST_PER_GSM_FR (number
                   of power scans per GSM frame) and max GSM cells (32).
                   Modified srchcrgsm_rssi_meas_result_struct_type to use
                   SRCHCRGSM_MAX_PWR_MEAS_BURST_PER_GSM_FR.
11/13/02   sh      Featurized entire file on FEATURE_CELL_RESELECT_INTER_S
08/17/02   sh      Created File.
===========================================================================*/


/* ==========================================================================
** Includes and Public Data Declarations
** ========================================================================*/

/* -----------------------------------------------------------------------
** Include Files
** ----------------------------------------------------------------------- */
#include "comdef.h"
#include "customer.h"
#include "l1def.h"
#include "l1rrcif.h"
#include "srchset.h"
#include "srchgsmdrv.h"
#include "srchcmmeas.h"

#include "wsrchlog.h"

/* -----------------------------------------------------------------------
** Constant / Define Declarations
** ----------------------------------------------------------------------- */

/* Number of cells for which BSIC identification must be done */
#define SRCHCRGSM_BSIC_IDENTIFY_NUM_CELL_MAX    1

/* Maximum number of power bursts that can be measured per
 * GSM frame during cell reselection.
 */
#define SRCHCRGSM_MAX_PWR_MEAS_BURST_PER_GSM_FR 6 

/* Maximum number of frames for power scans */
#define SRCHCRGSM_PWR_SCAN_CMD_MAX             18 /* up to 80ms for FACH */

#define FACH_GSM_OKSN_CYCLE   TRUE
#define FACH_GSM_ONE_OKSN     FALSE

/* Initial rssi of samples */
#define SRCHCRGSM_INIT_RSSI_DB  SRCHGSM_MIN_ACCEPTABLE_RX_LVL_dBm_x16

/* Repetition rate of BSIC within a 51-frame multiframe.*/
#define SRCH_BSIC_REPEAT_FRAME 10

/* -----------------------------------------------------------------------
** Type Declarations
** ----------------------------------------------------------------------- */

/* Measurement purpose for GSM cells */
typedef enum
{
  SRCHCRGSM_RSSI_SCAN,
  SRCHCRGSM_BSIC_IDENTIFY,
  SRCHCRGSM_BSIC_RECONF
} srchcrgsm_meas_purpose_enum_type;

/* FACH GSM measurement occasion types */
typedef enum
{
  SRCHCRGSM_FACH_MEAS_OKSN_GSM_BSIC_ID,
  SRCHCRGSM_FACH_MEAS_OKSN_GSM_BSIC_ID_RSSI,
  SRCHCRGSM_FACH_MEAS_OKSN_GSM_BSIC_RC_RSSI,
  SRCHCRGSM_FACH_MEAS_OKSN_GSM_RSSI,
  SRCHCRGSM_FACH_MEAS_OKSN_GSM_NUM
} srchcrgsm_fach_meas_oksn_gsm_enum_type;

/* Commands during GSM NSET search */
typedef enum
{
  /* sort GSM nset based on measured RSSI */
  SRCHCRGSM_RSSI_MEAS_DONE,
  /* Find BSIC Id of a cell */
  SRCHCRGSM_BSIC_IDENTIFY_DONE,
  /* Verify BSIC Id of a cell */
  SRCHCRGSM_BSIC_VERIFY_DONE,
  /* Done with GSM searching */
  SRCHCRGSM_SRCH_DONE
} srchcrgsm_cmd_enum_type;

/* Local command to advance to next state of GSM NSET search */
typedef struct
{
  /* local command header */
  l1_local_cmd_hdr_type hdr;

  /* status of command */
  boolean status;

  /* reason for command */
  srchcrgsm_cmd_enum_type reason;

} srchcrgsm_local_cmd_type;

/* RSSI scan result poll info struct definition */
typedef struct
{
  /* Number of RSSI scan command in this GAP */
  uint8 num_cmds;
  /* Number of command result received */
  uint8 num_result_rece;

  /* Each command result info */
  struct
  {
    /* GFN when the result of this command is expected */
    uint8 result_frame_num;
    /* Number of GSM bursts in this command */
    uint8 num_bursts;
    /* GSM cell array indicies for each bursts */
    uint8 cell_idx[SRCHCM_MAX_PWR_MEAS_BURST_PER_GSM_FR];

    /* measured RSSI for each burst */
    int16 rssi_db[SRCHCM_MAX_PWR_MEAS_BURST_PER_GSM_FR];

    uint8 lna_state[SRCHCM_MAX_PWR_MEAS_BURST_PER_GSM_FR];

    gl1_hw_rx_tags_type tags[SRCHCM_MAX_PWR_MEAS_BURST_PER_GSM_FR];

    #ifdef FEATURE_DUAL_SIM
    /* as_id received from RRC. Passed to GSM L1 for DSDS */
    sys_modem_as_id_e_type as_id;
    #endif
  } cmd_result[SRCHCRGSM_PWR_SCAN_CMD_MAX];
} srchcrgsm_rssi_meas_result_struct_type;

/* Each RSSI command result info */
typedef struct
{
    /* GFN when the result of this command is expected */
    uint8 result_frame_num;

    /* Number of GSM bursts in this command */
    uint8 num_bursts;

    /* GSM cell array indicies for each bursts */
    uint8 cell_idx[SRCHCM_MAX_PWR_MEAS_BURST_PER_GSM_FR];

    /* measured RSSI for each burst */
    int16 rssi_db[SRCHCM_MAX_PWR_MEAS_BURST_PER_GSM_FR];

    uint8 lna_state[SRCHCM_MAX_PWR_MEAS_BURST_PER_GSM_FR];

    gl1_hw_rx_tags_type tags[SRCHCM_MAX_PWR_MEAS_BURST_PER_GSM_FR];

    #ifdef FEATURE_DUAL_SIM
    /* as_id received from RRC. Passed to GSM L1 for DSDS */
    sys_modem_as_id_e_type as_id;
    #endif
} srchcrgsm_fach_rssi_cmd_result_type;

/* RSSI scan result poll info struct definition */
typedef struct
{
  /* Number of RSSI scan command in this GAP */
  uint8 num_cmds;
  /* Number of command result received */
  uint8 num_result_rece;
  /* Indicates if RSSI results got posted to measurement layer */
  boolean result_posted;
  /* Result buffer for each cmd */
  srchcrgsm_fach_rssi_cmd_result_type cmd_result[SRCHCRGSM_PWR_SCAN_CMD_MAX];
} srchcrgsm_fach_rssi_meas_result_struct_type;


/* -----------------------------------------------------------------------
** Global Constant Data Declarations 
** ----------------------------------------------------------------------- */


/* -----------------------------------------------------------------------
** Global Data Declarations
** ----------------------------------------------------------------------- */

/* RSSI Power Scan result buffer */
extern srchcrgsm_rssi_meas_result_struct_type srchcrgsm_rssi_result;

/* FACH RSSI Power Scan result buffer */
extern srchcrgsm_fach_rssi_meas_result_struct_type srchcrgsm_fach_rssi_result;
extern SRCH_FACH_INTER_RAT_MEAS_LOG_PKT_type srchcrgsm_fach_meas_log_pkt;

/* Index to the current RSSI in the rssi_db_samples circular buffer.
 * Wraps every SRCHCRGSM_NUM_RSSI_SAMPLES_MAX samples.
 */
extern uint8 srchcrgsm_cur_rssi_sample_idx;

/* Index of the next cell in the list of 4 strongest GSM cells
 * for BSIC Identification/Verification.
 */
extern uint8 srchcrgsm_cell_idx_bsic_verify;

/* flag to indicate BSIC verification timer expired */
extern boolean srchcrgsm_bsic_verif_tmr_expired;

/* RSSI burst is getting triggered inside SCH gap */
extern boolean srchcrgsm_rssi_in_sch_trigger_status;

extern srch_fach_gsm_rssi_log_struct_type  srchcrgsm_rssi_meas_in_sch_log_pkt;

/* =======================================================================
**                          Macro Definitions
** ======================================================================= */



/* =======================================================================
**                        Function Declarations
** ======================================================================= */

/*===========================================================================

FUNCTION      SRCHCR_GSM_INIT

DESCRIPTION
  This function initializes the srchgsm driver, initializes variables
  related to cell reselection gsm measurements, and registers
  callback functions for RSSI, ACQ, and SCH decode measurements.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Callbacks for GSM measurements are registered and srchgsm driver
  initialized.
===========================================================================*/
extern void srchcrgsm_init( void );

/*===========================================================================

FUNCTION      SRCHCRGSM_MEAS_CMD

DESCRIPTION
  This function will send the CM Startup command to the mDSP and
  depending on the gsm_meas_cmd, formulates the corresponding GSM
  measurement command to be called at the appropriate GSM frame number
  and then sends the command to scheduler. The scheduler will then send
  the command to the GSM L1 drivers at the appropriate frame.
  This function can be called in the task context.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void srchcrgsm_meas_cmd
(
  srchcrgsm_meas_purpose_enum_type meas_cmd
);

/*===========================================================================

FUNCTION      SRCHCRGSM_VERIFY_BSIC_ID

DESCRIPTION
  Perform BSIC verification on the 4 strongest GSM cells.
  - Perform BSIC verification on cells that have valid timing info
  - Perform BSIC identification on cells that have no timing info

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void srchcrgsm_verify_bsic_id( uint8 cell_idx );

/*===========================================================================
FUNCTION      SRCHCR_GSM_PROCESS_RSSI_MEAS_RESULT

DESCRIPTION
  This function will update the RSSI results for all the GSM Cells for
  which the RSSI measurements were made.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void srchcr_gsm_process_rssi_meas_result( boolean cm_meas );

/*===========================================================================

FUNCTION      SRCHCRGSM_CLEANUP

DESCRIPTION
  This function reinitializes variables related to cell reselection
  GSM measurements and also de-registers callbacks functions for
  RSSI, ACQ, and SCH decode measurements.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Callbacks for GSM measurements are registered and srchgsm driver
  initialized.
===========================================================================*/
extern void srchcrgsm_cleanup( void );

/*===========================================================================
FUNCTION      SRCHCRGSM_CLEANUP_CMD_DONE

DESCRIPTION
  This function is a call back function to check whether GSM mDSP is done 
  with suspend command. 

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void srchcrgsm_cleanup_cmd_done( uint8 num_cmds_left );

/*===========================================================================
FUNCTION      SRCHCRGSM_CLEANUP_CMD_DONE

DESCRIPTION
  This function is a call back function to check whether GSM mDSP is done 
  with suspend command so state cleanup can proceed. Awake means it is still not suspended

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void srchcrgsm_state_cleanup_cmd_done(uint8 num_cmds_left);

/*===========================================================================

FUNCTION      SRCHCRGSM_UPDATE_GSM_INFO_TO_CELL

DESCRIPTION
  This function updates the cell and timing info found during acquisition
  to the cell structure that is passed in.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void srchcrgsm_update_gsm_info_to_cell
(
  uint8 cur_gfn,
  int32 time_offset,
  srch_gsm_cell_struct_type* cell_ptr
);

/*===========================================================================

FUNCTION      SRCHCRGSM_CELL_IN_NBR_LIST

DESCRIPTION
  This function searches the neighbor list to check if the
  <BSIC, ARFCN> pair is present.

DEPENDENCIES
  None

RETURN VALUE
  Pointer to cell, if cell with <BSIC, ARFCN> exists,
  NULL otherwise

SIDE EFFECTS
  srchcrgsm_cell_idx_bsic_verify is updated to point to new cell.
  If a best cell had been found, update srchcr_gsm_cell_ptr to this cell.
===========================================================================*/
srch_gsm_cell_struct_type* srchcrgsm_cell_in_nbr_list
(
  uint8 bsic_id,
  W_ARFCN_T arfcn
);

/*===========================================================================

FUNCTION      SRCHCRGSM_FACH_RSSI_CMD

DESCRIPTION   This function issues FACH GSM RSSI measurement command to
              scheduler.

DEPENDENCIES  Must be called at a FACH GSM oksn to do RSSI MEAS

RETURN VALUE  None

SIDE EFFECTS  None

===========================================================================*/
boolean srchcrgsm_fach_rssi_cmd( void );

/*===========================================================================

FUNCTION      SRCHCRGSM_FACH_RSSI_IN_SCH_CMD

DESCRIPTION   This function issues FACH GSM RSSI measurement in SCH command to
              scheduler.

DEPENDENCIES  This function is called at a FACH GSM SCH or BSIC Reconf oksn to do RSSI MEAS 
              if there is still space available inside the gap.

RETURN VALUE  None

SIDE EFFECTS  None

===========================================================================*/
boolean srchcrgsm_fach_rssi_in_sch_cmd(
  /* Total num bursts inside the gap */
  uint8 *num_total_bursts);

/*===========================================================================

FUNCTION      SRCHCRGSM_FACH_BSIC_ID_CMD

DESCRIPTION
  

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean srchcrgsm_fach_bsic_id_cmd( void );

/*===========================================================================

FUNCTION      SRCHCRGSM_FACH_BSIC_RECF_CMD

DESCRIPTION   This function issues FACH GSM BSIC reconfirmation command to
              scheduler.

DEPENDENCIES  Must be called at a FACH GSM oksn to do BSIC reconfirmation

RETURN VALUE  None

SIDE EFFECTS  None

===========================================================================*/
boolean srchcrgsm_fach_bsic_recf_cmd( void );

/*===========================================================================

FUNCTION      SRCHCRGSM_FACH_FIND_RECF_OKSN_AND_CELL

DESCRIPTION   This function finds suitable GSM cell and GSM oksn for BSIC
              reconfirmation in next 8 GSM oksn cycle, OR the suitable GSM
              cell only for next GSM occasion if cycle_oksn_sel is
              FACH_GSM_ONE_OKSN   

DEPENDENCIES  Must be called at the first GSM occasion in the next 8 GSM
              occasion cycle, or at a GSM BSIC ID occasion when no cell for
              BSIC ID.
              Must have obtained srchfach_n_intra_meas_parms.n_tti
              cycle_oksn_sel = FACH_GSM_OKSN_CYCLE, or FACH_GSM_ONE_OKSN

RETURN VALUE  FALSE if no suitable cell/oksn for BSIC reconf

SIDE EFFECTS  Update recf_suit_cell_idx and recf_suit_oksn_idx

===========================================================================*/
boolean srchcrgsm_fach_find_recf_oksn_and_cell( boolean cycle_oksn_sel );

/*===========================================================================

FUNCTION      SRCHCRGSM_FACH_RECONF_TIMER_EXPIRED

DESCRIPTION   This function looks up the T_re_confirm_GSM table to check if 
              timer expired.  

DEPENDENCIES  In FACH GSM measurements

RETURN VALUE  FALSE if OK, which is not expired

SIDE EFFECTS  None

===========================================================================*/
boolean srchcrgsm_fach_reconf_timer_expired( uint32 timer );

/*===========================================================================

FUNCTION      SRCHCRGSM_FACH_T_RECONFIRM_CALC

DESCRIPTION   This function calc T_reconfirm_GSM.  

DEPENDENCIES  FACH measurement occassion info available

RETURN VALUE  FALSE if FACH meas oksn invalid

SIDE EFFECTS  Update T_reconfirm_GSM

===========================================================================*/
boolean srchcrgsm_fach_t_reconfirm_calc(void);

/*===========================================================================

FUNCTION      SRCHCRGSM_FACH_BSIC_ID_TIMER_EXPIRED

DESCRIPTION   This function looks up the T_identify_GSM table to check if 
              timer expired.  

DEPENDENCIES  In FACH GSM measurements

RETURN VALUE  FALSE if OK, which is not expired

SIDE EFFECTS  None

===========================================================================*/
boolean srchcrgsm_fach_bsic_id_timer_expired( uint32 timer );

/*===========================================================================

FUNCTION      SRCHCRGSM_FACH_T_IDENTIFY_CALC

DESCRIPTION   This function calculates T_identify_GSM.  

DEPENDENCIES  FACH measurement occasion info available

RETURN VALUE  FALSE means invalid FACH meas oksn info

SIDE EFFECTS  Update T_identify_GSM

===========================================================================*/
boolean srchcrgsm_fach_t_identify_calc(void);

/*===========================================================================

FUNCTION      SRCHCR_SORT_GSM_NSET

DESCRIPTION
  Sort GSM neighbor cells based on the measured RSSI.
  srch_gsm_nset_ptr has the cells with the updated measurements.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void srchcr_sort_gsm_nset( void );


/*===========================================================================

FUNCTION      SRCHCRGSM_FACH_CLEANUP_PENDING_CMDS

DESCRIPTION
  Cleanup for GSM meas cmds already sent to GL1

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if done

SIDE EFFECTS
  None.
===========================================================================*/
boolean srchcrgsm_fach_cleanup_pending_cmds( void );

/*===========================================================================

FUNCTION        SRCHCRGSM_FACH_LOG_MEAS

DESCRIPTION     This function is to log CELL_FACH inter-RAT measurements.

DEPENDENCIES    None

RETURN VALUE    None
  
SIDE EFFECTS    logging.
                
===========================================================================*/
void srchcrgsm_fach_log_meas(void);

/*===========================================================================
FUNCTION      SRCHCRGSM_FACH_QUERY_KNOWN_CELL_QS_OFFSET

DESCRIPTION
  This function finds and adds known cell offsets in the same arfcn to the forbidden list..

DEPENDENCIES
  None.

RETURN VALUE
  Number of forbidden offsets which are added.

SIDE EFFECTS
  None.
===========================================================================*/
uint8 srchcrgsm_fach_query_known_cell_qs_offset(
  W_ARFCN_T arfcn,
  uint8 *frame_offset,
  uint16 *qs_offset);
void srchcrgsm_fach_calc_startup_parms
(
  uint8   gfn,        /* curr position in meas oksn */
  uint16  qs_offset,  /* curr position in meas oksn */
  srchcm_gsm_startup_cmd_param_struct_type* startup_cmd_params
  #ifdef FEATURE_DUAL_SIM
  /* AS ID to be passed to GL1 functions */
  , sys_modem_as_id_e_type as_id
  #endif
);

void srchcrgsm_fach_calc_ending_cmd_parms
(
  /* GFN at which ending command should go  */
  uint8 ending_frame_num,
  /* CM ending offset. This can be larger than QS per frame */
  uint16 ending_offset
  #ifdef FEATURE_DUAL_SIM
  /* AS ID to be passed to GL1 functions */
  , sys_modem_as_id_e_type as_id
  #endif
);

boolean srchcrgsm_fach_bsic_id_tone_result_poll_func(uint8 cur_gfn, boolean *rslt_detected);
boolean srchcrgsm_fach_bsic_id_result_poll_func(uint8 cur_gfn, boolean *rslt_detected);
boolean srchcrgsm_fach_bsic_recf_result_poll_func(uint8 cur_gfn, boolean *rslt_detected);
void srchcrgsm_poll_rssi_meas_result(uint8 cur_gfn);
void srchcrgsm_fach_decrement_bsic_id_tmr(void);
boolean srchcrgsm_fach_rssi_result_poll_func(uint8 cur_gfn);
void srchcrgsm_fach_rssi_bursts_populate
(
  uint8   first_gsm_cell_idx,
  uint8*  curr_gfn,
  uint16* curr_offset_qs
  #ifdef FEATURE_DUAL_SIM
  /* AS ID to be passed to GL1 functions */
  , sys_modem_as_id_e_type as_id
  #endif
);

/*===========================================================================

FUNCTION      SRCHCRGSM_FACH_RSSI_BURSTS_POPULATE_IN_SCH

DESCRIPTION   This function populates bursts for FACH GSM RSSI commands if there is a space in SCH gap.

DEPENDENCIES  

RETURN VALUE  
uint8 - Total num bursts inside the gap

SIDE EFFECTS  Populate cmd buffer and result buffer, and update
              srchfach_gsm_rssi_parms

===========================================================================*/
uint8 srchcrgsm_fach_rssi_bursts_populate_in_sch
(
  uint8   first_gsm_cell_idx
  #ifdef FEATURE_DUAL_SIM
  /* AS ID to be passed to GL1 functions */
  , sys_modem_as_id_e_type as_id
  #endif
);

boolean srchcrgsm_fach_bsic_id_sch_dec_result_poll_func(uint8 cur_gfn, boolean *rslt_detected);
boolean srchcrgsm_fach_bsic_id_sch_dec_cmd
(
    uint8  curr_gfn,       /* after CM start up */
    uint16 cur_offset_qs,  /* after CM start up */
    uint8  meas_end_gfn,   /* before CM cleanup */
    uint16 meas_end_offset /* before CM cleanup */
);

void srchcrgsm_acq_result_poll_func( uint8 cur_gfn );
void srchcrgsm_sch_decode_poll_func( uint8 cur_gfn );

/*===========================================================================

FUNCTION      SRCHCRGSM_FACH_CM_INIT

DESCRIPTION   This function CM FACH GSM Init command

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None

===========================================================================*/
extern void srchcrgsm_fach_cm_init( uint32 rxlm_home_buf_idx,
                                    uint32 rxlm_away_buf_idx
                                  );

/*===========================================================================
FUNCTION  SRCHCRGSM_FACH_SEND_CMD_

DESCRIPTION
  This function will prepare for the cmd sequence and make it get ready to send 
  to our srchgsmdrv cmd handler

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void srchcrgsm_fach_send_cmd(
  /* Type of measurement purpose for GSM search */
  srchcrgsm_fach_meas_oksn_gsm_enum_type meas_type);

/*===========================================================================

FUNCTION      SRCHCRGSM_GET_GSM_MEAS_BAND_INFO

DESCRIPTION
  This function will call mcal function to allocate W2G RF buffer and send 
  WRM start meas request. This is used for idle W2G search.

DEPENDENCIES
  None

RETURN VALUE
  None
SIDE EFFECTS
  None
===========================================================================*/
void srchcrgsm_get_gsm_meas_band_info(
  wrm_freq_input_type *band_info);

/*===========================================================================

FUNCTION      SRCHCRGSM_ALLOCATE_RF_BUF_AND_SEND_WRM_START_MEAS

DESCRIPTION
  This function will call mcal function to allocate W2G RF buffer and send 
  WRM start meas request. This is used for idle W2G search.

DEPENDENCIES
  None

RETURN VALUE
  TRUE - TRM request was granted
  FALSE - TRM request was denied

SIDE EFFECTS
  None
===========================================================================*/
boolean srchcrgsm_allocate_rf_buf_and_send_wrm_start_meas(
  uint32 dirat);

void srchcrgsm_allocate_w2g_rf_bufs(void);

/*===========================================================================
FUNCTION      SRCHCRGSM_FACH_QUERY_SRCH_IN_PROGRESS

DESCRIPTION
  This API returns the status of ongoing W2G search results in FACH state.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE is W2G FMO search results are pending

SIDE EFFECTS
  None.
===========================================================================*/
extern boolean srchcrgsm_fach_query_srch_in_progress(void);

#ifdef FEATURE_QSH_DUMP
/*===========================================================================
FUNCTION  srchcrgsm_get_idle_fmo_gsm_meas_info

DESCRIPTION
  This function is to collect the idle/fmo w2g meas info for debugging. 

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
extern void srchcrgsm_get_idle_fmo_gsm_meas_info(void *debug_buf, boolean idle_meas);
#endif /* FEATURE_QSH_DUMP */

#endif /* SRCHCRGSM_H */
