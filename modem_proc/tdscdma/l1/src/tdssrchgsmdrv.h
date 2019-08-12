#ifndef TDSSRCHGSMDRV_H
#define TDSSRCHGSMDRV_H

/*============================================================================
                            S R C H C M D R V . H
DESCRIPTION

Copyright (c) 2000-2014 Qualcomm Technologies, Incorporated.  All Rights Reserved.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government. Diversion contrary to U.S. law prohibited.
============================================================================*/

/* ==========================================================================
					  INCLUDE FILES FOR MODULE
===========================================================================*/


#include "tdscdma_variation.h"
#include "tdsuecomdef.h"
//#include "mdspext.h"


#include "gl1_hw.h"

#ifdef FEATURE_TDSCDMA_CELL_RESELECT_INTER_S
#include "tdsl1rrcif.h"
#endif

//#ifdef FEATURE_WCDMA_IRAT_RF_BUF_SUPPORT
//#include "tdsrfmif.h"
//#endif

#include "tdssrchparm.h"
#include "tdssrchgsmdrv.h"
#include "tdsmathutil.h"
#include "tdssrchset.h"
#include "comdef.h"
//#include "rfm.h"

/* -----------------------------------------------------------------------
** Constant / Define Declarations
** ----------------------------------------------------------------------- */
/* Size of power burst size */
#define TDSSRCHGSMDRV_GSM_PWR_SCAN_BURST_SIZE_QS (GL1_HW_CM_PWR_MEAS_CMD_DURATION_QS)

#define TL1_HW_CM_CLEANUP_CMD_DURATION_QS 468

/* Power scan window start offset after gap start */
#define TDSSRCHGSMDRV_GSM_T_START_QS_PWR_SCAN (GL1_HW_CM_STARTUP_CMD_DURATION_QS)
/* Power scan burst win size (this include RF tune + burst size) */
#define TDSSRCHGSMDRV_GSM_PWR_SCAN_BURST_MEAS_WIN_SIZE_QS ((GL1_HW_CM_PWR_MEAS_CMD_OVERHEAD_QS) + (TDSSRCHGSMDRV_GSM_PWR_SCAN_BURST_SIZE_QS))

#ifndef FEATURE_TDSCDMA_DSDS
#define TL1_HW_CM_CLEANUP_CMD_OVERHEAD_QS (24 + 20) //GL1_HW_CM_CLEANUP_CMD_OVERHEAD_QS 
#else
#define TL1_HW_CM_CLEANUP_CMD_OVERHEAD_QS (24 + 25)  //CR553660
#endif

#define TDSSRCHGSMDRV_GFW_ADC_SSW_DELAY_ALPHA 28 

/* CM start up time. GSM startup start this many QS before GAP starts */
#define TDSSRCHGSMDRV_GSM_CM_START_UP_TIME_QS (GL1_HW_CM_STARTUP_CMD_OVERHEAD_QS)


/* This includes the time required for cleanup */
#define TDSSRCHGSMDRV_GSM_T_CLEAN_QS_PWR_SCAN ((TL1_HW_CM_CLEANUP_CMD_OVERHEAD_QS) + (TL1_HW_CM_CLEANUP_CMD_DURATION_QS))

#define TDSSRCHGSMDRV_CLEANUP_CMD_DURATION_CX8  4423 // 4423 = tdssrchgsm_qs_to_tds_cx8((TL1_HW_CM_CLEANUP_CMD_DURATION_QS))
#define TDSSRCHGSMDRV_CLEANUP_CMD_OVERHEAD_CX8 512  //0.05ms as the overhead
#define TDSSRCHGSMDRV_MIN_GAP_LENGTH_CX8 ((TIME_DURATION_FOR_GSM_RSSI) - (TDSSRCHGSMDRV_CLEANUP_CMD_DURATION_CX8) - (TDSSRCHGSMDRV_CLEANUP_CMD_OVERHEAD_CX8))

/*G RX stop from RX Acq is delayed due to TX start on sub2 (traffic sub). 
  T2G IRAT gap by 100usecs to account for the bus contention issue.*/
#define TDSSRCHGSMDRV_DR_BUS_CONTENTION_OVERHEAD_QS 109   // 109 qs = 100us
//#define TDSSRCHGSMDRV_DR_BUS_CONTENTION_OVERHEAD_CX8 1740   // 1740 cx8 = 170us

/* The following variable states the number of power scans that can be done
   in the given gap length.
   It uses a WCDMA slot length 666 microsec as 722 GSM QS
   (5000 / 4615) * (10000 / 15).
   Note that 1024 chips at the start and 1024 chips at the end are not
   usable for GSM measurement
   2048 WCDMA chips = 577 GSM QS */
   
#define TDSSRCHGSMDRV_GSM_PWR_SCAN_CNT_GAP_LEN_I(len) (((((len * 722) - 577) - \
                                               TDSSRCHGSMDRV_GSM_T_START_QS_PWR_SCAN) - \
                                               TDSSRCHGSMDRV_GSM_T_CLEAN_QS_PWR_SCAN) / \
                                              TDSSRCHGSMDRV_GSM_PWR_SCAN_BURST_MEAS_WIN_SIZE_QS)
                                              
