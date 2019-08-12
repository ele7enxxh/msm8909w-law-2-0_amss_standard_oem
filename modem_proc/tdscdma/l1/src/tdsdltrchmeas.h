#ifndef TDSDLTRCHMEAS_H
#define TDSDLTRCHMEAS_H

/*============================================================================
                           T D S D L T R C H M E A S . H
GENERAL DESCRIPTION

EXTERNALIZED FUNCTIONS


INTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2001 - 2007 by Qualcomm Technologies, Inc.  All Rights Reserved.

============================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/tdscdma/l1/src/tdsdltrchmeas.h#1 $ $DateTime: 2016/12/13 07:58:19 $ $Author: mplcsds1 $

when        who     what, where, why
--------    ---     --------------------------------------------------------
09/17/10    jintaoh      Created this file.
===========================================================================*/


#include "tdsl1def.h"


/* -----------------------------------------------------------------------
** Constant / Define Declarations
** ----------------------------------------------------------------------- */

/* DCH In-sync/out-of sync related defines */
/* --------------------------------------- */

/* Qin and Qout thresholds.  */
#define Q_IN  (5 << 10)    /* 2 dB in Q10, Corresponds to 20% BER point */
#define Q_OUT (-10 << 10) /* -2 dB  in Q10, Corresponds to 30% BER point */

/* Qsbin and Qsbout thresholds.  */
#define QSB_IN  (4 << 10)   /* 2 dB in Q10, Corresponds to 20% BER point */
#define QSB_OUT (2 << 10) /* -2 dB  in Q10, Corresponds to 30% BER point */

/* when continue out of sync(dpch and sb) frame nubmer, then turn of TX  */
#define TDSDLTRCHMEAS_TURN_OFF_TX_TIMER 5

/* when after recfg, firs frame cnt which not turn off TX*/
#define TDSDLTRCHMEAS_TURN_OFF_TX_TIMER_AFTER_RECFG 100


/* Macro required for energy filtering */
#define TDSDLTRCHMEAS_DPCCH_ENG_FILTER_LEN_PH1 4
#define TDSDLTRCHMEAS_DPCCH_ENG_FILTER_LEN_PH2 16

#define TDSDLTRCHMEAS_DPCH_SYNC_MAX_CRC_ERR_FRAME 16 //(TDSDLTRCHMEAS_DPCH_SYNC_MAX_CRC_ERR_TIME/TDSDLTRCHMEAS_DPCH_SAMPLE_DUR_MS)

#define TDSDLTRCHMEAS_DPCH_SYNC_PH1_DUR_UNIT_FR 16

/* Out of servive area declaration for high BCH BLER in BCH state */
/* -------------------------------------------------------------- */

/* Minimum BCH block errors in a given accumulation */
#define TDSDLTRCHMEAS_BCH_HIGH_BLER_OOS_MIN_ERR_COUNT    25
/* Minimum percentage BLER on BCH TrCh */
#define TDSDLTRCHMEAS_BCH_HIGH_BLER_OOS_MIN_BLER_PERCENT 50

#define TDSDLTRCHMEAS_FAST_CU_EXPIRE_TIME_ENLARGE 20  
#define TDSDLTRCHMEAS_FAST_CU_FOWARD_STEP  1
#define TDSDLTRCHMEAS_FAST_CU_BACKOFF_STEP 64 

/* BLER measurement related type definition */
/* ---------------------------------------- */

/* This enum defines the BLER measurement criteria. */
typedef enum
{
  TDSDLTRCHMEAS_BLER_NO_REPORT,           /* No reporting required, it is disabled */
  TDSDLTRCHMEAS_BLER_REPORT_AUTO_PERIODIC_MEAS /* Periodic meas. criteria based on time */
} tdsdltrchmeas_bler_report_type_enum_type;

/* BLER measurement SETUP structure.
   This structure defines the parameter for given set of TrCh Ids.
   If the measurement criteria is Quality measurement then the
   quality measurement parameters are also given. Other reporting
   criteria does need parameters */
