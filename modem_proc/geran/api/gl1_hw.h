#ifndef GL1_HW_H
#define GL1_HW_H

/*========================================================================
                      GSM GPRS NPL1 FRAME LAYER HEADER FILE
DESCRIPTION
   This file defines the interfaces to the GSM/GPRS NPL1 FRAME LAYER.
   The GSM/GPRS NPL1 frame layer abstracts all hardware concepts. The frame
   layer functions typically need to be called periodically every frame.

Copyright (c) 2001-2013 Qualcomm Technologies, Inc.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government. Diversion contrary to U.S. law prohibited.
========================================================================== */

/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/api/gl1_hw.h#1 $ $DateTime: 2016/12/13 08:00:21 $ $Author: mplcsds1 $

when        who     what, where, why
--------    ---     --------------------------------------------------------
28/03/16    js      CR951664: FR 29530: 1X+G QTA 
18/05/15    jj      CR 839611 New skip last measurement in T2G if command is late 
09/03/15    dg      CR800014 During first attempt of NCELL SCH decode, ignore SCH data if metrics is not available
02/12/14   cja      CR760267 For X2G SCH decode structure make sch_data an array and add metrics
07/11/14    cja     CR751049 Add cm FCCH and SCH results API (ready for parallel FCCH/SCH)
27/10/14    js      CR746020 Macro for W to start next irat activity after 14ms
27/10/14    cja     CR746760 In X2G power measure tie results buffer index to state machine
20/05/14   dg       CR613013 Long Term fix on GL1 for TS 34.123 & TC 8.3.11.12 GCF Testcase
                    failure due to RACH failure on GSM cell
07/08/14    og      CR706449. Fix compilation errors when dual sim is not defined.
04/08/14    cja     CR697147 Add target RF device to gl1_hw_cm_rf_info_t
28/07/14    zf      CR700730: Removing RXLM buf idx in set GFW app mode
18/06/14    cos     CR682031 - GL1 control algorithm and changes for supporting AEQ PHASE 4
03/07/14    og      CR652720. L2G DSDA bring-up updates.
27/06/14    ss      CR686989 Add HDR support to gl1_hw_qta_startup API
09/06/14    pg      CR676887 Add RF_sub_id to as_id mapping API
15/05/14    cja     CR656165 Add as_id parameter to API for CM gap complete
12/05/14    cja     CR662836 Add support for qta_cleanup_pending to Bolt.
02/05/14    cja     CR658621 Optimise QTA and T2G F3. 
28/04/14    cja     CR656165 Add external API for CM gap complete
14/04/14    ss      CR646261 FR GSM CM INIT via message in GL1 task context
14/03/14    pa      CR631907:Bolt: Fixed compilation errors on enabling FR_17618 feature.
13/01/13    am      CR592893 Cap power measurement result on spur channels
10/03/13    ka      CR628885 Remove unused functions
19/02/14    am      CR604674 Take spur into account for T2G acquisition
20/01/14    jk      CR503359 - GL1 changes to support Rx diversity
13/12/13    cja     CR589473 Add setting GfwAppModeType in GfwHostRfmEnterModeCmd
21/08/13     dv     CR532560 Improve T2G SCH performance
04/06/13    cja     CR481423 New RF IRAT API to add RF ID
12/01/14    jk       CR572347 - GL1 FTM API changes to support Rx diversity
08/01/14    us       CR562238 Partial T2G gap usage for RSSI measurement
10/12/13    ws      CR 587586 - Remove DUAL_SIM featurisation in API
05/11/13    cja     CR570704 Add T2G RF pre-building startup/cleanup scripts
31/10/13    cgc     CR569801  Add wrapper functions gl1_cm_mutex_lock() gl1_cm_mutex_unlock() gl1_cm_hw_mcpm_restore_cb()
01/10/13    ws      CR547477  WCDMA DBDC W2G support
21/08/13    dv      CR532560 Improve T2G SCH performance
04/06/13    cja     CR481423 New RF IRAT API to add RF ID
22/02/13    cja     CR455527 Remove featurisation from files in API directory
14/04/13    pjr     CR475908 dual data spaced firmware queue
13/04/13    mc      CR471759 DSDS: QTA Support : Initial code changes & Triton Bring-Up
11/03/13    pjr     CR461202 - Removed #include "tcxomgr.h"
10/03/13    ka      CR628885 Remove unused functions
07/03/13    pg      CR460577: further IRAT I/F updates for support of LTE Carrier Aggregation
14/02/13    pg      CR452994: IRAT I/F updates for support of LTE Carrier Aggregation
05/12/12    pg      CR427361: GFW/RF shared memory interface cleanup
26/11/12    ws      CR424198 - API backward compatible with TRITON DSDA changes
16/10/12    ws      CR409719 - DIME/TRITON FTM API compatability support
24/09/12    og      CR402775.T2G BSIC triple buffered.
04/09/12    og      CR395649. T2G Acquisition and BSIC decode enhancements.
04/09/12    og      CR391191. Prevent T2G RSSI 0 results.
11/07/12    cja     CR370714 Replace CM Acq RF timing definitions with RF API
02/08/12    ky      Exporting the gl1 functions to FTM code
25/07/12    ky      Moving the message router initialisation for blocking messages to
                    l1 task initialisations
