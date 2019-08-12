#ifndef TDSSRCHGSM_H
#define TDSSRCHGSM_H

/*============================================================================
                            S R C H C M D R V . H
FILE: tdssrchgsm.c

GENERAL DESCRIPTION
  This file contains implementation of functions for inter frequency and
  inter RAT measurement during the compressed mode.

EXTERNALIZED FUNCTIONS

INTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS


Copyright (c) 2000-2014 by Qualcomm Technologies Incorporated.  All Rights Reserved.

============================================================================*/


/* ==========================================================================
                  INCLUDE FILES FOR MODULE
 ===========================================================================*/

#include "tdscdma_variation.h"
#include "comdef.h"
#include "stdlib.h"

/*from tdscmmeas.h*/
#include "customer.h"
#include "tdsl1rrcif.h"
#include "tdsl1m.h"

#if (defined(FEATURE_CM_SEARCH)) 

//#include "tdsdlinklist.h"
#include "tdssrchgsmdrv.h"
#include "msgr_types.h"

#ifdef TEST_FRAMEWORK
#error code not present
#endif

/* -----------------------------------------------------------------------
** Constant / Define Declarations
** ----------------------------------------------------------------------- */

/***
 ***  TTT Management
 ***/
typedef enum
{
  TDS_TTT_NOT_SET,
  TDS_TTT_SET,
  TDS_TTT_WAITING_TO_REPORT,
  TDS_TTT_REPORTED
} tdssrchgsm_cell_ttt_state_enum_type;

/* Number of cells for which BSIC identification must be done */
#ifdef FEATURE_MINI_BSIC_RECONF_IN_DRX
#define TDSSRCHGSM_BSIC_IDENTIFY_NUM_CELL_MAX    1
#else
#define TDSSRCHGSM_BSIC_IDENTIFY_NUM_CELL_MAX    4
#endif

/* Initial rssi of samples */
/*neveruse*/
#define TDSCRGSM_INIT_RSSI_DB  (-110 * 16)


//#define FEATURE_MEASURE_SCHEDULE_TEST

#define TDSSRCH_MEAS_SET_IDX_INVALID 0xFF
/* Macros for time stamp diff */
#define TDSSRCHGSM_MEAS_MAX_TIME_COUNT_VAL 0xFFFF
#define TDSSRCHGSM_MEAS_TIME_DIFF(this_ts, ref_ts) \
  ((ref_ts) < (this_ts)) ? \
  (((ref_ts) + TDSSRCHGSM_MEAS_MAX_TIME_COUNT_VAL) - (this_ts)) : \
  ((ref_ts) - (this_ts))
   
/* Inter RAT measurement related macro */
/* ----------------------------------- */

/* Max inter-RAT cells */
#define TDSSRCHGSM_SET_INTER_RAT_CELL_MAX 32

#if defined(FEATURE_CM_MEASUREMENT)

  /* GSM measurement related macro */
  /* -------------------------------- */
/* Max GSM cells */
#define TDSSRCHGSM_SRCH_SET_GSM_CELL_MAX TDSSRCHGSM_SET_INTER_RAT_CELL_MAX

/* Max BSIC Id and verify cells */
#define TDSSRCHGSM_CELL_BSIC_IDENTIFY_MAX 8

#ifdef FEATURE_MINI_BSIC_RECONF_IN_DRX
#define TDSSRCHGSM_CELL_BSIC_IDENTIFY_MAX_DRX    1
#else
#define TDSSRCHGSM_CELL_BSIC_IDENTIFY_MAX_DRX    4
#endif

#define TDSSRCHGSM_CELL_BSIC_IDENTIFY_MAX_DCH 4

#define TDSSRCHGSM_CELL_BSIC_IDENTIFY_MAX_FACH 4


#define TDSSRCHGSM_CELL_BSIC_VERIFY_MAX   8

#define TDSSRCHGSM_NUM_MAX_FORBIDDEN_OFFSET 3

  /* GSM RSSI scan related macros */

/* number of frame required to do power meas and process in frame */
#define TDSSRCHGSM_PWR_SCAN_RESULT_READ_FR_DELAY     3
/* Number of power scan command possible per GAP */
#define TDSSRCHGSM_PWR_SCAN_CMD_MAX_PER_GAP 3
/* Min GSM RSSI samples required */
#define TDSSRCHGSM_MIN_PWR_SCAN_SAMPLES 3
/* Max power scan in one measurement period */
#define TDSSRCHGSM_MAX_PWR_SCAN_PER_MEAS_PRD 20
/* measurement period in WCDMA frame units */
#define TDSSRCHGSM_MEAS_PRD_UNIT_TDS_SUBFRAME 96 /* 480 ms */
/* initial RSSI value for LNA gain select */
#define TDSSRCHGSM_INIT_RSSI_VAL (-70 * 16)
/* RSSI must be above this level for cell to be in BSIC verification list */
//#define TDSSRCHGSM_BSIC_VER_MIN_RSSI_DBMX16 (-110 * 16)
/* Timeout and max timer values for RSSI scans */
#define TDSSRCHGSM_MAX_RSSI_MEAS_PRD_W_FR 1000 /* 10 sec */
#define TDSSRCHGSM_RSSI_RESULT_FETCH_TIMEOUT_W_FR 50 /* .5 sec */


  /* BSIC Id related macros */

/* this the maximum number of cell for which BSIC Id meas info 
   is created in update function.
   One is the minimum required. Second is if the first one is not
   valid for BSIC Id (deleted/identified) */
#define TDSSRCHGSM_MAX_BSIC_ID_MEAS_UPDATE_INFO 1
#define TDSSRCHGSM_MAX_BSIC_RECONF_MEAS_UPDATE_INFO 1
/* number of frame after ACQ start cmd frame, cleanup can be sent */
#define TDSSRCHGSM_ACQ_CLEANUP_MIN_FR_DELAY     3

/* in unit W frames 1 second */
/*change it from W 100 frames to TD 200 subframes*/
#define TDSSRCHGSM_BSIC_ID_FORBID_AFTER_F_RECONF_ABOR_TIMER 200 

/* bsic_rc_meas_age absolute max value */
#define TDSSRCHGSM_BSIC_VER_MEAS_AGE_MAX 0xFFFF

  /* SCH decode related macros */

/* number of time the BSIC is allowed to be decoded in a GAP
   eligible to decode the SCH in cell reconfirmation state */
#define TDSSRCHGSM_MAX_BSIC_DEC_ATTEMPTS 2

/* Number of frames needed to produce SCH decode burst matrix */
#define TDSSRCHGSM_SRCH_SCH_DEC_BURST_MATRIX_OFFSET_CMD_FRAME 3
/* Number of frames required for SCH decode result processing after
   the end of burst */
#define TDSSRCHGSM_SRCH_SCH_DECODE_TIME_UNIT_FRAME 3

/* SCH mDSP header length */
#define TDSSRCHGSM_NUM_WORD16_SCH_RX_HDR_MDSP  MDSP_RX_HDR_SIZE
/* SCH decoded data length in mDSP */
#define TDSSRCHGSM_SCH_PAYLOAD_SIZE_MDSP       2
/* SCH decode result buffer.
   5 header word16 + 2 word16 for SCH payload 25 bits */
#define TDSSRCHGSM_NUM_WORD16_SCH_RX_DATA_MDSP \
  (TDSSRCHGSM_NUM_WORD16_SCH_RX_HDR_MDSP + TDSSRCHGSM_SCH_PAYLOAD_SIZE_MDSP)
/* Difference between the mDSP and GL1 HW header size */
#define TDSSRCHGSM_GL1_HW_MDSP_RX_HDR_DIFF     ((sizeof(gl1_defs_rx_hdr_struct)/sizeof(uint16)) - TDSSRCHGSM_NUM_WORD16_SCH_RX_HDR_MDSP)
/* Total SCH reault buffer size */
#define TDSSRCHGSM_NUM_SCH_RESULT_BUF_WORD16 \
  (TDSSRCHGSM_NUM_WORD16_SCH_RX_DATA_MDSP + TDSSRCHGSM_GL1_HW_MDSP_RX_HDR_DIFF)
/* SCH decoded data offset from the */
#define TDSSRCHGSM_SCH_PAYLOAD_OFFSET \
  (TDSSRCHGSM_NUM_WORD16_SCH_RX_HDR_MDSP + TDSSRCHGSM_GL1_HW_MDSP_RX_HDR_DIFF)
/*+CR409965 for update bsic cell count when state transition from FACH to PCH*/
#define TDSSRCHGSM_CHECK_INVALID_CELL_STRUCT_PTR(cell_ptr) \
  ((cell_ptr < (&(tdssrchgsm_meas_inter_rat_cells[0].cell.gsm))) \
    ||(cell_ptr > (&(tdssrchgsm_meas_inter_rat_cells[TDSSRCHGSM_SET_INTER_RAT_CELL_MAX * 2-1].cell.gsm))))
/*+CR409965 for update bsic cell count when state transition from FACH to PCH*/
  
#define TDSSRCHGSM_SCH_GET_BSIC(word16_1, word16_2) \
  ((word16_1 >> 2) & 0x3F)
#define TDSSRCHGSM_SCH_GET_RFN_T1(word16_1, word16_2) \
  (((word16_2 & 0x80) >> 7) | \
   ((word16_1 & 0xFF00) >> (8-1)) | \
   ((word16_1 & 0x3) << 9))
#define TDSSRCHGSM_SCH_GET_RFN_T2(word16_1, word16_2) \
  ((word16_2 >> 2) & 0x1F)
#define TDSSRCHGSM_SCH_GET_RFN_T3_PRIME(word16_1, word16_2) \
  (((word16_2 & 0x100) >> 8) | \
   ((word16_2 & 0x3) << 1))
#define TDSSRCHGSM_SCH_RFN_T3_PRIME_TO_T3(rfn_t3_prime) \
  ((rfn_t3_prime * 10) + 1)
#define TDSSRCHGSM_SCH_RFN_TO_HYPER_FRAME_NUMBER(rfn_t1, rfn_t2, rfn_t3) \
  ((rfn_t1 * 26 * 51)  + (((26 + rfn_t3 - rfn_t2) % 26) * 51) + rfn_t3)
  
  /* GSM meas info update related macros */
  
/* invalid GSM meas info related index */
#define TDSSRCHGSM_MEAS_INFO_IDX_INVALID 0xFF
/* This is the number of frame offset in which the compressed
   mode GAP is there. 0 means that gap is in next WCDMA frame
   1 means GAP is after 1 frame after the next frame */
#define TDSSRCHGSM_ADV_INFO_COUNT 1
//#define TDSSRCHGSM_ADV_INFO_COUNT 4