typedef struct
{
  /* measurement criteria.
     If measurement is to be disabled send TDSDLTRCHMEAS_BLER_NO_REPORT */
  tdsdltrchmeas_bler_report_type_enum_type meas_criteria;

  /* number of TrChs that follow above critera for BLER reporting */
  uint8 num_trch_id;
  /* Trch list */
  uint8 trch_id_list[TDSL1_DL_MAX_TRCH_ID];
} tdsdltrchmeas_l1_dl_bler_meas_setup_req_struct_type;

/* BLER measurement parameter structure.
   It defines the measurement criteria. If the criteria is quality measurement
   then it defines its parameters too. Other criteria doesn't have parameters */
typedef struct
{
  /* BLER reporting criteria */
  tdsdltrchmeas_bler_report_type_enum_type report_type;
} tdsdltrchmeas_bler_report_param_struct_type;

/* BLER measurement stats structure */
typedef struct
{
  /* CRC result received */
  uint32 dl_bler_num_tb_w_crc_accum;
  /* CRC result with CRC failed received */
  uint32 dl_bler_num_tb_w_crc_err_accum;
  /* desensed CRC result received*/
  uint32 dl_bler_num_tb_w_desensed_crc_accum;
  /* desensed CRC result with CRC failed received */
  uint32 dl_bler_num_tb_w_desensed_crc_error_accum;
  /*SB number*/
  uint8 dl_bler_num_sb;
  /*TTIpunctured number*/
  uint8 dl_num_ttipunctured;

} tdsdltrchmeas_bler_state_struct_type;

//@dingyuan FAST CU 
typedef enum
{
  TDSDLTRCHMEAS_NON_SB,
  TDSDLTRCHMEAS_WEAK_SB,
  TDSDLTRCHMEAS_STRONG_SB          
} tdsdltrchmeas_sb_rating_enum_type;

/* DCH in-sync/out-of-sync detection related type definition */
/* --------------------------------------------------------- */

/*--------------------------------------------------------------------------
                  ENUM: LOW_LEVEL_SYNC_IND_ENUM_TYPE

This enumerates the possible output indications from the DCH low level
sync detection sub-module.

IN_SYNC_IND     an in sync indication
OUT_SYNC_IND    an out of sync indication
NO_IND          neither indication was seen to be true, no indication made
ERR_IND         both the in and out of sync conditions were true
--------------------------------------------------------------------------*/
typedef enum {
  TDSDLTRCHMEAS_LL_SYNC_IN_SYNC_IND,
  TDSDLTRCHMEAS_LL_SYNC_OUT_SYNC_IND,
  TDSDLTRCHMEAS_LL_SYNC_NO_IND,
  TDSDLTRCHMEAS_LL_SYNC_ERR_IND
} tdsdltrchmeas_low_level_sync_ind_enum_type;

/*--------------------------------------------------------------------------
                       ENUM: DCH_SYNC_STATE_ENUM_TYPE

This enumerates the two possible sync states, IN and OUT of sync.
--------------------------------------------------------------------------*/
typedef enum {
  TDSDLTRCHMEAS_IN_SYNC_STATE,
  TDSDLTRCHMEAS_OUT_OF_SYNC_STATE
} tdsdltrchmeas_dch_sync_state_enum_type;

/*--------------------------------------------------------------------------
                  STRUCTURE: DL_SYNC_DET_VAR_STRUCT_TYPE

This struture contains the persistent variables for DCH state sync
detection.
--------------------------------------------------------------------------*/