03/07/12    ky      Async message router sleep, app mode, enter mode interface changes
25/05/12    pg      Add support for RF Task
18/06/12    og      CR371571. Provide a zero QS SCH uncertainty window to TL1.
21/05/12    og      CR361191.
25/04/12    ws      CR 355838 FR 1604 - GERAN changes to cleanup WCDMA- GL1 API
19/04/12    cja     CR349237 Add API for FTM to reset edge core
10/04/12    og      CR350683. Provide x2g cleanup error info to TL1.
30/03/12    cja     CR346018 RxFE recovery in FTM
21/03/12    jj      CR340963 Avoid scheduling g2x gap when frame is slammed
10/02/12    ws      CR 336194 Enable TD-SCDMA IRAT for GERAN
03/02/12    og      CR323669. Provide a #define for the FCCH/SCH rssi threshold value.
22/09/11    cja     CR307155  Replace RF timing definitions with RF API
21/09/11    cja     CR303973 Add 70QS cleanup overhead for Nikel.
20/09/11    cja     CR307007 Set acq XO value in CM startup.
08/09/11    jj      CR302378  Added GL1 changes for RSB along with spectral inversion
26/08/11    og      Port over CR286561.
16/08/11    jj      CR 299789 Reduced the pwr meas length to 48 symbol
16/08/11    cja     CR301307 Changes following code review.
15/06/11    cja     Fix compiler warning
08/06/11    cja     Nikel G2W/W2G changes (so WFW does G2W startup, W2G cleanup)
20/05/11    cja     Nikel add GL1_HW_INIT_NON_SIGNALLING for GPLT and FTM
09/05/11    cja     Nikel add RF API for timing info
28/04/11    cja     Add Nikel freq control by GFW
20/04/11    cja     Nikel add spectral inversion bit
17/03/11    cja     CR279518 Add single slot FTM power store for GPRS/EDGE
05/04/11    cja     L1 I/F change for Nikel IRAT
15/03/11    cja     Add initial changes for Nikel IRAT
27/01/11    cja     Changes for RxLM and TxLM for NikeL
11-02-16    tjw     API content split out from the original GERAN internal file
                    previously used by GERAN clients.
========================================================================== */

#ifndef CUSTOMER_H
  #include "customer.h"
#endif

#ifndef COMDEF_H
  #include "comdef.h"
#endif

#include "rex.h"

/* rfm.h is required by TDSCDMA, not needed by GERAN.
   A shame as it includes rfm_types.h and that is included over 2000 times. */
#include "rfm.h"
#include "rfmeas_types.h"
#include "rfm_hdr_agc_types.h"
#include "rfmeas_wcdma.h"
#include "rfmeas_hdr.h"
#include "ftm.h"

#include "gl1_defs.h"
#include "msgr_geran.h"
#include "mdsp_intf.h"

#include "msgr.h"
#include "geran_msg.h"

#include "geran_dual_sim.h"
#include "lte_irat_types.h"


/* -----------------------------------------------------------------------
** Constant / Define Declarations
** ----------------------------------------------------------------------- */

/* Search width in symbols while camped on the TCH */
/* The DSP uses this constant and does a search */
/* indexed from 0 to SEARCH_WIDTH,  which means */
/* that mDSP uses 13 in Idle and Dedicated mode */
#define GL1_HW_TCH_SEARCH_WIDTH           (12)

/* Search width in symbols while in GPRS mode */
#define GL1_HW_GPRS_SEARCH_WIDTH          (10)

/* Default SCH async search width */
#define GL1_HW_SCH_SEARCH_WIDTH_DEFAULT   (80)
/* Default SCH async search width for T2G in symbols*/
#define GL1_HW_T2G_SCH_SEARCH_WIDTH_DEFAULT  (60)

#define GL1_DEFS_MAX_ASSIGNED_UL_TS  (4)

#define GL1_MAX_SCH_OCTETS (4)

extern void gl1_hw_mcpm_call_config_cmd(gas_id_t gas_id);
extern boolean g1l_hw_is_mcpm_tx_on (gas_id_t gas_id);
extern boolean gl1_hw_check_metrics_done(gas_id_t gas_id);

/*Variable checks if SET TX BAND command is issued to GFW*/
extern boolean gl1_set_tx_band_cmd_iss_to_gfw[];
/*Flag specifies that RF_TX_ENABLE_CNF is received from RF*/
extern boolean l1_rf_tx_enable_cnf_rcv[];

/* -----------------------------------------------------------------------
**                      Compressed Mode Declarations
** ----------------------------------------------------------------------- */

typedef enum
{
  GL1_HW_ONE_BURST_IN_GAP = 0,
  GL1_HW_FIRST_BURST_IN_GAP,
  GL1_HW_MIDDLE_BURST_IN_GAP,
  GL1_HW_LAST_BURST_IN_GAP
} gl1_hw_burst_position_type;

typedef rfcom_mode_enum_type  gl1_hw_cm_rat_t;

typedef rf_card_band_type     gl1_hw_cm_scell_rf_band_t;

typedef rfcom_lte_bw_type     gl1_hw_cm_scell_bw_t;

typedef struct
{
  gl1_hw_cm_rat_t            serving_rat;
  gl1_hw_cm_scell_rf_band_t  serving_band;
  uint16                     serving_chan;
  uint8                      startup_buf_idx;
  uint8                      cleanup_buf_idx;
  gl1_hw_cm_scell_bw_t       bw;
  uint32                     rxlm_gsm_buf_idx;
  uint32                     rxlm_cleanup_buf_idx;

  uint32                     num_source_item;
  rfm_meas_param_type        source_param[MAX_SOURCE_ITEM];
  rfm_device_enum_type       target_rf_device;

    union
  {
    rfm_meas_1x_setup_and_build_scripts_param_type  onex_tune_parm;
    rfm_meas_hdr_setup_and_build_scripts_param_type hdr_tune_params;
    rfm_meas_wcdma_setup_and_build_scripts_param_type w_tune_params;
    lte_irat_l2x_tuneaway_param_s l2x_tuneaway_param;
  } u;
  /*
  **  to be removed after ML1 support for union api has been added
  */
  lte_irat_l2x_tuneaway_param_s l2x_tuneaway_param;
} gl1_hw_cm_rf_info_t;

typedef struct
{
   uint16  id;
   int32   tone_offset;
   int32   coarse_freq;
   int32   fine_freq;
   int32   afc_freq;
   uint16  snr;
   boolean sch_decoded;
} gl1_hw_cm_tone_results_t;

typedef struct
{
   uint16 id;
   uint8  crc_pass;
   boolean metrics_valid;
   uint8  sch_data[GL1_MAX_SCH_OCTETS];
   int32  acq_time_offset;
   int32  freq_offset;
   int16  sch_timing_offset;
   uint16 snr;
   dBx16_T pwr_dBm_x16;
} gl1_hw_cm_sch_results_t;


/* Flags for controlling RxDiversity in burst level */
#define ENABLE_RXDIVERSITY    1
#define ENABLE_DIVERSITY_RX   2
#define ENABLE_PRIMARY_RX     4
#define ENABLE_IMRD           8
typedef struct
{
  uint8 enableRxd   :1; //Enable Rx Diversity
  uint8 enableDrx   :1; //Use Drx
  uint8 enablePrx   :1; //Use Prx
  uint8 enableIMRD  :1; //Enable Rx Diversity for Idle (IMRD)
  uint8 unused      :4;
}gl1_rxd_control_type;

