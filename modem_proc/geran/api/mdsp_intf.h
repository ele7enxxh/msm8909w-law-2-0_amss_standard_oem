#ifndef MDSP_INTF_H
#define MDSP_INTF_H

/*===========================================================================
                          M D S P _ I N T F . H


GENERAL DESCRIPTION
  This header file contains macros and funtion prototypes for general
  utility functionality for the mDSP.
  It is intended for external clients to include.


Copyright (c) 2000-2013 Qualcomm Technologies, Inc.

===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.


when         who     what, where, why
----------  -------- ------------------------------------------------------
01/05/14    zc     CR658120 GSM ASDIV NV read only done once
14/01/14    jj       CR 600114 tx- diversity in dime SGLTE part-2
10/12/13    ws       CR 587586 - Remove DUAL_SIM featurisation in API
04/10/13    ws       CR 570719 - Correct MDSP sleep external API for TDS L1
31/10/13    cgc      CR569801  Add wrapper function with mdsp_cm_set_ftsm()
06/08/13    ws       CR521868: Introduce API mdsp_rf_set_iq_log() to enable IQ logging on 2nd RF chain
03/09/13    npt      CR524770 - GL1 changes to support RFLM FED
10/07/13    ws       CR 510980 Implement external API gl1_cm_mdsp_ready_for_sleep()
06/08/13    ws       CR521868: Introduce API mdsp_rf_set_iq_log() to enable IQ logging on 2nd RF chain
22/02/13    cja      CR455527 Remove featurisation from files in API directory
28/03/13    jk       CR452710: Changes to support continuous IQ logging and GSM XO calibration
20/02/13    cja      CR455032 Changes for build without TD feature
23/01/13    jk       CR427351: passing pointer to the thermistor read CCS event buffer to RF
22/01/13    cja      CR443834 Extend mdsp_awake to indicate FW enable/disable
10/12/12    cja      CR411308 Dime T2G changes (msg router to GFW)
05/12/12    pg       CR427361: GFW/RF shared memory interface cleanup
26/11/12    ws       CR424198 - API backward compatible with TRITON DSDA changes
05/10/12    jj       CR407050 - mdsp_load_nv()  is not called in   GL1
31/07/12    ky       Fixed the compilation issues after removing the redundant strcutures
18/10/12    pg       CR412013: HMSC to use standard power measurement length
16/10/12    ws       CR409719 - DIME/TRITON FTM API compatability support
19/07/12    tc       CR380260 Gl1 interface update for MOD indicator
03/07/12    zf       CR341842: Opt19: Increase power monitor number to 5 per PCH frame.
29/06/12    jj       CR374950 GL1 changes for 38 symbol HMSC monitor
31/07/12    ky       Fixed the compilation issues after removing the redundant strcutures
                     in SW INTF.h file
20/06/12    ky       Added support of using new CCS interface and passing new CCS
                     buffer pointers in RF task messages
08/06/12    cja     CR363269 Do not clear IRAT SBI/NonSBI buffers
25/05/12    pg       Add support for RF Task
24/05/12    ky       Fixed compilation issue
23/05/12    jj      CR364221 remove FEATURE_GSM_TX_PCL
09/05/12    jj      CR330315 compile error correction for FEATURE_GSM_TX_PCL
25/04/12    ws      CR 355838 FR 1604 - GERAN changes to cleanup WCDMA- GL1 API
28/02/12    cja     CR338055 Recovery for Nikel RxFE Stuck
09/02/12    cja      CR333023 Send IR configure command from ISR not task
02/02/12    cja      CR329327 Ensure mdsp init complete before using command buffer
02/02/12    cja      CR328278 Use semaphore in MDSP command buffer header
30/01/12    rc      GL1 QPA new shared variables
27/01/12    ky      Added mechanism to disable/enable vamos in firmware
23/01/12    ky      vamos changes
11-02-16    tjw     Internal content split out from the original GERAN file
                    previously used by GERAN clients.
========================================================================== */

#ifndef CUSTOMER_H
  #include "customer.h"
#endif
#ifndef COMDEF_H
  #include "comdef.h"
#endif

