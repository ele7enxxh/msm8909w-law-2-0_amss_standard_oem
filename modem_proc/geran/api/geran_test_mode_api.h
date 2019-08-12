#ifndef GERAN_TEST_MODE_API_H
#define GERAN_TEST_MODE_API_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                    GERAN TEST MODE API FUNCTIONS

GENERAL DESCRIPTION

  This module provides utility test mode functions, 
  typically for FACTORY TEST MODE (FTM)

Copyright (c) 2010-2014 Qualcomm Technologies, Inc.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/api/geran_test_mode_api.h#1 $

when       who       what, where, why
--------   ---       ----------------------------------------------------------
11/02/14   jk        CR614254:GL1 changes to support IP2 calibration
17/04/14   cos       CR636543 - Changes for enabling Rx Diversity on Bolt - GL1/GRF intf updates
05/03/14   rb        CR625629  GL1 changes for FTM G2W IRAT
03/03/14   nm        CR612266 - Correct usage of gas id in gl1_hw_ftm_get_rx_power_results
03/01/14   jk        CR572347 - GL1 FTM API changes to support Rx diversity
11/04/13   jk        CR473972:GERAN L1 changes to handle as_id as an input parameter to the APIs
18/03/13   pjr       CR464499 - removed featurasation from geran_test_mode_api_cm_gsm_init
11/03/13   pjr       CR461202 - add missing test mode prototypes
15/11/12   ws        CR416979 - add missing test mode prototypes
16/10/12   ws        CR409719 - DIME/TRITON FTM API compatability support
10-12-08   tjw       Initial Revision
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#ifndef CUSTOMER_H
  #include "customer.h"
#endif
#ifndef COMDEF_H
  #include "comdef.h"
#endif
#include "queue.h"
#include "sys_type.h"
#include "gs.h"
#include "gl1_defs.h"
#include "gl1_hw.h"
#include "wfw_sw_cmd_proc_intf.h"


/* Forward declaration of struct/union required in header files */
struct geran_test_mode_api_l1_mac_sig_tag;
union geran_test_mode_api_mac_l1_sig_tag;
#include "gmacl1sig.h"
#include "gmacsigl1.h"

/*****************************************************************************
***
***     Public Defines & Constants
***
*****************************************************************************/

/*****************************************************************************
***
***     Public Type Definitions
***
*****************************************************************************/

/************************************************************************
* Union of all L1 to MAC message structures sent to FTM 
*
*
*************************************************************************/

typedef union
{
  /* application dependent signals */
  l1_mac_tbf_freq_ass_cnf_t   tbf_freq_ass_cnf; 
  l1_mac_tbf_rel_confrim_t    tbf_released; 
  /* GPRS specification primitives */
  mac_ph_connect_ind_t        connect_ind;
  mac_ph_data_ind_t           data_ind; 

} geran_test_mode_api_l1_mac_msg_u;
  
typedef struct geran_test_mode_api_l1_mac_sig_tag
{
  q_link_type                       link;
  byte                              task_id;
  l1_mac_sig_id_t                   sig_id;
  geran_test_mode_api_l1_mac_msg_u  msg;
} geran_test_mode_api_l1_mac_sig_t;

/*************************************************************************
* Union of all MAC to L1 message structures sent to FTM 
*************************************************************************/

typedef union geran_test_mode_api_mac_l1_sig_tag
{

  /*application dependent signals*/
  mac_l1_freq_config_t          mac_l1_freq_config;
  mac_l1_dynamic_tbf_config_t   mac_l1_dynamic_tbf_config;
  mac_l1_dl_tbf_config_t        mac_l1_dl_tbf_config;
  mac_l1_release_t              mac_l1_release;
  mac_l1_ts_reconfig_t          mac_l1_ts_reconfig;
} geran_test_mode_api_mac_l1_sig_t; 

/*************************************************************************
* Shadow ber_status struct for the FTM to L1 task functions
* geran_test_mode_api_ftm_gsm_ber_status_type shadows
* ftm_gsm_ber_status_type
*************************************************************************/

