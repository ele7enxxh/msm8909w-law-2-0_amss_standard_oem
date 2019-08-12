#ifndef L1CMMEAS_H
#define L1CMMEAS_H
/*==========================================================================

             L1 Compressed Mode Measurement Manager Code

DESCRIPTION
  This file contains code to process measurement control messages, manage
  compressed mode measurements required, cells to be measured, and perform
  measurement reporting when necessary.

  Copyright (c) 2002 - 2013 by Qualcomm Technologies, Inc.
  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/wcdma/l1/offline/src/l1cmmeas.h#1 $
$DateTime: 2016/12/13 08:00:12 $
$Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/22/14   ag      Fixed Compilation issues
08/07/14   jk      Fixed search compiler warnings.
06/24/14   mk      Support for changing EARFCN variable type from uint16 to uint32.
06/13/14   db      Do not send periodic measurements untill first CM measurement gap after LTA gap.
05/27/14   jk      Send periodic measurement for 3X if it was due during a DSDS LTA gap.
03/10/14   ks/cc   Add LTE and GSM PLT cmds
01/22/14   jk      Added support for CER SNR pruning for reducing spurious Event 
                   3A-C reports for LTE TDD cells. 
09/12/13   jd      Searcher redesign
08/20/13   zr      Adding downlink bandwidth to LTE cell structure
05/03/13   zr      Making max num of meas cells depend on FDD/TDD mode
04/23/13   zr      Allow Event 3D to be reported as defined by TS 25.331, sec. 10.3.7.30
03/29/13   zr      Trimming max LTE cell list size from 15 to 9
10/15/12   jd      Memory Optimization Changes
07/24/12   geg     Upmerge WCDMA.MPSS.1.0 to WCDMA.MPSS.2.0
06/21/12   zr      Added support for LTE TDD cell detection and measurement
06/11/12   vs      MBMS feature cleanup.
06/08/12   zr      Added prototype to determine the active filter index
06/04/12   zr      Adding minimum RSRP macro
03/07/12   zr      Not reporting LTE cell when there is a suitable WCDMA cell on
                   the same frequency
02/27/12   zr      Adding interface to retrieve timing info for LTE cells during HHO
07/06/11   dp      upmerge from offline/main
06/07/11   sa      prune out spurious LTE cells that are not detected for a certain duration of time
05/11/11   sa      Added function prototype.
03/12/11   sa      Fixed issues in W2L code.
01/31/11   sa\nd   Added callback function that will post local command to process 
                   periodic measurements instead of processing it in timer context.
12/8/10    sa      Removed compilation warnings.
11/24/10   sa      Added code to support W2L measurements under feature
                   FEATURE_WCDMA_CM_LTE_SEARCH.
05/03/10   rc       Added function protoypes.
05/08/09   sup      Changing the includes for the renaming of header files  dec.h, 
                    enci.h, l1m.h, srch.h, srchi.h, srchlog.h to wdec.h, wenci.h, 
                    wl1m_v.h, wsrch.h, wsrchi.h, wsrchlog.h
10/03/08   hk       Bringing in 7k Mailine fixes
04/25/05   mc       Added P4 DateTime and Author header.
04/14/05   vb       Externed l1cmsrch_remove_interrat_measurement_event()
01/28/05   rc       Added function declaration for l1cmmeas_event_report_evaluation()
                    function.
12/27/04   rc       1. Added the function declaration for
                       l1cmmeas_reset_cell_reporting_state() function.
08/30/04   rc       1. Added the function declaration for 
                      l1cmmeas_ttt_check_timer_callback() function.
07/21/03   djm      1. addition of DPCH TGPS validation function to be used
                       for validation support of TGPS active to active transitions
01/21/03   djm      1. lint
12/06/02   djm      1. expose cio scaling for srchset filtered_rssi sorting
12/04/02   djm      1. modify mechanism for determining TGPS active/inactive
11/26/02   djm      1. remove test code enum
10/17/02   djm      1. move SRCHCM_ interface macros (unshared) back to l1cmmeas.c
10/02/02   djm      1. modification to use srchcmmeas.h interface for
                       measurement done signal processing.
09/13/02   djm      1. addition of DPCH status info validation function
09/11/02   djm      1. added unittest code for srchcm_signal_ack()
09/06/02   djm      1. added first level functions for TGPS configuration for
                       use by either dl_dpch_notify() setup or measurement
                       control processing.  cut out to FEATURE_CM_SUPPORTED define
                    2. cut out TGPS runtime error processing w/
                       FEATURE_CM_TGPS_RUNTIME_ERROR define
09/04/02   djm      include srchcmmeas.h for srchcm_delete_inter_rat_cell_to_set defn
08/22/02   djm      l1cmsrch_send_l1_measurement_control_msg() added for WPLT
08/21/02   djm      pull interrat measurement data into FEATURE_INTERRAT_HANDOVER_WTOG
07/18/02   djm      addition of l1_notify_rrc_compressed_mode_runtime_error()
07/17/02   djm      file created
===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "wcdma_variation.h"
#include "customer.h"
#include "l1rrcif.h"
#include "wl1m.h"

#include "srchcmmeas.h"
#include "wsrchiratmeas.h"

#ifdef FEATURE_WCDMA_CM_LTE_SEARCH
#include "wsrchlte.h"
#endif

/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/

/* To Tell Driver to remove a cell pointer */
#define SRCHCM_REMOVE_GSM_CELL_FOR_MEAS(cell_ptr) \
           srchcm_delete_inter_rat_cell_to_set( (cell_ptr), SRCH_INTER_RAT_CELL_GSM_TYPE )