/* GSM advance info buffer count.
   Keep 1 extra buffer as the GSM mDSP command buffer pointers
   are held in this in the measurement info structure */
#define TDSSRCHGSM_ADV_INFO_BUF_COUNT 2

#define TDSSRCHGSM_OLD_RECONF_STRONG_ID_RSSI_DELTA_DBMX16 (3 * 16)

/*zy modified codes----TBD*/

#define TDSCRHGSM_BSIC_ID_TOBE_ABORT_SUBFRAME  500

#define TDSCRHGSM_BSIC_RC_TOBE_ABORT_SUBFRAME 500

#define TDSCRHGSM_BSIC_ID_ABORT_MAX_SUBFRAME  1000  
#define TDSCRHGSM_BSIC_RC_ABORT_MAX_SUBFRAME  1000  
/*define the min timer between two BSIC RC*/
#define TDSCRHGSM_BSIC_RC_DISTANCE  40 

#define TDSSRCHGSM_MAX_CMD_NUM  3



#endif

/*global variable to enable debugging*/
extern boolean tds_debugging;


//extern uint8 tdscrgsm_cell_idx_bsic_verify;

/* flag to indicate BSIC verification timer expired */
//extern boolean tdscrgsm_bsic_verif_tmr_expired;

extern boolean tdssrchgsm_pending_cleanup;
extern uint8 tdssrchgsm_last_gap_end_count;
extern boolean tdssrchgsm_cm_init_processing;

/*from tdscmmeas.h*/
/* This variable is for doing immediate BSIC ver status update to drivers.
   It is latest snapshot of BSIC verification status in driver */
extern boolean tdssrchgsm_gsm_cell_cur_bsic_ver_status;

/* bsic verification requirements have changed */
/* run nset list cells if any bsic_verify_reqd, set all cells to verify */
extern boolean tdssrchgsm_bsic_verify_update_required;

  /* current state of the measurements, if one requires, all cells will be verified */
#define TDSSRCHGSM_BSIC_VERIFY_UPDATE_NECESSARY()  (tdssrchgsm_bsic_verify_update_required=TRUE)
#define TDSSRCHGSM_BSIC_VERIFY_UPDATE_DONE()       (tdssrchgsm_bsic_verify_update_required=FALSE)
#define TDSSRCHGSM_BSIC_VERIFY_UPDATE_COMPLETE()   ( !tdssrchgsm_bsic_verify_update_required )

extern boolean tdssrchmeas_suspend_gsm_meas_scheduling;

/* -----------------------------------------------------------------------
** Type Declarations
** ----------------------------------------------------------------------- */

/* Inter RAT measurement related type definition */
/* --------------------------------------------- */

/* Inter RAT cell type enum definition */
typedef enum
{
  TDSSRCHGSM_SRCH_INTER_RAT_CELL_UNUSED_TYPE,
  TDSSRCHGSM_SRCH_INTER_RAT_CELL_GSM_TYPE,
  TDSSRCHGSM_SRCH_INTER_RAT_CELL_IS95_TYPE,
  TDSSRCHGSM_SRCH_INTER_RAT_CELL_IS2000_TYPE
} tdssrchgsm_srch_inter_rat_cell_type_enum_type;


  /* Inter RAT GSM measurement related type definition */
  /* --------------------------------------------- */

/* Measurement purpose for GSM cells */
typedef enum
{
  TDSSRCHGSM_RSSI_SCAN,
  /*add fcch detection according to measurement priority*/
  TDSSRCHGSM_TONE_DETECT,
  TDSSRCHGSM_BSIC_IDENTIFY,
  TDSSRCHGSM_BSIC_RECONF,
  /*BSIC RC or ID is about to timeout*/
  TDSSRCHGSM_BSIC_TIMEOUT
} tdssrchgsm_meas_purpose_enum_type;

/* GSM cell state */
typedef enum
{
  TDSSRCHGSM_SRCH_SET_UNUSED,
  TDSSRCHGSM_SRCH_SET_DELETED,
  TDSSRCHGSM_SRCH_SET_USED
} tdssrchgsm_drv_cell_state_enum_type;

/* GSM cell measurement state */
typedef enum
{
  TDSSRCHGSM_SRCH_MEAS_NA,
  TDSSRCHGSM_SRCH_MEAS_DELETED,
  TDSSRCHGSM_SRCH_MEAS_RSSI,
  TDSSRCHGSM_SRCH_MEAS_BSIC_IDENTIFY_ABORTED,
  TDSSRCHGSM_SRCH_MEAS_BSIC_IDENTIFY,
  TDSSRCHGSM_SRCH_MEAS_BSIC_RECONF
} tdssrchgsm_srch_meas_state;

/***
 ***  L1CMSRCH State
 ***/
typedef enum
{
  TDSSRCHGSM_DCH_INACTIVE,
  TDSSRCHGSM_DCH_ACTIVE
} tdssrchgsm_state_enum_type;

/* GSM cell BSIC Id structure definition */
typedef struct
{
  /* Only lower 3 bits are valid */

  /* network colour code */
  uint8 ncc;
  /* base station colour code */
  uint8 bcc;
} tdssrchgsm_bsic_struct_type;

/* GSM cell for measurement info structure definition */
typedef struct
{
  /* This points to the inter-RAT cell strcuture pointer that
     include this cell info */
  void* cell_inter_rat_self_ptr;

  /* frequency channel number */
  W_ARFCN_T arfcn;
#if 0
  /* Indicate if BSIC verification required or NOT */
  /*------ the field will be removed later ------*/   boolean bsic_verify_reqd;
#endif
  /* measurement state/result */
  /* ------------------------ */

  /* GSM cell state */
  tdssrchgsm_drv_cell_state_enum_type cell_state;

  /* GSM search/meas state */
  tdssrchgsm_srch_meas_state meas_state;

  /* When the measurement command for the cell is sent, its value is set as TRUE. 
    * When to receive the cell's measurement result, its value will be set as FALSE.*/
  boolean meas_ongoing; /*++++++ this is a new field ++++++*/

  /* If this cell is to be included for rssi measurements. Only one arfcn measurement 
    * suffice. So we take cell for RSSI measurements only when this flag is TRUE.*/
  boolean unique_arfcn;  /*++++++ this is a new field ++++++*/
  
  /* RSSI measured from power scan */
 /*------ the field will be removed later ------*/ 
 /*zy note:this field will be kept now because it will be used when scheduled
    maybe delete in long future*/
 boolean rssi_valid;
 /*------ the field will be removed later ------*/  
 /*zy note:this field will be kept now because it will be used when scheduled
    maybe delete in long future*/
 int16 rssi_dbmx16;

  /* indicate that this cell is under BSIC Id */
  boolean bsic_id_active;

  /* timing information for FCCH aquisition */
  boolean timing_valid;
  uint8 tdsframe_offset;
  uint16 qs_offset;

  uint32 hyper_frame_offset;

  /* this information is for a forbidden QS offset for this cell
     It is reset at entry in to BSIC Id state and also reset
     when coming back from BSIC Id aborted */
  uint8  num_forbidden_offset;
  uint8  forbidden_frame_offset[TDSSRCHGSM_NUM_MAX_FORBIDDEN_OFFSET];
  uint16 forbidden_qs_offset[TDSSRCHGSM_NUM_MAX_FORBIDDEN_OFFSET];

  /* Frequency error for this cell */
  int16  frequency_offset;

  /* timer for barring cell to get RSSI sorted and be involved in
     BSIC Id process after reconfirm abort. Desired position of
     forbidden cell is at end of list. All cell sort and move function
     looks at end of BSIC Id list and excludes cells that have this
     flag set. Also timer update function looks at same place to
     update timer and update timer and reset forbid_bsic_id flag */
  boolean forbid_bsic_id;
  uint16  forbid_bsic_id_timer;

  /* BSIC for the cell found in SCH decode */
  boolean bsic_valid;
  tdssrchgsm_bsic_struct_type bsic;

  /* number of gaps used to BSIC identify */
  //uint16 num_gaps_used_bsic_id;
  /* indicate for how long the BSIC id has been tried,
     in units of TDS subframe */
  uint16 bsic_id_meas_age;
  /* indicate for how long the BSIC was last verified,
     in units of TDS subframe */
  uint16 bsic_rc_meas_age;
  /* number of times BSIC is failed to be decoded */
  uint8 bsic_rc_dec_fail_attempt;
  
} tdssrchgsm_drv_cell_struct_type;

/* ARFCN RSSI sample information strcuture */
typedef struct
{
  /* indicate that this ARFCN info is valid */
  boolean valid;

  /* ARFCN value */
  W_ARFCN_T arfcn;

#ifdef FEATURE_TDSCDMA_ABSOLUTE_PRIORITY	
	boolean prior_present;
	int8 priority;
#endif

  /* RSSI sample head and tail. At start head = tail = 0 */
  uint8 sample_head;
  uint8 sample_tail;

  /* last measured RSSI value */
  int16 last_meas_rssi_dbmx16;
  /* indicate that RSSI has been measured at least once averaged over 3
     samples in a measurement period */
  boolean rssi_valid;

  /* RSSI samples for averaging */
  int16 rssi_dbmx16_samples[TDSSRCHGSM_MAX_PWR_SCAN_PER_MEAS_PRD];
  /* sample timestamp */
  uint16 sample_ts[TDSSRCHGSM_MAX_PWR_SCAN_PER_MEAS_PRD];

#ifdef FEATURE_MEASURE_SCHEDULE_TEST
  uint16 rssi_schedule_stamp;
  uint16 total_rssi_schedule_no;
  uint16 average_rssi_schedule_interval;
  uint16 min_rssi_schedule_interval;
  uint16 max_rssi_schedule_interval;
#endif
} tdssrchgsm_arfcn_info_struct_type;


/*it is a wrap round value 0-255*/
extern uint8 tdssrchgsm_message_id;

/* GSM measurement info struct definition.
   This structure keeps the parameters for various type of measurement and
   the GSM command parameters. The meas info is the UNION of all types of
   possible measurement. The measurement type is indicated by GSM measurement
   type. */