/* This type specifies the possible FTM GSM TCH slots */
typedef enum
{
    API_FTM_TCH_SLOT0 = 0,
    API_FTM_TCH_SLOT1,
    API_FTM_TCH_SLOT2,
    API_FTM_TCH_SLOT3,
    API_FTM_TCH_SLOT4,
    API_FTM_TCH_SLOT5,
    API_FTM_TCH_SLOT6,
    API_FTM_TCH_SLOT7,
    API_FTM_TCH_SLOT_INVALID
} geran_test_mode_api_ftm_gsm_ber_tch_slot_type;

/* This type specifies the allowed TCH modes */
typedef enum
{

    API_FTM_GSM_BER_TCH_FR   = 0,
    API_FTM_GSM_BER_TCH_HR   = 1,
    API_FTM_GSM_BER_TCH_EFR  = 2,
    API_FTM_GSM_BER_TCH_AMR  = 3

} geran_test_mode_api_ftm_gsm_ber_tch_mode_type;

/* 
 * This type specifies a buffer to hold network parameters used during 
 * the FTM GSM BER tests
 *
 */
typedef struct
{
    
    BSIC_T                               bsic;
} geran_test_mode_api_ftm_gsm_ber_network_parms_type;

/* 
 * This type specifies a buffer to hold the current state of the FTM GSM BER 
 * module
 *
 */

typedef struct
{
    uint8                                               tsc;
    ARFCN_T                                             arfcn;
    ARFCN_T                                             tch_arfcn;
    geran_test_mode_api_ftm_gsm_ber_tch_slot_type       tch_slot;
    geran_test_mode_api_ftm_gsm_ber_tch_mode_type       tch_mode;
    geran_test_mode_api_ftm_gsm_ber_network_parms_type  network_parms;
    uint8                                               tch_ordered_power_level;
} geran_test_mode_api_ftm_gsm_ber_status_type;

/*----------------------------------------------------------------------------*/
/* Callback function type for pre-cb and post-cb for a SW-FW cmd */
typedef void (*g2w_post_cb_fn)(gas_id_t gas_id);
typedef void (*g2w_pre_cb_fn)(gas_id_t gas_id);

/*===========================================================================

FUNCTION  GERAN_TEST_MODE_API_RESTART_SRB_LOOP

DESCRIPTION
  FTM access function to restart the SRB loop after a TS_RECONFIG.

DEPENDENCIES

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void geran_test_mode_api_restart_srb_loop( 
#ifdef FEATURE_DUAL_SIM
sys_modem_as_id_e_type as_id
#else
void
#endif /*FEATURE_DUAL_SIM */
);

/*===========================================================================
FUNCTION geran_test_mode_api_set_vctcxo_pdm_val_leave_chipxn_div2_clock_on

DESCRIPTION
  This function sets the TRK_LO_ADJ PDM to the desired value at the next
  frame boundary used by FTM.
===========================================================================*/
extern void geran_test_mode_api_set_vctcxo_pdm_val_leave_chipxn_div2_clock_on(int32 pdm_val,sys_modem_as_id_e_type as_id);

/*===========================================================================

FUNCTION gl1_hw_handle_ftm_do_rx_sweep_cal_req

DESCRIPTION
  Starts up the RX sweep process when GL1 receives the request in inactive (FTM)
  mode and then sends a message to L1 task to begin the sweep setup/handling.

DEPENDENCIES
  None

RETURN
  Status to FTM to tell if request to start sweep was successful

SIDE EFFECTS
  None
===========================================================================*/
extern boolean gl1_hw_handle_ftm_do_rx_sweep_cal_req( void *ftm_rx_sweep_def_p,
                                                      void  (*results_cb)( void * ) );

/*===========================================================================
FUNCTION  gl1_hw_ftm_get_rx_power_results

DESCRIPTION
  This function retrieves Rx power results in TCH dedicated mode for FTM mode

RETURNS
   TRUE  - results valid
   FALSE - results invalid
===========================================================================*/

