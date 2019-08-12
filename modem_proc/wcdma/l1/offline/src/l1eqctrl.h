#ifndef L1EQCTRL_H
#define L1EQCTRL_H
/*===========================================================================
                 WCDMA L1 Equalizer Controller

GENERAL DESCRIPTION
  This file contains the code for controlling the operation of equalizer

EXTERNALIZED FUNCTIONS

INTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS


Copyright (c) 2005 - 2012 by Qualcomm Technologies, Inc.  All Rights Reserved.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/wcdma/l1/offline/src/l1eqctrl.h#1 $
$DateTime: 2016/12/13 08:00:12 $
$Author: mplcsds1 $

when       who    what, where, why
--------   ---    ---------------------------------------------------------
05/23/13   geg    WSW decoupling (except for component of QCHAT)
02/08/12   vs     feature cleanup.
09/22/09   sup    Removed log code assignments and placed them
                  in log_codes_wcdma_int_v.h
10/03/08   hk     Bringing in 7k Mailine fixes
06/10/08   ms     Added PACKED_POST attribute for structures to make it
                  compatible for GNU compiler.
08/30/07   ms     Added minimum reframing interval variable to eq_params_db
08/17/07   ms     Added Predictive CQI co-efficient variables to eq_params_db
06/29/07   ms     Added CQI filter gain constants for Non-CLTD
                  Added Pth constants for Rxd and Non-Rxd
06/27/07   ms     Changed 0x4215 EQ Debug log to 0x4771. Defined
                  FEATURE_WCDMA_EQ_DBG_LOG_PKT_V3 internally.
05/17/07   ms     Handled PN Position wraparound while determining EQ window
04/11/07   mc     Added support to disable EQ on S-CPICH
04/09/07   mc     Changed EQ NV item definition, CQI BLER alignment for Type3
02/06/07   mc     Added support for Type3 receiver
01/17/07   mc     Changed CQI offset for CLTD
11/22/06   mc     Added CQI filter gain constants.
10/27/06   mc     Removed CLTD featurization around SCCH pilot constants.
10/25/06   mc     Separated EQ+CLTD and Speed adaptation features
                  under different feature names
10/24/06   mc     Featurize EQ Phase2 specific features.
09/27/06   mc     Added support for EQ speed adaptation.
09/07/06   mc     Added support for EQ+CLTD
07/10/06   mc     Added macro for invalid start window position.
04/26/06   mc     Added a new field to the EQ debug log packet.
04/14/06   mc     Changed logical OR to bitwise OR in macro to set EQ mask.
04/10/06   mc     Added support for EQ log packets
04/04/06   mc     Added the prototype for querying EQ support.
11/29/05   mc     Initial version. Added support for Equalizer under the 
                  feature flag FEATURE_WCDMA_EQ_CONTROLLER

===========================================================================*/

/* ==========================================================================
** Includes and Variable Definitions
** ========================================================================*/

/* -----------------------------------------------------------------------
** Include Files
** ----------------------------------------------------------------------- */

#include "comdef.h"
#include "customer.h"
#include "log_codes_wcdma_int.h"

/* ---------------------------- */
/* Check for necessary features */
/* ---------------------------- */


/* -----------------------------------------------------------------------
** Constant / Define Declarations
** ----------------------------------------------------------------------- */

/* Log packet with 2 additional fields for Type3 receiver */
    
/* New parameters for mdsp for Tyep3 EQ enhancement */
    
#ifndef EQ_DBG_PKT
#define EQ_DBG_PKT EQ_DBG_PKT_0x4215
#else
#error "Redefinition of the log command code: EQ_DBG_PKT"
#endif


/* HSDPA bit field mask and shift in EQ control register */
#define EQ_HSDPA_ACTIVE_BMSK      0x01
#define EQ_HSDPA_ACTIVE_SHFT      0

