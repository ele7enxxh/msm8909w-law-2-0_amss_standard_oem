#ifndef GPL1_GPRS_UTILS_H
#define GPL1_GPRS_UTILS_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                  L1   U T I L I T I E S   P R O C E S S I N G
                       H E A D E R   F I L E  F O R  G P R S

DESCRIPTION
   Layer 1 utility function module for GPRS 


Copyright (c) 2001-2013 Qualcomm Technologies, Inc.

===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/gl1/inc/gpl1_gprs_utils.h#1 $

when       who      what, where, why
--------   -------- ---------------------------------------------
11/03/16   bg        CR983597 FR30575: SR-DSDS G+L with G to L tuneway - GL1 changes
29/12/14   sp/aga    CR774163: Update GFW to active_gap when opening GTA gap
03/10/14   aga    CR732455 Changes required to tune away framework for TH 1.0 single sim build 
02/06/10   cja      Correct typo in #define GPL1_GPRS_UTILS_H
04/11/05   og       Adjustements to start time checks, resolves CR 79207.
02/06/05   og       Removal of header-header includes.
09/07/04   ws     Added shift_out_fbi_e function
06/11/03   gfr    Support for quad-band.
11/07/03   ws     Added  gpl1_gprs_is_frame_num_later()
08/07/03   gw     GRR/L1 SCE cleanup. 
20/05/03   ws     Modifications to use correct starting time calculation
14/02/03   pjr    Added PBCCH to enum, (will be removed later)
13/02/03   ws     Added prototype for gpl1_gprs_get_pr_value()
04/12/02   DLH    Modified gpl1_gprs_calculate_arfcns proto.
09/18/02   ws     Added prototype for  gpl1_gprs_adjust_51_starting_time()
09/06/02   pjr    Changed change_value parameter of gpl1_gprs_adjust_mod_num
                  to int32.
09/05/02   pjr    Modified gpl1_gprs_get_starting_time to return a boolean.
08/19/02   pjr    Added prototype for gpl1_gprs_adjust_mod_num()
06/27/02   ws     Added support of gpl1_gprs_get_starting_time() and 
                  gpl1_gprs_chk_blk_bndry()
06/21/02   DLH    Added gpl1_gprs_calculate_paging_data
06/21/02   DLH    Added gpl1_gprs_calculate_arfcns
06/21/02   DLH    Added gpl1_gprs_get_arfcns
04/12/01   WS     Initial Version

===========================================================================*/


/* Avoid including header files */

/*===========================================================================

                         DATA DECLARATIONS

===========================================================================*/



/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/

/*===========================================================================*/

enum
{
    PRACH,
    PTCCH,
    NORM,
    PBCCH
};

/*===========================================================================

FUNCTION  GPL1_GPRS_CALCULATE_PAGING_DATA



DESCRIPTION
  This function does the paging parameter calculations as in GSM 05.02
  section 6.5.6

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void gpl1_gprs_calculate_paging_data(void);

/*===========================================================================

FUNCTION  GPL1_GPRS_GET_ARFCN

DESCRIPTION
  This function calculates the next frequency when frequency hopping. The
  algorithm is given in GSM 05.02 section 6.2.3.

DEPENDENCIES
  pointer to channel data, offset

RETURN VALUE
  next_ARFCN (from mobile frequency allocation list)

SIDE EFFECTS
  None

===========================================================================*/
ARFCN_T gpl1_gprs_get_ARFCN(frequency_information_T *channel_data, uint8 offset, uint32 input_FN);

/*===========================================================================

FUNCTION  GPL1_GPRS_CALCULATE_ARFCNS

DESCRIPTION
  This function calculates 4 hopping frequencies for any type of GPRS radio block.
  Hence the use of the Mode parameter.  The returns a pointer to an array of 4 freqs.
  This function does the paging parameter calculations as in GSM 05.02
  section 6.5.3.

DEPENDENCIES
  pointer to frequency list.

RETURN VALUE
  next frequency ( from mobile frequency allocation list )

SIDE EFFECTS
  None

===========================================================================*/
void gpl1_gprs_calculate_arfcns(ARFCN_T arfcns[4], frequency_information_T *channel_data, uint8 mode, gas_id_t gas_id);

/*===========================================================================

FUNCTION  GPL1_GPRS_CALCULATE_NCELL_ARFCNS

DESCRIPTION
  Like gpl1_gprs_calculate_arfcns() but takes a frame number argument to
  allow for ncell-serving cell offset.
  
  Only works for mode==NORM and mode==PBCCH.

DEPENDENCIES
  None

RETURN VALUE
  next frequency (from mobile frequency allocation list)

SIDE EFFECTS
  None

===========================================================================*/
void gpl1_gprs_calculate_ncell_arfcns( ARFCN_T                  arfcns[4], 
                                       frequency_information_T *channel_data_ptr, 
                                       uint8                    mode, 
                                       uint32                   ncell_FN );

