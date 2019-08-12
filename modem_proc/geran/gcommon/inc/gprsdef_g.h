/*****************************************************************************
***
*** TITLE
***
***  GPRS COMMON DEFINITIONS
***
***
*** DESCRIPTION
***
***  Provides a common point of reference for all GPRS file specific data
***  data types, defines, scaling factors and wrapping functions. All GPRS
***  modules should include this file.
***
***
*** Copyright (c) 2001-2013 Qualcomm Technologies, Inc.
*** All Rights Reserved. QUALCOMM Proprietary.
***
*** Export of this technology or software is regulated by the U.S.
*** Government. Diversion contrary to U.S. law prohibited.
***
*****************************************************************************/


/*****************************************************************************
***
***                        EDIT HISTORY FOR MODULE
***
***  This section contains comments describing changes made to the module.
***  Notice that changes are listed in reverse chronological order.
***
*** $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/gcommon/inc/gprsdef_g.h#1 $
***
*** when       who     what, where, why
*** --------   ---     ------------------------------------------------------
*** 10-04-01   tjw     Add customer.h and comdef.h to all SU API files
*** 07/01/09   sjw     Moved INVALID_ARFCN and ARFCNS_EQUAL into gprsdef.h
***
*****************************************************************************/

#ifndef INC_GPRSDEF_G_H
#define INC_GPRSDEF_G_H


/*****************************************************************************
***
***     Included Files
***
*****************************************************************************/

#include "geran_variation.h"
#ifndef CUSTOMER_H
  #include "customer.h"
#endif

#ifndef COMDEF_H
  #include "comdef.h"
#endif
#include "geran_multi_sim.h"

#ifdef FEATURE_GSM_DTM
#include "nas_exp.h"
#include "sys_type.h"
#include "rr_l1.h"
#include "rr_l1_g.h"
#endif /* FEATURE_GSM_DTM */

/*****************************************************************************
***
***     Public Type Definitions
***
*****************************************************************************/
typedef enum
{
  GERAN_CLIENT_NULL = 0,
  GERAN_CLIENT_GL1  = 1,
  GERAN_CLIENT_GL2  = 2,
  GERAN_CLIENT_GRR  = 3,
  GERAN_CLIENT_GMAC = 4,
  GERAN_CLIENT_GRLC = 5,
  GERAN_CLIENT_GLLC = 6,
  GERAN_CLIENT_GDRV = 7,
  GERAN_CLIENT_MDSP = 8,
  GERAN_CLIENT_MAX
} geran_client_enum_t;

/*****************************************************************************
***
***     Public Defines & Constants
***
*****************************************************************************/

/*==================================================================
** Time period constants definition. To be refined by the L1 team
**==================================================================
*/

/*------------------------------------
** timeslot, frame and block periods
**------------------------------------
*/
#define TIMESLOT_PERIOD_NS        (577)
#define FRAME_PERIOD_NS           (8*TIMESLOT_PERIOD_NS)
#define FRAME_PERIOD_MS           (FRAME_PERIOD_NS/1000)
#define BLOCK_PERIOD_NS           (4*FRAME_PERIOD_NS)
#define BLOCK_PERIOD_MS           (BLOCK_PERIOD_NS/1000)

/* Guard timer for recovery  during uplink tbf establishment - Failure
** for RACH Response or No Response for Est Req made through PDAN.
*/
#define TBF_EST_REQ_GUARD_TMR    (15*1000) /* 15 seconds */

// ===========================================================================
//  GPRS Common Type Defines
//
//  Simple types that benefit from type naming for the purposes of
//  appreciability and code cohesion.
// ===========================================================================


// ===========================================================================
//  GPRS Common Primary Definitions
// ===========================================================================


/********************************************************
*               NEW STRUCTURES FOR IPA                  *
********************************************************/
typedef enum
{
  NO_DEL,
  DEL_ONE,
  DEL_ALL
} delete_pdu_cause_t;

typedef enum
{
  NO_CELL_CHANGE_NO_PURGE      = 0x0, /* no cell reselection has happened between no service to service */
  CELL_CHANGE_PURGE_GMM_SIG    = 0x1, /* cell reselection has happened between no service to service */
  NO_CELL_CHANGE_PURGE_GMM_SIG = 0x2  /* no cell reselection has happened but grlc needs to delete gmm messages. */
                                      /* Special case for T3142 AND T3172 timers */
} action_on_ps_access_t;