/* CM bit field mask and shift in EQ control register */
#define EQ_CM_ACTIVE_BMSK         0x02
#define EQ_CM_ACTIVE_SHFT         1

/* DPCH_CLTD bit field mask and shift in EQ control register */
#define EQ_DPCH_CLTD_BMSK         0x04
#define EQ_DPCH_CLTD_SHFT         2

/* DPCH_STTD bit field mask and shift in EQ control register */
#define EQ_DPCH_STTD_BMSK         0x08
#define EQ_DPCH_STTD_SHFT         3


/* RL S-CPICH bit field mask and shift in the EQ control register */
#define EQ_RL_SCPICH_BMSK         0x20
#define EQ_RL_SCPICH_SHFT         5

/* CPICH Diveristy mask and shift in the EQ control register */
#define EQ_DIV_PILOT_BMSK         0x40
#define EQ_DIV_PILOT_SHFT         6


/* Macro to extract the bit field value from the EQ control mask */
#define EQ_GET_CTRL_BF_PARAM(buffer, type) ((buffer & EQ_##type##_BMSK) >> EQ_##type##_SHFT)

/* Macro to set the bit foeld value in the EQ control mask */
#define EQ_SET_CTRL_BF_PARAM(type, value) \
  eq_ctrl_mask = ((eq_ctrl_mask & ~(EQ_##type##_BMSK)) | ((value << EQ_##type##_SHFT) & EQ_##type##_BMSK))

/* EQ start window invalid result */
#define EQ_INVALID_START_WIN      0xFFFFFFFF

/* Max Finger PN position above which there is 
   potential wraparound of EQ window */
#define EQ_PNPOS_WRAPAROUND_TAIL  300000

/* Min Finger PN position below which there is
   potential wraparound of EQ window */
#define EQ_PNPOS_WRAPAROUND_HEAD  8000

/* Action mask manipulation fields */
#define EQ_OVERRIDE                       (0xC00 & eq_nv_mask)
#define EQ_RXD_SUPPORTED                  (0x200 & eq_nv_mask)
#define EQ_DEREGISTER_CM_CB               (0x100 & eq_nv_mask)
#define EQ_REGISTER_CM_CB                 (0x080 & eq_nv_mask)
#define EQ_DEREGISTER_TD_CB               (0x040 & eq_nv_mask)
#define EQ_REGISTER_TD_CB                 (0x020 & eq_nv_mask)
#define EQ_REGISTER_WINDOW_UPDATE_CB      (0x010 & eq_nv_mask)
#define EQ_DEREGISTER_WINDOW_UPDATE_CB    (0x008 & eq_nv_mask)
#define EQ_SET_CLEANUP_BIT                (0x004 & eq_nv_mask)
#define EQ_ENABLE_NEEDED                  (0x002 & eq_nv_mask)
#define EQ_MDSP_PARAM_UPDATE_NEEDED       (0x001 & eq_nv_mask)

#define EQ_OVERRIDE_MODE                  ((eq_nv_mask & 0xC00) >> 10)

/* Macros needed for EQ+CLTD */
#define EQ_SAMP_CONST_PILOT_CLTD          0x002A0000
#define EQ_SAMP_CONST_PILOT_NON_CLTD      0x001E001E

/* EQ rake CQI offset with EQ+CLTD */
#define EQ_RAKE_CQI_OFFSET_CLTD           0xf3f33334 /* -24.1 in Q23 */ 
/* EQ rake CQI offset with no diversity */
#define EQ_RAKE_CQI_OFFSET_NON_CLTD       0xf5266667 /* -21.7 in Q23 */

/* EQ Post FFT divide Pth with no diversity */
#define EQ_POST_FFT_DIVIDE_PTH_NO_RXD     0x73

/* EQ Post FFT divide Pth with Rx diversity */

/* CQI diversity pilot/noise filt FF and FB gain for no CLTD */
#define EQ_CQI_DIV_FILT_FF_GAIN_NON_CLTD  0x333
#define EQ_CQI_DIV_FILT_FB_GAIN_NON_CLTD  0x3CCC

/* CQI diversity pilot filter FF gain */
#define EQ_CQI_DIV_PILOT_FILT_FF_GAIN_CLTD  0x63D
#define EQ_CQI_DIV_PILOT_FILT_FB_GAIN_CLTD  0x39C3
/* CQI diversity pilot filter FF gain */
#define EQ_CQI_DIV_NOISE_FILT_FF_GAIN_CLTD  0x3EF
#define EQ_CQI_DIV_NOISE_FILT_FB_GAIN_CLTD  0x3C11

/* Constants for CQI BLER alignment */

/* CQI BLER alignment constants without RxD */
#define EQ_CQI_MAP_ADJ_THRES0               0xFCC0
#define EQ_CQI_MAP_ADJ_THRES1               0xFE80
#define EQ_CQI_MAP_ADJ_THRES2               0x0
#define EQ_CQI_MAP_ADJ_THRES3               0x0100
#define EQ_CQI_MAP_ADJ_THRES4               0x7F80
#define EQ_CQI_MAP_ADJ_GRADIENT1            0x0
#define EQ_CQI_MAP_ADJ_GRADIENT2            0x0
#define EQ_CQI_MAP_ADJ_GRADIENT3            0x0

/* Enhanced EQ parameters s*/

typedef enum
{
  EQ_ONLY_RAKE_MODE = 1,
  EQ_ONLY_EQ_MODE,
  EQ_AUTO_MODE
}eq_override_enum_type;

/* Structure enumerating EQ states */
typedef enum
{
  EQ_INACTIVE,
  EQ_ACTIVE,
  EQ_SUSPEND,
  EQ_NOOP /* This state is applicable only to EQ debug packet logging */
}eq_state_enum_type;

/* EQ config parameter database */
typedef struct
{
  /* Control parameters */
  uint16 eq_enable;
  uint16 eq_override;
  uint16 eq_cleanup;

  /* Reconfigurable parameters */
  uint16 fd_est_ctl;
  uint16 phase_accum_bypass_ctrl;
  uint16 samp_ram_pn_cfg;
  uint32 cir_win_start_init;
  uint32 cqi_pwr_msr_off;
  
 /* CLTD related parameters */
  uint32 samp_pdsch_const_pilot;
  uint32 samp_scch_const_pilot;
 
  /* One time initialization */
  uint16 cir_est_filt_ff_gain;
  uint16 cir_est_filt_fb_gain;

  uint16 auto_cov_est_filt_ff_gain;
  uint16 auto_cov_est_filt_fb_gain;
  uint16 cross_cov_est_filt_ff_gain;
  uint16 cross_cov_est_filt_fb_gain;

  uint16 tt_centering_shift_thres;
  uint16 tt_move_cx2_value;
  uint32 norm_min_max_sq_tap;
  uint16 norm_temp_scaling_sq_num;

  uint16 lmmse_cir_win_filt_ff_gain;
  uint16 lmmse_cir_win_filt_fb_gain;
  uint16 lmmse_cir_noise_eng_scale_factor;
  uint32 lmmse_min_scale_noise_eng;
  uint32 lmmse_max_scale_noise_eng;
  uint16 post_fft_divide_pth;

  uint16 cqi_pilot_filt_ff_gain;
  uint16 cqi_div_pilot_filt_ff_gain;
  uint16 cqi_pilot_filt_fb_gain;
  uint16 cqi_div_pilot_filt_fb_gain;
  uint16 cqi_noise_filt_ff_gain;
  uint16 cqi_div_noise_filt_ff_gain;
  uint16 cqi_noise_filt_fb_gain;
  uint16 cqi_div_noise_filt_fb_gain;
  uint16 cqi_snr_filt_ff_gain;
  uint16 cqi_snr_filt_fb_gain;
  uint16 cqi_map_adj_thres0;
  uint16 cqi_map_adj_thres1;
  uint16 cqi_map_adj_thres2;
  uint16 cqi_map_adj_thres3;
  uint16 cqi_map_adj_thres4;
  uint16 cqi_map_adj_gradient1;
  uint16 cqi_map_adj_gradient2;
  uint16 cqi_map_adj_gradient3;
  uint16 cqi_map_adj_gradient4;
  uint16 cqi_switch_delta_max;
  uint16 hs_pdsch_beta;
  uint16 hs_scch_beta;
  uint16 cqi_switch_ctr_max;
  uint16 cqi_switch_thres;
  uint16 cqi_switch_update_mode;
  uint32 cqi_snr_cqi_off;
  uint32 cqi_eq_rake_cqi_off;
  uint32 pn_ovsf_cfg;
  uint32 samp_eq_ctl;
  uint16 reframe_delta;
  uint16 reframe_thres;
  uint16 rx_div_mode;


  /* Predictive CQI Co-efficients */
  uint16 pred_cqi_eq_coeff;
  uint16 pred_cqi_rake_coeff;
  uint16 pred_cqi_prev_rake_coeff;

}eq_params_shadow_type;

/* -----------------------------------------------------------------------
** Global Data Declarations
** ----------------------------------------------------------------------- */

/* Flag to indicate that EQ configuration is in progress */
extern boolean eq_cfg_in_progress;
/* The EQ global control mask */
extern uint8   eq_ctrl_mask;


/* EQ Logging related definitions */
/* ------------------------------ */

/* Defined in services\diag\log.c */
typedef PACKED struct PACKED_POST
{
  uint16 len;  /* Specifies the length, in bytes of 
                 the entry, including this header. */

  uint16 code; /* Specifies the log code for the 
                  entry as enumerated above.       
                  Note: This is specified as word 
                  to guarantee size.               */

  qword ts;    /* The system timestamp for the log entry. The upper 48 bits
                  represent elapsed time since 6 Jan 1980 00:00:00 
                  in 1.25 ms units. The low order 16 bits represent elapsed 
                  time since the last 1.25 ms tick in 1/32 chip units 
                  (this 16 bit counter wraps at the value 49152).          */
} eq_log_header_struct_type;

/* Number of samples in one EQ debug log packet */
#define EQ_NUM_MAX_EQ_DBG_SAMPLES     100
/* Number of EQ debug log buffers maintained by S/W */
#define EQ_LOG_NUM_BUF                2
/* Size of one EQ debug log packet */
#define EQ_DBG_LOG_SZ_PER_SAMPLE  sizeof(eq_dbg_info_struct_type)
/* Number of W16 that has to be read from mDSP for each subframe */
#define EQ_DBG_LOG_INFO_NUM_W16_PER_ENTRY  7

/* Structure defining the EQ debug info parameters */
typedef PACKED struct PACKED_POST
{
  uint16 time_tracking_n_round_bits;
  uint16 cir_energy;
  uint16 max_sq_tap;
  uint16 avg_noise_energy;
  /* Temporary fields added for Systems folks */
  uint16 reframe_ctr; 
}eq_dbg_info_struct_type;

/* EQ debug log packet */
LOG_RECORD_DEFINE(EQ_DBG_PKT)
  uint16 starting_global_sub_fn;
  uint8  num_samples;
  eq_dbg_info_struct_type info[EQ_NUM_MAX_EQ_DBG_SAMPLES];
LOG_RECORD_END

/* EQ reconfig log packet */
LOG_RECORD_DEFINE(EQ_RECFG_PKT)
  uint32 ovsf_scr_info;
  uint8  cfg_info;
  uint16 cir_start_win;
LOG_RECORD_END

/* -----------------------------------------------------------------------
** Local Data Declarations
** ----------------------------------------------------------------------- */

/* =======================================================================
**                            Function Definitions
** ======================================================================= */

/*===========================================================================
FUNCTION eq_get_cur_state

DESCRIPTION
  This function returns the current EQ S/W state. If this function is called
  even if EQ is never active, this will return EQ_INACTIVE.

DEPENDENCIES
  None

RETURN VALUE
  Current S/W EQ state

SIDE EFFECTS
  None
===========================================================================*/

extern eq_state_enum_type eq_get_cur_state(void);

/*===========================================================================
FUNCTION eq_idle_init

DESCRIPTION
  This function initializes the EQ state machine at L1 IDLE.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

extern void eq_idle_init(void);

/*===========================================================================
FUNCTION eq_init

DESCRIPTION
  This function initializes all the one time and control parameters.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

extern void eq_init(void);

/*===========================================================================
FUNCTION eq_query_hs_eq_supported

DESCRIPTION
  This function returns TRUE or FALSE depending on whether EQ is supported
  or not as per the EQ NV value. There is only one NV item (6) for which
  EQ is not supported and only Rake is used.
  
DEPENDENCIES
  EQ NV is assumed to be read before calling this function.

RETURN VALUE
  TRUE/FALSE reflection if EQ is supported or not.

SIDE EFFECTS
  None
===========================================================================*/

extern boolean eq_query_hs_eq_supported(void);



/*===========================================================================
FUNCTION eq_cm_act_deact_callback

DESCRIPTION
  This is the callback function registered with CM module. This function 
  posts a local command to process the act/deact of CM with respect to EQ. 
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

extern void eq_cm_act_deact_callback(
  /* State of CM, TRUE if any pattern is active */
  boolean cm_active);

/*===========================================================================
FUNCTION eq_td_update_callback

DESCRIPTION
  This is the callback function registered with demod module. This function 
  sets appropriate bit in the EQ control mask and posts a local command to
  execute EQ controller if EQ configuration is already not in progress.
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

extern void eq_rl_update_callback(
  /* Bit 2: CLTD, Bit 3: STTD, Bit 5: S-CPICH */
  uint8 td_status);

/*===========================================================================
FUNCTION        eq_frame_window_calc
                
FILE NAME       dltri.c

DESCRIPTION     This function calculates the start of EQ window frame only
                if HS configuration is not pending. This function is called 
                as a parameter to the EQ controller callback function.

DEPENDENCIES    None

RETURN VALUE    Start of EQ window frame in Cx2

SIDE EFFECTS    None
===========================================================================*/

extern uint32 eq_frame_window_calc(void);

/*===========================================================================
FUNCTION eq_triage_window_update_callback

DESCRIPTION
  This is the callback function registered with triage module. This function 
  sets the params changed bit and writes the window frame info to mDSP.
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

extern void eq_triage_window_update_callback(void);

/*===========================================================================
FUNCTION eq_set_pending_cmd_flag

DESCRIPTION
  Sets the EQ configuration pending flag
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

extern void eq_set_pending_cmd_flag(void);

/*===========================================================================
FUNCTION eq_controller

DESCRIPTION
  This function is the master controller of EQ. It scans the EQ current state
  and control mask to determine EQ next state. Once the current state and next
  state is known, it sets the action mask with appropriate actions to be 
  performed for this state transition. Executes the actions in the action mask.
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

extern void eq_controller(void);

/*===========================================================================
FUNCTION eq_mdsp_update_config

DESCRIPTION
  This function writes the asynchronous control and reconfigurable parameters
  to mDSP and then sends a sync command to act on these new parameters.
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

extern void eq_mdsp_update_config(void);

/*===========================================================================
FUNCTION eq_mdsp_param_update_done

DESCRIPTION
  This function is the response to the sync command sent to mDSP. This
  function is called after the sync command posted to mDSP has completed.
  This function restores the state variables and call the controller again
  if the pending flag is set.
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

extern void eq_mdsp_param_update_done(
  /* status of mDSP command */
  uint16 status);