extern boolean gl1_hw_ftm_get_rx_power_results(uint32 *rssi, dBx16_T *pwr_dbm,
                                                     uint32 *rssi_divrx, dBx16_T *pwr_dbm_divrx,
                                                     sys_modem_as_id_e_type as_id);

/*===========================================================================
===
===  FUNCTION      geran_test_mode_api_gprs_l1_mac_acc_func
===
===  DESCRIPTION
===  Used by FTM to send messages to L1. FTM is required to add the IMH
===  header to the message before sending it to L1 using gs_send_message
===
===  INPUT
===  Pointer to L1 message structure of the message to be sent
===
===  DEPENDENCIES
===
===
===  RETURN VALUE
===
===
===  SIDE EFFECTS
===
===
===========================================================================*/
void geran_test_mode_api_gprs_l1_mac_acc_func(geran_test_mode_api_mac_l1_sig_t *common_msg_ptr
#ifdef FEATURE_DUAL_SIM
,sys_modem_as_id_e_type as_id
#endif /*FEATURE_DUAL_SIM */ 
);

/*===========================================================================

FUNCTION       GERAN_TEST_MODE_API_GET_BEST_SIX_INFO_NO_OF_ENTRIES

DESCRIPTION
               Access function

DEPENDENCIES
  none

===========================================================================*/
uint32 geran_test_mode_api_get_best_six_info_no_of_entries(void *msg);
 

/*===========================================================================

FUNCTION        GERAN_TEST_MODE_API_SEND_FTM_START_GPRS_IDLE_MODE_REQ
DESCRIPTION     Sends a fixed message to GL1 to get into GPRS IDLE mode 
                using the 51 multiframe CCCH.
.

DEPENDENCIES    GL1 is synchronized to BCCH already.  
    

RETURN VALUE
   None.

SIDE EFFECTS    Sends configuration message to L1.
   

===========================================================================*/
void geran_test_mode_api_send_FTM_START_GPRS_IDLE_MODE_REQ(
#ifdef FEATURE_DUAL_SIM
sys_modem_as_id_e_type as_id
#else
void
#endif /*FEATURE_DUAL_SIM */  
);

/*===========================================================================

FUNCTION       GERAN_TEST_MODE_API_SEND_FTM_START_GSM_MODE_REQ

DESCRIPTION
               Sends a Start GSM Mode Request from FTM to L1 task

DEPENDENCIES
  none

===========================================================================*/
gs_status_T geran_test_mode_api_send_FTM_START_GSM_MODE_REQ(
sys_modem_as_id_e_type as_id,
boolean enableRxd,
boolean enableDrx,
boolean enablePrx
);

/*===========================================================================

FUNCTION       GERAN_TEST_MODE_API_SEND_FTM_SELECT_SPECIFIC_BCCH_REQ

DESCRIPTION
               Sends a Select Specific Bcch Req from FTM task to L1

DEPENDENCIES
  none

===========================================================================*/
gs_status_T geran_test_mode_api_send_FTM_SELECT_SPECIFIC_BCCH_REQ( uint16 chanNum, sys_band_T band
#ifdef FEATURE_DUAL_SIM
, sys_modem_as_id_e_type as_id
#endif /*FEATURE_DUAL_SIM */ 
);

/*===========================================================================

FUNCTION      GERAN_TEST_MODE_API_SEND_FTM_START_IDLE_REQ

DESCRIPTION
              Sends a Start IDLE Request from FTM to L1 task

DEPENDENCIES
  none

===========================================================================*/
gs_status_T geran_test_mode_api_send_FTM_START_IDLE_REQ(
#ifdef FEATURE_DUAL_SIM
sys_modem_as_id_e_type as_id
#else
void
#endif /*FEATURE_DUAL_SIM */ 
);