typedef struct
{
  /* indicate if this measurement info is valid. The info is Invalid
     to start with. If gets updates with each GSM advance info update */
  boolean valid;

  /* indicate that this information is in use and shall not be over-written */
  boolean in_use;

  /* indicate the CFN at which this info is applicable */
  uint16 sfn;

  /* GSM measurement type. indicate the type of measurement info kept
     in the union below */
  tdssrchgsm_meas_purpose_enum_type meas_purpose;

  /* indicate that gap is used with overridden or alternate purpose */
  /*zy note:this field will be deleted later*/
  //boolean gap_purpose_is_oride;

  tdssrchgsmdrv_gap_param_tds_struct_type tds_gap_params;
  /* GSM GAP parameters. indicate GFN, QS offset etc. */
  tdssrchgsmdrv_gap_param_gsm_struct_type gsm_gap_params;
  /*GSM startup and cleaup command parameters. This is applicable to any type of
       GSM measurement */
  tdssrchgsmdrv_gsm_startup_cmd_param_struct_type gsm_startup_cmd_params;
  tdssrchgsmdrv_gsm_cleanup_cmd_param_struct_type gsm_cleanup_cmd_params;

  union
  {
    /* RSSI scan info */
    /* ------------- */

    /* RSSI scan GSM measurement and command parameters */
    struct
    {
      /* RSSI scan measurement parameters */
      uint8 num_max_cells;
      tdssrchgsmdrv_gsm_pwr_scan_params_struct_type pwr_scan_params;

      tdssrchgsmdrv_gsm_pwr_scan_cmd_param_struct_type ps_cmd_params;

      /*Xiaoning - to be deleted */
      /* RSSI scan GSM command parameters */
      //uint8 num_pwr_scan_cmd;
      //tdssrchgsmdrv_gsm_pwr_scan_cmd_param_struct_type cmd_params[TDSSRCHGSM_PWR_SCAN_CMD_MAX_PER_GAP];
    } pwr_meas;

    /* BSIC Id parameter */
    /* ----------------- */

    struct
    {
      /* trigger function set this value to save the cell index in the meas params array
         that is a candidate for this measurement */
      uint8 meas_cell_info_idx;
      /* number of cells in the candidate array */
      uint8 num_cell_info;
      struct
      {
        /* BSIC Id measurement parameters */
        tdssrchgsm_drv_cell_struct_type *cell_ptr;
        tdssrchgsmdrv_gsm_bsic_id_params_struct_type bsic_id_params;

        /* FCCH ACQ detect OR SCG decode command parameters */
        union
        {
          struct
          {
            tdssrchgsmdrv_gsm_start_acq_cmd_param_struct_type acq_start;
            tdssrchgsmdrv_gsm_stop_acq_cmd_param_struct_type  acq_stop;
          } acq_cmd;
          /*zy note:modify from sch_decode[TDSSRCHGSMDRV_GSM_SCH_DEC_CMD_MAX_PER_GAP] to sch_decode_cmd*/
          tdssrchgsmdrv_gsm_sch_decode_cmd_param_struct_type sch_decode_cmd;
        } cmd_params;
      } bsic_id_info[TDSSRCHGSM_MAX_BSIC_ID_MEAS_UPDATE_INFO];
    } bsic_id;

    /* BSIC reconf parameter */
    /* --------------------- */

    struct
    {
      /* trigger function set this value to save the cell index in the meas params array
         that is a candidate for this measurement */
      uint8 meas_cell_info_idx;
      /* Number of cell in the candidate measurement parameter list */
      uint8 num_cell_info;
      struct
      {
        /* SCH decode measurement parameters */
        tdssrchgsm_drv_cell_struct_type *cell_ptr;
        tdssrchgsmdrv_gsm_bsic_reconf_params_struct_type bsic_reconf_params;

        /* SCH decode GSM command parameters */
        tdssrchgsmdrv_gsm_sch_decode_cmd_param_struct_type cmd_params;
      } bsic_reconf_info[TDSSRCHGSM_CELL_BSIC_VERIFY_MAX];
    } bsic_reconf;

  } meas_info;
} tdssrchgsm_meas_info_struct_type;

  /* GSM measurement result poll info structures */
  /* ---------------------------------------------- */

/* RSSI scan result poll info struct definition */
typedef struct
{
  /* Number of RSSI scan command in this GAP */
  //uint8 num_cmds;
  /* Number of command result received */
  //uint8 num_result_rece;

  /* number of commands sent to mDSP */
  //uint8 num_cmds_waiting;
  /* number of commands sent to mDSP */
  uint8 num_result_waiting;

  uint8 cmd_read;

  uint8 cmd_write;
  /* timeout after number after commands are scheduled */
  //uint8 result_fetch_timeout_unit_wframe;

  /* Each command result info */
  struct
  {
    uint8 message_id;
    /* GFN when the result of this command is expected */
    uint8 result_frame_num;
    /* Number of GSM bursts in this command */
    uint8 num_bursts;
    /* GSM cell array indicies for each bursts */
    uint8 cell_idx[TDSSRCHGSMDRV_MAX_PWR_MEAS_BURST_PER_GSM_FR];
    /* frequency channel number */
    W_ARFCN_T arfcn[TDSSRCHGSMDRV_MAX_PWR_MEAS_BURST_PER_GSM_FR];
    /* measured RSSI for each burst */
    int16 rssi_dbmx16[TDSSRCHGSMDRV_MAX_PWR_MEAS_BURST_PER_GSM_FR];

    /* LNA state of each burst */
    uint8 lna_state[TDSSRCHGSMDRV_MAX_PWR_MEAS_BURST_PER_GSM_FR];

    gl1_hw_rx_tags_type tags[TDSSRCHGSMDRV_MAX_PWR_MEAS_BURST_PER_GSM_FR];
  } cmd_result[TDSSRCHGSM_MAX_CMD_NUM];
} tdssrchgsm_rssi_meas_result_struct_type;

extern tdssrchgsm_rssi_meas_result_struct_type tdssrchgsm_meas_rssi_result;

extern uint8 tdssrchgsm_fcch_detect_result_head;

extern uint8 tdssrchgsm_sch_decode_result_head;

extern uint8 tdssrchgsm_num_fcch_decode_result_waiting;
    
extern uint8 tdssrchgsm_num_sch_decode_result_waiting;
    

#ifdef TEST_FRAMEWORK 
  #error code not present
#endif


/* FCCH tone detect poll struct information */
typedef struct
{

  uint8 message_id;
  /* indicate that command for FCCH detect has been sent to mDSP */
  //boolean acq_start_cmd_sent;
  //boolean acq_stop_cmd_sent;
  //boolean acq_cleanup_req_sent;

  /* cell pointer for which the FCCH ACQ is being done */
  tdssrchgsm_drv_cell_struct_type *cell_ptr;
  /* ACQ start number. Used to establish initial frame offset */
  uint16 acq_start_frame;
  /* ACQ start window offset. Used to establish QS offset */
  uint16 acq_win_start_offset;

  /* GSN at which the result is expected */
  uint8 result_frame_num;
  /* Number of frames for which the FCCH ACQ result need to be polled.
     This is based on the ACQ windoe length */
  //uint8 num_results;
  /* frame number in which to send cleanup */
  uint8 cleanup_req_frame_num;

  /* Number of frames for which result has been polled */
  //uint8 num_result_polled;
  /* indicate if tone is detected in polled frames */
  boolean tone_is_detected;
  /* SNR of last strongest tone detected */
  uint16 tone_snr;
  /* Tone offset w.r.t. ACQ window for last strongest tone detected */
  int16 tone_offset;
  /* Frequency offset for the tone */
  int16 tone_freq_offset;
} tdssrchgsm_fcch_detect_result_struct_type;

/* SCH decode poll info structure definition */
typedef struct
{
  uint8 message_id;
  /* indicate that command for SCH decode has been sent to mDSP */
  //boolean cmd_sent;
  /* indicate this result was meant for bsic Id */
  boolean purpose_bsic_id;
  /* cell pointer for which the SCH decode is being done */
  tdssrchgsm_drv_cell_struct_type *cell_ptr;
  /* Current timeline GFN at which SCH is being decoded. This is the frame
     burst at which SCH burst starts */
  uint32 decode_frame_num;
  /* Hyper frame number in current timeline at which SCH burst start */
  uint32 decode_hyper_frame_num;

  /* GFM at which to get the SCH decode burst matrix */
  uint8 burst_matric_frame_num;
  /* GFN at which result is expected */
  uint8 result_frame_num;

  boolean time_valid;
  int32 time_offset;

  #if 0
  /* SCH decode burst matric contents */
  boolean burst_matric_valid;
  gl1_defs_metrics_rpt burst_matric_rpt;
  #endif
  /* SCH decode status. TRUE is burst is received and CRC passes */
  boolean sch_decode_status;
  /* SCH decode header and data buffer */
  //uint16 buffer[TDSSRCHGSM_NUM_SCH_RESULT_BUF_WORD16];  
  uint16 buffer[TDSSRCHGSM_SCH_PAYLOAD_SIZE_MDSP];
  
  /* GL1 HW abstraction to allow RF parameters to be passed back to GL1 HW driver
     This is transparent to WL1. */
  gl1_hw_rx_tags_type tags;
} tdssrchgsm_sch_decode_result_struct_type;


/* Inter RAT cell structure definiiton.
   Only GSM cell definition is included */
typedef struct tdssrchgsm_inter_rat_cell_struct_tag_type
{
  /* indicate the cell is in use of not */
  boolean cell_used;
  /* Cell type */
  tdssrchgsm_srch_inter_rat_cell_type_enum_type cell_type;
  /* Host info pointer */
  void* host_info_ptr;

  union
  {
    /* cell self pointer. This MUST be the first cell in the
       cell structure definition */
    struct tdssrchgsm_inter_rat_cell_struct_tag_type* self_ptr;
    /* GSM calls */
    tdssrchgsm_drv_cell_struct_type gsm;
  } cell;
}tdssrchgsm_inter_rat_cell_struct_type;

/* -----------------------------------------------------------------------
** Global Data Declarations
** ----------------------------------------------------------------------- */

/* number of the GSM cells */
extern uint8 tdssrchgsm_drv_cell_cnt;
/* All GSM cells participating in RSSI measurement */
extern tdssrchgsm_drv_cell_struct_type *tdssrchgsm_drv_cell_ptr[TDSSRCHGSM_SRCH_SET_GSM_CELL_MAX];

/* current cell index that is under BSIC confirmation */
extern tdssrchgsm_drv_cell_struct_type *tdssrchgsm_drv_cell_bsic_verified;

/* RSSI measurement age in units of WDMA frame (10 ms) */
extern uint32 tdssrchgsm_rssi_meas_age;

/* this indicate that interRAT search is done */
extern boolean tdssrchgsm_rssi_meas_done_status;
extern boolean tdssrchgsm_bsic_verified_status;
extern boolean tdssrchgsm_cell_bsic_ver_status_update_pending;

extern tdssrchgsm_fcch_detect_result_struct_type tdssrchgsm_fcch_detect_result[TDSSRCHGSM_MAX_CMD_NUM];
extern tdssrchgsm_sch_decode_result_struct_type tdssrchgsm_sch_decode_result[TDSSRCHGSM_MAX_CMD_NUM];

extern tdssrchgsm_state_enum_type  tdssrchgsm_srch_state;