/*===========================================================================
FUNCTION eq_read_eq_ctrl_from_nv

DESCRIPTION
  This function reads the EQ operation mode from NV
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  eq_nv_value is updated with the calue read from NV.
===========================================================================*/

/* Reads EQ operation mode from NV */
extern void eq_read_eq_ctrl_from_nv( void );

/*===========================================================================
FUNCTION eq_handle_override_cmd

DESCRIPTION
  This function sets the EQ override mode as per the value passed in as a
  parameter. It then calls the eq_controller to set effect to the EQ
  override. Because the override mode does not change EQ enable/disable 
  functionality, a flag "eq_override_test_command" is set which makes sure
  that eqParamsCleanup bit is not set while sending the config command to mDSP.
    
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  eq_nv_mask updated with the new override mode
===========================================================================*/

extern void eq_handle_override_cmd(uint8 oride_val);

/*===========================================================================
FUNCTION eq_generate_recfg_log_pkt

DESCRIPTION
  This function generates the EQ reconfiguration log packet. This function is 
  called as a part of the post callback function of the EQ config mDSP sync
  command.
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

extern void eq_generate_recfg_log_pkt(void);

/*===========================================================================
FUNCTION eq_logging_init

DESCRIPTION
  This function initializes the state variables used in generating the EQ
  debug log packet. This function is called from eq_init at the time of 
  UE boot up.
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  All EQ debug log packet state variables reset
===========================================================================*/