#ifdef FEATURE_WTOL_TDD_CM_SUPPORT
/* An invalid CER SNR threshold */
#define L1CMMEAS_CER_SNR_INVALID_THRESH 0xffff 

/* The maximum number of measurements that affects the CER_SNR threshold */
#define L1CMMEAS_CER_SNR_THRESH_MAX_MEAS 6 
#endif /* FEATURE_WTOL_TDD_CM_SUPPORT */
/*===========================================================================

                        FUNCTION PROTOTYPES

===========================================================================*/

#if !defined(FEATURE_WPLT)
/******************************************************************************
*******************************************************************************
              RRC Interface Functions
*******************************************************************************
******************************************************************************/

/*=============================================================================
  FUNCTION     : L1_GET_EUTRA_MEASUREMENT_DATA
  -- prototype located in l1rrcif.h --
=============================================================================*/ 
 
/*=========================================================================
FUNCTION     : L1_GET_INTERRAT_MEASUREMENT_DATA
  --prototype located in l1rrcif.h--
=========================================================================*/


#endif /* !FEATURE_WPLT */

#define L1CMMEAS_INVALID_INDEX (0xFFFF)

#ifdef FEATURE_WCDMA_CM_LTE_SEARCH

#define L1CMMEAS_EUTRA_SRCH_PERIOD_MS 480L

#define L1CMMEAS_FILTER_EUTRA_ENG(filt_eng,rsrp,elapsed_time, filt_coeff, srch_period)      \
    srch_interf_filter_eng(filt_eng, rsrp, elapsed_time,  filt_coeff,srch_period)


/* Up to 9 cells can be measured at once on FDD frequencies */
#define WSRCHIRATMEAS_EUTRA_MAX_NUM_MEAS_CELLS_FDD 9 

/* Status indicator after attempting to insert a EUTRA frequency to the 
   measurement layer database */
typedef enum
{
  /* The frequency was successfully inserted */
  L1CMMEAS_EUTRA_FREQ_ADD_STATUS_SUCCESS,

  /* The frequency list is full */  
  L1CMMEAS_EUTRA_FREQ_ADD_STATUS_LIST_FULL,

  /* The bandwidth information for the cell is invalid */
  L1CMMEAS_EUTRA_FREQ_ADD_STATUS_INVALID_BW,

  /* The blacklist is full so the new frequency's blacklist could not be processed */
  L1CMMEAS_EUTRA_FREQ_ADD_STATUS_BLACKLIST_FULL
} l1cmmeas_eutra_freq_add_status_type;



/**** TO DO ******** IS this structure present in DRV SIDE?? **/

/* Struct for saving and storing the meas response returned from LTE, 
to be passed on to measurements layer */


#define L1CMMEAS_EUTRA_FILTER_COEF_IN_USE(fc)         ((int32)(fc)  != (int32) L1_FILTER_COEF_INVALID)  

#define L1CMMEAS_EUTRA_INVALID_INDEX (0xFF)
#define L1CMMEAS_CELL_AGE_EXPIRY     5

/* Externed*/
extern uint8 filt_idx_used_for_sort;

extern boolean srchcm_eutra_inter_rat_search_done_status;
extern uint8 l1cmmeas_num_eutra_meas_in_prog;

extern wsrchiratmeas_freq_list_struct_type  l1cmmeas_eutra_freq_waitlist_db;

#define L1CMMEAS_GET_FILT_RSRP(cell_ptr,fc_index)   (cell_ptr->filt_params[fc_index].filt_rsrp)
#define L1CMMEAS_GET_FILT_RSRQ(cell_ptr,fc_index)   (cell_ptr->filt_params[fc_index].filt_rsrq)

#define L1CMMEAS_LTE_CELLS_MEM_SIZE   (sizeof(wsrchiratmeas_lte_cell_struct_type)*L1_MAX_EUTRA_CELLS_PER_FREQUENCY)     

typedef struct
{
   uint8 num_cells;
   uint16 phy_cell_id[L1_MAX_REP_EUTRA_CELLS_PER_FREQUENCY];
}l1_eutra_meas_rpt_cells;
#endif

typedef enum
{
  TTT_NOT_SET,
  TTT_SET,
  TTT_WAITING_TO_REPORT,
  TTT_REPORTED
} l1cmsrch_cell_ttt_state_enum_type;


/*
 * Measurement Structure and Tables
 */