/*===========================================================================

FUNCTION       GERAN_TEST_MODE_API_SEND_FTM_CHANNEL_ASSIGN_REQ

DESCRIPTION
               Sends Channel Assign Request from FTM to L1 task

DEPENDENCIES
  none

===========================================================================*/
gs_status_T geran_test_mode_api_send_FTM_CHANNEL_ASSIGN_REQ(geran_test_mode_api_ftm_gsm_ber_status_type ber_status
#ifdef FEATURE_DUAL_SIM
, sys_modem_as_id_e_type as_id
#endif /*FEATURE_DUAL_SIM */ 
);
void geran_test_mode_api_send_FTM_HWTC_CHANNEL_ASSIGN_REQ(byte ts, geran_test_mode_api_ftm_gsm_ber_status_type ber_status
#ifdef FEATURE_DUAL_SIM
, sys_modem_as_id_e_type as_id
#endif /*FEATURE_DUAL_SIM */ 
);

/*===========================================================================

FUNCTION       GERAN_TEST_MODE_API_SEND_FTM_CHANNEL_RELEASE_REQ

DESCRIPTION
               Sends Channel Release Request from FTM to L1 task

DEPENDENCIES
  none

===========================================================================*/
gs_status_T geran_test_mode_api_send_FTM_CHANNEL_RELEASE_REQ(geran_test_mode_api_ftm_gsm_ber_status_type ber_status
#ifdef FEATURE_DUAL_SIM
, sys_modem_as_id_e_type as_id
#endif /*FEATURE_DUAL_SIM */ 
);

/*===========================================================================

FUNCTION       GERAN_TEST_MODE_API_SEND_FTM_STOP_GSM_MODE_REQ

DESCRIPTION
               Sends a Stop GSM Mode Request from FTM to L1 task

DEPENDENCIES
  none

===========================================================================*/
gs_status_T geran_test_mode_api_send_FTM_STOP_GSM_MODE_REQ(
#ifdef FEATURE_DUAL_SIM
sys_modem_as_id_e_type as_id
#else
void
#endif /*FEATURE_DUAL_SIM */ 
);

/*===========================================================================

FUNCTION geran_test_mode_api_do_tch_loopback

DESCRIPTION
  This function tells the mdsp to put the TCH into loopback A,B or C or no
  loopback.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void geran_test_mode_api_do_tch_loopback(gl1_defs_loopback_type loopback, sys_modem_as_id_e_type as_id);

/*===========================================================================

FUNCTION geran_test_mode_api_get_FN

DESCRIPTION
  This function returns the current GERAN TDMA Frame number based on the
  assigned AS ID
DEPENDENCIES
  None

RETURN VALUE
  uint32 TDMA frame number

SIDE EFFECTS
  None
===========================================================================*/

uint32 geran_test_mode_api_get_FN(sys_modem_as_id_e_type as_id);

/*===========================================================================

FUNCTION geran_test_mode_api_get_qsym_count

DESCRIPTION
  This function returns the current GERAN TDMA quater symbol count based on the
  assigned AS ID
DEPENDENCIES
  None

RETURN VALUE
  uint32 quater symbol count 0- 5000

SIDE EFFECTS
  None
===========================================================================*/

uint32 geran_test_mode_api_get_qsym_count(sys_modem_as_id_e_type as_id);

/*===========================================================================

FUNCTION  geran_test_mode_api_start_geran

DESCRIPTION
  This function initializes the GSM hardware devices.  It also registers
  a TDMA frame processing handler.


  task_wait_handler - Needed by the RF component of hardware to implement
  a polling mechanism on the calling tasks context.

  task_wait_sig_1   - Signal used to notify the calling task that the
  RF driver is ready to proceed.

  task_wait_sig_2   - Signal used for mdsp timeout download.

===========================================================================*/
void geran_test_mode_api_start_geran(
                                      void (*tdma_isr_ptr)(sys_modem_as_id_e_type),
                                      rex_tcb_type  *task_ptr,
                                      void          (*task_wait_handler)(rex_sigs_type, sys_modem_as_id_e_type),
                                      rex_sigs_type task_wait_sig_1,
                                      rex_sigs_type task_wait_sig_2,
                                      gl1_init_rflm_type gsm_init_rflm_state,
                                      sys_modem_as_id_e_type as_id,                              
	                                  boolean enableRxd,
	                                  boolean enableDrx,
	                                  boolean enablePrx                                      
                                     );