#define TDSSRCHGSMDRV_GSM_PWR_SCAN_CNT_GAP_LEN(len) TDSSRCHGSMDRV_GSM_PWR_SCAN_CNT_GAP_LEN_I(len)
                                             
/* T start and cleanup for FCCH detect */
#define TDSSRCHGSMDRV_GSM_T_START_QS_FCCH_DETECT ((GL1_HW_CM_STARTUP_CMD_DURATION_QS) + (GL1_HW_CM_ACQ_CMD_OVERHEAD_QS))
#define TDSSRCHGSMDRV_GSM_T_CLEAN_QS_FCCH_DETECT ((TL1_HW_CM_CLEANUP_CMD_OVERHEAD_QS) + (TL1_HW_CM_CLEANUP_CMD_DURATION_QS))

/* T start and cleanup for SCH detect */
#define TDSSRCHGSMDRV_GSM_T_START_QS_SCH_DEC ((GL1_HW_CM_STARTUP_CMD_DURATION_QS) + (GL1_HW_CM_ASYNC_RX_CMD_OVERHEAD_QS))
#define TDSSRCHGSMDRV_GSM_T_CLEAN_QS_SCH_DEC ((TL1_HW_CM_CLEANUP_CMD_OVERHEAD_QS) + (TL1_HW_CM_CLEANUP_CMD_DURATION_QS))
/* SCH burst size */
#define TDSSRCHGSMDRV_GSM_SCH_BURST_SIZE_QS (GL1_HW_CM_ASYNC_RX_CMD_DURATION_QS)

#ifdef FEATURE_TDSCDMA_L1_GSM_ALLOW_FLEX_UNCERTAINTY_WIN

/* SCH uncertaintity window or search width for initial identification */
#define TDSSRCHGSMDRV_GSM_BSIC_ID_SCH_OPTIMUM_UNCERTAINTY_WIN_QS (GL1_HW_X2G_TDS_SCH_SEARCH_WIDTH_QS / 2)
#define TDSSRCHGSMDRV_GSM_BSIC_ID_SCH_UNCERTAINTY_WIN_QS (40)
/* SCH decode window for initial identification */
#define TDSSRCHGSMDRV_GSM_BSIC_ID_SCH_BURST_DEC_SZ \
          (TDSSRCHGSMDRV_GSM_SCH_BURST_SIZE_QS + \
           (2 * TDSSRCHGSMDRV_GSM_BSIC_ID_SCH_UNCERTAINTY_WIN_QS))
           
/* SCH uncertaintity window or search width for BSIC reconfirmation */
#define TDSSRCHGSMDRV_GSM_BSIC_RECONF_SCH_OPTIMUM_UNCERTAINTY_WIN_QS (GL1_HW_X2G_TDS_SCH_SEARCH_WIDTH_QS / 2)
#define TDSSRCHGSMDRV_GSM_BSIC_RECONF_SCH_UNCERTAINTY_WIN_QS (20)
/* SCH decode window for BSIC reconfirmation */
#define TDSSRCHGSMDRV_GSM_BSIC_RECONF_SCH_BURST_DEC_SZ \
          (TDSSRCHGSMDRV_GSM_SCH_BURST_SIZE_QS + \
           (2 * TDSSRCHGSMDRV_GSM_BSIC_RECONF_SCH_UNCERTAINTY_WIN_QS))

#else

/* SCH uncertaintity window or search width for initial identification */
#define TDSSRCHGSMDRV_GSM_BSIC_ID_SCH_UNCERTAINTY_WIN_QS (GL1_HW_X2G_TDS_SCH_SEARCH_WIDTH_QS / 2)
/* SCH decode window for initial identification */
#define TDSSRCHGSMDRV_GSM_BSIC_ID_SCH_BURST_DEC_SZ \
          (TDSSRCHGSMDRV_GSM_SCH_BURST_SIZE_QS + \
           (2 * TDSSRCHGSMDRV_GSM_BSIC_ID_SCH_UNCERTAINTY_WIN_QS))
           
/* SCH uncertaintity window or search width for BSIC reconfirmation */
#define TDSSRCHGSMDRV_GSM_BSIC_RECONF_SCH_UNCERTAINTY_WIN_QS (GL1_HW_X2G_TDS_SCH_SEARCH_WIDTH_QS / 2)
/* SCH decode window for BSIC reconfirmation */
#define TDSSRCHGSMDRV_GSM_BSIC_RECONF_SCH_BURST_DEC_SZ \
          (TDSSRCHGSMDRV_GSM_SCH_BURST_SIZE_QS + \
           (2 * TDSSRCHGSMDRV_GSM_BSIC_RECONF_SCH_UNCERTAINTY_WIN_QS))

#endif /* FEATURE_TDSCDMA_L1_GSM_ALLOW_FLEX_UNCERTAINTY_WIN */

//#define FEATURE_MEASURE_SCHEDULE_TEST

#ifdef FEATURE_MEASURE_SCHEDULE_TEST_2
#define TDSSRCHGSMDRV_GSM_T_START_QS_PWR_SCAN 0
#define TDSSRCHGSMDRV_GSM_T_CLEAN_QS_PWR_SCAN 320
#define TDSSRCHGSMDRV_GSM_PWR_SCAN_BURST_SIZE_QS 272
#define GL1_HW_CM_PWR_MEAS_CMD_OVERHEAD_QS 260
#endif