/* FEATURE_GSM_RX_DIVERSITY_PDT  || FEATURE_GSM_RX_DIVERSITY_IDLE */ 
typedef enum
{
   GL1_RXD_CH_UNDEFINED,
   GL1_RXD_CS_CHANNEL,
   GL1_RXD_PS_CHANNEL,
   GL1_RXD_IDLE_CHANNEL,
   GL1_UL_OR_RRBP_CHANNEL
/* temporary debug channels for monitoring and logging purposes */
   ,GL1_RXD_SCH
   ,GL1_RXD_RACH_PRACH
/* end of temp channels */
} gl1_rxd_channel_type;
/* FEATURE_GSM_RX_DIVERSITY_PDT  || FEATURE_GSM_RX_DIVERSITY_IDLE */ 

/* Bitmask used to update mdsp_aeq4_ctl */
#define AEQ4_UPDATE_NV				0x1		//updates control_nv	
#define AEQ4_ENABLED 				0x2		//updates enableAeq4
#define AEQ4_SACCH_CNT				0x4		//updates sacch_cnt
#define AEQ4_ENABLED_AND_SACCH_CNT	0x6		//updates sacch_cnt and enableAeq4

typedef struct
{
  boolean control_nv;		
  boolean enableAeq4;		
  uint8   sacch_cnt;		
} gl1_hw_dtm_rx_aeq4_type;

/* the minimum received RSSI value that is considered acceptable to attempt
   acquiring a GSM  cell (in 1/16th db units).  This should match the value of
   SYS_PARAM_MIN_ACCEPTABLE_RX_LVL_dBm_x16 in gl1_sys_params.h */

#define GL1_HW_MIN_ACCEPTABLE_RX_LVL_dBm_x16  (-107*16)
/* systems suggested to put threshold of -101dBm on spur channels */
#define GL1_HW_MIN_ACCEPTABLE_DESENSE_RX_LVL_dBm_x16  (-101*16)

#define GL1_HW_CM_GUARD_PERIOD                  50

#define GL1_HW_CM_STARTUP_CMD_OVERHEAD_QS       0                 // before gap starts
#define GL1_HW_CM_CLEANUP_CMD_OVERHEAD_QS       0                   // overhead required

#define  GL1_HW_CM_STARTUP_CMD_DURATION_QS      gl1_hw_cm_startup_cmd_duration_qs( )
#define GL1_HW_CM_PWR_MEAS_CMD_OVERHEAD_QS      gl1_hw_cm_power_meas_cmd_overhead_qs( ) // tune time
#define GL1_HW_CM_CLEANUP_CMD_DURATION_QS       gl1_hw_cm_cleanup_cmd_duration_qs( )
#define GL1_HW_CM_ACQ_CMD_OVERHEAD_QS           gl1_hw_cm_power_meas_cmd_overhead_qs( )
#define GL1_HW_CM_ASYNC_RX_CMD_OVERHEAD_QS      gl1_hw_cm_power_meas_cmd_overhead_qs( )

#define GL1_HW_CM_PWR_MEAS_CMD_DURATION_QS      (MDSP_PWR_MEAS_LEN*4)   //length in quarter symbols, of data
#define GL1_HW_CM_ASYNC_RX_CMD_DURATION_QS      600                 // SCH burst size

#define GL1_HW_CM_SCH_SEARCH_WIDTH_QS           (GL1_HW_SCH_SEARCH_WIDTH_DEFAULT * 4)

#define GL1_HW_X2G_TDS_SCH_SEARCH_WIDTH_QS      (GL1_HW_T2G_SCH_SEARCH_WIDTH_DEFAULT * 4)

#define  GL1_HW_MIN_GAP_BTW_NEXT_X2G_ACTIVITY   14

/* CHIPXN_DIV2 Clock disabling */
typedef enum {
  TURN_CHIPXN_DIV2_CLOCK_OFF = 0,
  LEAVE_CHIPXN_DIV2_CLOCK_ON
} chipxn_control_t;


/*===========================================================================

FUNCTION gl1_hw_mcpm_restore_cb

DESCRIPTION
  Callback from MCPM to restore registers after power collapse.

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/

void gl1_cm_hw_mcpm_restore_cb( sys_modem_as_id_e_type as_id);

/* 569801 Deprecated api to be removed after ML1 changes */

void gl1_hw_mcpm_restore_cb( void );


/*===========================================================================

FUNCTION gl1_cm_hw_cleanup_async_rx

DESCRIPTION
  Wrapper function for gl1_hw_cleanup_async_rx
  Clean up drivers after an async rx.  Used to make aborting and early
  decoding cleanup simpler for the message layer.

===========================================================================*/
void gl1_cm_hw_cleanup_async_rx ( sys_modem_as_id_e_type as_id);

/* 569801 Deprecated api to be removed after ML1 changes */
void gl1_hw_cleanup_async_rx ( void);



/*===========================================================================

FUNCTION gl1_cm_mutex_lock

DESCRIPTION
  Clean up drivers after an async rx.  Used to make aborting and early
  decoding cleanup simpler for the message layer.

===========================================================================*/

void gl1_cm_mutex_lock (sys_modem_as_id_e_type as_id);

/*===========================================================================

FUNCTION gl1_cm_mutex_unlock

DESCRIPTION
  Clean up drivers after an async rx.  Used to make aborting and early
  decoding cleanup simpler for the message layer.

===========================================================================*/
void gl1_cm_mutex_unlock (sys_modem_as_id_e_type as_id);


/*===========================================================================

FUNCTION  gl1_hw_stop_acquisition

DESCRIPTION
  This function stops the tone detection at the specified offset into the
  frame. The offset does not include the DSP's frame tick setup margin

DEPENDENCIES
  This function should only be called after starting a tone detection
  by invoking gl1_hw_start_acquisition

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void gl1_hw_stop_acquisition(uint16 offset
#ifdef FEATURE_DUAL_SIM
                              ,sys_modem_as_id_e_type as_id
#endif /*FEATURE_DUAL_SIM*/
);