// ===========================================================================
//  GPRS SNDCP Primary Definitions
// ===========================================================================

// Things that contribute to the scaling or performance of the GPRS stack.


// ===========================================================================
//  GPRS LLC Primary Definitions
// ===========================================================================

// Things that contribute to the scaling or performance of the GPRS stack:


// ===========================================================================
//  GPRS RLC Primary Definitions
// ===========================================================================

// Things that contribute to the scaling or performance of the GPRS stack:


// ===========================================================================
//  GPRS PL1 Primary Definitions
// ===========================================================================

// Things that contribute to the scaling or performance of the GPRS stack.


// ===========================================================================
//  GPRS xxxx Primary Definitions
// ===========================================================================

// Things that contribute to the scaling or performance of the GPRS stack.


/*****************************************************************************
***
***     Public Data References
***
*****************************************************************************/

#ifdef FEATURE_GSM_DTM

typedef enum
{
  NO_TBF_PRESENT,
  UL_TBF_PRESENT,
  DL_TBF_PRESENT,
  UL_DL_TBF_PRESENT
} dtm_tbf_type_t;


typedef struct
{
  byte     alpha;
  byte     t_avg_w;
  byte     t_avg_t;
  boolean  pc_meas_chan;
  byte     n_avg_i;
  byte     gprs_ms_txpwr_max_cch;
  boolean  access_burst_type;
  byte     drx_timer_max;
  uint8    bep_period;
} gprs_l1_cell_opt_t;

typedef struct
{
   channel_type_T       channel_type;
   byte                 subchannel;
   byte                 TN;
}cs_chan_info_t;


typedef struct
{
  boolean                        DTX_indicator;        /*True = DTX enabled*/
  byte                           power_level;      /*CS power level to use*/
  channel_mode_T                 channel_mode;
  cs_chan_info_t                 cs_chan_info;
  multirate_config_T             multi_rate_config;    /*Used to define AMR channel*/
  boolean                        cipher_flag; 
  cipher_algorithm_T             cipher_algorithm;

}cs_chan_desc_t;

#endif /* FEATURE_GSM_DTM */

#ifdef FEATURE_GPRS_PS_HANDOVER

typedef enum
{
  PSHO_GTOW_INITIATE,/* This is only used by GMAC in case of GTOW
                        PS  handover to inform GRR to initiate GTOW PSHO*/
  PSHO_SUCCESS,
  PSHO_FAILURE
}psho_status_t;

typedef enum
{
  PSHO_GTOG,
  PSHO_GTOW,
  PSHO_WTOG,
  PSHO_NONE
}psho_type_t;

#endif /* FEATURE_GPRS_PS_HANDOVER */

/*****************************************************************************
***
***     Public Macros
***
*****************************************************************************/

/* ARFCN_T helper macros, see also grr/inc/rr_multiband.h */
#define INVALID_ARFCN  0xFFFF

#define ARFCNS_SAME_CHANNEL(a,b) ((a).num == (b).num)

  #define ARFCNS_SAME_BAND(a,b) ((a).band == (b).band)
  #define ARFCNS_SAME_CHANNEL_AND_BAND(a,b) (ARFCNS_SAME_CHANNEL(a,b) && ARFCNS_SAME_BAND(a,b))

/* Macro ARFCNS_EQUAL invokes a function for GERAN clients because the macro 
   has a conditional compilation which is not permitted in the API, but within
   GERAN continue to use the macro */
#ifdef ARFCNS_EQUAL
#undef ARFCNS_EQUAL
#endif
#define ARFCNS_EQUAL(a,b) ARFCNS_SAME_CHANNEL_AND_BAND((a),(b))

/*****************************************************************************
***
***     Public Functions
***
*****************************************************************************/

/**
  @brief Inform CM layer that test mode is in progress.
*/
extern void geran_send_cm_test_mode_on(const gas_id_t gas_id);

/**
  @brief Inform CM layer that test mode is no longer in progress.
*/
extern void geran_send_cm_test_mode_off(const gas_id_t gas_id);

/* Can be called by any module, and will cause RR to restart RR, L1, MAC and RLC */
extern void geran_initiate_recovery_restart(const geran_client_enum_t, gas_id_t gas_id);

/* Query function to determine whether soft restart functionality is available */
extern boolean geran_get_nv_recovery_restart_enabled( gas_id_t gas_id );

#endif /* INC_GPRSDEF_H */

/*** EOF: don't remove! ***/