/* =======================================================================
**                        Function Declarations
** ======================================================================= */

extern uint8 tdssrchgsm_pick_tobe_expire_bsic
( tdssrchgsmdrv_gap_param_gsm_struct_type *gap_info );



extern boolean tdssrchgsm_check_sch_id_fall_in_gap(
  /* GSM meas info struct pointer */
  uint8 index_cell,
  /* transmission gap parameter */
  tdssrchgsmdrv_gap_param_gsm_struct_type *gap_param_for_gsm);


extern boolean tdssrchgsm_check_sch_rc_fall_in_gap(
  /* GSM meas info struct pointer */
  uint8 index_cell,
  /* transmission gap parameter */
  tdssrchgsmdrv_gap_param_gsm_struct_type *gap_param_for_gsm);

extern uint8 tdssrchgsm_sche_gsm_meas
(uint8 gap_num,
 tdssrchgsmdrv_gap_param_gsm_struct_type *gsm_gap_params_ptr,
 tdssrchgsm_meas_purpose_enum_type meas_purpose);

/* =======================================================================
**                            Function Definitions
** ======================================================================= */

/*===========================================================================
FUNCTION tdssrchgsm_meas_init
         tdssrchgsm_meas_init_on_resume

DESCRIPTION
  This function initializes the GSM search and measurement related database,
  variables and states. The following initializations are done:

  WCDMA Inter-Frequency:
  - None (Most of then are done in WCDMA search module as the inter-frequency
    cells shares the same structures as the intra-frequenct cell do.)
  Inter-RAT GSM:
  - Set the GSM new cell counts to 0
  - Zero out all of the cell structures.

    RSSI measurement
  - Initializes the cell pointers to NULL.
  - Set the GSM cell (active and deleted) counts to 0
  - RSSI measurement last GSM cell index to INVALID

    BSIC verification
  - Set the BSIC verification cell count to 0
  - Set the cell pointers to NULL
  - Set the GSM BSIC id and reconf cell index to INVALID

    Result processing
  - Set the RSSI measurement call back function

DEPENDENCIES
  This function must be called after the GSM search drivers are initialized
  otherwise the result processing call back function will be over written.

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void tdssrchgsm_meas_init_on_resume(void);
extern void tdssrchgsm_meas_init(void);

/*===========================================================================
FUNCTION tdssrchgsm_t2g_ctrl_nv_init

DESCRIPTION
  This function initiates the global variables to enable/disable T2G IRAT acitvities in FACH, DCH and IDLE states, using NV item.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void tdssrchgsm_t2g_ctrl_nv_init(void);

/*===========================================================================
FUNCTION tdssrchgsm_meas_update_time_counter

DESCRIPTION
  This function increments time counter

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void tdssrchgsm_meas_update_time_counter(void);

/*===========================================================================
FUNCTION tdssrchgsm_meas_init_cell_db

DESCRIPTION
  This function initializes the cell database.

  Inter-Frequency cell DB init
  - Set the cell count to 0
  - set the cell pointers to NULL
  - Set the current cell under search to INVALID

  Inter-RAT
  - Sets the cell count to 0
  - Zero out the cell structure
  - Initialize the cell pointer to cell structure pointers

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void tdssrchgsm_meas_init_cell_db(void);

extern void tdssrch_meas_init_cell_db(void);

/*===========================================================================
FUNCTION srchcm_meas_update_num_cmd_sent
         tdssrchgsm_update_sch_decode_cmd_sent

DESCRIPTION
  This function updates the number of command(s) sent to mDSP. This is used
  for unwinding the RF drivers at the time of cleanup.

  The caller of these functions must lock the W2G mutex.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void tdssrchgsm_update_rssi_num_cmd_sent(void);
extern void tdssrchgsm_update_fcch_decode_cmd_sent(void);
extern void tdssrchgsm_update_sch_decode_cmd_sent(void);

extern void tdssrchgsm_cleanup_cmd_cb_0(void);
extern void tdssrchgsm_cleanup_cmd_cb_1(void);
extern void tdssrchgsm_cleanup_cmd_cb_2(void);

/*===========================================================================
FUNCTION tdssrchgsm_do_gsm_meas

DESCRIPTION
  This function actually performs the GSM measurements. It uses the measurement
  info pointer saved by trigger function and calls the appropriate function
  based on purpose to do the measurement.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

extern void tdssrchgsm_do_gsm_meas(
  /* WCDMA CFN in which gap start */
  uint16 sfn);

/*===========================================================================
FUNCTION tdssrchgsm_dummy_process_rssi_result

DESCRIPTION
  This function sorts out the bsic verification list once the RSSI result is
  done and all the states are changed to IDLE.

  This function is called from GSMTR ISR as well as from TASK when a cleanup
  is happening. The caller must get the W2G mutex. This function does not
  grab it!!!

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void tdssrchgsm_dummy_process_rssi_result(void);

/*===========================================================================
FUNCTION tdssrchgsm_trigger_gsm_rssi_meas

DESCRIPTION
  This function triggers the GSM RSSI (power scan) measurements. It updates
  the cell ARFCN to the mDSP commands for power scan. It returns the trigger
  status based on power scan commands required for mDSP. False is returned
  on following conditions:
  - No GSM cell present
  - Number of cells that can be measured in this GAP is 0.
  - Number of doable cells left in this RSSI measurement cycle is 0. This
    condition can only happen if the remaining GSM cell are all deleted one
    with measurement purpose NA.
  It also updates the result processing related information so that result
  can be read and processed.

DEPENDENCIES
  None

RETURN VALUE
  TRUE or FALSE as per trigger status

SIDE EFFECTS
  None
===========================================================================*/

extern boolean tdssrchgsm_trigger_gsm_rssi_meas(
  /* measurement info for RSSI measurement */
  tdssrchgsm_meas_info_struct_type *meas_info);

/*===========================================================================
FUNCTION tdssrchgsm_adjust_gsm_gap_start_rssi_meas

DESCRIPTION
  This function adjusts the GSM gap start time when it's RSSI purpose. The start time needs to be adjusted when 
  the gap has more than 4 time slots, and the gap starts from time slot 2.
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean tdssrchgsm_adjust_gsm_gap_start_rssi_meas(
  /* transmission gap parameter */
  tdssrchgsmdrv_gap_param_gsm_struct_type *gap_param_for_gsm,
  /* GSM meas info struct pointer */
  tdssrchgsm_meas_info_struct_type *meas_info);

/*===========================================================================
FUNCTION tdssrchgsm_rssi_meas_recal_gsm_gap_start

DESCRIPTION
  This function is to recalculate the GSM gap start time when it's RSSI purpose.
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean tdssrchgsm_rssi_meas_recal_gsm_gap_start(
  /* transmission gap parameter */
  tdssrchgsmdrv_gap_param_gsm_struct_type *gap_param_for_gsm,
  /* GSM meas info struct pointer */
  tdssrchgsm_meas_info_struct_type *meas_info,
 /*Number of GSM cells measured in this gap*/
  uint8 num_cells_for_rssi_meas);


/*===========================================================================
FUNCTION tdssrchgsm_update_gsm_rssi_meas

DESCRIPTION
  This function update the RSSI measurement related information based on the
  compressed mode GAP information. This function assumes that F/W can process
  the number of RSSI measurement as specified in Spec. It distributes the
  measurement position for the possible cell count in the available gap.
  While calculating the available gap space it excludes the time required
  for GSM start up and cleanup. These are defined as macros and can be tuned.
    TDSSRCHGSMDRV_GSM_T_START_QS_PWR_SCAN
    TDSSRCHGSMDRV_GSM_T_CLEAN_QS_PWR_SCAN
  These are specified in GSM quater symbol resolution.

  It calls the GSM driver function to get the burst window position and actual
  burst start offset for the commands.

  Burst window: This include the burst setup time and actual burst sample
  accumulation time. Actual burst is always at the end of burst window.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

extern void tdssrchgsm_update_gsm_rssi_meas(
  /* gap length in wcdma slots */
  uint8 gap_length_slots,
  /* transmission gap parameter */
  tdssrchgsmdrv_gap_param_gsm_struct_type *gap_param_for_gsm,
  /* GSM meas info struct pointer */
  tdssrchgsm_meas_info_struct_type *meas_info);