/* GSM IRQ config field related macro */
#define TDSSRCHGSMDRV_GSTMR_MICRO_IRQ_MSB_IGNORE_EVEN_ODD_BIT 0x0002

/* GSM 51 multiframe number */
#define TDSSRCHGSMDRV_GSM_MULTI_FRAME_COUNT 51
/* GSM Quater symbols per GSM frame */
#define TDSSRCHGSMDRV_GSM_NUM_QS_PER_FRAME 5000

#define TDSSRCHGSMDRV_GSM_MAX_MDSP_CMD_BUF_SIZE 20

/* The number of GSM frame by which cmd frame number can lead the
   GSM frame number. Other wise it is treated as missed cmd */
#define TDSSRCHGSMDRV_GSM_MAX_FR_NUM_DIFF       18

/* GSM mDSP command related macros */
#define TDSSRCHGSMDRV_MAX_PWR_MEAS_BURST_PER_GSM_FR 8

/* invalid GSM frame number for WCDMA gap */
#define TDSSRCHGSMDRV_GSM_FRAME_NUM_INVALID 0xFFFFFFFF

/* Number of GSM RSSI Samples per GSM carrier */
#define TDSSRCHGSMDRV_FACH_NUM_GSM_RSSI_PER_CARRIER  3

/* Inter RAT measurement related macro */
/* ----------------------------------- */
#define TDSSRCHGSMDRV_GSM_MDSP_CMD_FTSM_QS  FRAME_TICK_SETUP_MARGIN_QS
/* number of SCH decode cmd in a GAP */
#define TDSSRCHGSMDRV_GSM_SCH_DEC_CMD_MAX_PER_GAP 2
/* -----------------------------------------------------------------------
** Type Declarations
** ----------------------------------------------------------------------- */

/* GSM command result poll call back function type */
//typedef void TDSSRCHGSMDRV_CMD_RESULT_POLL_CB_FUNC_TYPE(uint8);
/* call back fucntion for command cleanup */
/* GSM command cleanup pre call back function type */
//typedef boolean TDSSRCHGSMDRV_CMD_CLEANUP_PRE_CB_FUNC_TYPE(void);
/* GSM command result poll call back function type */
//typedef void TDSSRCHGSMDRV_CMD_CLEANUP_POLL_CB_FUNC_TYPE(uint8);

/* GSM mDSP command enum type definiiton */
typedef enum
{
  TDSSRCHGSMDRV_CM_STARTUP_MDSP_CMD,
  TDSSRCHGSMDRV_CM_CLEANUP_MDSP_CMD,
  TDSSRCHGSMDRV_PWR_MEAS_MDSP_CMD,
  TDSSRCHGSMDRV_ACQ_START_MDSP_CMD,
  TDSSRCHGSMDRV_ACQ_STOP_MDSP_CMD,
  TDSSRCHGSMDRV_ASYNC_SCH_RECE_MDSP_CMD,
  TDSSRCHGSMDRV_NUM_MDSP_CMD
} tdssrchgsmdrv_cmd_type_enum_type;

/* GAP parameters in TD time unit structure definition */
typedef struct
{
  /* GSM gap frame number */
  uint16 subsfn;
  /* GSM gap QS offset */
  uint16 chipx8;
}tdssrchgsmdrv_tds_time_struct_type;

/* GAP parameters in TD time unit structure definition */
typedef struct
{
  /* GSM gap start time */
  tdssrchgsmdrv_tds_time_struct_type start_time;
  /* GSM gap end time */
  tdssrchgsmdrv_tds_time_struct_type end_time;
  /* GSM gap duration */
  uint32 duration;
  /* snapshot of TL1 send GAP info to TFW*/
  tdssrchgsmdrv_tds_time_struct_type send_time;
}tdssrchgsmdrv_gap_param_tds_struct_type;

/* GAP parameters in GSM time unit structure definition */
typedef struct
{
  /* GSM gap start frame number */
  uint32 frame_num;
  /* GSM gap start QS offset */
  uint16 offset_qs;
  /* GSM gap length in GSP QS */
  uint32 gap_len_qs;
} tdssrchgsmdrv_gap_param_gsm_struct_type;

/* Common minimum GSM command type structure definition */
typedef struct
{
  /*GL1_HW_CM_STARTUP_CMD_DURATION_QS*/
  uint16 gl1_startup_cmd_duration_qs;
  /*GL1_HW_CM_CLEANUP_CMD_DURATION_QS*/
  uint16 gl1_cleanup_cmd_duration_qs;
  /*GL1_HW_CM_PWR_MEAS_CMD_OVERHEAD_QS*/
  uint16 gl1_power_meas_cmd_overhead_qs;
} tdssrchgsmdrv_gsm_cmd_param_struct_type;

/* GSM startup command structure definition */
typedef struct
{
  /* Command send frame number */
  uint32 frame_num;
  /* GSM startup QS offset */
  uint16 offset_qs_to_start;
  //#ifdef FEATURE_WCDMA_IRAT_RF_BUF_SUPPORT
  /* RF buffer info */
  void *rf_buf;
  uint8 cleanup_buf_idx;
  /* Serving W band */
  //tdsrf_card_band_enum_type serving_band;
  /* Serving W DL chan */
  uint16 serving_chan;
  //#endif
} tdssrchgsmdrv_gsm_startup_cmd_param_struct_type;

