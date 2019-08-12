#ifndef GL1_HWI_H
#define GL1_HWI_H
/*===========================================================================
                 GSM GPRS NPL1 FRAME LAYER INTERNAL HEADER FILE
DESCRIPTION
  This file contains internal definitions for the GSM/GPRS npl1 frame layer.

EXTERNALIZED FUNCTIONS

INTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2003-2014 Qualcomm Technologies, Inc.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government. Diversion contrary to U.S. law prohibited.

===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/gdrivers/inc/gl1_hwi.h#1 $ $DateTime: 2016/12/13 08:00:21 $ $Author: mplcsds1 $

when        who     what, where, why
--------    ---     --------------------------------------------------------
16/12/15    br      CR952495 P1 Static analysis and comple warning cleanup
03/06/14    cja     CR673927 Do not call RF sleep, if Rx already off.
19/02/14    cja     CR513250 G band in IRAT replace rf_card_band_type with rfgsm_band_type
19/11/13    ssh/ip  CR547956 DSDS/DSDA GL1 timeline Opt (parallelize the RF config. with GL1 warm-up)
27/11/13    cja     CR521061 Remove unused RF API and mainline FEATURE_GSM_RFA_TASK.
23/10/13   sp       CR563626: Disable acq_in_progress in next frame after Stop_Acq is issued.
15/08/13   sk       CR524039 GBTA bringup changes
29/07/13    sk      CR519663 Partial QBTA bringup changes
17/04/13    zf      CR476301: Enable DSDS MCPM state update to sleep.
04/09/12    og      CR395649. T2G Acquisition and BSIC decode enhancements.
02/08/12    ky      Exporting the gl1 functions to FTM code 
29/05/12    cja     CR365748 Resume power measurements on g2x clean-up
25/05/12    pg      Add support for RF Task  02/03/11    cja     CR276826 Add int16 saturation value definition
23/02/10    pg      Updates for NCell FCCH when HMSC enabled 
12/01/11    cja     CR264690 Acquisition frequency capture range larger
24/11/10    cja     Integrate HMSC
29/06/10    ky      Added the performace logging for gstmr handlers and rf api's
20/05/10    og      9k irat updates.
17/09/09    nf      Removed unused APIs
05/08/09    ws      Removed FEATURE_GSM_MDSP_DTM it's now mainlined
28/08/08    cs      Implement the Fast Rx Sweep functionality for FTM
28/11/06    ws      High Lint warning fixes
08/19/05    gfr     QLint cleanup
07/29/05    gfr     DTM support
06/10/05    gfr     Mainline FEATURE_HARDWARE_CONTROLLED_CLK_REGIMES
04/22/05    gfr     Indicate if a conflict occured in gl1_hw_schedule_int_meas
04/12/05    gfr     Added NO_BSIC define
04/08/05    gfr     Combined GSM and GPRS channel types
04/08/05    gfr     Move logging code into gl1_hw_log.c and put scheduler
                    prototypes in gl1_hwi.h
04/25/04    gfr     Moved GL1_HW_GSM_CLK_REGIMES here.
11/05/03    gw      Added #define for NUM_GSM_CLKS_PER_SYMB.
11/05/03    gfr     Support for quad-band.
02/25/03    gw      Moved prototypes for gl1_hw_turn_tx_{on|off}() back to
                    gl1_hw.h.  These functions are needed outside the 
                    hw layer.
02/21/03     gw     Added protos for gl1_hw_turn_{rx|tx}_{on|off} functions.
02/12/03     gw     Added prototype for gl1_hw_sleep_cont_shutdown.
02/12/03     gw     Initial revision.
===========================================================================*/

#include "geran_variation.h"
#include "customer.h"


#include "comdef.h"

#include "gl1_msg.h"
#include "gl1_msg_g.h"
#include "mdsp_dtm.h"

#include "rfgsm_msg.h"

/*===========================================================================

                               DEFINITIONS

===========================================================================*/

/* ----------------------------------------------------------------------- */
/* Constants for GSM                                                       */
/* ----------------------------------------------------------------------- */
#define NUM_SYMB_PER_FRAME     1250
#define NUM_QS_PER_FRAME       (4*NUM_SYMB_PER_FRAME)
#define NUM_GSM_CLKS_PER_QS    12
#define NUM_GSM_CLKS_PER_FRAME (NUM_QS_PER_FRAME*NUM_GSM_CLKS_PER_QS)
#define NUM_GSM_CLKS_PER_SYMB  (4*NUM_GSM_CLKS_PER_QS)

/* Define to use to indicate not a valid BSIC */
#define NO_BSIC  ((uint8)(-1))

/*Define maximum int16 value for 32bit to 16bit saturation */
#define MAX_INT16    32767
#define MAX_NEG_UINT16   0x8000