typedef struct {
  /* TRUE if Sync detection is enabled, this occurs at the 1st frame
     boundary of a newly setup DL DPCH */
  boolean enabled;
  uint16 enabled_action_time;
  /* the N312 counter used for declaring the DPCH established */
  uint16 N312;

  /* the N313 counter used for detecting the OUT_OF_SYNC condition */
  uint16 N313;

  /* the N315 counter used for detecting the IN_SYNC condition */
  uint16 N315;

  /* TRUE when the DPCH is declared as established, FALSE before then */
  boolean dpch_estab;

  /* TRUE if in the 1st 160 ms of DCH sync detection operation */
  boolean in_phase_1;

  /* Counter to track number of frames after the Uplink DPCCH 
  establishment is done */
  //uint8 ul_dpcch_config_delay_cnt;

  /* TRUE if in the 1st 40 ms of first 160 ms (Phase 1) of DCH sync
     detection operation */
  uint8 in_first_40ms;

  /* the filter buffer for DPCCH energy filtering */
  int32 dpcch_eng_data_ph1[TDSDLTRCHMEAS_DPCCH_ENG_FILTER_LEN_PH1];
  //boolean dpcch_eng_indicator_ph1[TDSDLTRCHMEAS_DPCCH_ENG_FILTER_LEN_PH1];   

  int32 dpcch_eng_data_ph2[TDSDLTRCHMEAS_DPCCH_ENG_FILTER_LEN_PH2];
  //boolean dpcch_eng_indicator_ph2[TDSDLTRCHMEAS_DPCCH_ENG_FILTER_LEN_PH2]; 

  /* the DPCCH energy accumulator */
  int32 dpcch_eng_accum_ph1;
  int32 dpcch_eng_accum_ph2;

  /* the DPCCH filter data buffer head pointer */
  uint16 dpcch_filter_head_ph1;
  uint16 dpcch_filter_head_ph2;

  /* frame count for the phase 1 */
  uint32 frame_cnt_ph1;
  //uint32 frame_cnt_ph1_after_estab;

  /* keep track of CRC errors, number of TB with CRC count */
  uint16  num_tb_with_crc[TDSDLTRCHMEAS_DPCH_SYNC_MAX_CRC_ERR_FRAME];
  uint16  num_tb_with_crc_err[TDSDLTRCHMEAS_DPCH_SYNC_MAX_CRC_ERR_FRAME];
  uint8  tb_with_crc_cnt_head;
  uint16 num_tb_rece_with_crc_accum;
  uint16 num_tb_rece_with_crc_err_accum;
  //uint16 consec_bad_trblk_cnt;

  /*bitmask of the sb, 1 index exceed qsbin and qsbout*/
  uint16 sb_qin_bm;
  uint16 sb_qout_bm;

  /*bitmask for crc or SB exist for current frame*/
  uint16 crc_or_sb_exist_bm;
  /*the minmum tti length of the current cctrch, n idex n*10 ms*/
  uint8 minmum_tti;

  /*indicator the place of the sb in the minmum TTI [1...TTI] */
  //uint8 sb_in_tti_place;
  
  /* the DCH sync state */
  tdsdltrchmeas_dch_sync_state_enum_type sync_state;

  /* how many consecutive low level in sync indications have been received */
  uint16 in_sync_cnt;

  /* how many consecutive low level out of sync indications have been
  received */
  uint16 out_sync_cnt;

  tdsdltrchmeas_low_level_sync_ind_enum_type ll_sync_status;

  /* number of frames in which turn off tx */
  //uint16 frame_cnt_turn_off_tx;

  //uint16 frame_cnt_turn_off_tx_after_recfg;

  /* number of frames in which the T312 timer expires */
  uint16 t312_unit_frame;
  /* number of frames in which the T313 timer expires */
  uint16 t313_unit_frame;

  /* indicate if T313 counters are active */  
  boolean t313_active;

  /* stores the currect timer count of the timers */
  uint16 t312_cur_count;
  uint16 t313_cur_count;
} tdsdltrchmeas_sync_det_var_struct_type;

/* COMMAND: TDS_DL_TRBLK_CRC_STATUS_CMD */
/* -------------------------------- */

/* This structure contains parameters for the TDS_DL_TRBLK_CRC_STATUS_CMD. */
/* --------------------------------------------------------------------*/
typedef struct {
  tdsl1def_l1_local_cmd_hdr_type hdr;

  /* the number of transport blocks recieved this frame */
  uint8 ntrblks;

  /* the number of transport blocks received in error this frame */
  uint8 nerr;

} tdsdltrchmeas_trblk_crc_status_type;

/*------This structure contains parameters for the fast cu---------*/
typedef struct {
    uint16  expire_timer;    //if recieve no crc or weak sb over this time ,we will trigger RLF at once. count in subframe 5ms
    uint16  count_timer;
    uint32  hs_crc_accum; //agch and scch right crc count. 
} tdsdltrchmeas_fast_cu_type;
//@dingyuan should check vanni's dyn mem

/* -----------------------------------------------------------------------
** Global Data Declarations
** ----------------------------------------------------------------------- */

/* =======================================================================
**                        Function Declarations
** ======================================================================= */