/* GSM cleanup command structure definition */
typedef struct
{
  /* Command send frame number */
  uint32 frame_num;
  /* GSM cleanup QS offset */
  uint16 offset_qs_to_end;
  /* GL1 returned status*/
  boolean status;
} tdssrchgsmdrv_gsm_cleanup_cmd_param_struct_type;

/* GSM RSSI power meas command structure definition */
typedef struct
{
 uint8 msg_id;
  /* Command send frame number */
  uint32 frame_num;
  /* number RSSI scan burst */
  uint8 num_bursts;
  /* ARFCN of the carrier for which RSSI scan to be done */
  W_ARFCN_T carrier_arfcn[TDSSRCHGSMDRV_MAX_PWR_MEAS_BURST_PER_GSM_FR];
  /* starting offset of start of the burst */
  uint16 offset_to_start[TDSSRCHGSMDRV_MAX_PWR_MEAS_BURST_PER_GSM_FR];
  /* expected RSSI strength fo rLNA range selection */
  int16 signal_strength_dbx16[TDSSRCHGSMDRV_MAX_PWR_MEAS_BURST_PER_GSM_FR];
  /* Burst position in the GAP */
  gl1_hw_burst_position_type burst_position[TDSSRCHGSMDRV_MAX_PWR_MEAS_BURST_PER_GSM_FR];
  /* GL1 HW abstraction to allow RF parameters to be passed back to GL1 HW driver
     This is transparent to WL1. */
  gl1_hw_rx_tags_type *tags;
  uint32 g_frame_number[TDSSRCHGSMDRV_MAX_PWR_MEAS_BURST_PER_GSM_FR];
//** for partial RSSI change**
  uint16                      startup_offset2;
  uint32                      startup_g_frame_number2;
//** for partial RSSI change** 
} tdssrchgsmdrv_gsm_pwr_scan_cmd_param_struct_type;

/* GSM FCCH ACQ start command structure definition */
typedef struct
{
  uint8 msg_id;
  /* Command send frame number */
  uint32 frame_num;
  /* GSM cell ARFCN */
  W_ARFCN_T arfcn;
  /* RSSI of the GSM cell */
  int16 signal_strength;
  /* ACQ start QS offset */
  uint32 start_offset;
  /* SCH to be decode if the tone is detected */
  /* Frequency offset for this GSM cell */
  int16 frequency_offset;
  boolean sch_decode;
#ifdef FEATURE_TDSCDMA_FCCH_OPTIMIZATION
  void *cell_ptr;
#endif
  /* GL1 HW abstraction to allow RF parameters to be passed back to GL1 HW driver
     This is transparent to WL1. */
  gl1_hw_rx_tags_type tags;
} tdssrchgsmdrv_gsm_start_acq_cmd_param_struct_type;

/* GSM FCCH ACQ stop command structure definition */
typedef struct
{
  /* Command send frame number */
  uint32 frame_num;
  /* GSM cell ARFCN */
  W_ARFCN_T arfcn;
  /* ACQ stop QS offset */
  uint32 stop_offset;
  /* indicate to perform cleanup for RF unwind along with stop ACQ */
  boolean do_cleanup;
} tdssrchgsmdrv_gsm_stop_acq_cmd_param_struct_type;

/* GSM SCH decode command structure definition */
typedef struct
{
  uint8 msg_id;
  /* Command send frame number */
  uint32 frame_num;
  /* GSM cell ARFCN */
  W_ARFCN_T arfcn;
  /* RSSI of the GSM cell */
  int16 signal_strength;
  /* SCH decode uncertaintity window */
  uint16 uncertainty_win_size;
  /* Offset of SCH burst center from frame boundary */
  uint16 offset_to_center_of_burst;
  /* Frequency offset for this GSM cell */
  int16 frequency_offset;
  #ifdef FEATURE_TDSCDMA_FCCH_OPTIMIZATION
  void *cell_ptr;
  boolean bsic_reconf;
  #endif
  /* GL1 HW abstraction to allow RF parameters to be passed back to GL1 HW driver
     This is transparent to WL1. */
  gl1_hw_rx_tags_type *tags;
} tdssrchgsmdrv_gsm_sch_decode_cmd_param_struct_type;

typedef void TDSSRCHGSMDRV_GSM_CMD_SENT_POST_CB_FUNC_TYPE(void);

/* GSM command structure definition */
typedef struct
{
  /* command type */
  tdssrchgsmdrv_cmd_type_enum_type cmd_type;
  /* frame number at which the command to be sent */
  uint8 frame_num;
  /* command pointer */
  void* cmd_param_ptr;
  /* cmd sent post call back function */
  TDSSRCHGSMDRV_GSM_CMD_SENT_POST_CB_FUNC_TYPE* cmd_sent_post_cb_func;
} tdssrchgsmdrv_mdsp_cmd_struct_type;

/* GSM frame number that counts 51 multiframe modulo count */
extern uint8 tdssrchgsmdrv_gsm_frame_num;