/*===========================================================================

FUNCTION  gl1_hw_cm_gsm_init_geran

DESCRIPTION
  Function to initialize the GSM Layer 1 interfaces that are required for
  compressed mode operation.  Interfaces include GSTMR, MDSP, RF.
  This function should be invoked whenever the WCDMA stack initializes.
  It also registers a TDMA frame processing handler.


  task_wait_handler - Needed by the RF component of hardware to implement
  a polling mechanism on the calling tasks context.

  task_wait_sig     - Signal used to notify the calling task that the
  RF driver is ready to proceed.


DEPENDENCIES

RETURN VALUE

SIDE EFFECTS
  None

===========================================================================*/
void gl1_hw_cm_gsm_init_geran
(
  void          (* tdma_isr_ptr)(sys_modem_as_id_e_type as_id),
  rex_tcb_type  *task_ptr,
  void          (* task_wait_handler)(rex_sigs_type),
  rex_sigs_type task_wait_sig,
  rex_sigs_type task_wait_sig_1,
  sys_modem_as_id_e_type as_id ,
  rfcom_mode_enum_type x2g_serving_rat
);

/*===========================================================================

FUNCTION  gl1_hw_cm_gsm_init

DESCRIPTION
  Function to initialize the GSM Layer 1 interfaces that are required for
  compressed mode operation.  Interfaces include GSTMR, MDSP, RF.
  This function should be invoked whenever the WCDMA stack initializes.
  It also registers a TDMA frame processing handler.


  task_wait_handler - Needed by the RF component of hardware to implement
  a polling mechanism on the calling tasks context.

  task_wait_sig     - Signal used to notify the calling task that the
  RF driver is ready to proceed.


DEPENDENCIES

RETURN VALUE

SIDE EFFECTS
  None

===========================================================================*/
void gl1_hw_cm_gsm_init
(
  void          (* tdma_isr_ptr)(sys_modem_as_id_e_type as_id),
  rex_tcb_type  *task_ptr,
  void          (* task_wait_handler)(rex_sigs_type),
  rex_sigs_type task_wait_sig,
  rex_sigs_type task_wait_sig_1
#ifdef FEATURE_DUAL_SIM
  ,sys_modem_as_id_e_type as_id
#endif
);


/*===========================================================================

FUNCTION  gl1_hw_start_acquisition

DESCRIPTION
  This function initiates a tone detection on the specified frequency
  at the specified offset into the frame and at the specified
  received signal level

DEPENDENCIES
  ARFCN              - Radio Channel Number on which to detect tone.
  rx_signal_strength - Anticipated Signal Strength at Antenna.
  offset             - Offset in quarter symbols at which to start tone
                       detection
  sch_decode         - TRUE:  decode SCH after tone is detected
                       FALSE: don't decode SCH
  AFC                - TRUE: adjust TCXO based on tone results
                       FALSE: don't adjust TCXO
  tags               - Pointer to where the gain will be stored

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void gl1_hw_start_acquisition
(
   ARFCN_T ARFCN,
   dBx16_T rx_signal_strength,
   uint16  timing_offset,
   int16   frequency_offset,
   boolean sch_decode,
   gl1_hw_acq_afc_type AFC,
   gl1_hw_rx_tags_type *tags
#ifdef FEATURE_DUAL_SIM
   ,sys_modem_as_id_e_type as_id
#endif
);

void gl1_hw_start_acquisition_geran
(
   ARFCN_T ARFCN,
   dBx16_T rx_signal_strength,
   uint16  timing_offset,
   int16   frequency_offset,
   boolean sch_decode,
   gl1_hw_acq_afc_type AFC,
   gl1_hw_rx_tags_type *tags,
   sys_modem_as_id_e_type as_id
);

/*===========================================================================

FUNCTION  gl1_hw_stop_acquisition

DESCRIPTION
  This function stops the tone detection at the specified offset into the
  frame. The offset does not include the DSP's frame tick setup margin

DEPENDENCIES
  This function should only be called after starting a tone detection
  by invoking gl1_hw_start_acquisition

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void gl1_hw_stop_acquisition_part1( uint16 offset
#ifdef FEATURE_DUAL_SIM
                                           ,sys_modem_as_id_e_type as_id
#endif /*FEATURE_DUAL_SIM*/
                                           );

extern void gl1_hw_stop_acquisition_part2(void);
/*===========================================================================

FUNCTION  gl1_hw_was_tone_detected

DESCRIPTION
  This function determines if a tone was detected and returns the tone
  offset returned by teh DSP if the FCCH was successfully decoded

DEPENDENCIES
  This function should only be called after starting a tone detection
  by invoking gl1_hw_start_tone_detection

RETURN VALUE
  TRUE - If tone was detected
  FALSE - If tone was not detected

SIDE EFFECTS
  None

===========================================================================*/
extern boolean gl1_hw_was_tone_detected
(
   ARFCN_T arfcn,  /* Used for logging only */
   uint16 *id,
   int32  *tone_offset,
   uint16 *coarse_freq,
   uint16 *fine_freq,
   uint16 *afc_freq,
   uint16 *snr
#ifdef FEATURE_DUAL_SIM
   ,sys_modem_as_id_e_type as_id
#endif /*FEATURE_DUAL_SIM*/
);

/*===========================================================================

FUNCTION  gl1_hw_was_tone_detected_geran

DESCRIPTION
  This function determines if a tone was detected and returns the tone
  offset returned by teh DSP if the FCCH was successfully decoded

DEPENDENCIES
  This function should only be called after starting a tone detection
  by invoking gl1_hw_start_tone_detection

RETURN VALUE
  TRUE - If tone was detected
  FALSE - If tone was not detected

SIDE EFFECTS
  None

===========================================================================*/
boolean gl1_hw_was_tone_detected_geran
(
   ARFCN_T arfcn,  /* Used for logging only */
   uint16 *id,
   int32  *tone_offset,
   uint16 *coarse_freq,
   uint16 *fine_freq,
   uint16 *afc_freq,
   uint16 *snr,
   sys_modem_as_id_e_type as_id
);
/*===========================================================================

FUNCTION  gl1_hw_cm_was_tone_detected

DESCRIPTION
  This function determines if a tone was detected and returns the tone
  offset returned by the DSP if the FCCH was successfully decoded

RETURN VALUE
  TRUE - If tone was detected
  FALSE - If tone was not detected

===========================================================================*/
boolean gl1_hw_cm_was_tone_detected
(
   ARFCN_T arfcn,  /* Used for logging */
   gl1_hw_cm_tone_results_t  *gl1_hw_cm_tone_results_ptr,
   sys_modem_as_id_e_type as_id
);