/*===========================================================================
FUNCTION tdssrchgsm_handle_rssi_meas_result

DESCRIPTION
  This function is a call back function. GSM frame tick handler calls this
  function to poll the RSSI measurement result. WHile triggering the RSSI
  measurement the result processing parameters are updated. In that GFN
  of the current command to poll indicate the GFN when the results are available.
  Once the GFN mactes the GFN for which the command is to be read, it calls the
  GSM driver function to read the RSSI result for that command. If all the
  command results are available it then calls the function to process the RSSI
  reault.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

extern void tdssrchgsm_handle_rssi_meas_result(
  gsm_irat_meas_search_rsp_t *gsm_rssi_result);

/*===========================================================================
FUNCTION tdssrchgsm_process_rssi_meas_result

DESCRIPTION
  This function processes the RSSI result. It updates the RSSI to cell
  structure.

  It also checks if all the GSM cells have been measured for RSSI,it triggers
  the RSSI processing that sorts the cells as per RSSI value and updates the
  BSIC verification list. The result processing also includes the addition of
  new cells being added and the removal of cells being deleted. Cells are not
  dynamically added/removed from list but are done in this post processing.
  All this is done in the L1 task context. Signal is set if all the measurement
  process RSSI measurement (which is surely done by now here), BSIC verification
  (ID and reconf) are done and in the idle state. If any of the measurement
  state is busy then it sets the global for each busy task so that it can set
  the L1 task to post process the result when it is done.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

extern void tdssrchgsm_process_rssi_meas_result(void);

/*===========================================================================
FUNCTION tdssrchgsm_sort_rssi_update_bsic_ver_list

DESCRIPTION
  This function must be called in task context. After calling this function
  the measurement control and report module may copy the necessary result variable
  to their local copy for evaluation and free the measurement state to IDLE so that
  measurement can proceed.
  
  This function do the following things in the order mentioned below.
  - Remove any deleted cell from the list
  - Do RSSI averaging of the all the samples received
  - Do the RSSI sorting only if there is cell left after processing the delete list
  - Create the Cell verification list. These are with highest N RSSI
  - Remove unwanted cell from BSIC verification list. Cell may be deleted or may have
    dropped from highest N RSSI cell that need BSIC verification
  - Add the cell that are not in BSIC verification list for initial BSIC identification
  - Add new cell to RSSI meas

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void tdssrchgsm_sort_rssi_update_bsic_ver_list(void);

/*===========================================================================
FUNCTION tdssrchgsm_check_reset_bsic_id_abort_cell_list

DESCRIPTION
  This function check if all teh cells have been aborted in BSIC Id phase.
  In this case it reset the state of all cells to BSIC Id and resorts the cells.

  It is called
  1. When from BSIC ver list refesh (task context)
     The covers if all BSIC ver list has aborted cells as result of all BSIC Id
     cells getting deleted.

  OR

  2. When any BSIC Id cell transitions to BSIC Id abort
     2.1 GSM gap with BSIC Id trigger
     2.2 At BSIC Id result poll
  
  1 and 2.2 may not be mutully exclusive but 1 and 2.1 are not. So TDS_INTLOCK
  the whole duration

  This function locks W2G mutex internally.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void tdssrchgsm_check_reset_bsic_id_abort_cell_list(void);

/*===========================================================================
FUNCTION tdssrchgsm_list_srch_egy_sort_comp_fn

DESCRIPTION
  RSSI sort function for Qsort

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

extern int8 tdssrchgsm_rssi_sort_comp_fn(
  const void *p1,
  const void *p2);
/*+CR409965 for update bsic cell count when state transition from FACH to PCH*/
/*===========================================================================
FUNCTION tdssrchgsm_update_bsic_list_transition_from_FACH_to_PCH

DESCRIPTION
  update the bsic verification list count when the state transition from FACH to PCH

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void tdssrchgsm_update_bsic_list_transition_from_FACH_to_PCH(void);
/*-CR409965 for update bsic cell count when state transition from FACH to PCH*/
/*===========================================================================
FUNCTION tdssrchgsm_update_bsic_ver_list

DESCRIPTION
  This function updates BSIC verification list. This isintended to be called
  in task context. Typical usage will be to call after every RSSI measurement
  done signal or can be called any time tehre is change in BSIC verification
  required status of GSM cell list.
  It look for global tdssrchgsm_cell_bsic_ver_status_update_pending and if
  TRUE it updates bsic_verify_reqd member in all cell structure with value
  of global tdssrchgsm_bsic_ver_status_to_update.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

extern void tdssrchgsm_update_bsic_ver_list(void);

/*===========================================================================
FUNCTION tdssrchgsm_update_bsic_ver_status

DESCRIPTION
  This function updates the BSIC verification required status for all GSM
  cells. It put the BSIC Id and BSIC reconf state in PROC state so that
  it can iterate thro' the list.
  If it can't put the BSIC id or BSIC reconf to PROC state it will set their
  respective global flag
  
  BSIC Id: tdssrchgsm_cell_bsic_ver_status_update_after_bsic_id_done
  BSIC reconf: tdssrchgsm_cell_bsic_ver_status_update_after_bsic_reconf_done
  
  BSIC verfication required state is saved to global variable
  tdssrchgsm_bsic_ver_status_to_update

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

extern void tdssrchgsm_update_bsic_ver_status(
  /* indicate BSIC verify status to update */
  boolean bsic_ver_reqd);

/*===========================================================================
FUNCTION tdssrchgsm_tone_detect_needed (void)

DESCRIPTION
  This function to check whether the tone detect will be needed in current subframe

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern boolean tdssrchgsm_tone_detect_needed(void);

/*===========================================================================
FUNCTION tdssrchgsm_bsic_id_abort_cell

DESCRIPTION
  This function do BSIC Id abort for a cell

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void tdssrchgsm_bsic_id_abort_cell(
  /* GSM cell structure pointer */
  tdssrchgsm_drv_cell_struct_type *cell_ptr);

/*===========================================================================
FUNCTION tdssrchgsm_query_known_cell_qs_offset

DESCRIPTION
  This function queries the known cell offset in BSIC reconfirmation list,
  if it exists returns the offset in function argument frame offset and
  qs_offset passed as pointer.

DEPENDENCIES
  None

RETURN VALUE
  Number of cells found forbidden

SIDE EFFECTS
  None
===========================================================================*/

extern uint8 tdssrchgsm_query_known_cell_qs_offset(
  /* cell ARFCN to cehck in reconf list */
  W_ARFCN_T arfcn,
  /* number of max cell to find out */
  uint8 num_max_cell,
  /* if cell exists, tdsframe_offset is returned in this */
  uint8 *tdsframe_offset,
  /* if cell exists, qs_offset is returned in this */
  uint16 *qs_offset);

/*===========================================================================
FUNCTION tdssrchgsm_trigger_gsm_bsic_id_meas

DESCRIPTION
  This function triggers the BSIC Id measurement. It scanns through the list of
  cells in the measurement info. It assumes that cells will be put in the order
  if priority of their measurement state. It just goes thro' the list and grab
  the first available one.

DEPENDENCIES
  None

RETURN VALUE
  TRUE or FALSE as per trigger status.

SIDE EFFECTS
  None
===========================================================================*/

extern boolean tdssrchgsm_trigger_gsm_bsic_id_meas(
  /* GSM measurement info */
  tdssrchgsm_meas_info_struct_type *meas_info);


/*===========================================================================
FUNCTION tdssrchgsm_update_gsm_bsic_id_meas

DESCRIPTION
  This function updates the BSIC Id parameters of all cells that can do BSIC
  identification. It receives the gap parameters and calculate the search
  parameters. In the BSIC Id there are 2 parts.
  FCCH timing detect: If the timing is not known then it calculates the FCCH
                      aquisition window start position and window size.
  SCH decode: If the FCCH timing is known then it checks if the SCH decode can
              done in the GAP. If it can be done it updates the SCH decode
              position in the gap in measurement info.

  It scans through the list of cells that can do BSIC Id. These are the cell
  with timing invalid and/or BSIC invalid. Each cell can have two measurement
  states
    BSIC Identify (This is bSIC identify for the first time)
    BSIC Id aborted (This cell has timed out for number of gaps to be utilzed
                     for identifying the BSIC)
  Former one is given preference over the latter. The first cell that satisfy
  this criteria is chosen.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

extern void tdssrchgsm_update_gsm_bsic_id_meas(
  /* transmission gap parameter */
  tdssrchgsmdrv_gap_param_gsm_struct_type *gap_param_for_gsm,
  /* GSM meas info struct pointer */
  tdssrchgsm_meas_info_struct_type *meas_info,
  boolean appoint_cell_flag,uint8 cell_index);


extern void tdssrchgsm_update_gsm_tone_det_meas
(
  /* transmission gap parameter */
  tdssrchgsmdrv_gap_param_gsm_struct_type *gap_param_for_gsm,
  /* GSM meas info struct pointer */
  tdssrchgsm_meas_info_struct_type *meas_info,
  boolean appoint_cell_flag,uint8 cell_index);


extern void tdssrchgsm_sort_meas_info_start_time(void);

/*===========================================================================
FUNCTION tdssrchgsm_handle_fcch_detect_result

DESCRIPTION
  This function is a call back function. GSM frame tick handler calls this
  function to poll the FCCH detect result. 

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

extern void tdssrchgsm_handle_fcch_detect_result(
gsm_irat_acquisition_rsp_t *gsm_acq_result);

/*===========================================================================
FUNCTION tdssrchgsm_handle_sch_decode_result

DESCRIPTION
  This function is a call back function. GSM frame tick handler calls this
  function to poll the SCH decode result. This is a common poll function for
  SCH decode on BSIC Id and BSIC reconfirmation.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

extern void tdssrchgsm_handle_sch_decode_result(
gsm_irat_sch_burst_rsp_t *gsm_sch_result);

/*===========================================================================
FUNCTION tdssrchgsm_update_bsic_id_result_after_sch_decode

DESCRIPTION
  This function processes the SCH decode resulr for BSIC identification.
  It do the appropriate measurement state transition as needed.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

extern boolean tdssrchgsm_update_bsic_id_result_after_sch_decode(
  /* GSM cell pointer info */
  tdssrchgsm_drv_cell_struct_type *cell_ptr,
  /* SCH decode result */
  boolean sch_decode_result);

/*===========================================================================
FUNCTION tdssrchgsm_update_bsic_reconf_result_after_sch_decode

DESCRIPTION
  This function processes the SCH decode resulr for BSIC re-confirmation.
  It do the appropriate measurement state transition as needed.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

extern boolean tdssrchgsm_update_bsic_reconf_result_after_sch_decode(
  /* GSM cell pointer info */
  tdssrchgsm_drv_cell_struct_type *cell_ptr,
  /* SCH decode result */
  boolean sch_decode_result);

/*===========================================================================
FUNCTION tdssrchgsm_bsic_reconf_abort_timer_update

DESCRIPTION
  This function update the reconf abort update timer. This is called every
  WCDMA frame by L1 GSM manager to check the BSIC reconfirm timer and BSIC
  decode attemptfor the cells that are there for BSIC reconfirmation.
  If any cell has expired its timer for BSIC reconfirmation then its timing
  and BSIC is assumed invalid. It is put back for BSIC identification.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

extern void tdssrchgsm_bsic_reconf_abort_timer_update(void);

/*===========================================================================
FUNCTION tdssrchgsm_force_reconf_abort

DESCRIPTION
  This function forces the cell out from reconf list. The typical usage of
  this function is when meas eval and report module finds that cell with BSIC
  found doesn't exists in the cell list then it force aborts the cell and put
  it back to BSIC Id list with that cell timing marked as forbidden.
  
  This function is intended to be called from lower priority ISR or task context.

DEPENDENCIES
  None

RETURN VALUE
  TRUE or FALSE based on trigger result.

SIDE EFFECTS
  None
===========================================================================*/

extern void tdssrchgsm_force_reconf_abort(
  /* cell pointer of the cell to do reconf abort */
  tdssrchgsm_drv_cell_struct_type *cell_ptr,
  /* last QS ofset to be marked forbidden */
  boolean mark_last_qs_forbiden);

/*===========================================================================
FUNCTION tdssrchgsm_trigger_gsm_bsic_reconf_meas

DESCRIPTION
  This function triggers the BSIC reconf measurement. It iterate through cells
  that can do BSIC reconf and find the cell with maximum BSIC verification age.
  If it finds a cell it saves the cell index in to global and return TRUE other
  wise it sends FALSE for no trigger.

DEPENDENCIES
  None

RETURN VALUE
  TRUE or FALSE based on trigger result.

SIDE EFFECTS
  None
===========================================================================*/

extern boolean tdssrchgsm_trigger_gsm_bsic_reconf_meas(
  /* GSM meas info struct pointer */
  tdssrchgsm_meas_info_struct_type *meas_info);