/* GSM related typedefinition */
/* -------------------------- */
typedef enum
{
  TDSSRCHGSMDRV_GSM_MEAS_STATE_IDLE,
  TDSSRCHGSMDRV_GSM_MEAS_STATE_READY,
  TDSSRCHGSMDRV_GSM_MEAS_STATE_MEAS,
  TDSSRCHGSMDRV_GSM_MEAS_STATE_DONE,
  TDSSRCHGSMDRV_GSM_MEAS_STATE_RES_PROC
} tdssrchgsmdrv_gsm_meas_state_enum_type;

typedef struct
{
  /* start frame number at which pwr scan can be started */
  uint32 start_frame_num;
  /* This is the starting offset in the start frame */
  uint16 start_offset_qs;
  /* window size in which the power scan can be done.
     Note that this can be more than 1 frame length */
  uint32 win_size_qs;
} tdssrchgsmdrv_gsm_pwr_scan_params_struct_type;

typedef struct
{
  /* cell channel frequency */
  W_ARFCN_T arfcn;
  /* timing valid. If valid then SCH decode otherwise FCCH detect */
  boolean timing_valid;
  /* first GSM frame number in which to start FCCH detect or SCH decode*/
  uint32 first_frame;
  /* offset in frame where to start SCH decode or FCCH detect*/
  uint16 offset_qs;
  /* for SCH decode */
  /* frequency offset for this cell from the fcch tone detect */
  int16 frequency_offset;
  /* for FCCH detect */
  /* window size for FCCH detect */
  uint32 fcch_win_size;
  /* for SCH decode */
  /* number of frame available for SCH decode */
  /*zy note:this field will be deleted*/
  uint8 num_frames;
 #ifdef FEATURE_TDSCDMA_L1_GSM_ALLOW_FLEX_UNCERTAINTY_WIN
  /* number of extra QS available for improving odds of SCH decode */
  uint16 extra_qs_uncertainty_win[TDSSRCHGSMDRV_GSM_SCH_DEC_CMD_MAX_PER_GAP];
 #endif
} tdssrchgsmdrv_gsm_bsic_id_params_struct_type;

typedef struct
{
  /* cell channel frequency */
  W_ARFCN_T arfcn;
  /* GSM frame number in which to do SCH decode*/
  uint32 sch_decode_fr_num;
  /* offset in frame where to start SCH decode */
  uint16 sch_decode_qs_offset;
  /* frequency offset for the SCH decode */
  int16 frequency_offset;
 #ifdef FEATURE_TDSCDMA_L1_GSM_ALLOW_FLEX_UNCERTAINTY_WIN
  /* number of extra QS available for improving odds of SCH decode */
  uint16 extra_qs_uncertainty_win;
 #endif
} tdssrchgsmdrv_gsm_bsic_reconf_params_struct_type;

#define TDSSRCHGSMDRV_W2G_INTLOCK()  REX_ISR_LOCK(&tdssrchgsmdrv_w2g_crit_sect)
#define TDSSRCHGSMDRV_W2G_INTFREE()  REX_ISR_UNLOCK(&tdssrchgsmdrv_w2g_crit_sect)


/* =======================================================================
**                        Function Declarations
** ======================================================================= */

/*===========================================================================
FUNCTION tdssrchgsmdrv_init
         tdssrchgsmdrv_init_on_resume

DESCRIPTION
  This function initializes the variables related to this module.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

//extern void tdssrchgsmdrv_init_on_resume(void);
extern void tdssrchgsmdrv_init(void);
extern void tdssrchgsmdrv_init_not_resume(void);

#ifdef FEATURE_TDSCDMA_TRM_NEW_API
/*===========================================================================
FUNCTION tdssrchgsmdrv_deregister_buf_init

DESCRIPTION
  This function initializes the deregister buffer (deregister GSM bands with TRM).

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void tdssrchgsmdrv_deregister_buf_init(void);
#endif

#if 0
/*===========================================================================
FUNCTION tdssrchgsmdrv_gsm_frame_tick_handler

DESCRIPTION
  This function implements the GSM frame tick hanldler. This function must
  be register to GSM timer module to be called back every GSM frame. It do
  the following actions:
  - Get the GSM time QS and frame number
  - Sequence the command that can be sent in this frame
  - Call the result poll call back function.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

extern void tdssrchgsmdrv_gsm_frame_tick_handler(void);
#endif
/*===========================================================================
FUNCTION tdssrchgsmdrv_gsm_get_gfn

DESCRIPTION
  This function is a utility function to return current GFN.

DEPENDENCIES
  None

RETURN VALUE
  Current GSM frame number.

SIDE EFFECTS
  None
===========================================================================*/

extern uint8 tdssrchgsmdrv_gsm_get_gfn(void);

/*===========================================================================
FUNCTION tdssrchgsmdrv_gfn_is_odd_frame

DESCRIPTION
  This function is a utility function to get the odd frame bit value

DEPENDENCIES
  None

RETURN VALUE
  TRUE id Odd bit is TRUE

SIDE EFFECTS
  None
===========================================================================*/

extern boolean tdssrchgsmdrv_gfn_is_odd_frame(void);