/*===========================================================================

FUNCTION  geran_test_mode_api_stop_geran

DESCRIPTION
  This function performs any cleanup necessary when leaving GSM mode.

  If deep_sleep is TRUE then the Layer 1 hardware will be powered
  down.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void geran_test_mode_api_stop_geran(boolean deep_sleep, sys_modem_as_id_e_type as_id);

/*===========================================================================

FUNCTION  geran_test_mode_api_turn_tx_on

DESCRIPTION
  This function turns the transmit RF chain on

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void geran_test_mode_api_turn_tx_on (sys_modem_as_id_e_type as_id );

/*===========================================================================

FUNCTION  geran_test_mode_api_set_dedicated

DESCRIPTION
  This function sets dedicated mode state info in GERAN drivers

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void geran_test_mode_api_set_dedicated( boolean dedicated,sys_modem_as_id_e_type as_id );

/*===========================================================================

FUNCTION  geran_test_mode_api_rf_set_tx_band

DESCRIPTION
  This function performs any cleanup necessary when leaving GSM mode.

  If deep_sleep is TRUE then the Layer 1 hardware will be powered
  down.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void geran_test_mode_api_rf_set_tx_band(rfgsm_band_type band,sys_modem_as_id_e_type as_id);


/*===========================================================================

FUNCTION  geran_test_mode_api_rf_get_freq_error

DESCRIPTION
  Gets a copy of the current stored frequency error to be applied via DPLL
 
DEPENDENCIES
  None

RETURN VALUE
  frequency error int32
SIDE EFFECTS
  None

===========================================================================*/
int32 geran_test_mode_api_rf_get_freq_error(sys_modem_as_id_e_type as_id);

/*===========================================================================

FUNCTION  geran_test_mode_api_set_idle_mode

DESCRIPTION
  This function sets idle mode state info in GERAN drivers

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void geran_test_mode_api_set_idle_mode(boolean,sys_modem_as_id_e_type as_id);


/*===========================================================================

FUNCTION  geran_test_mode_api_set_idle_mode

DESCRIPTION
  This function sets idle mode state info in GERAN drivers

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void geran_test_mode_api_schedule_frame_tick (uint32 timestamp,sys_modem_as_id_e_type as_id);

/*===========================================================================

FUNCTION geran_test_mode_api_sched_rx_tx

DESCRIPTION
   Called every frame tick after the rx and tx handlers have been executed
   to issue any scheduled receives or transmits.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void geran_test_mode_api_sched_rx_tx (sys_modem_as_id_e_type as_id);

/*===========================================================================

FUNCTION geran_test_mode_api_rx_dtm_gprs

DESCRIPTION
  Schedule a GPRS receive.

===========================================================================*/
void geran_test_mode_api_rx_dtm_gprs
(
   const gl1_hw_rx_signal_type      signal_info[GERAN_API_MAX_GL1_DEFS_MAX_DL_MSGS],
   uint8                            num_tn,
   const gl1_hw_rx_pkt_burst_type   *burst_info,
   gl1_hw_rx_tags_type              tags[GERAN_API_MAX_GL1_DEFS_MAX_DL_MSGS],
   sys_modem_as_id_e_type           as_id
);

/*===========================================================================

FUNCTION geran_test_mode_api_rx_dtm_gsm

DESCRIPTION
  Schedule a GSM receive.

===========================================================================*/
void geran_test_mode_api_rx_dtm_gsm
(
   const gl1_hw_rx_signal_type   *signal_info,
   const gl1_hw_rx_burst_type    *burst_info,
   gl1_hw_rx_tags_type           *tags,
   sys_modem_as_id_e_type        as_id

);

/*=============================================================================
FUNCTION  geran_test_mode_api_get_rx_metrics

DESCRIPTION
   Retrieves metrics information. This function is to be called every frame ISR.
   If there is no data or metrics to be returned then the corresponding buffer
   pointer will be set to 'NULL'.

DEPENDENCIES

RETURN VALUE
   None

SIDE EFFECTS
   None
============================================================================= */
void geran_test_mode_api_get_rx_metrics
(
   gl1_defs_rx_pkt_metrics_type **metrics_info,
   uint8                        num_msgs,
   boolean                      get_usf_info,
   boolean                      get_hard_decisions,
   gl1_hw_rx_tags_type          *tags,
   sys_modem_as_id_e_type       as_id

);