/*===========================================================================
FUNCTION tdsdltrchmeas_meas_init

DESCRIPTION
  This function initialize the trch related measurement.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

extern void tdsdltrchmeas_meas_init(void);

/* BLER measurement related function */
/* --------------------------------- */

/*===========================================================================
FUNCTION tdsdltrchmeas_bler_setup_meas_critera

DESCRIPTION
  This function sets up the BLER measurement criteria. It can be used to
  set/modify/remove any BLER measurement for a TrCh Id. The BLER stats and
  parameters are reset when CCTrCh is setup for new PhyChan ADD. The BLER
  measurement is not cancelled when CCTrCh is dropped. It is assumed that
  next Phychan Add/CCTrCh setup will reset it or it will be turned off
  explicitly by using this function.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

extern void tdsdltrchmeas_bler_setup_meas_critera(
  /* bler meas req */
  tdsdltrchmeas_l1_dl_bler_meas_setup_req_struct_type *bler_meas_setup_req);

/*===========================================================================
FUNCTION tdsdltrchmeas_bler_reset_stats_params

DESCRIPTION
  This function resets the BLER stats and params for the given TrCh Id.
  The measurement criteria is set to NO REPORT. All counters/values are
  set to 0. All flags are set to FALSE.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

extern void tdsdltrchmeas_bler_reset_stats_params(
  /* TrCh id */
  uint8 trch_id);

/*===========================================================================
FUNCTION tdsdltrchmeas_trch_bler_info_update

DESCRIPTION
  This function is called for updating the BLER info update. This functions
  updates the number of CRC received and the number of CRC errors received.
  It assumes that number of TB received is only 1 (for example in BTFD TrCh
  pass/fail, RFCI decode fail)

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

extern void tdsdltrchmeas_trch_bler_info_update(
  /* TrCh Id */
  uint8 trch_id,
  /* number of CRC results */
  uint8 crc_results,
  /* CRC errors */
  uint8 crc_errors,
  /* desensed indicator*/
  boolean desensed_indicator);

/*===========================================================================
FUNCTION tdsdltrchmeas_bler_info_update_w_tb_info

DESCRIPTION
  This function is similar to tdsdltrchmeas_trch_bler_info_update except that it has
  explicit TB CRC pass/fail information passed to it.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

extern void tdsdltrchmeas_bler_info_update_w_tb_info(
  /* TrCh Id */
  uint8 trch_id,
  /* number of CRC results. This is equal to number of TBs */
  uint8 crc_results,
  /* CRC errors */
  uint8 crc_errors,
  /* CRC pass/fail result for each TB, LSB is last TB decoded */
  uint32 tb_crc_result,
  /* desensed indicator*/
  boolean desensed_indicator);

/*===========================================================================
FUNCTION tdsdltrchmeas_SB_info_update

DESCRIPTION
  This function is called by tdsdecdata,when there is a SB.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void tdsdltrchmeas_sb_info_update(  
  /* TrCh Id */
  uint8 trch_id,
  /* number of sb */
  uint8 sb_num);

/*===========================================================================
FUNCTION tdsdltrchmeas_ttipunctured_info_update

DESCRIPTION
  This function is called by tdsdecdata,when there is a punctured TTI.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void tdsdltrchmeas_ttipunctured_info_update(  
  /* TrCh Id */
  uint8 trch_id,
  /* number of sb */
  uint8 ttipunctured_num);

/*===========================================================================
FUNCTION tdsdltrchmeas_bler_periodic_report

DESCRIPTION
  This function calculate the BLER log values for the list of TrCh ids
  passed to the function. It checks for the reporting type of the BLER
  and if it is not the periodic measurement then it set the value to zero.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

extern void tdsdltrchmeas_bler_periodic_report(
  /* number of TrCh Id in the list */
  uint8 num_trch,
  /* TrCh Id list for which to report the measurement */
  uint8 *trc_id_list,
  /* Array in which the BLER log values are reported */
  uint8 *bler_log_val,
  /* BLER calculation window */
  uint16 report_dur_ms);

/* DCH in-sync/out-of-sync detection related function */
/* -------------------------------------------------- */