#if 0
/*===========================================================================
FUNCTION tdssrchgsmdrv_update_gap_params_to_gsm_tl

DESCRIPTION
  This function convert the WCDMA compressed gap parameters to GSM timeline.
  The GSM gap is the GAP in which measurement can be done. It excludes the UL
  transmission time and DL pilot time in the start and end of gap respectively.
  
  See description in function comments.

DEPENDENCIES
  None

RETURN VALUE
  Compressed mode GAP in GSM time info structure pointer.

SIDE EFFECTS
  None
===========================================================================*/

extern tdssrchgsmdrv_gap_param_gsm_struct_type* tdssrchgsmdrv_update_gap_params_to_gsm_tl(
  /* WCDMA frame offset from the GAP. If 0 indicate that GAP is in the next frame */
  uint8 wcdma_frame_offset,
  /* GAP start inf slot number in WCDMA slot unit */
  uint8 gap_start_slot_num,
  /* GAP length in WCDMA slot units */
  uint8 gap_length);
#endif
/*===========================================================================
FUNCTION tdssrchgsmdrv_send_cmd

DESCRIPTION
  This function sends the GSM command. It checks for the GSM frame number of
  the command to be sent. If this is the frame in which the command can be sent
  then immediately send it other wise it queues the command in comand Q.
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

extern void tdssrchgsmdrv_send_cmd(
  /* GSM Command type */
  tdssrchgsmdrv_cmd_type_enum_type cmd_type,
  /* GSM command pointer */
  void* gsm_mdsp_cmd_ptr,
  /* call back function after command has been sent */
  TDSSRCHGSMDRV_GSM_CMD_SENT_POST_CB_FUNC_TYPE* cmd_sent_post_cb_func);

/*===========================================================================
FUNCTION tdssrchgsmdrv_send_cmd

DESCRIPTION
  This function sends the GSM command immediately. It calls appropriate
  forwarding function in this module to send the command to GSM mDSP drivers.
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

extern void tdssrchgsmdrv_send_cmd_now(
  /* GSM Command type */
  tdssrchgsmdrv_cmd_type_enum_type cmd_type,
  /* GSM command pointer */
  void* gsm_mdsp_cmd_ptr,
  /* call back function after command has been sent */
  TDSSRCHGSMDRV_GSM_CMD_SENT_POST_CB_FUNC_TYPE* cmd_sent_post_cb_func);

  extern void tdssrchgsmdrv_send_rsp_now(
  /* GSM Command type */
  tdssrchgsmdrv_cmd_type_enum_type cmd_type,
  /* GSM command pointer */
  void* gsm_mdsp_cmd_ptr);

/*===========================================================================
FUNCTION tdssrchgsmdrv_send_gsm_startup_cmd

DESCRIPTION
  This function sends GSM startup command.
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

extern void tdssrchgsmdrv_send_gsm_startup_cmd(
  /* startup command parameters */
  tdssrchgsmdrv_gsm_startup_cmd_param_struct_type *cm_startup_cmd_ptr);

/*===========================================================================
FUNCTION tdssrchgsmdrv_send_gsm_cleanup_cmd

DESCRIPTION
  This function sends GSM cleanup command.
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

extern void tdssrchgsmdrv_send_gsm_cleanup_cmd(
  /* Cleanup command parameters */
  tdssrchgsmdrv_gsm_cleanup_cmd_param_struct_type *cm_cleanup_cmd_ptr);

/*===========================================================================
FUNCTION tdssrchgsmdrv_send_pwr_meas_cmd

DESCRIPTION
  This function sends GSM RSSI measure command.
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

extern void tdssrchgsmdrv_send_pwr_meas_cmd(
  /* RSSI meas command parameters */
  tdssrchgsmdrv_gsm_pwr_scan_cmd_param_struct_type *pwr_meas_cmd_ptr);

#if 0
/*===========================================================================
FUNCTION tdssrchgsmdrv_read_pwr_meas_result

DESCRIPTION
  This function reads the RSSI meas result from GSM driver
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

extern void tdssrchgsmdrv_read_pwr_meas_result(
  /* number of RSSI scan burst */
  uint8 num_bursts,
  /* RSSI measured values */
  int16 *rssi_meas_buffer,
  /* lna state values */
  uint8 *lna_state_buffer
  #if 0
  /* GSM HW frame layer tags */
  ,gl1_hw_rx_tags_type *tags
  #endif
);
#endif
/*===========================================================================
FUNCTION tdssrchgsmdrv_send_acq_start_cmd

DESCRIPTION
  This function sends ACQ start command.
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

extern void tdssrchgsmdrv_send_acq_start_cmd(
  /* FCCH ACQ start command parameters */
  tdssrchgsmdrv_gsm_start_acq_cmd_param_struct_type *acq_start_cmd_ptr);

/*===========================================================================
FUNCTION tdssrchgsmdrv_send_acq_stop_cmd

DESCRIPTION
  This function sends ACQ stop command.
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

extern void tdssrchgsmdrv_send_acq_stop_cmd(
  /* FCCH ACQ stop command parameters */
  tdssrchgsmdrv_gsm_stop_acq_cmd_param_struct_type *acq_stop_cmd_ptr);