/*===========================================================================
FUNCTION tdssrchgsm_update_gsm_bsic_reconf_meas

DESCRIPTION
  This function updates the BSIC reconfirmation info in the maesurement info.
  It iterates thro the cell in BSIC verifivation set and based on the GAP and
  GSM cell timing info available it finds the cell that can do the BSIC
  reconformation. It doesn't check for the BSIC verification age. It updates
  the BSIC reconf related info to meas info structure.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

extern void tdssrchgsm_update_gsm_bsic_reconf_meas(
  /* GSM meas info struct pointer */
  tdssrchgsm_meas_info_struct_type *meas_info,
  /* transmission gap parameter */
  tdssrchgsmdrv_gap_param_gsm_struct_type *gap_param_for_gsm,
  boolean appoint_cell_flag,uint8 cell_index);

#if 0
/*===========================================================================
FUNCTION tdssrchgsm_meas_cmd_cleanup_done

DESCRIPTION
  This function is a callback function that is set by function
  tdssrchgsm_cleanup_gsm_pending_meas when it calls tdssrchgsmdrv_clean_pending_cmd_q
  function to cleanup the GSM pending Q.
  It waits for N gsm frame as defined by TDSSRCHGSM_NUM_FR_WAIT_FOR_CLEANUP
  macro after all the commands have been issued to mDSP

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

typedef void TDSSRCHGSM_CMD_CLEANUP_DONE_CB_FUNC_TYPE(void);

//extern void tdssrchgsm_meas_cmd_cleanup_done(uint8 num_cmd_pending);


/*===========================================================================
FUNCTION tdssrchgsm_meas_cmd_pre_cleanup

DESCRIPTION
  This function cleans up is called in GSTMR and unwinds RF stack.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern boolean tdssrchgsm_meas_cmd_pre_cleanup(void);
#endif
/*===========================================================================
FUNCTION tdssrchgsm_cleanup_gsm_pending_meas

DESCRIPTION
  This function cleanup and GSM command in pending Q and unwinds RF stack
  based on commands that have been or will sent will cleanup is done.

DEPENDENCIES
  None

RETURN VALUE
  boolean - TRUE if cleanup completes during function invocation.
          \ FALSE if cleanup is to finish later in time.

SIDE EFFECTS
  None
===========================================================================*/

extern boolean tdssrchgsm_cleanup_gsm_pending_meas(void);

/*===========================================================================
FUNCTION tdssrchgsm_add_cell_to_ver_list

DESCRIPTION
  This function adds the cell to the BSIC verification list.The BSIC
  verifcation list is organized as following
  
  BSIC verification list start
   - First index cell for BSIC Id measurement state
  |
  |    Cells with initial BSIC Id
  |
  |
  |- First index cell for BSIC Id aborted state
  |
  |    Cells with BSIC Id aborted due to N_id_abort
  |
  |-First index cell for BSIC reconfirmation state
  |
  |    Cells for BSIC reconfirmation 
  |
  |--------------------------------------------

  This function takes the cell pointer and put the cell in the BSIC
  verification list in the area as specified by measurement state.
  If the RSSI sort is asked for BSIC Id and BSIC Id aborted it put
  the cell based on RSSI in front of the cell whose RSSI is less than
  its own other wise it is added to the end of that area.
  It updates the various counters and first indicies for various parts
  of the cell BSIC verification.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

extern void tdssrchgsm_add_cell_to_ver_list(
  /* GSM cell pointer to add */
  tdssrchgsm_drv_cell_struct_type *cell_ptr,
  /* Cell BSIC verification state based on which cell is added */
  tdssrchgsm_srch_meas_state meas_state,
  /* indicate if the cell is added based on the RSSI it is added
     at the end of list */
  boolean sort_by_rssi);

/*===========================================================================
FUNCTION tdssrchgsm_remove_cell_from_ver_list

DESCRIPTION
  This function removes the cell to the BSIC verification list. It check the
  meas state of the cell and updates the first index and movers the cells up.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

extern void tdssrchgsm_remove_cell_from_ver_list(
  /* cell index that need to be removed */
  uint8 cell_idx);

/*===========================================================================
FUNCTION tdssrchgsm_move_cell_in_ver_list

DESCRIPTION
  This function moves the cell within the cell verification list.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

extern void tdssrchgsm_move_cell_in_ver_list(
  /* GSM cell pointer to add */
  tdssrchgsm_drv_cell_struct_type *cell_ptr,
  /* Cell BSIC verification state based on which cell is moved */
  tdssrchgsm_srch_meas_state new_meas_state,
  /* indicate if the cell is added based on the RSSI it is added
     at the end of list */
  boolean sort_by_rssi);

#ifdef FEATURE_TDSCDMA_L1_LOGGING_ENABLED

/*===========================================================================
FUNCTION tdssrchgsm_log_info_update_submit

DESCRIPTION
  This function submit the GSM meas log. It manages the log buffers also. The
  CM log packets are double buffered. the reson for the double buffereing is that
  once a log is completely filled in trigger functions, the log info is still
  pending waiting for th eresult to be available.
  In the meas time the second log buffer is used to fill the info.
  
  This function is called periodically every WCDMA frame.

DEPENDENCIES
  None

RETURN VALUE
  TRUE or FALSE based SCH decodable.

SIDE EFFECTS
  None
===========================================================================*/

extern void tdssrchgsm_log_info_update_submit(uint8 pending_log_pkt_idx);

#endif /* FEATURE_TDSCDMA_L1_LOGGING_ENABLED */

/*===========================================================================
FUNCTION tdssrchgsm_get_empty_inter_rat_cell

DESCRIPTION
  This function gets pointer from inter-RAT empty set. It gets the pointer
  from end of the list and adjust the empty set count.

DEPENDENCIES
  None

RETURN VALUE
  Cell pointer or NULL if not able to get any.

SIDE EFFECTS
  None
===========================================================================*/

extern tdssrchgsm_inter_rat_cell_struct_type* tdssrchgsm_get_empty_inter_rat_cell(void);

/*===========================================================================
FUNCTION tdssrchgsm_get_empty_inter_rat_cell

DESCRIPTION
  This function puts the cell pointer to appropriate cell list based on the
  cell set type. At this moment only GSM cell set type is supported.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

extern boolean tdssrchgsm_put_inter_rat_cell_to_set(
  tdssrchgsm_inter_rat_cell_struct_type *cell_ptr,
  tdssrchgsm_srch_inter_rat_cell_type_enum_type set_type);

/*===========================================================================
FUNCTION tdssrchgsm_delete_inter_rat_cell_to_set

DESCRIPTION
  This function delete cell pointer from appropriate cell list based on the
  cell set type. The deleted cell is not put to the empty cell set. The caller
  of this function must the cell pointer to the empty set.
  At this moment only GSM cell set type is supported.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

extern void tdssrchgsm_delete_inter_rat_cell_to_set(
  tdssrchgsm_inter_rat_cell_struct_type *cell_ptr,
  tdssrchgsm_srch_inter_rat_cell_type_enum_type set_type);

/*===========================================================================
FUNCTION TDSSRCHGSM_ADD_GSM_CELL_FOR_MEAS

DESCRIPTION
  This function adds a single GSM cell from inter-RAT cell set list.

DEPENDENCIES
  None

RETURN VALUE
  Inter-RAT Cell pointer added. Null if not able to get an empty one.

SIDE EFFECTS
  None
===========================================================================*/

extern tdssrchgsm_inter_rat_cell_struct_type* tdssrchgsm_add_gsm_cell_for_meas(
  /* ARFCN for GSM cell */
  W_ARFCN_T arfcn,
  /* BSIC verification required for this cell */
  boolean bsic_verify_reqd
);

/*===========================================================================
FUNCTION tdssrchgsm_add_arfcn_in_unique_list

DESCRIPTION
  This function adds ARFCN to unique ARFCN list. It it already exists then
  it just return with message print.

DEPENDENCIES
  None

RETURN VALUE
  boolean: if the ARFCN is unique, return value is TRUE. Otherwise, its value is FALSE.

SIDE EFFECTS
  None
===========================================================================*/

extern boolean tdssrchgsm_add_arfcn_in_unique_list(
  /* ARFCN to add */
  W_ARFCN_T *arfcn);

/*===========================================================================
FUNCTION tdssrchgsm_query_cell_exist_with_arfcn

DESCRIPTION
  This function scans thro' GSM cell list and check if it has ARFCN same as
  passed in function argument

DEPENDENCIES
  None

RETURN VALUE
  Number of cells with that ARFCN

SIDE EFFECTS
  None
===========================================================================*/

extern uint8 tdssrchgsm_query_cell_exist_with_arfcn(
  /* ARFCN to add */
  W_ARFCN_T *arfcn,
  /* indicated all cell to scan or the one that are noe deleted ones */
  boolean scan_all_cells,
  boolean update_unique_arfcn_flag);

/*===========================================================================
FUNCTION tdssrchgsm_get_arfcn_info_ptr

DESCRIPTION
  This function scans thro' GSN ARFCN list and returns the pointer to ARFCN
  info with same ARFCN as passed in function argument otherwise return NULL

DEPENDENCIES
  None

RETURN VALUE
  pointer to tdssrchgsm_arfcn_info_struct_type structure type or NULL

SIDE EFFECTS
  None
===========================================================================*/

extern tdssrchgsm_arfcn_info_struct_type* tdssrchgsm_get_arfcn_info_ptr(
  /* ARFCN to add */
  W_ARFCN_T *arfcn);

/*===========================================================================
FUNCTION tdssrchgsm_del_arfcn_in_unique_list

DESCRIPTION
  This function removes ARFCN to unique ARFCN list. This function expect that
  ARFCN do exists in list otherwise it prints error message

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

extern void tdssrchgsm_del_arfcn_in_unique_list(
  /* ARFCN to delete */
  W_ARFCN_T *arfcn);

/*===========================================================================
FUNCTION tdssrchgsm_trim_arfcn_sample_tail_to_max_age

DESCRIPTION
  This function trims the sample accumulation tail with in the max age w.r.t.
  current time to avoid stale sample accumulation

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

extern void tdssrchgsm_trim_arfcn_sample_tail_to_max_age(
  /* Maximum age of RSSI sample allowed */
  uint16 max_age);

/*===========================================================================
FUNCTION tdssrchgsm_trim_arfcn_sample_tail_to_max_age

DESCRIPTION
  This function gets the average of at least 3 samples and at least 480 ms
  worth of samples and average them out

DEPENDENCIES
  None

RETURN VALUE
  TRUE or FALSE indicating is average RSSI value is valid or not

SIDE EFFECTS
  If successful then RSSI value pointer passed asargument to function is
  updated
===========================================================================*/

extern boolean tdssrchgsm_get_arfcn_rssi(
  /* ARFCN to get RSSI value for */
  W_ARFCN_T *arfcn,
  /* Average RSSI value */
  int16 *ave_rssi_dbmx16,
  /* This ignores minimum requirement of 480 ms period, 3 samples is met.
     it is assumed that this requirement is being maintained by measurement control */
  boolean ignore_min_requirement);