/*=============================================================================
FUNCTION geran_test_mode_api_get_rx_data

DESCRIPTION
   Retrieves data information. This function is to be called every frame ISR. If
   there is no data or metrics to be returned then the corresponding buffer
   pointer will be set to 'NULL'.

DEPENDENCIES
   The frame layer must be initialized.

RETURN VALUE
   None

SIDE EFFECTS
   None
============================================================================= */
boolean geran_test_mode_api_get_rx_data
(
   boolean                   ptcch_type,
   gl1_defs_tn_type          tn,
   gl1_hw_rx_tags_type       *tag,
   boolean                   last_attempt,
   gl1_defs_rx_pkt_data_type *msg_info,
   sys_modem_as_id_e_type    as_id

);

/*===========================================================================

FUNCTION geran_test_api_get_sync_rx_metrics

DESCRIPTION
  This function retreives the metrics for the burst received in the previous
  frame.

DEPENDENCIES
  A burst receive command must have been issued two frames previously to
  cause a burst to be received in the previous frame.

RETURN VALUE
  True/False depending on if a Interferer Blocker is seen.

SIDE EFFECTS
  None
===========================================================================*/
boolean geran_test_mode_api_get_sync_rx_metrics
(
   gl1_defs_metrics_rpt     *rpt,
   gl1_hw_rx_tags_type      *tags,
   sys_modem_as_id_e_type   as_id
);


/*===========================================================================

FUNCTION  geran_test_mode_api_rf_tx_burst

DESCRIPTION
  Send MSGR message to RF Task for Tx Burst processing

DEPENDENCIES
   

RETURN VALUE
   None

SIDE EFFECTS
   None  
===========================================================================*/
void geran_test_mode_api_rf_tx_burst(uint8                    num_assigned,
                                     uint8                    num_slots,
                                     rfgsm_band_type          band,
                                     uint16                   arfcn,
                                     int32                    freq_error,
                                     uint16                   pa_index[GERAN_API_MAX_GL1_DEFS_MAX_UL_MSGS],
                                     gl1_defs_modulation_type mod[GERAN_API_MAX_GL1_DEFS_MAX_UL_MSGS],
                                     sys_modem_as_id_e_type as_id);

/*===========================================================================
FUNCTION  geran_test_api_log_burst_metrics

DESCRIPTION
  This function should be called when burst metrics are available
  It will send a log packet.
RETURN VALUE
  None

SIDE EFFECTS
  Calls l1_log_receive_metrics_packet() when done
===========================================================================*/
void geran_test_mode_api_log_burst_metrics
(
  gl1_msg_chan_type            channel, 
  gl1_msg_metrics_rpt          *rpt_ptr[],
  sys_modem_as_id_e_type       as_id
);

/*===========================================================================
FUNCTION  geran_test_mode_api_is_gfw_error_detected

DESCRIPTION
  This function should be called when burst metrics are available
  It will send a log packet.
RETURN VALUE
  None

SIDE EFFECTS
  Calls l1_log_receive_metrics_packet() when done
===========================================================================*/
boolean geran_test_mode_api_is_gfw_error_detected( sys_modem_as_id_e_type as_id );

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
void geran_test_mode_api_start_acquisition
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
void geran_test_mode_api_stop_acquisition(uint16 offset,sys_modem_as_id_e_type as_id);

/*===========================================================================

FUNCTION  GL1_SET_SACCH_TX_PWR_CONTROL

DESCRIPTION
  This function will set an indication as to use or ignore the
  SACCH tx power control level.

DEPENDENCIES

RETURN VALUE
  gs_status_T GS_SUCCESS

SIDE EFFECTS
  None

===========================================================================*/
gs_status_T geran_test_mode_api_set_sacch_tx_pwr_control(boolean tx_pwr_control,sys_modem_as_id_e_type as_id);