#include "gfw_sw_intf.h"
#include "geran_dual_sim.h"
#include "sys.h"

#define MAX_CM_RSSI_MEASUREMENTS_ARRAY_SIZE    MDSP_MAX_STANDALONE_RSSI_MEASUREMENTS

/****************************************************************************
                   mDSP   DEFINITIONS  and  ENUMERATIONS
 ***************************************************************************/

/* SBI control define required by wtr RF Driver*/
#define SBI_MDSP_CTL_FIFO_IMM   ( 0 )

/*--------------------------*/
/*  Linear/Polar PA defines */
/*--------------------------*/
#define MDSP_LINEAR_PA  0xFFFF
#define MDSP_POLAR_PA   0

/* Values for the modulation bit in the egprs_misc field */
#define MDSP_MODULATION_8PSK       0
#define MDSP_MODULATION_GMSK       1

/* Burst type enum.  This is used to select which SBI/non-SBI buffer to use.
   Note many types are overlayed to save host interface segment memory,
   thus care must be taken that these burst types are not issued in the same
   frame.  Be careful when changing any of these as the overlaps have been
   carefully chosen to avoid such conflicts. */
typedef enum
{
   RX_BURST             = 0,
   TX_BURST             = 1,
   MON_BURST_1          = 2,
   MON_BURST_2          = 3,
   MON_BURST_3          = 4,
   STOP_ACQ_BURST       = 5,
   MON_BURST_4          = TX_BURST,
   MON_BURST_5          = STOP_ACQ_BURST,
   MON_BURST_6          = RX_BURST,
   START_ACQ_BURST      = MON_BURST_1,
   ASYNC_RX_BURST       = MON_BURST_2,
   MON_BURST_7          = 6,
   CM_MON_BURST_1       = MON_BURST_1,
   CM_MON_BURST_2       = MON_BURST_2,
   CM_MON_BURST_3       = MON_BURST_3,
   CM_MON_BURST_4       = MON_BURST_4,
   CM_MON_BURST_5       = MON_BURST_7,
   CM_MON_BURST_6       = 7,
   CM_MON_BURST_7       = 8,
   CM_MON_BURST_8       = 9,
   CM_MON_BURST_9       = STOP_ACQ_BURST,
   CM_MON_BURST_10      = 10,
   CM_STARTUP_BURST     = RX_BURST,
   CM_CLEANUP_BURST     = 11,
   WCDMA_RF_ON_BURST    = 12,
   WCDMA_RF_OFF_BURST   = 13,
   /* In LCU we had to add a new dummy SBI buffer WCDMA_DPLL_BURST */
   /* to fix a HW  bug such that the  dummy SBI buffer  allows the */
   /* clock to be on for  the  mDSP  accessing  certain registers. */
   WCDMA_DPLL_BURST       = 14,
   CM_STARTUP_BURST2      = 15,
   CM_CLEANUP_BURST2      = 16,
   CM_STARTUP_BURST1      = 17,
   CM_CLEANUP_BURST1      = 18,
   G2X_STARTUP_BURST      = 19,
   G2X_CLEANUP_BURST      = 20,

/*######################################*/
/* WARNING: If you add a burst type you */
/* must update                          */
/* sema check in mdsp_check_semaphores. */
/*######################################*/

/* Define the number of burst types */
/* Defined regardless of FEATURE_GSM_GPRS_QDSP6 (in which case
   MDSP_NUM_BURST_TYPES is already defined) */
   API_MDSP_NUM_BURST_TYPES

} mdsp_burst_type;

#define MAX_NMBR_DESENSE_CHNL_PER_BAND 25
#define LAST_ELEMNT_IN_CHNL_LIST 0xffff

typedef struct
{
uint16 G850_chan_list [MAX_NMBR_DESENSE_CHNL_PER_BAND];
uint16 G900_chan_list [MAX_NMBR_DESENSE_CHNL_PER_BAND];
uint16 G1800_chan_list[MAX_NMBR_DESENSE_CHNL_PER_BAND];
uint16 G1900_chan_list[MAX_NMBR_DESENSE_CHNL_PER_BAND];
} mdsp_xo_desense_arfcn_struct;