/*===========================================================================
FUNCTION tdssrchgsmdrv_send_acq_cleanup_req

DESCRIPTION
  This function sends cleanup req for RF driver POP for ACQ start
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

extern void tdssrchgsmdrv_send_acq_cleanup_req(void);


/*===========================================================================
FUNCTION tdssrchgsmdrv_send_sch_decode_cmd

DESCRIPTION
  This function send GSM SCH decode async read command.
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

extern void tdssrchgsmdrv_send_sch_decode_cmd(
  /* SCH decode command parameter */
  tdssrchgsmdrv_gsm_sch_decode_cmd_param_struct_type *sch_decode_cmd_ptr);

/*===========================================================================
FUNCTION tdssrchgsmdrv_set_sch_decode_search_width

DESCRIPTION
  This function sets the SCH decode search width. This function should be
  called after calling tdssrchgsmdrv_send_sch_decode_cmd.
  Function tdssrchgsmdrv_send_sch_decode_cmd calls gl1_hw_rx_async_sch_burst which
  sets the default SCH search width.
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

extern void tdssrchgsmdrv_set_sch_decode_search_width(
  /* number of Symbols for SCH to decode */
  uint16 search_width_qs);

#if 0
/*===========================================================================
FUNCTION tdssrchgsmdrv_check_pending_cmd_q

DESCRIPTION
  This function checks cmd Q for any pending cmd that need not be sent
  as the part of cleanup.
  
DEPENDENCIES
  None

RETURN VALUE
  TRUE of FALSE based on remaining pending commands

SIDE EFFECTS
  None
===========================================================================*/

extern boolean tdssrchgsmdrv_check_pending_cmd_q(
  /* Cmd cleanup poll call back function */
  TDSSRCHGSMDRV_CMD_CLEANUP_POLL_CB_FUNC_TYPE *cb_func);

/*===========================================================================
FUNCTION tdssrchgsmdrv_clean_pending_cmd_q

DESCRIPTION
  This function cleans cmd Q for any pending cmd that need not be sent
  as the part of cleanup.
  
DEPENDENCIES
  None

RETURN VALUE
  TRUE of FALSE based on remaining pending commands

SIDE EFFECTS
  None
===========================================================================*/
extern void tdssrchgsmdrv_clean_pending_cmd_q(void);


/*===========================================================================
FUNCTION tdssrchgsmdrv_set_cmd_cleanup_pre_cb

DESCRIPTION
  This function sets the callback function for tdssrchgsmdrv_cmd_cleanup_pre_cb_func.
  
DEPENDENCIES
  None

RETURN VALUE
  TRUE of FALSE based on remaining pending commands

SIDE EFFECTS
  None
===========================================================================*/
extern void tdssrchgsmdrv_set_cmd_cleanup_pre_cb(
  /* Cmd cleanup pre call back function */
  TDSSRCHGSMDRV_CMD_CLEANUP_PRE_CB_FUNC_TYPE *cb_func);
#endif

extern tdssrchgsmdrv_tds_time_struct_type tdssrchgsmdrv_calc_tds_end_time(
tdssrchgsmdrv_tds_time_struct_type tds_start_time,
tdssrchgsmdrv_gsm_startup_cmd_param_struct_type gsm_start,
tdssrchgsmdrv_gsm_cleanup_cmd_param_struct_type gsm_end);
/*===========================================================================
FUNCTION tdssrchgsmdrv_update_gsm_startup_cmd_params

DESCRIPTION
  This function updates the GSM startup parameters. It puts the CM start up
  command TDSSRCHGSMDRV_GSM_CM_START_UP_TIME_QS before the gap startup time. It
  also adjusts the command frame and QS offset based of GSM FTSM.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void tdssrchgsmdrv_update_gsm_startup_cmd_params(
  /* gap parameters in GSM time units */
  tdssrchgsmdrv_gap_param_gsm_struct_type *gap_param_for_gsm,
  /* GSM startup command parameters */
  tdssrchgsmdrv_gsm_startup_cmd_param_struct_type *startup_cmd_params);

/*===========================================================================
FUNCTION tdssrchgsmdrv_update_gsm_cleanup_cmd_params

DESCRIPTION
  This function updates the GSM cleanup parameters. It set the cleanup command
  to be sent at the clean up GFN passed to the function and adjust the command
  frame and QS offset for FTMS.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void tdssrchgsmdrv_update_gsm_cleanup_cmd_params(
  /* GFN at which cleanup command should go  */
  uint32 cleanup_frame_num,
  /* GSM cleanup offset. This can be larger than QS per frame */
  uint16 cleanup_offset,
  /* clean up command parameters */
  tdssrchgsmdrv_gsm_cleanup_cmd_param_struct_type *cleanup_cmd_params);

/*===========================================================================
FUNCTION tdssrchgsmdrv_update_gsm_rssi_cmd_params

DESCRIPTION
  This function update the RSSI scan command buffre. The required number of RSSI
  scan may span multiple GSM frames. This function checks the location of each
  burst in GAP and put the burst in their respective command buffer.

DEPENDENCIES
  None

RETURN VALUE
  Number of RSSI scan command updated.

SIDE EFFECTS
  None
===========================================================================*/
extern uint8 tdssrchgsmdrv_update_gsm_rssi_cmd_params(
  /* Number of burst to be programmed in this GAP */
  uint8 num_bursts,
  /* RSSI scan measurement parameters info */
  tdssrchgsmdrv_gsm_pwr_scan_params_struct_type *pwr_scan_params,
  /* RSSI scan GSM command parameters */
  tdssrchgsmdrv_gsm_pwr_scan_cmd_param_struct_type *pwr_cmd_params);