/*===========================================================================

FUNCTION  gl1_hw_cm_was_sch_decoded

DESCRIPTION
  This function polls the MDSP buffers to verify whether there are
  SCH results available.  The availability of results indicates that
  the MDSP has detected and decoded an SCH.  The CRC is a strong indicator
  that the SCH burst is decoded correctly.  The SCH burst contains
  a 10-bit CRC.

RETURN VALUE
  TRUE - If SCH results were available
  FALSE - If SCH results were not available

===========================================================================*/
boolean gl1_hw_cm_was_sch_decoded
(
   ARFCN_T arfcn,              /* Used for logging */
   boolean sch_burst,          /* TRUE indicates get results for gl1_hw_rx_async_sch_burst, FALSE for acquisition results */ 
   gl1_hw_rx_tags_type *tags,  /* Contains gain settings used */
   gl1_hw_cm_sch_results_t  *gl1_hw_cm_sch_results_ptr,
   sys_modem_as_id_e_type as_id
);

/*===========================================================================

FUNCTION  gl1_hw_was_sch_decoded

DESCRIPTION
  This function polls the MDSP buffers to verify whether there are
  SCH results available.  The availability of results indicates that
  the MDSP has detected and decoded an SCH.  The CRC is a strong indicator
  that the SCH burst is decoded correctly.  The SCH burst contains
  a 10-bit CRC.

DEPENDENCIES
  ARFCN              - Radio Channel Number on which to acquire.
  rx_power_index - Anticipated Signal Strength at Antenna.

RETURN VALUE
  time_offset        - The time offset from the start of acquisition
                       in quarter symbol units.
  sch_data           - The decoded data from the received SCH burst
  mdsp_payload       - Optional parameter to retrieve metrics associated
                       with the SCH burst.

SIDE EFFECTS
  None

===========================================================================*/
extern boolean gl1_hw_was_sch_decoded
(
   ARFCN_T arfcn,  /* Used for logging only*/
   uint16 *id,
   int32  *time_offset,
   uint8  *sch_data,
   uint16 *mdsp_payload,
   uint8  *crc_pass
#ifdef FEATURE_DUAL_SIM
   ,sys_modem_as_id_e_type as_id
#endif /* FEATURE_DUAL_SIM*/
);

/*===========================================================================

FUNCTION  gl1_hw_was_sch_decoded_geran
DESCRIPTION
  This function polls the MDSP buffers to verify whether there are
  SCH results available.  The availability of results indicates that
  the MDSP has detected and decoded an SCH.  The CRC is a strong indicator
  that the SCH burst is decoded correctly.  The SCH burst contains
  a 10-bit CRC.

DEPENDENCIES
  ARFCN              - Radio Channel Number on which to acquire.
  rx_power_index - Anticipated Signal Strength at Antenna.

RETURN VALUE
  time_offset        - The time offset from the start of acquisition
                       in quarter symbol units.
  sch_data           - The decoded data from the received SCH burst
  mdsp_payload       - Optional parameter to retrieve metrics associated
                       with the SCH burst.

SIDE EFFECTS
  None

===========================================================================*/
boolean gl1_hw_was_sch_decoded_geran
(
   ARFCN_T arfcn,  /* Used for logging only*/
   uint16 *id,
   int32  *time_offset,
   uint8  *sch_data,
   uint16 *mdsp_payload,
   uint8  *crc_pass,
   sys_modem_as_id_e_type as_id
);
/*===========================================================================

FUNCTION  gl1_hw_rx_async_sch_burst

DESCRIPTION
  This function initiates an asynchronous burst receive process for a SCH
  burst.

DEPENDENCIES
  This function can be executed at any time but will obviously result
  in failed received bursts if issued at the wrong time.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void gl1_hw_rx_async_sch_burst
(
   ARFCN_T  ARFCN,
   int16    signal_strength,
   uint16   timing_offset,
   int16    frequency_offset,
   gl1_hw_rx_tags_type *tags
#ifdef FEATURE_DUAL_SIM
   ,sys_modem_as_id_e_type as_id
#endif /*FEATURE_DUAL_SIM*/
);

void gl1_hw_rx_async_sch_burst_geran
(
   ARFCN_T  ARFCN,
   int16    signal_strength,
   uint16   timing_offset,
   int16    frequency_offset,
   gl1_hw_rx_tags_type *tags,
   sys_modem_as_id_e_type as_id

);

extern void gl1_hw_x2g_rx_async_sch_burst
(
   ARFCN_T               ARFCN,
   int16                 signal_strength,
   uint16                timing_offset,
   int16                 frequency_offset,
   gl1_hw_rx_tags_type*  tags,
   uint8*                sch_metrics_buffer_idx_ptr,
   uint16*               sch_metrics_seq_num_ptr,
   uint8*                sch_data_buffer_idx_ptr,
   sys_modem_as_id_e_type as_id

);

void gl1_hw_x2g_rx_async_sch_burst_on_rotator
(
   ARFCN_T               ARFCN,
   int16                 signal_strength,
   uint16                timing_offset,
   int16                 frequency_offset,
   gl1_hw_rx_tags_type*  tags,
   uint8*                sch_metrics_buffer_idx_ptr,
   uint16*               sch_metrics_seq_num_ptr,
   uint8*                sch_data_buffer_idx_ptr,
   sys_modem_as_id_e_type as_id
);