/* Definitions for the PA Lookup Table */
#define MAX_RAMPON_ENTRIES           30  /* 30 entries or less  */
#define MAX_RAMPOFF_ENTRIES          30  /* 30 entries or less  */

typedef struct
{
  uint8    step_size;                      /* duration of rampup/rampdown steps   */
  int16    rampon[MAX_RAMPON_ENTRIES+1];   /* Ramp up entries + "end flag 0xffff" */
  int16    rampoff[MAX_RAMPOFF_ENTRIES+1]; /* Ramp up entries + "end flag 0xffff" */
} mdsp_pa_lut_ramp_type;

typedef struct
{
  int8                    used_comp_val;
  mdsp_pa_lut_ramp_type   pa_ramp;
} mdsp_pa_ramp_type;

/* Structure to hold the number of SBI and nonSBI per step */
typedef PACKED struct
{
   uint16            num_sbi;
   uint8             num_nonsbi;
   uint64            activate_grfc_mask;
   uint64            deactivate_grfc_mask;
} mdsp_ftm_tx_cal_grfc_type;

/* Structure to hold the number of steps per GSM frame */
typedef PACKED struct
{
  uint32                     step_duration;   /* The width of each step.  May need to be variable */
  boolean                    last_tx_cal_cmd; /* last command indicator */
  uint32                     start_offset;
  uint32                     num_steps;     /* number of steps */
  mdsp_ftm_tx_cal_grfc_type  *steps_buffer; /* pointer that holds the sbi/grfc
buffer for number of steps */
  boolean                    first_tx_cal_cmd;
  GfwTxCalDmaData64BitType   *dma_data;
  uint16                     dma_length;
  boolean                    modulation_type;
  int16                      modulator_start_time_offset_adj;
} mdsp_ftm_tx_cal_struct;

/* AM/AM and AM/PM tables for the polar RF path */

typedef struct
{
   uint16 am_am_table[4][MDSP_AM_AM_TABLE_ENTRIES];  /* (MDSP_AM_AM_TABLE_ENTRIES in gfw_sw_intf.h) */
   uint16 am_pm_table[MDSP_AM_PM_TABLE_ENTRIES];
} mdsp_amam_ampm_tables_type;

#define MDSP_PWR_MEAS_LEN  (48)

#define G2X_STARTUP_BURST_IDX    0
#define G2X_CLEANUP_BURST_IDX    1
/****************************************************************************
                            FUNCTION PROTOTYPES
 ***************************************************************************/

 /*===========================================================================

FUNCTION mdsp_ftm_issue_qdsp6_command

DESCRIPTION
  Sends GFW command from rf driver ftm code

===========================================================================*/

void mdsp_ftm_issue_qdsp6_command (GfwCmdBufType *cmd, GfwMdspCmdType  cmdId , sys_modem_as_id_e_type as_id );


/* ---------- Sleep functions ---------- */
/*===========================================================================

FUNCTION mdsp_gsm_drv_go_to_sleep, mdsp_gsm_drv_wakeup

DESCRIPTION
  These functions are similar to mdsp_go_to_sleep, mdsp_wakeup, but they should
  be called when we are in WCDMA mode

===========================================================================*/
void mdsp_gsm_drv_go_to_sleep( void );
void mdsp_gsm_drv_wakeup( void );

/*===========================================================================

FUNCTION mdsp_set_iq_log

DESCRIPTION
  This function is called by rf to set fw flag for iq samples

============================================================================*/
void mdsp_set_iq_log  ( boolean value);

/*===========================================================================


DESCRIPTION
  This function is called by RF drivers to set fw flag for iq samples in
  multisim targets

============================================================================*/
void mdsp_rf_set_iq_log(boolean value,sys_modem_as_id_e_type as_id);

/*===========================================================================

FUNCTION mdsp_send_iq_samples_to_rf

DESCRIPTION
  This function sends iq samples to rf

============================================================================*/
void mdsp_rf_send_iq_samples_to_rf(sys_modem_as_id_e_type as_id );


void mdsp_ftm_cfg_pa_profile_polar
 (
   mdsp_pa_ramp_type *ramp_tbl,
   sys_modem_as_id_e_type as_id
 );