/*=============================================================================
FUNCTION  gl1_hw_write_tx_pkt_data

DESCRIPTION
   Writes a block/frame of data to the DSP memory. If the message buffer in the
   mDSP is full then one of the messages in the mDSP buffer will be discarded
   to make space for the submitted message (if the submitted message is a higher
   priority message).
   Returns TRUE if the message was successfully written to the mDSP.
============================================================================= */
boolean geran_test_mode_api_write_tx_pkt_data
(
   gl1_defs_tx_pkt_data_type *msg_ptr,
   uint8                     len,
   gl1_defs_tn_type          tn,
   gl1_hw_pkt_msg_type       msg_type,
   gl1_hw_channel_type       chan_type,
   gl1_hw_tx_tags_type       *tags,
   sys_modem_as_id_e_type    as_id
);

/*=============================================================================
FUNCTION  gl1_hw_get_tx_scheduled_result

DESCRIPTION
   Returns if the message associated with the given tag was transmitted.
   If discard_untxed is set then untransmitted messages are cleared.

============================================================================= */
boolean geran_test_mode_api_get_tx_scheduled_result
(
   gl1_hw_tx_tags_type      *tags,
   boolean                  discard_untxed,
   sys_modem_as_id_e_type   as_id

);

/*===========================================================================

FUNCTION geran_test_mode_api_tx_dtm_gprs

DESCRIPTION
  Schedule a GPRS transmit.

===========================================================================*/
void geran_test_mode_api_tx_gprs
(
   const gl1_hw_tx_signal_type       *signal_info,
   uint8                             num_tn,
   const gl1_hw_tx_pkt_burst_type    *burst_info,
   gl1_hw_tx_tags_type               *tags,
   uint8                             num_msgs,
   uint8                             num_ul_timeslots,
   sys_modem_as_id_e_type            as_id
);

/*===========================================================================

FUNCTION  geran_test_mode_api_cm_get_power_measure_results

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
boolean geran_test_mode_api_cm_get_power_measure_results
(
   gl1_hw_measurement_T         samples[],
   uint8                        num,             /* Number of results to process  */
   gl1_hw_rx_tags_type          tags[],
   sys_modem_as_id_e_type       as_id
);

/*===========================================================================

FUNCTION  geran_test_mode_api_cm_power_measure

DESCRIPTION
  This function initiates one to nine power measurements in the next frame.


DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void geran_test_mode_api_cm_power_measure
(
   ARFCN_T                      ARFCN[],               /* Absolute Radio Frequency Channels  */
   dBx16_T                      rx_signal_strength[],  /* Signal Strengths at Antenna        */
   uint8                        num,                                           /* Number of measurements to make     */
   uint16                       offset[],              /* When to make a measurement in qs   */
   gl1_hw_burst_position_type   position[],            /* position of burst within a gap     */
   gl1_hw_rx_tags_type          tags[],
   sys_modem_as_id_e_type       as_id
);

/*===========================================================================

FUNCTION  geran_test_mode_api_async_intf_app_mode_config

DESCRIPTION
  Send MSGR message to GFW Task to run app mode config. Use MSGR blocking wait
  to receive app mode config rsp from GFW before continuing

===========================================================================*/
void geran_test_mode_api_async_intf_app_mode_config
(
   GfwAppModeType               app_mode,
   uint32                       rxlm_buf_idx,
   sys_modem_as_id_e_type       as_id
);


/*===========================================================================

FUNCTION  geran_test_mode_api_cm_startup

DESCRIPTION
  Calls to invoke the CM startup at the beginning of the gap.

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS
  None

===========================================================================*/
void  geran_test_mode_api_cm_startup
( 
   uint16                       offset,
   const gl1_hw_cm_rf_info_t*   cm_rf_info_ptr,
   sys_modem_as_id_e_type       as_id
);