extern void eq_logging_init(void);

/*===========================================================================
FUNCTION eq_log_prepare_action

DESCRIPTION
  This function prepares the logging action that is to be executed after there
  has potentially been a change in the EQ state. This function is called 
  just before the EQ config sync command is sent to mDSP.
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  EQ logging variables set to take on the new EQ action.
===========================================================================*/

extern boolean eq_log_prepare_action(
  /* Action for HS UL accumulation and logging */
  eq_state_enum_type action,
  /* EQ start CFN */
  uint8 start_cfn);

/*===========================================================================
FUNCTION eq_init_log_buffer

DESCRIPTION
  This function prepares the EQ debug logging buffer and sets the header in 
  the buffer. Also it updates the EQ debug available buffers.
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  EQ debug available buffers updated.
===========================================================================*/

extern void eq_init_log_buffer(
  /* The current CFN at which logging starts */
  uint8 this_cfn);

/*===========================================================================
FUNCTION eq_update_logging

DESCRIPTION
  This function is called every frame as a part of HDR read event and updates 
  the EQ debug log packet accumulation. It scans for the last 5 subframes in 
  the previous frames and populates the accumulation log buffer. It scans for
  the action type to be stop, and if so it stops the logging and flushes out
  the buffer at the correct subframe. At the moment when the log buffer has 
  reached its maximum accumulation capability it flushes the log buffer to 
  DIAG and prepares a new log buffer for accumulation.
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

extern void eq_update_logging(void);

/*===========================================================================
FUNCTION eq_dbg_send_log_submit_cmd

DESCRIPTION
  This function checks if there has been a previous log buffer submitted to 
  DIAG thathas not been processed and if so, it flags error and returns.
  Otherwise it posts a L1 local command to submit the EQ debug log buffer
  to DIAG 
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  eq_log_submit is modified and the index of the log buffer to be submitted
  is stored in eq_dbg_log_buf_idx_to_submit.
===========================================================================*/

extern void eq_dbg_send_log_submit_cmd(
  /* EQ debug log buffer index to submit */
  uint8 log_buf_idx);

/*===========================================================================
FUNCTION eq_submit_log_buffer

DESCRIPTION
  This function submits the log buffer to DIAG and updates the EQ debug 
  logging variables depending on what buffer was submitted and if logging is 
  unmasked in QXDM 
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  EQ debug logging variables updated.
===========================================================================*/

extern void eq_submit_log_buffer(void);

#endif