/*===========================================================================

FUNCTION mdsp_ftm_update_amam_ampm_tables

DESCRIPTION
  This functions flags the AM/AM and AM/PM tables as having changed.

===========================================================================*/
void mdsp_ftm_update_amam_ampm_tables(boolean new_am_am_table, boolean new_am_pm_table,sys_modem_as_id_e_type as_id);

/*===========================================================================

FUNCTION mdsp_ftm_cfg_amam_ampm_tables

DESCRIPTION
  This functions configures the AM/AM and AM/PM tables for uplink
  power control.  It should be called once to set the pointer to the
  table location.

===========================================================================*/
void mdsp_ftm_cfg_amam_ampm_tables(mdsp_amam_ampm_tables_type *amam_ampm_tables,sys_modem_as_id_e_type as_id);


/*===========================================================================

FUNCTION mdsp_ftm_program_cal_amam_table

DESCRIPTION
  This function copies AM/PM tables into GFW shared interface during
  pre-distortion Calibration

===========================================================================*/
void mdsp_ftm_program_cal_amam_table (uint16* am_am_tbl, uint16* am_pm_tbl , sys_modem_as_id_e_type as_id );

/*===========================================================================

FUNCTION mdsp_ftm_set_tx_burst_offset

DESCRIPTION
  These functions set and set the offset of the Tx burst from its true
  slot 3 offset.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void mdsp_ftm_set_tx_burst_offset (uint16 tx_burst_offset,sys_modem_as_id_e_type as_id);

/*===========================================================================

FUNCTION mdsp_setup_linear_pa

DESCRIPTION  This is called by the RF driver when GSM is activated. This
             sets up the Config2 initialisation to the mDSP.

===========================================================================*/
void mdsp_setup_linear_pa ( uint16 tx_cfg2_flag_val, sys_modem_as_id_e_type as_id );

/*===========================================================================

FUNCTION mdsp_xo_desense_arfcn

DESCRIPTION
  This function called by rf to update arfcn list rquired Dc spur removal

===========================================================================*/
void  mdsp_xo_desense_arfcn (mdsp_xo_desense_arfcn_struct   *pointer, sys_modem_as_id_e_type as_id);

/*===========================================================================

FUNCTION mdsp_rf_xo_desense_arfcn

DESCRIPTION
  This function called by rf to update arfcn list rquired Dc spur removal

===========================================================================*/
void  mdsp_rf_xo_desense_arfcn (mdsp_xo_desense_arfcn_struct   *pointer,sys_modem_as_id_e_type as_id);

/*===========================================================================

FUNCTION mdsp_ftm_start_tx_data_cont

DESCRIPTION
  This function tells the mDSP to transmit user defined data.
  Explanation:
  This function is for KV calibration in FTM mode by sending
  training data to reduce calibration time. It is similar to
  dynamic KV  calibration in online mode.  Instead of having
  G FW hard coding the training data as in online mode,  FTM
  code will pass training data  (user define)  to  FW during
  calibration.  Also,  we  don’t want G  FW to hard code the
  training  data in  continuous  TX.  This avoids disrupting
  RF calibration due to continuous TX being used in multiple
  target calibration.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void mdsp_ftm_start_tx_data_cont ( uint16 *buffer, uint16 buf_len,sys_modem_as_id_e_type as_id );

/*===========================================================================

FUNCTION mdsp_ftm_start_tx_rnd_cont

DESCRIPTION
  This function tells the mDSP to transmit a tone every frame with random data.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void mdsp_ftm_start_tx_rnd_cont ( uint8 gmsk_mod,sys_modem_as_id_e_type as_id );

/*===========================================================================

FUNCTION mdsp_start_tx_rnd_cont

DESCRIPTION
  This function tells the mDSP to transmit a tone every frame with random data.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

/*===========================================================================

FUNCTION mdsp_ftm_start_tx_tone_cont

DESCRIPTION
  This function tells the mDSP to transmit a tone every frame.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void mdsp_ftm_start_tx_tone_cont ( sys_modem_as_id_e_type as_id );

/*===========================================================================

FUNCTION mdsp_ftm_stop_tx_tone_cont

DESCRIPTION
  This function tells the mDSP to stop transmitting a tone every frame.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void mdsp_ftm_stop_tx_tone_cont ( sys_modem_as_id_e_type as_id );

/*===========================================================================

FUNCTION mdsp_ftm_wait_for_frame_tick

DESCRIPTION
  This function will not return until just after the next frame tick.

DEPENDENCIES
  mdsp_switch_command_buffers() must be installed as a GSTMR interrupt
  handler or this function will not return at all.

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void mdsp_ftm_wait_for_frame_tick (sys_modem_as_id_e_type as_id);


/*===========================================================================

FUNCTION mdsp_cm_set_ftsm

DESCRIPTION
  Wrapper function for mdsp_set_ftsm()

DEPENDENCIES

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void mdsp_cm_set_ftsm (uint16 ftsm, sys_modem_as_id_e_type as_id);


/*===========================================================================

FUNCTION mdsp_wakeup

DESCRIPTION
  This changes the awake state of the mDSP drivers.  When the
  drivers are awake the function mdsp_switch_command buffers will read
  from the mDSP shared memory on every frame tick.  When they are asleep
  this does not happen.  The mDSP drivers must be put in the asleep state
  (by calling mdsp_go_to_sleep) before the mDSP clk regime is disabled.

  Depending on feature definitions these functions my also vote for the
  mDSP clock regime with mdsp services.

  We do not display error messages directly in this function as it may
  disturb the sleep timeline.

===========================================================================*/
boolean mdsp_wakeup_geran(boolean comp_mode, sys_modem_as_id_e_type as_id);