/*===========================================================================
FUNCTION tdssrchgsm_proceed_arfcn_rssi_meas_processing

DESCRIPTION
  This function scans ARFCN list for to see if any ARFCN meets minimum
  requirement of 480 ms and 3 samples.
  OR if there is any valid cell for RSSI processing

  If there is atleast one cell that can do RSSI processing or NO cell at all
  that is valid it returns TRUE.

  This function is intended to be called at RSSI processing function at
  callback RSSI done when last ARFCN index has been hit

  Check done in this function are simple and don't check for any RSSI sample
  age to eliminate any aged sample.

DEPENDENCIES
  None

RETURN VALUE
  TRUE or FALSE

SIDE EFFECTS
  None
===========================================================================*/

extern boolean tdssrchgsm_proceed_arfcn_rssi_meas_processing(
  /* number of ARFCN valid */
  uint8 *num_valid_arfcn,
  /* number of minimum samples in valid ARFCN */
  uint8 *min_sample,
  /* min oldest sample age w.r.t. to current time */
  uint16 *min_oldest_rssi_sample_age);

#if 0
/*===========================================================================
FUNCTION tdssrchgsm_cleanup_gsm_meas

DESCRIPTION
  This function cleans up all pending gsm measurements. This is typically called
  after cm has been suspended. If the gsm frame tick handler is not deregistered 
  in this routine it will get deregistered by posting a local command in the 
  post callback

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern boolean tdssrchgsm_cleanup_gsm_meas(
  /* call back function when GSM cmd has finished in mDSP in worst case */
  TDSSRCHGSM_CMD_CLEANUP_DONE_CB_FUNC_TYPE* cb_func);
#endif
/*===========================================================================
FUNCTION tdssrchgsm_meas_init_meas_drv

DESCRIPTION
  This function initialises cm measurement module data structure and drivers
  needed for compressed mode operation

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void tdssrchgsm_meas_init_meas_drv(
  /*flag to indicate resuming wcdma when coming from other RAT*/
  boolean resuming_wcdma, 
  /*flag indicating dch_to_dch hho*/
  boolean TDSL1MSETUP_IS_IN_DCH_TO_DCH_HHO);

#if 0
/*===========================================================================
FUNCTION tdssrchgsm_cleanup_meas_drv

DESCRIPTION
  This function cleans up compressed mode measurements needed for dch -> other 
  state transition or because of HHO

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void tdssrchgsm_cleanup_meas_drv(
  /* Post search done call back function */
  TDSSRCHGSM_CMD_CLEANUP_DONE_CB_FUNC_TYPE* gsm_cleanup_cb_func,
  /*module cleanup required or not*/
  boolean module_cleanup_required);
#endif

#endif /* (FEATURE_CM_SEARCH) */


extern void tdssrchgsm_begin_do_gsm_handler(void);


/*from tdscmmeas.h*/
/******************************************************************************
*******************************************************************************
              INIT Functions
*******************************************************************************
******************************************************************************/

/*=========================================================================

FUNCTION     : tdssrchgsm_init

DESCRIPTION  : This function will Initialize the L1GSMMEAS subsystem

DEPENDENCIES : None

RETURN VALUE : None

SIDE EFFECTS : None

=========================================================================*/
void tdssrchgsm_init ( void );

//#endif /* FEATURE_CM_SUPPORTED */


/******************************************************************************
*******************************************************************************
              L1M Entry Points To GSM Measurement Control
*******************************************************************************
******************************************************************************/

/*===========================================================================
FUNCTION     tdssrchgsm_msmt_ctrl_msg_processing

DESCRIPTION
  Handle Measurement Control Message Processing

DEPENDENCIES
  None

RETURN VALUE
  FALSE if measurement control message processing identified errors in the
  message

SIDE EFFECTS
  None
===========================================================================*/
boolean tdssrchgsm_msmt_ctrl_msg_processing( tdsl1_meas_cmd_type *meas_cmd );

/*===========================================================================
FUNCTION     tdssrchgsm_process_gsm_meas_done_sig

DESCRIPTION
  This function performs tds_l1_task  TDSL1M_GSM_MEAS_DONE_SIG signal processing
  when compressed mode inter-freq/inter-RAT measurements have been completed

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void tdssrchgsm_process_gsm_meas_done_sig( void );

/*===========================================================================
FUNCTION     tdssrchgsm_transition_into_cell_dch

DESCRIPTION
  L1 CM Search Subsystem routine for handling transition into CELL_DCH state

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void tdssrchgsm_transition_into_cell_dch( void );


/*===========================================================================
FUNCTION     tdssrchgsm_transition_from_cell_dch

DESCRIPTION
  L1 CM Search Subsystem routine for handling transition out of CELL_DCH state

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void tdssrchgsm_transition_from_cell_dch( void );

/*=========================================================================
FUNCTION     : tdssrchgsm_cio_value_scaled

DESCRIPTION  : This function returns the scaled CIO (cell individual offset)
               value for use in srchset.c for sorting GSM cells based on
               adjusted filtered rssi, ie. filtered_rssi + cio.

DEPENDENCIES : None

RETURN VALUE : None

SIDE EFFECTS : None

=========================================================================*/
int32  tdssrchgsm_cio_value_scaled( int16 cio );


/*===========================================================================
FUNCTION     tdssrchgsm_reset_cell_reporting_state

DESCRIPTION
  This function resets the reporting state of all the cells to TDS_TTT_NOT_SET
  when we resume on WCDMA from GSM.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void tdssrchgsm_reset_cell_reporting_state( void );


/*===========================================================================
FUNCTION     tdssrchgsm_event_report_evaluation

DESCRIPTION
  This function evaluates the inter-rat events to check if we need to 
  send a report.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

void tdssrchgsm_event_report_evaluation( void );

/*===========================================================================
FUNCTION     tdssrchgsm_remove_interrat_measurement_event

DESCRIPTION
  This function will (re-)initialize the event specified, from the
  inter-rat measurement list, thereby performing a remove operation.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void tdssrchgsm_remove_interrat_measurement_event( uint32 event_idx );

#if 0
/*=========================================================================
FUNCTION     : tdssrchgsm_update_tgps_info

DESCRIPTION  : This function will change the status ACTIVE/INACTIVE of
               a currently registered compressed mode sequence.  It will
               also set measurement id when a valid TGMP is provided.

DEPENDENCIES : None

RETURN VALUE : TRUE/FALSE, update succeeded or failed

SIDE EFFECTS : None

=========================================================================*/
boolean tdssrchgsm_update_tgps_info(uint16 tgpsi,uint16 measid);

#endif

/*===========================================================================
FUNCTION     tdssrchgsm_gsm_time_diff

DESCRIPTION
  This function computes the gsm_time_diff value for inclusion in
  the periodic measurement report message.

  Definition of this can be found in 25.133 section 9.1.10.2

  gsm_time_diff takes a frame offset within a 51 frame multi-frame (0-50)
  and a quarter symbol offset within one GSM frame.
  It determines the time offset in 57ms (ie.  3060/(4096*13)ms) granularity.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
uint16 tdssrchgsm_gsm_time_diff( uint32 mul51_frame_offset, uint32 qs_offset );


/*===========================================================================
FUNCTION     L1CMSRCH_SEND_EVENT_MSMT_RPT_MSG

DESCRIPTION
  This function builds and sends a measurement report message when
  event based triggers that a report should be sent.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void tdssrchgsm_send_inter_sys_event_msmt_rpt_msg(uint32 event_idx);

/*===========================================================================
FUNCTION     L1CMSRCH_SEND_PERIODIC_MSMT_RPT_MSG

DESCRIPTION
  This function builds and sends a measurement report message when
  periodic reporting triggers that a report should be sent.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void tdssrchgsm_send_inter_sys_periodic_msmt_rpt_msg(uint32 event_idx, boolean  last_periodic_rpt);


/*===========================================================================
FUNCTION     tdssrchgsm_get_empty_event

DESCRIPTION
  Grab an empty event from the event array[]

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
uint32 tdssrchgsm_get_empty_event( void );

/*===========================================================================
FUNCTION     tdssrchgsm_find_event

DESCRIPTION
  Locate the event specified in the event array.  The inter_s_evt parameter
  is only valid/used if rpt_mode == TDSL1_EVENT_TRIGGER_RPT.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
uint32 tdssrchgsm_find_event( uint32 measid );

/*===========================================================================
FUNCTION     tdssrchgsm_process_periodic

DESCRIPTION
  This function performs periodic event processing at the time the
  periodic timer expires.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void tdssrchgsm_process_periodic( uint32  event_idx );

/*===========================================================================
FUNCTION     TDSL1CMSRCH_PROCESS_GSM_PERIODIC_CB

DESCRIPTION
  This callback function get called when G2T periodic timer expires.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void tdssrchgsm_process_gsm_periodic_cb( uint32  event_idx );

/*===========================================================================
FUNCTION     tdssrchgsm_process_event3b

DESCRIPTION
  This function will perform event 3b processing when measurements
  have been completed.

  Event 3b: The estimated quality of the other system is below a certain threshold

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void tdssrchgsm_process_event3b( uint32 event_idx );

/*===========================================================================
FUNCTION     tdssrchgsm_process_event3c

DESCRIPTION
  This function will perform event 3c processing when measurements
  have been completed.

  Event 3c: The estimated quality of the other system is above a certain threshold

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean tdssrchgsm_process_event3c( uint32 event_idx );

/*===========================================================================
FUNCTION     tdssrchgsm_process_event3a

DESCRIPTION
  This function will perform event 3a processing when measurements
  have been completed.

  Event 3a: The estimated quality of the currently used UTRAN frequency is
            below a certain threshold *and* the estimated quality of the
            other system is above a certain threshold

DEPENDENCIES
  l1 intra-freq interface for determining estimated quality of currently used
  utran

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void tdssrchgsm_process_event3a( uint32 event_idx );

/*===========================================================================
FUNCTION     tdssrchgsm_process_event3d

DESCRIPTION
  This function will perform event 3d processing when measurements
  have been completed.

  Event 3d: Change of best cell in other system

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void tdssrchgsm_process_event3d( uint32 event_idx );

/*=========================================================================

FUNCTION     : TDSSRCHGSM_COPY_GSM_CELL_INFO

DESCRIPTION  : This function copy gsm cell information while source cell's rssi is valid.
             
DEPENDENCIES : None

INPUT PARAMETERS :
  tdssrchset_gsm_cell_struct_type * source_cell_ptr
  tdssrchset_gsm_cell_struct_type * target_cell_ptr

RETURN VALUE : None
      
SIDE EFFECTS : None

=========================================================================*/
extern void tdssrchgsm_copy_gsm_cell_info
(
  tdssrchset_gsm_cell_struct_type * source_cell_ptr,
  tdssrchset_gsm_cell_struct_type * target_cell_ptr
);