/*===========================================================================

FUNCTION gpl1_gprs_adjust_51_starting_time

DESCRIPTION
  This function adjusts the starting time from mod42432 format to the absolute
  FN as specified in (GSM 04.18 10.5.2.38) 

DEPENDENCIES
  None

RETURN VALUE
  starting_time_elapsed - true if the starting time has passed
                          false if the starting time is in the future

SIDE EFFECTS
  Locks out the isr while starting time is being calculated

===========================================================================*/
boolean gpl1_gprs_adjust_51_starting_time( uint32*  start_time_ptr, gas_id_t gas_id );

/*===========================================================================

FUNCTION gpl1_gprs_chk_blk_bndry

DESCRIPTION
  This function checks the supplied frame number lies on a block boundary.
  If NOT on a block boundary returns TRUE.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void gpl1_gprs_chk_blk_bndry(uint32 *st_ptr);

/*===========================================================================

FUNCTION gpl1_gprs_get_starting_time

DESCRIPTION
  This function checks the supplied starting time and determines adjusts the
  value to lie on a block boundary.

DEPENDENCIES
  None

RETURN VALUE
  Boolean indicating if starting time has elapsed.

SIDE EFFECTS
  None

===========================================================================*/
boolean gpl1_gprs_get_starting_time( uint32*    start_time_out_ptr,
                                     uint32     start_time_in,
                                     channel_t  st_mode, gas_id_t gas_id );


/*===========================================================================

FUNCTION  GPL1_GPRS_ADJUST_MOD_NUM

DESCRIPTION
  This function takes the input value and applies the change value, a check is
  then made against the modulo number to ensure the result is still in the
  correct range.

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS
  None

===========================================================================*/
uint32 gpl1_gprs_adjust_mod_num(uint32 input_value, int32 change_value, int32 adjust_mod);

/*===========================================================================

FUNCTION  GPL1_GPRS_GET_PR_VALUE

DESCRIPTION
  This function retrieves the PR value from either an RLC/MAC control block
  or an RLC data block. The retrieved bits are then passed through the
  l1_compute_pr value which converts the value to dBx16_T

DEPENDENCIES

RETURN VALUE
  Pr_value in dBx16_T

SIDE EFFECTS
  None

===========================================================================*/

dBx16_T gpl1_gprs_get_pr_value(uint8 *data_block);


/*===========================================================================

FUNCTION gpl1_gprs_is_frame_num_later

DESCRIPTION
  Determines if one frame number is later than another taking into account
  wrapping of the frame number.


When a > b
  |--------------------------b----------a----------------------|
  |                  |                         |               |
  |                  |<---FN_WRAP_THRESHOLD--->|               |
 a is later than b
  
  |---------b------------------------------------------a-------|
  |                  |                         |               |
  |                  |<---FN_WRAP_THRESHOLD--->|               |
 a is not later than b

  
When b > a
  |---------------------a----------b---------------------------|
  |       |                                       |            |
  |       |<-FN_MAX_PLUS_ONE - FN_WRAP_THRESHOLD->|            |
 a is not later than b
  
  |-----a----------------------------------------------b-------|
  |       |                                       |            |
  |       |<-FN_MAX_PLUS_ONE - FN_WRAP_THRESHOLD->|            |
 a is later than b

  |                                                            |
  |                                                            |
  |                                                            |
  |<---------------------FN_MAX_PLUS_ONE---------------------->|

DEPENDENCIES
  None

RETURN VALUE
  TRUE       fn_a is later than fn_b
  FALSE      fn_a is not later than fn_b

SIDE EFFECTS
  None
===========================================================================*/
boolean gpl1_gprs_is_frame_num_later( uint32 fn_a, uint32 fn_b);

#include "geran_variation.h"
/*===========================================================================

FUNCTION  GPL1_GPRS_SHIFT_OUT_FBI_E

DESCRIPTION
  This function retrieves the PR value from either an RLC/MAC control block
  or an RLC data block. The retrieved bits are then passed through the
  l1_compute_pr value which converts the value to dBx16_T

DEPENDENCIES

RETURN VALUE
  Pr_value in dBx16_T

SIDE EFFECTS
  None

===========================================================================*/
void gpl1_gprs_shift_out_fbi_e(uint8 *data_ptr,uint8 len);


typedef enum
{
  G2X_TA_PRI_RESERVED = 0, 
  G2X_TA_PRI_TBF_TS_CHANGE,
  G2X_TA_PRI_WID_SEARCH,
  G2X_TA_PRI_SERV_BCCH,
  
#ifdef FEATURE_GSM_GPRS_READ_CCCH_IN_XFER
  G2X_TA_PRI_SERV_PCH,
#endif /* FEATURE_GSM_GPRS_READ_CCCH_IN_XFER */

  G2X_TA_PRI_TRM_UNLOCK, 
  G2X_TA_PRI_MAX
} gpl1_g2x_ta_priority_t;


typedef enum 
{ 
  NO_ACT = 0,
  WAITING,
  NEXT_ACT,
  SUSPENDED, 
  ONGOING,
  STOPPED
} gpl1_g2x_ta_act_status_t;

typedef enum 
{ 
  RRBP_ONGOING = 0,
  HL_VETO,
  T3192_RUNNING,
  TEST_MODE_ON, 
  TIMING_ADVANCE_NOT_VALID,
  DS_ABORT_TIMEDOUT,
  G2X_TA_TRM_NOT_GRANTED
} gpl1_g2x_ta_abort_reason_t;