typedef struct
{
  uint16                         meas_id;
    /* measurement identity */

  uint16  periodic_rpt_cnt;
    /* Periodic Timer Counter, for when termination expected */
  rex_timer_type * periodic_timer;
    /* Periodic Timer, for timer registration */
  boolean periodic_suspended;

  /* indicates transition into condition where UMTS has fallen below threshold */
  boolean event3a_umts_qual_below_thresh; 

  l1_meas_report_mode_enum_type meas_evt_type;
    /* reporting criteria, periodic or event and params */
  l1_per_rpt_crit_struct_type          periodic_crit;
  l1_inter_sys_event_crit_struct_type  event_crit;

  l1_meas_report_trans_mode_enum_type  rpt_trans_mode;

  /*
   * Measurement Quantity Information
   */
  /* filter_coefficient parameters */
  uint32 fc_index;
    /* filter coefficient storage index */
  boolean bsic_verify_reqd;
    /* just track whether this event configured bsic_verification_required */
  l1_intra_freq_meas_quan_struct_type intraf_meas_quan;
    /* intra-frequency measurement quantity for use in event 3A UTRAN meas */
    /* filter_coef is unused/unsupported  */

  struct
  {
    l1cmsrch_cell_ttt_state_enum_type state;
    uint16  time;
  } ttt[L1_MAX_CELL_MEAS];
    /* time to trigger for each cell, set for this event */

  /*
   * Event 3d identifies a change in the best cell
   *  This variable is the cellid of the current best cell
   */
  srch_gsm_cell_struct_type * event3d_best_cell_ptr;

#ifdef FEATURE_WCDMA_CM_LTE_SEARCH
   // Current best_cell_id
   uint16 event3d_eutra_best_cell_id; 

   // Index of the EARFCN that holds the current best cell
   uint8 event3d_eutra_best_freq_idx;

   wsrchiratmeas_lte_cell_struct_type  *event3d_eutra_best_cell_ptr;
   l1_eutra_meas_rpt_quantity_enum_type eutra_meas_quan;
   l1_eutra_meas_rpt_cells    rpt_cell_db;
#endif
  /*
   * Msg Report Management Information
   */
  uint16 max_num_rpt_cells;
    /* if provided will indicate 1..12 max number of cells to report */

  boolean reporting_cell_status_included;
  /* If reporting cell status not included do not report measured cells */ 

  l1_inter_sys_dch_rpt_quan_struct_type        rpt_quantity;
    /* reporting quantity, include time diff & GSM RSSI? */

} l1cmsrch_inter_rat_event_info_struct_type;  



extern l1cmsrch_inter_rat_event_info_struct_type  l1_inter_rat_event_array[L1_MAX_MEAS_EVENT];
#ifdef FEATURE_WCDMA_CM_LTE_SEARCH
extern l1cmsrch_inter_rat_event_info_struct_type  l1_eutra_inter_rat_event_array[L1_MAX_EUTRA_MEAS_EVENT];
#endif

/******************************************************************************
*******************************************************************************
              INIT Functions
*******************************************************************************
******************************************************************************/

/*=========================================================================

FUNCTION     : L1CMMEAS_INIT

DESCRIPTION  : This function will Initialize the L1CMMEAS subsystem

DEPENDENCIES : None

RETURN VALUE : None

SIDE EFFECTS : None

=========================================================================*/
void l1cmmeas_init ( void );


/******************************************************************************
*******************************************************************************
              Compressed Mode Gap Configuration
*******************************************************************************
******************************************************************************/

/*===========================================================================
FUNCTION     L1_COMPRESSED_MODE_VALIDATE_TGPS_CONFIG

DESCRIPTION
  Validate DPCH TGPS Info provided by RB Configuration Message

DEPENDENCIES
  None

RETURN VALUE
  FALSE if validation failed

SIDE EFFECTS
  None
===========================================================================*/
boolean l1_compressed_mode_validate_tgps_config( l1_cm_info_struct_type * p_cm_info );

/*===========================================================================
FUNCTION     L1_COMPRESSED_MODE_TGPS_CONFIG

DESCRIPTION
  Configure TGPS Compressed Mode Sequences in Setup

DEPENDENCIES
  None

RETURN VALUE
  FALSE if measurement control message processing identified errors in the
  message

SIDE EFFECTS
  None
===========================================================================*/
boolean l1_compressed_mode_tgps_config( l1_cm_info_struct_type * p_cm_info );


/*===========================================================================
FUNCTION     L1_COMPRESSED_MODE_VALIDATE_TGPS_STATUS_INFO

DESCRIPTION
  Validate DPCH Status Info provided by measurement control message

DEPENDENCIES
  None

RETURN VALUE
  FALSE if validation failed

SIDE EFFECTS
  None
===========================================================================*/
boolean l1_compressed_mode_validate_tgps_status_info
           (
               l1_cm_status_info_struct_type * p_cm_status_info
           );


/*===========================================================================
FUNCTION     L1_COMPRESSED_MODE_TGPS_STATUS_CONFIG

DESCRIPTION
  Configure TGPS Compressed Mode Sequences via Measurement Control Message
  DPCH Status Info IE.

DEPENDENCIES
  None

RETURN VALUE
  FALSE if measurement control message processing identified errors in the
  message

SIDE EFFECTS
  None
===========================================================================*/
boolean l1_compressed_mode_tgps_status_config(
                                     uint32 trans_id,
                                     uint32 meas_id,
                                     l1_cm_status_info_struct_type * p_cm_status_info
                                   );


/******************************************************************************
*******************************************************************************
              L1M Entry Points To Compressed Mode Measurement Control
*******************************************************************************
******************************************************************************/