/*===========================================================================

FUNCTION  gl1_hw_get_async_rx_data

DESCRIPTION
  This function attempts to retrieve data from the mdsp buffers for an
  async burst.

  This function may be called repeatedly to poll for results being
  available.  The parameter last_time is provided to support this
  fuctionality.  If this functions returns TRUE or last_time is TRUE
  then the next call to this function will expect that another block
  has been decoded (buffer pointers are updated).  Unless this
  function returns TRUE or last_time is TRUE this function will not
  move on to the next block. I.e. on the last attempt to get the
  data for a particular block last_time must be set to TRUE.

DEPENDENCIES
  An async block or burst must have been received -
  via 4 gl1_hw_rx_async_bcch_burst commands or a single SCH burst -
  prior to calling this function.  There is a delay between the bursts being
  received and the data being available.

RETURN VALUE
  TRUE   - a block has been dedoded (the CRC may have passed or failed)
  FALSE  - no block has been decoded

SIDE EFFECTS
  If last_time is TRUE or this functions returns TRUE then the data for
  this block cannot be retrieved again.

===========================================================================*/
extern boolean gl1_hw_get_async_rx_data
(
   gl1_hw_channel_type      chan,
   gl1_defs_rx_hdr_struct   *rpt_hdr,
   uint8                    *data,
   uint16                   data_len,
   boolean                  last_time
#ifdef FEATURE_DUAL_SIM
   ,sys_modem_as_id_e_type   as_id
#endif
);

/*===========================================================================

FUNCTION  gl1_hw_get_async_rx_data_geran

DESCRIPTION
  This function attempts to retrieve data from the mdsp buffers for an
  async burst.

  This function may be called repeatedly to poll for results being
  available.  The parameter last_time is provided to support this
  fuctionality.  If this functions returns TRUE or last_time is TRUE
  then the next call to this function will expect that another block
  has been decoded (buffer pointers are updated).  Unless this
  function returns TRUE or last_time is TRUE this function will not
  move on to the next block. I.e. on the last attempt to get the
  data for a particular block last_time must be set to TRUE.

DEPENDENCIES
  An async block or burst must have been received -
  via 4 gl1_hw_rx_async_bcch_burst commands or a single SCH burst -
  prior to calling this function.  There is a delay between the bursts being
  received and the data being available.

RETURN VALUE
  TRUE   - a block has been dedoded (the CRC may have passed or failed)
  FALSE  - no block has been decoded

SIDE EFFECTS
  If last_time is TRUE or this functions returns TRUE then the data for
  this block cannot be retrieved again.

===========================================================================*/
boolean gl1_hw_get_async_rx_data_geran
(
   gl1_hw_channel_type      chan,
   gl1_defs_rx_hdr_struct   *rpt_hdr,
   uint8                    *data,
   uint16                   data_len,
   boolean                  last_time,
   sys_modem_as_id_e_type   as_id

);
extern boolean  gl1_hw_x2g_get_async_rx_sch_data
(
   gl1_defs_rx_hdr_struct*  rpt_hdr_ptr,
   uint8                    data[GL1_MAX_SCH_OCTETS],
   uint16                   data_len,
   boolean                  last_time,
   uint8                    sch_data_buffer_idx,
   uint16                   sch_data_seq_num,
   sys_modem_as_id_e_type   as_id

 );
/*===========================================================================

FUNCTION gl1_hw_get_async_rx_metrics_geran

DESCRIPTION
  This function retrieves the metrics for the async burst received in the
  previous frame.
  Sch flag is true for the ncell sch and it will false for the ccch decoding

DEPENDENCIES
  An async receive command must have been issued two frames previously to
  cause an async burst to be received in the previous frame.

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean gl1_hw_get_async_rx_metrics_geran
(
   gl1_defs_metrics_rpt    *rpt,
   boolean                 last_time,
   gl1_hw_rx_tags_type     *tags,
   boolean                 sch_flag,
   sys_modem_as_id_e_type  as_id
);

/*===========================================================================

FUNCTION gl1_hw_get_async_rx_metrics

DESCRIPTION
  This function retrieves the metrics for the async burst received in the
  previous frame.
  Sch flag is true for the ncell sch and it will false for the ccch decoding

DEPENDENCIES
  An async receive command must have been issued two frames previously to
  cause an async burst to be received in the previous frame.

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern boolean gl1_hw_get_async_rx_metrics
(
   gl1_defs_metrics_rpt    *rpt,
   boolean                 last_time,
   gl1_hw_rx_tags_type     *tags,
   boolean                 sch_flag
#ifdef FEATURE_DUAL_SIM
   ,sys_modem_as_id_e_type  as_id
#endif
);

extern boolean gl1_hw_x2g_get_async_sch_rx_metrics
(
   gl1_defs_metrics_rpt*  rpt,
   boolean                last_time,
   gl1_hw_rx_tags_type*   tags,
   uint8                  metrics_buffer_idx,
   uint16                 seq_num,
   sys_modem_as_id_e_type  as_id
);


/*===========================================================================

FUNCTION  gl1_hw_cm_get_power_measure_results

DESCRIPTION
  Returns 9 rssi values.

DEPENDENCIES
  gl1_hw_cm_power_measure must have been called two frames previously

RETURN VALUE
  samples       - array of 9 measured powers (received signal strength)
  num           - number of results to process. This number should match
                  the num parameter used in calling the
                  gl1_hw_cm_power_measure() function.
  tags          - Pointers to where the gain will be stored

  (It is the responsiblity of the calling function to provide storage for
   results, regardless of how many bursts were recieved.  It is also
   the responsibility of the calling function to know how many bursts were
   received.)

  TRUE  - power results were available
  FALSE - power results were not available

SIDE EFFECTS
  None

===========================================================================*/
extern boolean gl1_hw_cm_get_power_measure_results
(
   gl1_hw_measurement_T  samples[],
   uint8                 num,              /* Number of results to process  */
   gl1_hw_rx_tags_type   tags[]
#ifdef FEATURE_DUAL_SIM
   ,sys_modem_as_id_e_type      as_id
#endif
);

/*===========================================================================

FUNCTION  gl1_hw_cm_get_power_measure_results_geran

DESCRIPTION
  Returns 9 rssi values.

DEPENDENCIES
  gl1_hw_cm_power_measure must have been called two frames previously

RETURN VALUE
  samples       - array of 9 measured powers (received signal strength)
  num           - number of results to process. This number should match
                  the num parameter used in calling the
                  gl1_hw_cm_power_measure() function.
  tags          - Pointers to where the gain will be stored

  (It is the responsiblity of the calling function to provide storage for
   results, regardless of how many bursts were recieved.  It is also
   the responsibility of the calling function to know how many bursts were
   received.)

  TRUE  - power results were available
  FALSE - power results were not available

SIDE EFFECTS
  None

===========================================================================*/
boolean gl1_hw_cm_get_power_measure_results_geran
(
   gl1_hw_measurement_T  samples[],
   uint8                 num,              /* Number of results to process  */
   gl1_hw_rx_tags_type   tags[],
   sys_modem_as_id_e_type      as_id
);
/*===========================================================================

FUNCTION  gl1_hw_cm_startup. gl1_hw_cm_cleanup

DESCRIPTION
  Calls to invoke in the beginning and end of gap

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS
  None

===========================================================================*/
extern void  gl1_hw_cm_startup( uint16                      offset,
                                const gl1_hw_cm_rf_info_t*  cm_rf_info_ptr
#ifdef FEATURE_DUAL_SIM
                                ,sys_modem_as_id_e_type      as_id
#endif /* FEATURE_DUAL_SIM*/
                                 );