typedef enum 
{ 
  G2X_TA_DISABLE_RXTX_NULL = 0,
  G2X_TA_DISABLE_RXTX_SUSPENDING,
  G2G_TA_DISABLE_RXTX_TRM_WAIT_SET_FW_ACTIVE_GAP,
  G2X_TA_DISABLE_RXTX_TRM_WAIT,
  G2X_TA_DISABLE_RXTX_FW_LOAD_WAIT,
  G2X_TA_DISABLE_RXTX_SUSPENDED, /* Used by G2X_TA_PRI_WID_SEARCH and G2X_TA_PRI_SERV_BCCH */
  G2X_TA_DISABLE_RXTX_ABORTING,  /* Used by G2X_TA_PRI_WID_SEARCH and G2X_TA_PRI_SERV_BCCH */
 // G2X_TA_DISABLE_RXTX_ABORTED  /* Used by G2X_TA_PRI_WID_SEARCH and G2X_TA_PRI_SERV_BCCH */
} gpl1_g2x_ta_disable_rx_tx_state;


typedef struct
{
  trm_client_enum_t         client_id;
  uint32                    frame_number;
  uint16                    duration;
  gpl1_g2x_ta_act_status_t  status; 
  async_ccch_type_t         async_ccch_type; /* TBF Suspension or GBTA */
} gpl1_g2x_ta_table_element_t;


typedef struct 
{
  uint32                          nbcch_fn;
  uint32                          wIDsearch_fn;
} gpl1_g2x_ta_data_sub_act_info_t;


typedef struct 
{
  trm_client_enum_t               tbf_client_id;
  trm_client_enum_t               winning_client_id;
  trm_unlock_event_enum_t         event;
  uint32                          unlock_in_sclk;
  trm_reason_enum_t               winning_client_reason;
  uint32                          prev_g2x_end_FN;
} gpl1_g2x_ta_trm_unlock_t;

typedef struct
{
  gpl1_g2x_ta_trm_unlock_t trm_unlock;
  gpl1_g2x_ta_data_sub_act_info_t non_trm_act; 
  gpl1_g2x_ta_table_element_t table[G2X_TA_PRI_MAX];	
} gpl1_g2x_ta_data_t;



boolean gpl1_g2x_ta_update_table_field( gpl1_g2x_ta_priority_t pri, trm_client_enum_t client_id, uint32 frame_number, uint16 duration, gas_id_t gas_id);
boolean gpl1_g2x_ta_cancel_table_field( gpl1_g2x_ta_priority_t pri, gas_id_t gas_id);
boolean gpl1_g2x_ta_suspend_table_field( gpl1_g2x_ta_priority_t pri, gas_id_t gas_id);
boolean gpl1_g2x_ta_is_suspension_ongoing(gas_id_t gas_id, gpl1_g2x_ta_priority_t *pri_ongoing);
boolean gpl1_g2x_ta_is_activty_running(gpl1_g2x_ta_priority_t *pri_ongoing,gas_id_t gas_id);
boolean gpl1_g2x_ta_stop_all_act(gas_id_t gas_id); 
void gpl1_g2x_ta_data_init(gas_id_t gas_id);
void gpl1_g2x_ta_debug_table(gas_id_t gas_id);

void gpl1_g2x_ta_scheduler(gas_id_t gas_id);
gpl1_g2x_ta_priority_t gpl1_g2x_ta_fire_sequencer(gas_id_t gas_id);
boolean gpl1_g2x_ta_set_ongoing(gpl1_g2x_ta_priority_t pri, gas_id_t gas_id);
boolean gpl1_g2x_ta_reset_ongoing(gas_id_t gas_id);
boolean gpl1_g2x_ta_abort(gpl1_g2x_ta_abort_reason_t reason_abort, gas_id_t gas_id);


boolean gpl1_g2x_ta_suspend_all_table(gas_id_t gas_id);
boolean gpl1_g2x_ta_next_planned_suspension(uint32 *suspension_fn, uint32 *resume_fn ,gas_id_t gas_id);
boolean gpl1_g2x_ta_is_act_planned(gpl1_g2x_ta_priority_t pri, gas_id_t gas_id);
boolean gpl1_g2x_ta_is_TS_act_imminent(gas_id_t gas_id);
void gpl1_gprs_ds_init_pch_suspension(gas_id_t gas_id);
boolean gl1_is_geran_tuneaway_allowed( gas_id_t gas_id );
extern gas_id_t gl1_get_gas_id_for_ptm_client( void );


#ifdef FEATURE_G2X_TUNEAWAY
gpl1_g2x_ta_act_status_t gpl1_g2x_ta_get_act_status(gpl1_g2x_ta_priority_t pri, gas_id_t gas_id);
boolean gpl1_g2x_ta_are_act_colliding(gpl1_g2x_ta_priority_t pri1, gpl1_g2x_ta_priority_t pri2, gas_id_t gas_id);
#endif /* FEATURE_G2X_TUNEAWAY */

#endif