boolean mdsp_wakeup(boolean comp_mode
#ifdef FEATURE_DUAL_SIM
                    , sys_modem_as_id_e_type as_id
#endif /*FEATURE_DUAL_SIM*/
                    );
/*===========================================================================

FUNCTION mdsp_ftm_prepare_for_sleep

DESCRIPTION
  Function called to check that the firmware is ready to go to sleep, and
  if so put it to sleep.  Does not disable clocks, this is done by calling
  mdsp_go_to_sleep.
  We do not display error messages directly in this function as it may
  disturb the sleep timeline.

===========================================================================*/
boolean mdsp_ftm_prepare_for_sleep ( sys_modem_as_id_e_type as_id );

/*============================================================================

FUNCTION MDSP_COLLECT_IQ_SAMPLES_CB_T

DESCRIPTION
  Callback function type to provide IQ samples retrieved from
  GERAN FW.

RETURN VALUE
  None

============================================================================*/

typedef void (*mdsp_collect_iq_samples_cb_t)
(
  GfwIqSamplesBuffer* buffer
);

/*============================================================================

FUNCTION MDSP_REGISTER_IQ_SAMPLES_CALLBACK

DESCRIPTION
  Function to register for callbacks.

  Calling this API with a valid callback function pointer signifies
  start of IQ capture. In this case Firmware is sent a command to start
  IQ capture.

  Calling this API with a NULL pointer signifies stoppage of IQ capture.
  In this case Firmware is sent a command to stop IQ capture.

RETURN VALUE
  TRUE  - Success.
  FALSE - Failure

============================================================================*/
boolean mdsp_register_iq_samples_callback ( mdsp_collect_iq_samples_cb_t callback );

/*===========================================================================

FUNCTION mdsp_set_iq_log_enable_flag

DESCRIPTION
  Enables/disables iq_log_enable flag.

============================================================================*/
void mdsp_set_iq_log_flag( boolean value);

/*===========================================================================

FUNCTION gl1_cm_mdsp_ready_for_sleep

DESCRIPTION
  External function called to check that the firmware is ready to go to sleep.

===========================================================================*/
boolean gl1_cm_mdsp_ready_for_sleep(sys_modem_as_id_e_type as_id);

/*===========================================================================

FUNCTION gl1_cm_mdsp_prepare_for_sleep

DESCRIPTION
  External function called to check that the firmware is ready to go to sleep.

===========================================================================*/
boolean gl1_cm_mdsp_prepare_for_sleep(sys_modem_as_id_e_type as_id);

void antenna_swth_get_nv_item_qsc (void);

#endif /* MDSP_INTF_H */