void  gl1_hw_cm_startup_geran( uint16                      offset,
                                const gl1_hw_cm_rf_info_t*  cm_rf_info_ptr,
                                sys_modem_as_id_e_type      as_id );


void gl1_hw_cm_cleanup ( uint16                      offset
#ifdef FEATURE_DUAL_SIM
                                ,sys_modem_as_id_e_type      as_id
#endif /* FEATURE_DUAL_SIM*/
                                );

void gl1_hw_cm_cleanup_geran ( uint16                      offset,
                                sys_modem_as_id_e_type      as_id );

/*===========================================================================

FUNCTION gl1_hw_cm_is_gap_complete

DESCRIPTION
  Whether GL1 gap os complete, cleanup processing is done.

===========================================================================*/
boolean gl1_hw_cm_is_gap_complete (sys_modem_as_id_e_type  as_id);

/*===========================================================================

FUNCTION  gl1_hw_qta_startup

DESCRIPTION
  This function initializes the RF for QTA operation, this is called by
  WCDMA prior to releasing TRM for a QTA gap.

===========================================================================*/
void gl1_hw_qta_startup( const gl1_hw_cm_rf_info_t*  cm_rf_info_ptr,
                         sys_modem_as_id_e_type      as_id );

/*===========================================================================

FUNCTION  gl1_hw_qta_cleanup

DESCRIPTION
  This function clears up the RF init after a QTA gap, this is called by
  WCDMA after it has re-acquired TRM at the end of the gap.

===========================================================================*/
void gl1_hw_qta_cleanup( sys_modem_as_id_e_type as_id );

extern boolean qta_empty_gap_cleanup(void);

extern void gl1_hw_x2g_startup( uint16                      offset,
                                const gl1_hw_cm_rf_info_t*  cm_rf_info_ptr,
                                uint32                      g_frame_number,
                                boolean                     x2g_rf_rxlm_update
#ifdef FEATURE_DUAL_SIM                                
                                ,sys_modem_as_id_e_type      as_id
#endif /*FEATURE_DUAL_SIM */
                                );

extern boolean gl1_hw_x2g_cleanup ( uint32                  offset,
                                    uint32                  g_frame_number
#ifdef FEATURE_DUAL_SIM                                
                                ,sys_modem_as_id_e_type      as_id
#endif /*FEATURE_DUAL_SIM */
                                  );

extern boolean gl1_hw_x2g_abort_rf_script(
#ifdef FEATURE_DUAL_SIM
                                sys_modem_as_id_e_type      as_id
#else
                                void
#endif /* FEATURE_DUAL_SIM*/ 
                                 );

extern void gl1_hw_x2g_build_startup_cleanup_script (const gl1_hw_cm_rf_info_t*  cm_rf_info_ptr,
                                                     boolean x2g_rf_rxlm_update
#ifdef FEATURE_DUAL_SIM    													 
                                                     ,sys_modem_as_id_e_type      as_id
#endif /*FEATURE_DUAL_SIM */													 
													);

extern uint16 gl1_hw_x2g_sch_burst(ARFCN_T    ARFCN,
                                 int16      signal_strength,
                                 uint16     timing_offset,
                                 int16      frequency_offset,
                                 gl1_hw_rx_tags_type*  tags_ptr,
                                 uint32                g_frame_number,
                                 boolean               reconfirm,
                                 void*                 gsm_cell_ptr
#ifdef FEATURE_DUAL_SIM                                
                                ,sys_modem_as_id_e_type      as_id
#endif /*FEATURE_DUAL_SIM */
                                   );

extern uint16 gl1_hw_x2g_start_acquisition(ARFCN_T ARFCN,
                                  dBx16_T rx_signal_strength,
                                  uint16  timing_offset,
                                  int16   frequency_offset,
                                  boolean sch_decode,
                                  gl1_hw_rx_tags_type*  tags_ptr,
                                  uint32                g_frame_number,
                                  void*                 gsm_cell_ptr
#ifdef FEATURE_DUAL_SIM                                
                                ,sys_modem_as_id_e_type      as_id
#endif /*FEATURE_DUAL_SIM */
                                           );

extern void gl1_hw_x2g_stop_acquisition ( uint16 offset, uint32 g_frame_number );

extern uint16 gl1_hw_x2g_power_measure(ARFCN_T                     ARFCN[MAX_CM_RSSI_MEASUREMENTS_REQ],               /* Absolute Radio Frequency Channels  */
                                     dBx16_T                     rx_signal_strength[MAX_CM_RSSI_MEASUREMENTS_REQ],  /* Signal Strengths at Antenna        */
                                     uint8                       num,                                           /* Number of measurements to make     */
                                     uint16                      offset[MAX_CM_RSSI_MEASUREMENTS_REQ],              /* When to make a measurement in qs   */
                                     gl1_hw_burst_position_type  position[MAX_CM_RSSI_MEASUREMENTS_REQ],            /* position of burst within a gap     */
                                     gl1_hw_rx_tags_type         tags[MAX_CM_RSSI_MEASUREMENTS_REQ],
                                     uint32                      g_frame_number[MAX_CM_RSSI_MEASUREMENTS_REQ],
                                     uint16                      offset2,
                                     uint32                      g_frame_number2
#ifdef FEATURE_DUAL_SIM									 
                                     ,sys_modem_as_id_e_type      as_id
#endif /*FEATURE_DUAL_SIM */
                                       );