/*===========================================================================
FUNCTION        DL_TRBLK_CRC_STATUS_RPT

FILE NAME       dl_dem.c

DESCRIPTION     This function is called by the downlink/decoder module at
                every decoder header event.  It indicates to the downlink/
                sync detection sub-module that on the CCTrCH referred to
                by the cctrch_table_index
                1) how many transport blocks were received
                2) how many transport blocks were received in error as
                   indicated by their CRC check status.

                This function then formats a L1 local command with this
                information and posts it to the L1 local command queue.

DEPENDENCIES    None

RETURN VALUE    None

SIDE EFFECTS    A TDS_DL_TRBLK_CRC_STATUS_CMD will be posted to the L1
                local command queue.
===========================================================================*/

extern void tdsdltrchmeas_trblk_crc_status_rpt(
  /* how many transport blocks received this frame */
  uint8 ntrblks,
  /* how many of them failed their CRC check */
  uint8 nfail);

/*===========================================================================
FUNCTION        DL_DCH_SYNC_INIT

FILE NAME       dl_dem.c

DESCRIPTION     This function initializes the low and high level DCH sync
                detection variables and state.

DEPENDENCIES    None

RETURN VALUE    None

SIDE EFFECTS    The DCH sync variables are initialized
===========================================================================*/

extern void tdsdltrchmeas_dch_sync_init(void);
/*===========================================================================
FUNCTION        DL_LOW_LEVEL_SYNC_DETECT

FILE NAME       dl_dem.c

DESCRIPTION     This function performs the low level sync detection in the
                DCH state as described by TS 25.214.

DEPENDENCIES    None

RETURN VALUE    The low level sync indication, one of the following
                TDSDLTRCHMEAS_LL_SYNC_IN_SYNC_IND,
                TDSDLTRCHMEAS_LL_SYNC_OUT_SYNC_IND,
                TDSDLTRCHMEAS_LL_SYNC_NO_IND,
                TDSDLTRCHMEAS_LL_SYNC_ERR_IND

SIDE EFFECTS    The uplink controller may be instructed to enable or disable
                the transmitter
===========================================================================*/

extern tdsdltrchmeas_low_level_sync_ind_enum_type tdsdltrchmeas_low_level_sync_detect(
  /* how many transport blocks received in total this frame */
  uint16 nblks,
  /* how many transport blocks received in error this frame */
  uint16 nerr,
  /* the current DPCCH energy sample */
  int32 burst_quality,  
  /* the current SB energy sample */
  int16 tdm_indicator,
  /*if the current frame is SB*/ 
  boolean special_burst_indicator,
  /*tfci of OTA*/
  uint16 tfci,
  /* if the frame is desensed */ 
  uint16 deSensedInd);

/*===========================================================================
FUNCTION        DL_HIGH_LEVEL_SYNC_DETECT

FILE NAME       dl_dem.c

DESCRIPTION     This function implements the high level sync detection

DEPENDENCIES    None

RETURN VALUE    None

SIDE EFFECTS    Indications may be sent to RRC, one of the following
                CPHY_PHYCHAN_ESTABLISHED_IND,
                CPHY_IN_SYNC_IND,
                CPHY_OUT_OF_SYNC_IND
===========================================================================*/

extern void tdsdltrchmeas_high_level_sync_detect(
  /* the indication received from the low level sync detector */
  tdsdltrchmeas_low_level_sync_ind_enum_type frame_ind);

/*===========================================================================
FUNCTION        tdsdltrchmeas_set_dch_sync_enabled

FILE NAME       dltrchmeas.c

DESCRIPTION     set tdsdltrchmeas_dch_sync_vars enable to true

DEPENDENCIES    None

RETURN VALUE    None

SIDE EFFECTS    None
===========================================================================*/
extern void tdsdltrchmeas_set_dch_sync_enabled(uint16 action_time);

/*===========================================================================
FUNCTION        tdsdltrchmeas_update_dch_sync_timer

FILE NAME       dltrchmeas.c

DESCRIPTION     update tdsdltrchmeas_dch_sync_vars time

DEPENDENCIES    None

RETURN VALUE    None

SIDE EFFECTS    None
===========================================================================*/
extern void tdsdltrchmeas_update_dch_sync_timer(tdsl1_dl_phycfg_dpch_sync_parms_struct_type *sync_det);