/*===========================================================================
FUNCTION     L1CMSRCH_MSMT_CTRL_MSG_PROCESSING

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
boolean l1cmsrch_msmt_ctrl_msg_processing( l1_meas_cmd_type *meas_cmd );

/*===========================================================================
FUNCTION     L1CMSRCH_PROCESS_CM_MEAS_DONE_SIG

DESCRIPTION
  This function performs wcdma_l1_task  L1_CM_MEAS_DONE_SIG signal processing
  when compressed mode inter-freq/inter-RAT measurements have been completed

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void l1cmsrch_process_cm_meas_done_sig( void );


/*===========================================================================
FUNCTION     L1CMSRCH_LOCAL_CM_MEAS_DONE_PROCESSING

DESCRIPTION
  Handle Compressed Mode Search Done Signal Generated Local Command

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void l1cmsrch_local_cm_meas_done_processing( l1m_local_cmd_type *lcmd );


/*===========================================================================
FUNCTION     L1CMSRCH_TRANSITION_INTO_CELL_DCH

DESCRIPTION
  L1 CM Search Subsystem routine for handling transition into CELL_DCH state

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void l1cmsrch_transition_into_cell_dch( void );


/*===========================================================================
FUNCTION     L1CMSRCH_TRANSITION_FROM_CELL_DCH

DESCRIPTION
  L1 CM Search Subsystem routine for handling transition out of CELL_DCH state

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void l1cmsrch_transition_from_cell_dch( void );

/*=========================================================================
FUNCTION     : L1CMMEAS_CIO_VALUE_SCALED

DESCRIPTION  : This function returns the scaled CIO (cell individual offset)
               value for use in srchset.c for sorting GSM cells based on
               adjusted filtered rssi, ie. filtered_rssi + cio.

DEPENDENCIES : None

RETURN VALUE : None

SIDE EFFECTS : None

=========================================================================*/
int32  l1cmmeas_cio_value_scaled( int16 cio );


/******************************************************************************
*******************************************************************************
        Compressed Mode TGPS Registration & Run-Time Error Reporting
*******************************************************************************
******************************************************************************/

/*=========================================================================
FUNCTION     : L1_IS_TGPS_REGISTERED

DESCRIPTION  : This function will tell the caller whether the TGPS is
               currently registered with the measurement control subsystem
               or not.

DEPENDENCIES : None

RETURN VALUE : TRUE/FALSE is registered or not

SIDE EFFECTS : None

=========================================================================*/
boolean l1_is_tgps_registered ( uint16 tgpsi );


/*=========================================================================
FUNCTION     : L1_UNREGISTER_TGPS

DESCRIPTION  : This function will unregister/remove a compressed mode
               TGPS that has been registered with the measurement control
               subsystem.

DEPENDENCIES : None

RETURN VALUE : None

SIDE EFFECTS : None

=========================================================================*/
void l1_unregister_tgps ( uint16 tgpsi );


/*=========================================================================
FUNCTION     : L1_REGISTER_TGPS

DESCRIPTION  : This function will register a compressed mode TGPS with
               the measurement control subsystem.

DEPENDENCIES : None

RETURN VALUE : TRUE/FALSE - registration successful or not

SIDE EFFECTS : None

=========================================================================*/
boolean l1_register_tgps
       (
          uint16 tgpsi,
          l1_cm_tg_meas_purpose_enum_type  tgmp,
          uint16 measid
       );


/*=========================================================================
FUNCTION     : L1_TGMP_ACTIVE

DESCRIPTION  : This function will determine whether the specified TGMP,
               measurement purpose, is currently active.

DEPENDENCIES : None

RETURN VALUE : None

SIDE EFFECTS : None

=========================================================================*/
boolean l1_tgmp_active
          (
             l1_cm_tg_meas_purpose_enum_type  tgmp
          );


/*=========================================================================
FUNCTION     : L1_COMPRESSED_MODE_RUNTIME_ERROR

DESCRIPTION  : This function will send an CM_TGPS_RUNTIME_ERROR_CMD
               L1 local command to the L1 task for necessary runtime
               error processing.

               This function is called by the L1 CM MGR when it has
               determined that a run-time error, compressed mode gap
               overlap would occur.

DEPENDENCIES : None

RETURN VALUE : None

SIDE EFFECTS : None

=========================================================================*/
void l1cmmeas_tgps_runtime_error ( uint16 tgpsi );


/*=========================================================================
FUNCTION     : L1CMMEAS_NOTIFY_RRC_COMPRESSED_MODE_RUNTIME_ERROR

DESCRIPTION  : This function will send an RRC_CPHY_COMPRESSED_MODE_IND
               notifying RRC of a run-time compressed mode gap failure.

DEPENDENCIES : None

RETURN VALUE : None

SIDE EFFECTS : None

=========================================================================*/
void l1cmmeas_notify_rrc_tgps_runtime_error
       (
         l1cmmeas_cm_runtime_error_cmd_type *p_cmd
       );