/*===========================================================================
FUNCTION     tdssrchgsm_add_filter_coef

DESCRIPTION
  This function will add a filter coefficient to the filter coefficient
  storage array if that filter_coef is not corrently in the array.
  Failure is indicated by a return value == TDSL1_MAX_FILTER_COEF_STORAGE.

DEPENDENCIES
  None

RETURN VALUE
  Success, index where filter_coef is stored
  Failure, TDSL1_MAX_FILTER_COEF_STORAGE

SIDE EFFECTS
  None
===========================================================================*/
uint32 tdssrchgsm_add_filter_coef( tdsl1_gsm_filter_coef_enum_type filter_coef );

/*===========================================================================
FUNCTION     tdssrchgsm_remove_filter_coef

DESCRIPTION
  This function removes a filter coefficient from the filter coefficient
  storage array.  One should make sure that no event is referencing this
  filter coefficient index before deleting.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void tdssrchgsm_remove_filter_coef( uint32 fc_index);

/*===========================================================================
FUNCTION     tdssrchgsm_msmt_ctrl_msg_inter_rat_processing

DESCRIPTION
  Handle Measurement Control Message Inter-RAT Related Processing
  as specified in section 8.4.1.3 of 25.331

DEPENDENCIES
  None

RETURN VALUE
  FALSE if measurement control message processing identified errors in the message

SIDE EFFECTS
  None
===========================================================================*/
boolean tdssrchgsm_msmt_ctrl_msg_inter_rat_processing( tdsl1_meas_cmd_type *meas_cmd );

/*===========================================================================
FUNCTION     tdssrchgsm_msmt_ctrl_msg_inter_freq_processing

DESCRIPTION
  Handle Measurement Control Message Inter-Frequency Related Processing

DEPENDENCIES
  None

RETURN VALUE
  FALSE if measurement control message processing identified errors in the message

SIDE EFFECTS
  None
===========================================================================*/
/*ARGSUSED*/
boolean tdssrchgsm_msmt_ctrl_msg_inter_freq_processing( tdsl1_meas_cmd_type *meas_cmd );

#if 0

/*===========================================================================
FUNCTION     tdssrchgsm_translate_dpch_status_info

DESCRIPTION
  Update the translation of DPCH Status Info, filling in the DPCH Info struct
  that can be passed to the L1CMMGR.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  tdssrchgsm_cm_config & tdssrchgsm_tgps_info global variables are set
===========================================================================*/
boolean tdssrchgsm_translate_dpch_status_info(
                                    tdsl1_cm_status_info_struct_type * p_cm_status_info);

/*===========================================================================
FUNCTION     tdssrchgsm_validate_dpch_status_info

DESCRIPTION
  Validate DPCH Status Info provided by measurement control message

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean tdssrchgsm_validate_dpch_status_info(tdsl1_cm_status_info_struct_type * p_cm_status_info);


/*===========================================================================
FUNCTION     tdssrchgsm_config_dpch_status_info

DESCRIPTION
  Configure TGPS Compressed Mode Sequences via Measurement Control Message
  DPCH Status Info IE.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
/*ARGSUSED*/
boolean tdssrchgsm_config_dpch_status_info( uint32 trans_id,
                                          uint32 meas_id,
                                          tdsl1_cm_status_info_struct_type * p_cm_status_info
                                        );

#endif
/*===========================================================================
FUNCTION     tdssrchgsm_swap_drv_cell

DESCRIPTION
  This function swaps host_info pointers and driver pointers for 2 cells.
  This is used when a BSIC is initially verified, but this BSIC matches
  that of another cell using the same ARFCN.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void tdssrchgsm_swap_drv_cell( tdssrchset_gsm_cell_struct_type       * p_cell,
                             tdssrchset_gsm_cell_struct_type       * other_p_cell );

/*===========================================================================
FUNCTION     tdssrchgsm_perform_filter_processing

DESCRIPTION  Does the filter processing after RSSI results are  
             received from RSSI and BSIC CM gaps.

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void tdssrchgsm_perform_filter_processing(tdssrchset_gsm_cell_struct_type *p_cell, 
                                                    boolean rssi_becoming_valid_flag);


/*===========================================================================
FUNCTION     L1CMMEAS utility functions

DESCRIPTION  

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/

W_ARFCN_T  TDSSRCHGSM_GET_ARFCN( void * p_drv_cell );
uint8   TDSSRCHGSM_GET_BSIC_NCC( void *p_drv_cell );
uint8   TDSSRCHGSM_GET_BSIC_BCC( void * p_drv_cell );
uint8   TDSSRCHGSM_GET_FRAME_OFFSET( void * p_drv_cell );
uint16  TDSSRCHGSM_GET_QS_OFFSET( void * p_drv_cell );
uint32  TDSSRCHGSM_GET_HYPER_FRAME_OFFSET( void * p_drv_cell );
int16  TDSSRCHGSM_GET_FREQUENCY_OFFSET( void * p_drv_cell );

void TDSSRCHGSM_SET_HOST_INFO_PTR( void * p_drv_cell, void * p_cell);
void * TDSSRCHGSM_GET_HOST_INFO_PTR( void * p_drv_cell );

boolean  TDSSRCHGSM_IS_RSSI_VALID( void * p_drv_cell );
int16  TDSSRCHGSM_GET_RSSI_X16( void * p_drv_cell );
boolean  TDSSRCHGSM_IS_BSIC_VALID( void * p_drv_cell );

#ifdef  FEATURE_TDSCDMA_CELL_RESELECT_INTER_S
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
extern void tdscrgsm_init( void );

/*===========================================================================

FUNCTION      TDSCRGSM_CLEANUP

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
extern void tdscrgsm_cleanup( void );

extern void tdssrchgsm_update_rf_scripts( void );

#endif /* FEATURE_TDSCDMA_CELL_RESELECT_INTER_S */


/*===========================================================================
FUNCTION     tdssrchgsm_cell_need_bsic_id_exist

DESCRIPTION
  

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

boolean tdssrchgsm_cell_need_bsic_id_exist( void );

/*===========================================================================
FUNCTION     tdssrchgsm_cell_need_bsic_rc_exist

DESCRIPTION
  

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

boolean tdssrchgsm_cell_need_bsic_rc_exist( void );

/*======================================================================
FUNCTION TDSSRCHGSM_ENABLE_PENDING_CLEANUP

DESCRIPTION
    set pending cleanup to TRUE
     
RETURN VALUE
    void

=======================================================================*/
extern void tdssrchgsm_enable_pending_cleanup(void);

extern void tdssrchgsm_disable_pending_cleanup(void);

extern boolean tdssrchgsm_pending_cleanup_status(void);

extern void tdssrchgsm_cleanup_gsm_meas(void);

/*===========================================================================
FUNCTION tdssrchgsm_cleanup_bsic_list

DESCRIPTION
  cleanup the bsic verification list count 

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void tdssrchgsm_cleanup_bsic_list(void);

/*===========================================================================
FUNCTION        CM_DCH_GSM_CLEANUP_DONE
                CM_DCH_INTERF_CLEANUP_DONE
                CM_STATE_MACHINE_SUSPEND_DONE

FILE NAME

DESCRIPTION     This is the DCH state CM cleanup done routine. It send a
                local command for module cleanup done for CM module and
                deregisters GSTMR tick handler.

DEPENDENCIES

RETURN VALUE    

SIDE EFFECTS
===========================================================================*/
//extern void tdssrchgsm_dch_gsm_cleanup_done(void);
//extern void tdssrchgsm_dch_interf_cleanup_done(void);
//extern void tdssrchgsm_state_machine_suspend_done(void);

#ifdef FEATURE_TDSCDMA_IRAT_FED_BASED
/*===========================================================================
FUNCTION tdssrchgsm_send_tfw_gap_cfg_cmd

DESCRIPTION
  Send the gap configuration cmd to TFW 

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void tdssrchgsm_send_tfw_gap_cfg_cmd(void);
#endif

#ifdef FEATURE_TDSCDMA_TRM_NEW_API

typedef struct
{
// Number of bands 
uint32 num_bands;

// Band Information
trm_frequency_info bands[MAX_BAND_INFO]; 
 
//Subframe to de-register GSM band
int16 subframe_number; 

uint32 transaction_id;
 
boolean in_use; 
} tdssrchgsm_deregister_gsm_band_type; 

#define DR_DSDS_GSM_GAP_IN_FRAME 4
#define DR_DSDS_T2G_TRANSACTION_ID 256
 
/*===========================================================================
FUNCTION tdssrchgsm_register_gsm_band_in_fach_dch

DESCRIPTION
  Register GSM band to TRM by calling set_state 

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean tdssrchgsm_register_gsm_band_in_fach_dch(void);

/*===========================================================================
FUNCTION tdssrchgsm_register_gsm_band_for_t2g_gap

DESCRIPTION
  Register GSM band to TRM by calling set_state for t2g gap

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean tdssrchgsm_register_gsm_band_for_t2g_gap(trm_freq_input_type gsm_band_register
                                                 #ifdef FEATURE_TDSCDMA_60MS_GAP_FOR_BSIC_OPT
                                                 , boolean is_13_g_frame_gap
                                                 #endif
                                                 );

/*===========================================================================
FUNCTION tdssrchgsm_deregister_gsm_band_in_fach_dch

DESCRIPTION
  De-register GSM band to TRM by calling set_state 

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean tdssrchgsm_deregister_gsm_band_in_fach_dch(boolean force_deregister);

/*===========================================================================
FUNCTION tdssrchgsm_add_gsm_band_for_trm_registration

DESCRIPTION
  Add the GSM bands which will be register with TRM in connected mode 

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
uint8 tdssrchgsm_add_gsm_band_for_trm_registration(uint8 gsm_cell_number_to_register, W_ARFCN_T *gsm_cell_arfcn, trm_freq_input_type *gsm_band_register);

/*===========================================================================

FUNCTION  tdssrchgsm_gsm_arfcn_to_trm_band

DESCRIPTION
  This function converts a GSM ARFCN to the TRM band type.

DEPENDENCIES
  None

RETURN VALUE
  RF band type

SIDE EFFECTS
  None

===========================================================================*/
trm_band_t tdssrchgsm_gsm_arfcn_to_trm_band( W_ARFCN_T arfcn );
#endif /*FEATURE_TDSCDMA_TRM_NEW_API*/

#endif /* TDSSRCHGSM_H */