/*===========================================================================
FUNCTION        TDS_DL_TRBLK_CRC_STATUS_CMD

FILE NAME       dl_dem.c

DESCRIPTION     This function handles the TDS_DL_TRBLK_CRC_STATUS_CMD.

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS
===========================================================================*/

/* COMMAND: TDS_DL_TRBLK_CRC_STATUS_CMD */
/* -------------------------------- */

extern void tdsdltrchmeas_trblk_crc_status_cmd(
  /* DL DCH trblk CRC report command */
  tdsdltrchmeas_trblk_crc_status_type *cmd);

/*===========================================================================
FUNCTION tdsdltrchmeas_ul_freeze_tpc

DESCRIPTION
  This function freezes the TPC UL accumulation. It also sets the various flag
  assist the unfreezing the UL TPC accumulation.
  This is intended to be used for UL TPC accumulation freeze for DPCH reconfig.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

void tdsdltrchmeas_ul_freeze_tpc(void);

/*===========================================================================
FUNCTION        DL_UPD_SYNC_PARMS

FILE NAME       dltrchmeas.c

DESCRIPTION     This function updates the constants related to DL sync-N312,
                N313 and N315 right away. The timers-T312 and T313 are updated
                if they are not running. Else, they are marked to be updated when
                the timers are stopped. (Sec-8.3.3.3 in 25.331 v3.18.0)

DEPENDENCIES    None

RETURN VALUE    None

SIDE EFFECTS    None
===========================================================================*/
extern void tdsdltrchmeas_upd_sync_parms(tdsl1_upd_dl_sync_parms_cmd_type *cmd);

/*===========================================================================
FUNCTION        tdsdltrchmeas_Set_dch_sync_minmum_tti

FILE NAME       dltrchmeas.c

DESCRIPTION     set tdsdltrchmeas_dch_sync_vars minmum_tti

DEPENDENCIES    None

RETURN VALUE    None

SIDE EFFECTS    None
===========================================================================*/
extern void tdsdltrchmeas_Set_dch_sync_minmum_tti(uint8 tti_length);

/*===========================================================================
FUNCTION tdsdltrchmeas_set_tfc_crc_exist_status //TDS

DESCRIPTION
  This function gets all TFC crc status, if this tfc has crc present set TRUE

DEPENDENCIES
  None

RETURN VALUE
  void
  
SIDE EFFECTS
  None
===========================================================================*/
void tdsdltrchmeas_set_tfc_crc_exist_status(void);
extern int16 tdsdltrchmeas_get_burst_quality(void);

/*===========================================================================
FUNCTION tdsdltrchmeas_current_sfn_is_tdm_on //TDS

DESCRIPTION
  This function gets current sfn is TDM on or off

DEPENDENCIES
  None

RETURN VALUE
    TRUE: ON
    FALSE: OFF
  
SIDE EFFECTS
  None
===========================================================================*/
boolean tdsdltrchmeas_current_sfn_is_tdm_on(uint16 current_sfn);

/*===========================================================================
FUNCTION tdsdltrchmeas_rl_failure_for_recovery_fw

DESCRIPTION
  This function send rl f to RRC when receive error ind from FW

DEPENDENCIES
  None

RETURN VALUE
    TRUE: ON
    FALSE: OFF
  
SIDE EFFECTS
  None
===========================================================================*/
#ifdef FEATURE_TDSCDMA_L1M_SELF_RESET_FW
boolean tdsdltrchmeas_rl_failure_for_recovery_fw(void);
#endif
/* Extern Functions */
extern void tdsedpch_tx_re_enable(void); 
extern boolean tdsrm_current_tfc_if_exist_crc(uint8 tfc_idx);


extern void tdsdltrchmeas_detect_fast_cu(uint16 nblk, uint16 nerr, 
                                  tdsdltrchmeas_sb_rating_enum_type  sb_rating,
                                  uint16 tdm_ind, 
                                  uint16 desensed_ind, 
                                  uint16 puncture_ind);

extern void tdsdltrchmeas_count_hs_crc_result(boolean hs_crc_pass);
extern void tdsdltrchmeas_fast_cu_init(void);

#ifdef FEATURE_TDSCDMA_FATAL_RECOVERY
extern void tdsdltrchmeas_send_hw_error_ind(void);
#endif

#endif /* DL_TRCH_MEAS_H */