/*===========================================================================

FUNCTION  geran_test_mode_api_cm_gsm_init

DESCRIPTION
  Function to initialize the GSM Layer 1 interfaces that are required for
  compressed mode operation.  Interfaces include GSTMR, MDSP, RF.
  This function should be invoked whenever the WCDMA stack initializes.
  It also registers a TDMA frame processing handler.

===========================================================================*/
void geran_test_mode_api_cm_gsm_init
(
   void                         (* tdma_isr_ptr)(sys_modem_as_id_e_type as_id),
   rex_tcb_type                 *  task_ptr,
   void                         (* task_wait_handler)(rex_sigs_type),
   rex_sigs_type                task_wait_sig,
   rex_sigs_type                task_wait_sig_1,
   sys_modem_as_id_e_type       as_id
);

/*===========================================================================

FUNCTION  geran_test_mode_api_cm_cleanup

DESCRIPTION
  Call to invoke the CM cleanup at the end of the gap.

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS
  None

===========================================================================*/
void geran_test_mode_api_cm_cleanup
( 
   uint16                       offset,
   sys_modem_as_id_e_type       as_id 
);

/*===========================================================================

FUNCTION geran_test_mode_api_cm_gsm_terminate

DESCRIPTION
  Cleanup the gl1_hw_cm layer interface.

DEPENDENCIES
  gl1_hw_cm_gsm_init() must be called first.

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void geran_test_mode_api_cm_gsm_terminate
( 
   sys_modem_as_id_e_type       as_id 
);
/*===========================================================================

FUNCTION geran_test_mode_api_g2w_init

DESCRIPTION
  Send the x2w mdsp init command to initialise the sync interface

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void geran_test_mode_api_g2w_init
( 
sys_modem_as_id_e_type       as_id
);
/*===========================================================================

FUNCTION geran_test_mode_api_start_g2w

DESCRIPTION
  Send the x2w start mode cmd to WFW

DEPENDENCIES
  geran_test_mode_api_send_g2w_mdsp_sync_init() must be called first.

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void geran_test_mode_api_start_g2w
( 
/* DL channel */
uint16 uarfcn,
/* Start Time */
uint16 start_time,
sys_modem_as_id_e_type       as_id
);
/*===========================================================================

FUNCTION geran_test_mode_api_g2w_rf_cfg

DESCRIPTION
  Send config and tune commands to WFW in X2W

DEPENDENCIES
  geran_test_mode_api_send_g2w_mdsp_sync_init() must be called first.

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void geran_test_mode_api_g2w_rf_cfg
( 
/* RxLM buffer index for neighbour */
uint32 g2w_rxlm_buf_idx,
/* Freq error in Q6 Hz format */
int32 freq_err_q6_hz,
/* Inverse W DL freq */
uint32 inv_dlo_freq,
/* pre call back function */
g2w_pre_cb_fn pre_cb,
/* post call back function */
g2w_post_cb_fn post_cb,
sys_modem_as_id_e_type       as_id
);
/*===========================================================================

FUNCTION geran_test_mode_api_g2w_sleep

DESCRIPTION
  Send the x2w sleep mode cmd to WFW

DEPENDENCIES
  geran_test_mode_api_send_g2w_mdsp_sync_init() must be called first.

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void geran_test_mode_api_g2w_sleep
( 
/* pre call back function */
g2w_pre_cb_fn pre_cb,
/* post call back function */
g2w_post_cb_fn post_cb,
sys_modem_as_id_e_type       as_id
);

/*===========================================================================

FUNCTION geran_test_mode_api_do_ip2cal

DESCRIPTION
  RF calls this API to trigger scheduling IP2 cal every frame till 
  RF asks GL1 to stop scheduling IP2 frames.

DEPENDENCIES
 geran_test_mode_api_start_geran or 
 geran_test_mode_api_send_FTM_START_GSM_MODE_REQ must be called
 before calling this funtion.

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void geran_test_mode_api_do_ip2cal
( 
   uint8  NoOfIP2CalSteps,
   uint32 ip2CalStepSizeQS,
   sys_modem_as_id_e_type as_id
);

#endif /* GERAN_TEST_MODE_API_H */