extern void tdssrchgsmdrv_update_gsm_rssi_cmd_params_temp(
  /* Number of burst to be programmed in this GAP */
  uint8 num_bursts,
  /* RSSI scan measurement parameters info */
  tdssrchgsmdrv_gsm_pwr_scan_params_struct_type *pwr_scan_params,
  /* RSSI scan GSM command parameters */
  tdssrchgsmdrv_gsm_pwr_scan_cmd_param_struct_type *pwr_cmd_params);

/*===========================================================================
FUNCTION tdssrchgsmdrv_update_gsm_bsic_id_acq_cmd_params

DESCRIPTION
  This function sets the FCCH ACQ parameters. It takes ACQ start frame and
  ACQ window size for FCCH detect and formulates the ACQ start and stop command.
  ACQ start connabd is set for doing ACQ at the start of ACQ window and ACQ stop
  is at the end of ACQ window.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void tdssrchgsmdrv_update_gsm_bsic_id_acq_cmd_params(
  /* BSIC Id parameters */
  tdssrchgsmdrv_gsm_bsic_id_params_struct_type *bsic_id_params,
  /* FCCH ACQ start cmd params */
  tdssrchgsmdrv_gsm_start_acq_cmd_param_struct_type *acq_start_cmd_params,
  /* FCCH ACQ stop cmd params */
  tdssrchgsmdrv_gsm_stop_acq_cmd_param_struct_type *acq_stop_cmd_params);

/*===========================================================================
FUNCTION tdssrchgsmdrv_update_gsm_bsic_id_sch_dec_cmd_params

DESCRIPTION
  This function sets the SCH decode command parameter.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void tdssrchgsmdrv_update_gsm_bsic_id_sch_dec_cmd_params(
  /* BSIC Id parameters */
  tdssrchgsmdrv_gsm_bsic_id_params_struct_type *bsic_id_params,
  /* SCH decode cmd params */
  tdssrchgsmdrv_gsm_sch_decode_cmd_param_struct_type *sch_decode_params,
  /* sch decode command index */
  uint8 command_idx);

/*===========================================================================
FUNCTION tdssrchgsmdrv_update_gsm_bsic_reconf_sch_dec_cmd_params

DESCRIPTION
  This function sets the SCH decode command parameter for BSIC reconf.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void tdssrchgsmdrv_update_gsm_bsic_reconf_sch_dec_cmd_params(
  /* BSIC Id parameters */
  tdssrchgsmdrv_gsm_bsic_reconf_params_struct_type *bsic_reconf_params,
  /* SCH decode cmd params */
  tdssrchgsmdrv_gsm_sch_decode_cmd_param_struct_type *sch_decode_params);

/*===========================================================================

  FUNCTION:  tdssrchgsmdrv_error_detected

=========================================================================*/
extern boolean tdssrchgsmdrv_error_detected(void);

/*===========================================================================
FUNCTION tdssrchgsmdrv_get_error_status

DESCRIPTION
  This function returns the error status of GSM RxFE.
  
DEPENDENCIES
  None

RETURN VALUE
  TRUE means it's bad
  FALSE means it's good

SIDE EFFECTS
  None
===========================================================================*/
extern boolean tdssrchgsmdrv_get_error_status(void);

/*===========================================================================
FUNCTION srchgsm_set_error_status

DESCRIPTION
    This function sets the error status in GSM.
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void tdssrchgsmdrv_set_error_status(boolean status);

/*===========================================================================
FUNCTION tdssrchgsmdrv_init_gsm

DESCRIPTION
  This function initializes GSM for T2G functionality.
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void tdssrchgsmdrv_init_gsm(rex_tcb_type*  task_ptr,
                      void           (* task_wait_handler)(rex_sigs_type),
                      rex_sigs_type  task_wait_sig,
                      rex_sigs_type  task_wait_sig_1);

/*===========================================================================
FUNCTION tdssrchgsmdrv_init_gsm_api_call

DESCRIPTION
  This function initializes GSM for T2G functionality, by using the API function call.
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void tdssrchgsmdrv_init_gsm_api_call(rex_tcb_type*  task_ptr,
                      void           (* task_wait_handler)(rex_sigs_type),
                      rex_sigs_type  task_wait_sig,
                      rex_sigs_type  task_wait_sig_1);

extern boolean tdssrchgsmdrv_rxlm_update_needed;

#ifdef FEATURE_TDSCDMA_IRAT_RF_SCRIPT_PREBUILD
extern boolean tdssrchgsmdrv_need_to_build_rf_script;

/*===========================================================================
FUNCTION tdssrchgsmdrv_t2g_abort_rf_script

DESCRIPTION
  This function sends the cmd of t2g abort rf script
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

extern void tdssrchgsmdrv_t2g_abort_rf_script(void);

/*===========================================================================
FUNCTION tdssrchgsmdrv_build_startup_cleanup_script

DESCRIPTION
  This function call GL1 API to build startup and cleanup script
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void tdssrchgsmdrv_build_startup_cleanup_script(void);
#endif

extern void tdssrchgsmdrv_terminate_gsm(void);

#endif /* SRCHGSMDRV_H */