/* A macro to check that pointers passed to firmware are aligned, this can
   be disabled once all pointers are validated if desired */
/* TODO  Currently both AMR and GPRS tx data pointers are unaligned, but
   fixing them is tricky as it has to do with how the compiler aligns arrays
   of uint8s. */
/* #define DEBUG_CHECK_FOR_UNALIGNED_MDSP_PTRS */
#ifdef DEBUG_CHECK_FOR_UNALIGNED_MDSP_PTRS
#define MDSP_PTR_CHECK(ptr)  { if ((int)ptr & 0x1) MSG_ERROR("Unaligned pointer 0x%08x",ptr,0,0); }
#else
#define MDSP_PTR_CHECK(ptr)
#endif


/*===========================================================================

                               VARIABLES

===========================================================================*/


/*===========================================================================
                           
                           FUNCTION PROTOTYPES
                           
===========================================================================*/

void gl1_hw_sleep_cont_init( void );
void gl1_hw_sleep_cont_shutdown( void );
extern void gl1_hw_sleep_set_rf_on( boolean rf_on, gas_id_t gas_id );
extern boolean gl1_hw_sleep_get_rf_on( gas_id_t gas_id );

void gl1_hw_turn_rx_on(boolean blocking, gas_id_t gas_id);
void gl1_hw_turn_rx_off( gas_id_t gas_id );

rfcom_gsm_band_type gl1_hw_arfcn_to_rfcom_gsm_band (ARFCN_T arfcn);
rf_card_band_type gl1_hw_arfcn_to_rf_card_gsm_band(ARFCN_T arfcn);

/* WCDMA init/inact functions */
void gl1_hw_wcdma_init(gas_id_t gas_id);
void gl1_hw_wcdma_inact(gas_id_t gas_id);

/*===========================================================================

FUNCTION  gl1_hw_gen_was_tone_detected

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
extern boolean gl1_hw_gen_was_tone_detected
(
   ARFCN_T  arfcn,  /* Used for logging only */
   acq_decode_rpt *fcch_rpt,
   gas_id_t gas_id
);

extern boolean gl1_hw_x2g_was_tone_detected
(
   ARFCN_T          arfcn,             /* Used for logging only */
   acq_decode_rpt*  fcch_rpt,
   uint16           seq_num,
   gas_id_t         gas_id
);

/*=============================================================================
  HW Scheduler functions
============================================================================= */
void gl1_hw_schedule_frame_delay (int16 delay,gas_id_t gas_id);
void gl1_hw_schedule_set_medium_access_mode (gl1_defs_access_mode_type mode,gas_id_t gas_id);
void gl1_hw_schedule_dtm_rx (const mdsp_dtm_rx_params_type *params,gas_id_t gas_id);
void gl1_hw_schedule_dtm_tx (const mdsp_dtm_tx_params_type *params,gas_id_t gas_id);

uint32 gl1_hw_schedule_pwr_meas
 (
   ARFCN_T* arfcns,
   uint32 max_num_meas,
   uint16 sched_offsets[MAX_MEAS_PER_FRAME],
   mdsp_gprs_monitor_coex_params coex_params[MAX_MEAS_PER_FRAME],
   boolean *mdsp_scheduled,
   boolean *hmsc_enable_ptr,
   gas_id_t gas_id
 );


boolean gl1_hw_schedule_int_meas
 (
   const gl1_hw_meas_signal_type signal_info[ GL1_DEFS_MAX_ASSIGNED_DL_TS ],
   uint32 num_freq,
   gas_id_t gas_id
 );
void gl1_hw_schedule_async_rx (gl1_hw_channel_type chan_type, uint16 timing_offset,gas_id_t gas_id);
void gl1_hw_schedule_start_acq (uint16 timing_offset,gas_id_t gas_id);
void gl1_hw_schedule_stop_acq (uint16 timing_offset,gas_id_t gas_id);
void gl1_hw_update_acq_in_progress(boolean status, gas_id_t gas_id );

#if defined(FEATURE_INTERRAT_HANDOVER_GTOW) || defined(FEATURE_INTERRAT_CELL_RESELECTION_GTOW )
void gl1_hw_schedule_rf_sample_ram (uint16 timing_offset,gas_id_t gas_id);
void gl1_hw_schedule_g2x_gap (uint32  timing_offset,gas_id_t gas_id);
#endif
uint32 gl1_hw_schedule_get_num_slots(gas_id_t gas_id);

void gl1_hw_cm_mode_sw_handler( sys_modem_as_id_e_type as_id );
void gl1_hw_disable_egprs_mode_handler (sys_modem_as_id_e_type as_id);
void gl1_hw_gsac_clk_off_handler (sys_modem_as_id_e_type as_id);
#endif /* GL1_HWI_H */