#if defined(FEATURE_WPLT)
/*===========================================================================
FUNCTION     L1CMSRCH_SEND_L1_MEASUREMENT_CONTROL_MSG

DESCRIPTION
  L1 CM Search Subsystem WPLT Test Function that will send a measurement
  control message

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
#define WPLT_L1CMMEAS_MODE 9999

void l1cmsrch_send_l1_measurement_control_msg( uint32 choice );

#endif /* FEATURE_WPLT */
/*===========================================================================
FUNCTION     L1CMMEAS_RESET_CELL_REPORTING_STATE

DESCRIPTION
  This function resets the reporting state of all the cells to TTT_NOT_SET
  when we resume on WCDMA from GSM.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void l1cmmeas_reset_cell_reporting_state( void );


/*===========================================================================
FUNCTION     L1CMMEAS_EVENT_REPORT_EVALUATION

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

void l1cmmeas_event_report_evaluation( void );

/*===========================================================================
FUNCTION     L1CMSRCH_REMOVE_INTERRAT_MEASUREMENT_EVENT

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
void l1cmsrch_remove_interrat_measurement_event( uint32 event_idx ,                  
                  l1cmsrch_inter_rat_event_info_struct_type *l1_inter_rat_events_db);


/*=========================================================================
FUNCTION     : L1CMMEAS_UPDATE_TGPS_INFO

DESCRIPTION  : This function will change the status ACTIVE/INACTIVE of
               a currently registered compressed mode sequence.  It will
               also set measurement id when a valid TGMP is provided.

DEPENDENCIES : None

RETURN VALUE : TRUE/FALSE, update succeeded or failed

SIDE EFFECTS : None

=========================================================================*/
boolean l1cmmeas_update_tgps_info(uint16 tgpsi,uint16 measid);



/*===========================================================================
FUNCTION     L1CMMEAS_GSM_TIME_DIFF

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
uint16 l1cmmeas_gsm_time_diff( uint32 mul51_frame_offset, uint32 qs_offset );


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
void l1cmsrch_send_inter_sys_event_msmt_rpt_msg(uint32 event_idx);

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
void l1cmsrch_send_inter_sys_periodic_msmt_rpt_msg(uint32 event_idx, boolean  last_periodic_rpt);


/*===========================================================================
FUNCTION     L1CMSRCH_GET_EMPTY_EVENT

DESCRIPTION
  Grab an empty event from the event array[]

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
uint32 l1cmsrch_get_empty_event( void );

/*===========================================================================
FUNCTION     L1CMSRCH_FIND_EVENT

DESCRIPTION
  Locate the event specified in the event array.  The inter_s_evt parameter
  is only valid/used if rpt_mode == L1_EVENT_TRIGGER_RPT.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
uint32 l1cmsrch_find_event( uint32 measid ,
                       l1cmsrch_inter_rat_event_info_struct_type *l1_inter_rat_event_db);


/*===========================================================================
FUNCTION     L1CMSRCH_PROCESS_PERIODIC_CB

DESCRIPTION
  This callback function get called when INTER-RAT periodic timer expires.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
 void l1cmsrch_process_periodic_cb( uint32  event_idx );

/*===========================================================================
FUNCTION     L1CMSRCH_PROCESS_EVENT3B

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
void l1cmsrch_process_event3b( uint32 event_idx );

/*===========================================================================
FUNCTION     L1CMSRCH_PROCESS_EVENT3C

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
void l1cmsrch_process_event3c( uint32 event_idx );

/*===========================================================================
FUNCTION     L1CMSRCH_PROCESS_EVENT3A

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
void l1cmsrch_process_event3a( uint32 event_idx );

/*===========================================================================
FUNCTION     L1CMSRCH_PROCESS_EVENT3D

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
void l1cmsrch_process_event3d( uint32 event_idx );

/*===========================================================================
FUNCTION     L1CMSRCH_ADD_FILTER_COEF

DESCRIPTION
  This function will add a filter coefficient to the filter coefficient
  storage array if that filter_coef is not corrently in the array.
  Failure is indicated by a return value == L1_MAX_FILTER_COEF_STORAGE.

DEPENDENCIES
  None

RETURN VALUE
  Success, index where filter_coef is stored
  Failure, L1_MAX_FILTER_COEF_STORAGE

SIDE EFFECTS
  None
===========================================================================*/
uint32 l1cmsrch_add_filter_coef( l1_gsm_filter_coef_enum_type filter_coef );

/*===========================================================================
FUNCTION     L1CMSRCH_REMOVE_FILTER_COEF

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
void l1cmsrch_remove_filter_coef( uint32 fc_index, l1_gsm_filter_coef_enum_type filter_coef );

/*===========================================================================
FUNCTION     L1CMMEAS_REMOVE_ARFCN_ARRAY_INDEX

DESCRIPTION
  Deletes the arfcn idx entry from the database.
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  The variable l1cmmeas_no_of_active_arfcns may be changed.
===========================================================================*/
void l1cmmeas_remove_arfcn_array_index( uint16 arfcn_idx );

/*===========================================================================
FUNCTION     L1CMMEAS_INTER_S_FIND_CELL

DESCRIPTION
  This function will search all neighbor set cells to find one that
  has an ARFCN that matches the one provided but a different BSIC.

DEPENDENCIES
  None

RETURN VALUE
  srch_gsm_cell_struct_type * - if not NULL, a pointer to the cell structure
                                for which ARFCN,matches but not BSIC.  
                                NULL if no match was found.
SIDE EFFECTS
  None
===========================================================================*/
srch_gsm_cell_struct_type * l1cmmeas_inter_s_find_cell( W_ARFCN_T arfcn, uint8 bsic_ncc, uint8 bsic_bcc );

/*===========================================================================
FUNCTION     L1CMMEAS_GET_EMPTY_ARFCN_ARRAY_INDX

DESCRIPTION
  This function returns an empty index in l1cmmeas_get_empty_arfcn_array_idx()

DEPENDENCIES
  None

RETURN VALUE
  uint16

SIDE EFFECTS
  None
===========================================================================*/
uint16  l1cmmeas_get_empty_arfcn_array_idx(void);