/*===========================================================================

FUNCTION  gl1_hw_cm_power_measure

DESCRIPTION
  This function initiates one to nine power measurements in the next frame.


DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void gl1_hw_cm_power_measure
(
   ARFCN_T                     ARFCN[MAX_CM_RSSI_MEASUREMENTS_ARRAY_SIZE],               /* Absolute Radio Frequency Channels     */
   dBx16_T                     rx_signal_strength[MAX_CM_RSSI_MEASUREMENTS_ARRAY_SIZE],  /* Signal Strengths at Antenna           */
   uint8                       num,                                           /* Number of measurements to make        */
   uint16                      offset[MAX_CM_RSSI_MEASUREMENTS_ARRAY_SIZE],              /* When to make a measurement in qs      */
   gl1_hw_burst_position_type  position[MAX_CM_RSSI_MEASUREMENTS_ARRAY_SIZE],            /* position of burst within a gap  */
   gl1_hw_rx_tags_type         tags[MAX_CM_RSSI_MEASUREMENTS_ARRAY_SIZE]
#ifdef FEATURE_DUAL_SIM
   ,sys_modem_as_id_e_type      as_id
#endif /*FEATURE_DUAL_SIM*/
);

extern void  gl1_hw_l2g_power_measure
(
   ARFCN_T                     ARFCN[MAX_CM_RSSI_MEASUREMENTS_ARRAY_SIZE],               /* Absolute Radio Frequency Channels  */
   dBx16_T                     rx_signal_strength[MAX_CM_RSSI_MEASUREMENTS_ARRAY_SIZE],  /* Signal Strengths at Antenna        */
   uint8                       num,                                                      /* Number of measurements to make     */
   uint16                      offset[MAX_CM_RSSI_MEASUREMENTS_ARRAY_SIZE],              /* When to make a measurement in qs   */
   gl1_hw_burst_position_type  position[MAX_CM_RSSI_MEASUREMENTS_ARRAY_SIZE],            /* position of burst within a gap     */
   gl1_hw_rx_tags_type         tags[MAX_CM_RSSI_MEASUREMENTS_ARRAY_SIZE],
#ifdef FEATURE_DUAL_SIM
   sys_modem_as_id_e_type      as_id,
#endif
   boolean                     high_priority
);

extern boolean gl1_hw_x2g_get_power_measure_results
(
      gl1_hw_measurement_T  samples[],
      uint8                 *num_ptr,             /* Number of results to process  */
      gl1_hw_rx_tags_type   tags[],
      uint8                 results_buffer,
      sys_modem_as_id_e_type      as_id,
      boolean     gl1_skip_last_burst
);

/*===========================================================================

FUNCTION gl1_hw_cm_gsm_terminate

DESCRIPTION
  Cleanup the gl1_hw_cm layer interface.

DEPENDENCIES
  gl1_hw_cm_gsm_init() must be called first.

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
#ifdef FEATURE_DUAL_SIM
extern void gl1_hw_cm_gsm_terminate( sys_modem_as_id_e_type      as_id );
#else
extern void gl1_hw_cm_gsm_terminate( void );
#endif /*FEATURE_DUAL_SIM*/

//void gl1_hw_cm_gsm_terminate_geran( sys_modem_as_id_e_type      as_id );

int32 gl1_hw_get_vctcxo_pdm_default_val(void);
int32 gl1_hw_hztophase(int32 hz);
void gl1_hw_set_async_rx_search_width (uint16 width
#ifdef FEATURE_DUAL_SIM
                                       ,sys_modem_as_id_e_type      as_id
#endif /*FEATURE_DUAL_SIM */
                                       );

#ifdef FEATURE_DUAL_SIM
boolean gl1_is_gfw_error_detected( sys_modem_as_id_e_type      as_id );
#else
boolean gl1_is_gfw_error_detected( void );
#endif /*FEATURE_DUAL_SIM*/
boolean gl1_is_gfw_error_detected_geran( sys_modem_as_id_e_type      as_id );


extern msgr_client_t gsm_l1_async_intf_msgr_client;
void gl1_hw_async_intf_app_mode_config(GfwAppModeType app_mode, sys_modem_as_id_e_type as_id);

void gl1_hw_async_intf_set_tx_band_cmd(sys_modem_as_id_e_type as_id);

uint16  gl1_hw_cm_power_meas_cmd_overhead_qs( void );
uint16  gl1_hw_cm_startup_cmd_duration_qs( void );
uint16  gl1_hw_cm_cleanup_cmd_duration_qs( void );
uint16  gl1_hw_cm_wpll_time_us( void );

boolean gl1_hw_cm_is_gsm_fw_awake(
#ifdef FEATURE_DUAL_SIM
   sys_modem_as_id_e_type      as_id
#else
   void
#endif /*FEATURE_DUAL_SIM*/
);

boolean gl1_hw_init_async_intf_msgr_mailbox(sys_modem_as_id_e_type as_id);

rfgsm_band_type gl1_hw_arfcn_to_rfgsm_band (ARFCN_T arfcn);

/*===========================================================================

FUNCTION gl1_hw_rf_map_device2_as_id

DESCRIPTION
  Converts rf device to AS_ID
===========================================================================*/

sys_modem_as_id_e_type gl1_hw_rf_map_device2_as_id(rfm_device_enum_type device,ftm_mode_type ftm_mode);

/*===========================================================================

FUNCTION gl1_hw_rf_map_rf_sub_id_to_as_id

DESCRIPTION
  Converts rf device to AS_ID
===========================================================================*/
sys_modem_as_id_e_type gl1_hw_rf_map_rf_sub_id_to_as_id(uint8 rf_sub_id);

/* X2G_CM_INT_VIA_MSG : Request and Response structure defination*/
typedef struct
{
  msgr_hdr_struct_type hdr;

    rfcom_mode_enum_type  serving_rat;

  void           (* tdma_isr_ptr)(sys_modem_as_id_e_type as_id);
  rex_tcb_type*  task_ptr;
  void          (* task_wait_handler)(rex_sigs_type);
  rex_sigs_type  task_wait_sig;
  rex_sigs_type task_wait_sig_1;
  sys_modem_as_id_e_type as_id;

}x2g_irat_cm_gsm_init_req_type;

typedef struct
{
  msgr_hdr_struct_type hdr;
  sys_modem_as_id_e_type as_id;
  uint8 status;

}x2g_irat_cm_gsm_init_rsp_type;
#endif
/* EOF */