/*===========================================================================
FUNCTION     L1CMSRCH_MSMT_CTRL_MSG_INTER_RAT_PROCESSING

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
boolean l1cmsrch_msmt_ctrl_msg_inter_rat_processing( l1_meas_cmd_type *meas_cmd );

/*===========================================================================
FUNCTION     L1CMSRCH_MSMT_CTRL_MSG_INTER_FREQ_PROCESSING

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
boolean l1cmsrch_msmt_ctrl_msg_inter_freq_processing( l1_meas_cmd_type *meas_cmd );

/*===========================================================================
FUNCTION     L1CMMEAS_TRANSLATE_DPCH_STATUS_INFO

DESCRIPTION
  Update the translation of DPCH Status Info, filling in the DPCH Info struct
  that can be passed to the L1CMMGR.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  l1cmmeas_cm_config & l1cmmeas_tgps_info global variables are set
===========================================================================*/
boolean l1cmmeas_translate_dpch_status_info(
                                    l1_cm_status_info_struct_type * p_cm_status_info);


/*===========================================================================
FUNCTION     L1CMMEAS_VALIDATE_DPCH_STATUS_INFO

DESCRIPTION
  Validate DPCH Status Info provided by measurement control message

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean l1cmmeas_validate_dpch_status_info(l1_cm_status_info_struct_type * p_cm_status_info);


/*===========================================================================
FUNCTION     L1CMMEAS_CONFIG_DPCH_STATUS_INFO

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
boolean l1cmmeas_config_dpch_status_info( uint32 trans_id,
                                          uint32 meas_id,
                                          l1_cm_status_info_struct_type * p_cm_status_info
                                        );


/*===========================================================================
FUNCTION     L1CMMEAS_SWAP_DRV_CELL

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
void l1cmmeas_swap_drv_cell( srch_gsm_cell_struct_type       * p_cell,
                             srch_gsm_cell_struct_type       * other_p_cell );

/*===========================================================================
FUNCTION     L1CMMEAS_PERFORM_FILTER_PROCESSING

DESCRIPTION  Does the filter processing after RSSI results are  
             received from RSSI and BSIC CM gaps.

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void l1cmmeas_perform_filter_processing(srch_gsm_cell_struct_type *p_cell, 
                                                    boolean rssi_becoming_valid_flag);


/*===========================================================================
FUNCTION     L1CMMEAS utility functions

DESCRIPTION  

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/

W_ARFCN_T  SRCHCM_GET_ARFCN( void * p_drv_cell );
uint8   SRCHCM_GET_BSIC_NCC( void *p_drv_cell );
uint8   SRCHCM_GET_BSIC_BCC( void * p_drv_cell );
uint8   SRCHCM_GET_FRAME_OFFSET( void * p_drv_cell );
uint16  SRCHCM_GET_QS_OFFSET( void * p_drv_cell );
uint32  SRCHCM_GET_HYPER_FRAME_OFFSET( void * p_drv_cell );
int16  SRCHCM_GET_FREQUENCY_OFFSET( void * p_drv_cell );

void SRCHCM_SET_HOST_INFO_PTR( void * p_drv_cell, void * p_cell);
void * SRCHCM_GET_HOST_INFO_PTR( void * p_drv_cell );
void  SRCHCM_SET_BSIC_VERIFY_REQD( void * p_drv_cell, boolean bsic_verify_reqd );

boolean  SRCHCM_IS_RSSI_VALID( void * p_drv_cell );
int16  SRCHCM_GET_RSSI_X16( void * p_drv_cell );
boolean  SRCHCM_IS_BSIC_VALID( void * p_drv_cell );



/*===========================================================================
FUNCTION     L1CMSRCH_REMOVE_MEAS

DESCRIPTION
  This function will remove measurement database for a system.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void l1cmsrch_remove_meas(uint32 meas_id,l1_rat_type_enum_type rat);

#ifdef FEATURE_WCDMA_CM_LTE_SEARCH
/*===========================================================================
FUNCTION     L1CMSRCH_MSMT_CTRL_MSG_EUTRA_INTER_RAT_PROCESSING

DESCRIPTION
  Handle Measurement Control Message EUTRA Inter-RAT Related Processing
  as specified in section 8.4.1.3 of 25.331

DEPENDENCIES
  None

RETURN VALUE
  FALSE if measurement control message processing identified errors in the message

SIDE EFFECTS
  None
===========================================================================*/
boolean l1cmsrch_msmt_ctrl_msg_eutra_inter_rat_processing( l1_meas_cmd_type *meas_cmd );

/*===========================================================================
FUNCTION     L1CMSRCH_REMOVE_EUTRA_FREQ_INFO

DESCRIPTION
  This function will remove eutra freuency from the frequency info list

DEPENDENCIES
  None

RETURN VALUE
  NONE

SIDE EFFECTS
  None
===========================================================================*/
void l1cmsrch_remove_eutra_freq_info(uint16 num_earfcn, uint16 *list_earfcn);



/*===========================================================================
FUNCTION     L1CMSRCH_ADD_EUTRA_FREQ_INFO

DESCRIPTION
  This function will add eutra freuency in frequency info list and will 
  update the added frequency in the CM drivers to do search on that freq.

DEPENDENCIES
  None

RETURN VALUE
  Status of the insertion attempt

SIDE EFFECTS
  None
===========================================================================*/
l1cmmeas_eutra_freq_add_status_type l1cmsrch_add_eutra_freq_info(l1_meas_eutra_frequency_list_type*
                                                                 add_freq_list);
/*===========================================================================
FUNCTION     L1CMSRCH_EUTRA_ADD_FILTER_COEF

DESCRIPTION
  This function will add a filter coefficient to the filter coefficient
  storage array if that filter_coef is not corrently in the array.
  Failure is indicated by a return value == L1_MAX_FILTER_COEF_STORAGE.

DEPENDENCIES
  None

RETURN VALUE
  Success, index where filter_coef is stored
  Failure, L1_MAX_FILTER_COEF_STORAGE

SIDE EFFECTS
  None
===========================================================================*/
uint32 l1cmsrch_eutra_add_filter_coef( l1_common_filter_coef_enum_type filter_coef );


/*===========================================================================
FUNCTION     L1CMSRCH_EUTRA_REMOVE_FILTER_COEF

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
void l1cmsrch_eutra_remove_filter_coef( uint32 fc_index,l1_common_filter_coef_enum_type filter_coef );

/*===========================================================================
FUNCTION     L1CMSRCH_GET_EUTRA_EMPTY_EVENT

DESCRIPTION
  Grab an empty event from the event array[]

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
uint32 l1cmsrch_get_empty_eutra_event( void );

/*===========================================================================
FUNCTION     L1CMSRCH_GET_EUTRA_NBR_FREQ_INFO

DESCRIPTION
  

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean l1cmmmeas_get_eutra_nbr_freq_info( l1_inter_sys_eutra_freq_list_struct_type * p_freq_list );

/*===========================================================================
FUNCTION     L1CMSRCH_REMOVE_EUTRA_FREQ_DB

DESCRIPTION
  This function will remove eutra freuency from the frequency info list

DEPENDENCIES
  None

RETURN VALUE
  NONE

SIDE EFFECTS
  None
===========================================================================*/
 boolean l1cmsrch_remove_eutra_freq_db(uint32 earfcn);

/*===========================================================================
FUNCTION     L1CMSRCH_RESET_EUTRA_FREQ_INFO

DESCRIPTION
  This function will remove eutra freuency from the frequency info list

DEPENDENCIES
  None

RETURN VALUE
  NONE

SIDE EFFECTS
  None
===========================================================================*/
void l1cmsrch_reset_eutra_freq_info(void);


/*===========================================================================
FUNCTION     L1CMMEAS_EUTRA_EVENT_REPORT_EVALUATION

DESCRIPTION
  This function performs processing of all active events to determine
  whether reporting is required.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
 void l1cmmeas_eutra_event_report_evaluation( uint8 freq_idx );

/*===========================================================================
FUNCTION     L1CMSRCH_PROCESS_EUTRA_EVENT3B

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
void l1cmsrch_process_eutra_event3b( uint32 event_idx ,uint8 freq_idx);


/*===========================================================================
FUNCTION     L1CMSRCH_PROCESS_EUTRA_EVENT3C

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
void l1cmsrch_process_eutra_event3c( uint32 event_idx,uint8 freq_idx );

/*===========================================================================
FUNCTION     L1CMSRCH_PROCESS_EUTRA_EVENT3A

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
void l1cmsrch_process_eutra_event3a( uint32 event_idx ,uint8 freq_idx);


/*===========================================================================
FUNCTION     L1CMSRCH_PROCESS_EUTRA_EVENT3D

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
void l1cmsrch_process_eutra_event3d( uint32 event_idx,uint8 freq_idx );



/*===========================================================================
FUNCTION     L1CMMEAS_RESET_EUTRA_FREQ_DB

DESCRIPTION
  This function will reset the frequency database at measurement side.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void l1cmmeas_reset_eutra_freq_db(void);

/*===========================================================================
FUNCTION     L1CMMEAS_SORT_LTE_CELLS

DESCRIPTION
  Sort LTE cells based on meas quantities.
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void l1cmmeas_sort_lte_cells(uint8 max_cnt);

/*===========================================================================
FUNCTION     L1CMMEAS_CHECK_IF_FILT_NOT_USED
DESCRIPTION
  This function will check if filter index is using by some other measurement or not

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
 boolean l1cmmeas_check_if_filt_not_used(
  uint8 filt_idx,
  uint8 meas_id
  );


/*===========================================================================
FUNCTION     L1CMMEAS_PRINT_MEAS_DB
DESCRIPTION
  This function will print all active measurements parameters.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void l1cmmeas_print_meas_db(void);

/*===========================================================================
FUNCTION     L1CMMEAS_RESET_CELL_DB
DESCRIPTION
  This function will reset the cell database in a frequency.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void l1cmmeas_reset_cell_db(uint8 freq_idx);

/*===========================================================================
FUNCTION     L1CMSRCH_EUTRA_PROCESS_PERIODIC

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
 void l1cmsrch_process_eutra_periodic_cb( uint32  event_idx );

/*===========================================================================
FUNCTION     L1CMSRCH_EUTRA_PROCESS_PERIODIC_RPTS

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
 void l1cmsrch_eutra_process_periodic_rpts( 
                             uint32 event_idx);

/*===========================================================================
FUNCTION     L1CMSRCH_SEND_EUTRA_EVENT_MSMT_RPT_MSG

DESCRIPTION
  This function sends measurement report message to RRC

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void l1cmsrch_send_eutra_event_msmt_rpt_msg(
                                 uint32 event_idx,
                                 uint8 freq_idx);

/*===========================================================================
FUNCTION     L1CMSRCH_EUTRA_RESET_TTT_ARRAY_STATE

DESCRIPTION
  This function will reset cell measurement data for event_idx

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void l1cmsrch_eutra_reset_ttt_array_state( uint8 event_idx );

/*===========================================================================
FUNCTION     L1CMMEAS_SHOULD_SEND_LTE_MEAS_RPT

DESCRIPTION
  Determines if the LTE measurement with the EARFCN specified by the given
  freq_indx should be reported to RRC.

DEPENDENCIES
  None

RETURN VALUE
  TRUE if the measurement should be reported, FALSE otherwise.

SIDE EFFECTS
  None
===========================================================================*/
boolean l1cmmeas_should_send_lte_meas_rpt(uint16 freq_indx);


int8 wsrchiratmeas_meas_quant_sort_comp_fn
(
  const void *p1,
  const void *p2
);

void l1cmmeas_sort_eutra_meas_cell_list(uint32 event_idx, l1_eutra_measured_results_struct_type* msmts);
int8 l1cmmeas_eutra_cell_list_earfcn_cmp_fn(const void* left, const void* right);
int8 l1cmmeas_eutra_cell_list_rsrp_cmp_fn(const void* left, const void* right);
int8 l1cmmeas_eutra_cell_list_rsrq_cmp_fn(const void* left, const void* right);
uint8 l1cmmeas_get_num_freqs_used(uint32 earfcn);

#endif/*FEATURE_WCDMA_CM_LTE_SEARCH */

/*===========================================================================
FUNCTION     L1CMMEAS_SHOULD_SEND_LTE_MEAS_RPT

DESCRIPTION
  Determines if the LTE measurement with the EARFCN specified by the given
  freq_indx should be reported to RRC.
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

void l1cmmeas_process_interrat_periodic_rpts(uint32 event_idx);

/*===========================================================================
  FUNCTION
    L1CMSRCH_FIND_LTE_CELL_IN_SORTED_LIST
 
  DESCRIPTION
    This function searches the measurement layer cell database for a cell with
    the given EARFCN and ID. If found, its index in wsrchiratmeas_sorted_lte_cell
    is returned.
    
  DEPENDENCIES
    None
  
  RETURN VALUE
    Cell index on success, 0xff on failure.
  
  SIDE EFFECTS
    None
 ===========================================================================*/
extern uint8 l1cmsrch_find_lte_cell_in_sorted_list
(
  uint32 earfcn, 
  uint16 phy_cell_id
);

/* LTE TDD utility functions */
#ifdef FEATURE_WTOL_TDD_CM_SUPPORT
int8 l1cmmeas_lte_rsrp_cmp_fn(const void* left, const void* right);
int8 l1cmmeas_lte_sss_corr_cmp_fn(const void* left, const void* right);
void l1cmmeas_lte_tdd_handle_cell_detection(const wsrchlte_cm_meas_rsp_info_struct_type* result);
void l1cmmeas_lte_tdd_handle_cell_measurement(const wsrchlte_cm_meas_rsp_info_struct_type* result);
void l1cmmeas_prepare_eutra_cell_list_for_drv(wsrchlte_cm_meas_rsp_info_struct_type* result);
#endif /* FEATURE_WTOL_TDD_CM_SUPPORT */

/*===========================================================================
  FUNCTION
    L1CMSRCH_SEND_SUSPEND_IRAT_PERIODIC_MEAS
  
  DESCRIPTION
    This function checks if periodic measurement was supposed to be sent during LTA tuneaway
  gaps. 
    
  DEPENDENCIES
    None
  
  RETURN VALUE
   None.
  
  SIDE EFFECTS
    None
 ===========================================================================*/
void l1cmsrch_send_suspended_irat_periodic_meas(void);
/*===========================================================================
  FUNCTION
    L1CMSRCH_SEND_SUSPEND_INTERRAT_PERIODIC_MEAS
  
  DESCRIPTION
    This function checks if GSM periodic measurement was supposed to be sent during LTA tuneaway
  gaps. 
    
  DEPENDENCIES
    None
  
  RETURN VALUE
   None.
  
  SIDE EFFECTS
    None
 ===========================================================================*/
void l1cmsrch_send_suspended_interrat_periodic_meas(void);
#ifdef FEATURE_WCDMA_CM_LTE_SEARCH
/*===========================================================================
  FUNCTION
    L1CMMEAS_SEND_SUSPEND_EUTRA_PERIODIC_MEAS
  
  DESCRIPTION
    This function checks if EUTRA periodic measurement was supposed to be sent during LTA tuneaway
  gaps. 
    
  DEPENDENCIES
    None
  
  RETURN VALUE
   None.
  
  SIDE EFFECTS
    None
 ===========================================================================*/
void l1cmsrch_send_suspended_eutra_periodic_meas(void);
#endif /*FEATURE_WCDMA_CM_LTE_SEARCH*/
#endif /* L1CMMEAS_H */
