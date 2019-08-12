#ifndef MCALWCDMA_CM_H
#define MCALWCDMA_CM_H

/*============================================================================*/
/** @file  
 * This module has definition and declaration related to MCAL UMTS compressed
 * mode module APIs
 */
/*============================================================================*/

/*============================================================================
Copyright (c) 2012 Qualcomm Technologies, Inc.  All Rights Reserved.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government. Diversion contrary to U.S. law prohibited.
============================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/wcdma/l1/offline/inc/mcalwcdma_cm.h#1 $
$DateTime: 2016/12/13 08:00:12 $
$Author: mplcsds1 $

when        who     what, where, why
--------    ---     --------------------------------------------------------
02/24/15    ap      Initial check-in for WL1 QSH framework 
11/19/14    gm      To implement changes to inform TRM if div is needed for measurement.
11/17/14    cc      Send gap start action time in CM/FMO for FW to get a reference frame
09/11/14    wz      release WRM lock if W2W srch can not conduct  
09/08/14    mm      Changed start slot field for FMO gaps to int8
07/28/14    cc      Type correction of variables for WRM request
06/19/14    cc      IRAT DR-DSDS check in
06/06/14    cc      Remove old WFW interface variables that are no longer used
06/03/14    ks      Remove old WFW interface variables that are no longer used
05/29/14    mm      Update IRAT/CM driver initialization and RxLM buffer allocation call flow
04/27/14    jkb     Add check for Feature dual sim with feature qta
04/14/14    ks      Remove sending XO values to WFW
03/10/14    ks      Bolt FED API checkin
10/15/13    mm      Initial FE-FACH check-in
09/23/13    as      Porting Dual Sim from Triton to Dime.
08/12/13    cc      DBDC CM W2W feature check in
04/23/13    mm      Passing neighbor RxLM information for W2W inter-freq measurements every CM/FACH gap.
                    Additional RxLM buffer for back to back CM gaps.
01/09/13    dp      Changes to allow sending w2l cm gap-end time to WFW
01/30/12    vs      Feature cleanup.
10/26/11    dp      Added RxD reconfig capabilities after irat searches
10/26/11    yh      Fix calculation of available search window in a gap
08/03/11    dp      Re-worked dynamic tune time code for NikeL bringup.  
                    Also added fixes for previous XO changes.
05/26/11    dp      Added changes to make tuning time value dynamic
05/26/11    dp      Added support for W2W XO Parameter passing to FW
04/27/11    dp      Added RXLM support for IRAT and W2W searches
06/14/10    yh      Add init rf buf function
05/07/10    yh      Add support for IRAT RF buffer control
06/15/09    uk      Support for sending START_FACH_MEAS cmd to fw before 
                    starting a FMO gap for GSM meas
04/20/09    yh      Compiler warning fixes.
01/22/09    stk/yh  Added enum for TGMP
11/14/08    uk      Added FMO Support
11/13/08    yh      Initial revision.

===========================================================================*/

/* ==========================================================================
** Includes and Public Data Declarations
** ========================================================================*/

/* -----------------------------------------------------------------------
** Include Files
** ----------------------------------------------------------------------- */

#include "comdef.h"
#include "customer.h"
#include "wfw_misc_intf.h"
#include "wl1rfmif.h"
#include "wl1trm.h"

/* -----------------------------------------------------------------------
** Constant / Define Declarations
** ----------------------------------------------------------------------- */
/** Number of RF buffers for W2W */
#define MCALWCDMA_CM_MAX_RF_SETUP_IDX     3
/** Number of RF buffers for IRAT (W2G and W2L) */
#define MCALWCDMA_CM_IRAT_MAX_RF_BUF      2
/** Invalid GSM frame number */
#define MCALWCDMA_CM_INVALID_GSM_FN       0xFF

/* this is a general purpose value representing an invalid amount of time.  
   It should only be used with fields that store time values. */
#define MCALWCDMA_CM_TIME_INVALID         0

/* Limit the max band to 5 */
#define MCALWCDMA_MAX_NUM_BAND            MAX_BAND_INFO
/* Invalid Band */
#define MCALWCDMA_INVALID_BAND            SYS_BAND_CLASS_NONE
/* Use this to convert microseconds into (rounded) chipx8.  Given the wcdma 
   parametrs at the time of writing this, this effectively this takes usecs and
   multiplies by 30.72 chipx8/usec.  The shifting to the left 14 is to preserve
   precision when dividing by 10000 (microseconds per frame term).  To correct 
   this, at the end you would normally shift right by 14 at the end, but in 
   order to round, we shift right by 13 at the end, add one, and then shift to 
   the right one more time for a total of 14 right shifts.  A uint64 is 
   required to keep the dividend from overflowing. The result should end up as 
   a uint32 though after the corrective right shifting. */
#define MCALWCDMA_CM_USECS_TO_CHIPX8(usecs) \
((uint32)((((((((uint64)(usecs))*CHIP_PER_FRAME*8)<<14)/(MSEC_PER_FRAME*1000))>>13) + 1)>>1))

/* -----------------------------------------------------------------------
** Type Declarations
** ----------------------------------------------------------------------- */

/* Call back function type declaration */
/* ----------------------------------- */

/* Enum type declaration */
/* --------------------- */
/** Enum to be the same type as mDSP for CM method */
typedef enum {
  /** Compressed mode by HLS */
  MCALWCDMA_CM_METHOD_HLS,
  /** Compressed mode by SF reduction */
  MCALWCDMA_CM_METHOD_SF_RED,
} mcalwcdma_cm_method_enum_type;

/** Enum to be the same type as mDSP for TGMP */
typedef enum {
  /** CM No measurement (debug only) */
  MCALWCDMA_CM_TGMP_NO_MEAS,
  /** CM interfrequency measurements */
  MCALWCDMA_CM_TGMP_INTER_FREQ,
  /** For W2G GSM measurements */
  MCALWCDMA_CM_TGMP_W2G_GSM_MEAS,
  /** For W2L LTE measurements */
  MCALWCDMA_CM_TGMP_W2L_LTE_MEAS,
  #if defined(FEATURE_DUAL_SIM) && defined(FEATURE_QTA)
  /*Added two new enums to support QTA Start-Stop cmds*/
  /*! Open gap for W2G quick tune away */
  MCALWCDMA_CM_TGMP_W2G_QTA_START,
  /*! Close gap after W2G quick tune away */
  MCALWCDMA_CM_TGMP_W2G_QTA_STOP,
  #endif
  MCALWCDMA_CM_TGMP_MAX
} mcalwcdma_cm_tgmp_enum_type;

typedef enum {
  MCALWCDMA_TRM_MEAS_DENIED,    /* Measure Req get denied, measurement can not start */
  MCALWCDMA_TRM_REQ_SUCCESS,    /* RF Path get granted, measurement can start */  
  MCALWCDMA_TRM_DIV_DENIED,     /* Diversity RF path get denied, measurement can not start */   
  MCALWCDMA_TRM_REQ_INVALID     /* invalid return value */
} mcalwcdma_cm_start_meas_enum_type;

/* Structure type declaration */
/* -------------------------- */
/** Structure for IRAT RF buffer control */
/* Struct for the pending RF buffers used for GSM */
typedef struct
{
  /* GSM Frame that the buffer can be freed */
  uint8 gfn_to_free;
} mcalwcdma_cm_gsm_rf_buf_struct_type;

typedef struct
{
  /** Buffer in use, not free */
  boolean buffer_in_use;
  /** Home RAT RxLM index */
  uint8 source_rxlm_index;
  /** TGMP of the buffer */
  mcalwcdma_cm_tgmp_enum_type tgmp;
  struct
  {
    /* Diversity chain gets granted or not, this info will need to pass to LTE */
    boolean diversity_granted;
    /* TRM request ID */
    wrm_irat_identifier req_id;
    /* IRAT duration for TRM to lock RF resource */
    wrm_duration_type meas_duration;
  } wrm_meas_req_info;

  union
  {
    mcalwcdma_cm_gsm_rf_buf_struct_type gsm_pending_rf_buf;
   /* To be included at a later stage */
   /* mcalwcdma_lte_rf_buf_struct_type lte_pending_rf_buf; */
  } irat_rf_buf;

  /* this structure also will contain the rxlm buffer index info passed to 
     other RAT's as part of searches */
  union
  {
    struct
    { 
      /* Interf neighbor RxLM buffer index */
      uint32 interf_buf_idx;
    } interf_info;

    /** (for GSM searches) buffer index for the WL1_XLM_RX_CHAIN0_IRAT_GSM_NBR
        client in addition to the WCDMA home client */ 
    struct
    {
      uint32 gsm_buf_idx;
    } gsm_info;

    /** (for LTE searches) buffer indicies for the 
        WL1_XLM_RX_CHAIN0_IRAT_LTE_NBR and WL1_XLM_RX_CHAIN1_IRAT_LTE_NBR
        clients */
    struct
    {
      uint32 lte_ch0_buf_idx;
      uint32 lte_ch1_buf_idx;
      uint32 lte_pri_dev_id;
      uint32 lte_div_dev_id;
    } lte_info;

  } rxlm_buf_info;

} mcalwcdma_cm_irat_rf_buf_ctrl_struct_type;
/** Structure for CM init parameters */
typedef struct
{
  /** WTR index info **/
  uint8 wtrindex;
  /** RPP type */
  uint16 rpp;
  /** ITP bit */
  uint16 itp;
  /** DL Frame type */
  uint16 dl_frame_type;
  /** UL compressed mode method */
  mcalwcdma_cm_method_enum_type ul_cm_method;
  /** DL compressed mode method */
  mcalwcdma_cm_method_enum_type dl_cm_method;
  /** First compressed slot */
  uint16 first_cm_slot;
  /** transmission gap length */
  uint16 tgl;
  /** transmission gap measurement purpose */
  uint16 tgmp;
  /** the rxlm buffer index holding the nbr-frequency primary Rx chain info */
  uint32 rxlm_setup_buf_idx;
  /** the rxlm buffer index holding the home-frequency primary Rx chain info */
  uint32 rxlm_home_buf_idx;
  /** the rxlm buffer index holding the home-frequency diversity Rx chain info */
  uint32 rxlm_home_rxd_buf_idx;
  /** gap start time in stmr */
  uint32 gap_action_time_in_stmr;
} mcalwcdma_cm_init_params_struct_type;

/* Structure type declaration */
/* -------------------------- */
/** Structure for FACH init parameters */
typedef struct
{
  /** WTR index info **/
  uint8 wtrindex;

  /** Combiner channel number/HW channel index **/
  uint8 chan_num;

  /** Start CFN of the gap */
  uint8 start_cfn;

  /** start slot of the gap */
  int8 start_slot;

  /** measurement period length in slots */
  uint8 gap_len_slots;

  /** measurement gap purpose */
  uint16 tgmp;

  /** the rxlm buffer index describing the tune-away information */
  uint32 rxlm_setup_buf_idx;
  /** the rxlm buffer index holding the home-frequency primary Rx chain info */
  uint32 rxlm_home_buf_idx;
  /** the rxlm buffer index holding the home-frequency diversity Rx chain info */
  uint32 rxlm_home_rxd_buf_idx;
  /** gap start time in stmr */
  uint32 gap_action_time_in_stmr;

} mcalwcdma_fach_init_params_struct_type;

/* -----------------------------------------------------------------------
** Global Data Declarations
** ----------------------------------------------------------------------- */
extern mcalwcdma_cm_irat_rf_buf_ctrl_struct_type mcalwcdma_cm_irat_rf_buf_ctrl[MCALWCDMA_CM_IRAT_MAX_RF_BUF];

/* this is the amount of time (in chips x8) it takes for RF to tune away to a 
   neighboring WCDMA frequency */
extern uint32 mcalwcdma_cm_w2w_tune_away_time_cx8;
/* this is the amount of time (in chips x8) it takes for RF to tune back to the
   home frequency at the end of a neighbor search */
extern uint32 mcalwcdma_cm_w2w_tune_back_time_cx8;
/* this is the amount of time (in chips x8) it takes for WFW to collect samples   
   before start doing any search inside a gap */
extern uint32 mcalwcdma_cm_w2w_samples_collection_time_cx8;

/* =======================================================================
**                        Function Declarations
** ======================================================================= */

/*===========================================================================
**
 *  This function initializes all the variables used by mcalwcdma_cm, mostly
 *  assigning FW-SW pointers.
 */
/*============================================================================*/
extern void mcalwcdma_cm_init(void);

/*===========================================================================
**
 *  This function finds available RF buffer indices to be used for W2G and W2L 
 *  searches. 
 */
/*============================================================================*/
extern boolean mcalwcdma_cm_get_free_rf_buf_indices(
  /** Type of IRAT request */
  mcalwcdma_cm_tgmp_enum_type tgmp,
  /** Buffer to be assigned */
  mcalwcdma_cm_irat_rf_buf_ctrl_struct_type **rf_buf);

/*===========================================================================
**
 *  This function cleans up the rf_buf for future use.
 */
/*============================================================================*/
extern void mcalwcdma_cm_clean_rf_buf(
  /** Buffer to be assigned */
  mcalwcdma_cm_irat_rf_buf_ctrl_struct_type *rf_buf);

/*===========================================================================
**
 *  This function inits all the rf_buffers.
 */
/*============================================================================*/
extern void mcalwcdma_cm_init_rf_buffers(void);
/*===========================================================================
**
 *  This function writes the home frequency index into mDSP.
 */
/*===========================================================================*/
extern void mcalwcdma_cm_set_cm_home_freq_idx(
  /** mDSP index for the home frequency */
  uint16 idx);

/*============================================================================*/
/**
 * This function sends a command to mdsp to init cm.
 *
 */
/*============================================================================*/
extern void mcalwcdma_cm_init_cmd(
  /** cm init params */
  mcalwcdma_cm_init_params_struct_type *mcalwcdma_cm_init_params);

/*============================================================================*/
/**
 * This function sends a command to mdsp to init FACH interF meas.
 *
 */
/*============================================================================*/
extern void mcalwcdma_cm_fach_init_cmd(
  /** FACH cfg parameters */
  mcalwcdma_fach_init_params_struct_type *mcalwcdma_fach_init_params);

/*============================================================================*/
/**
 * This function sets the DL outer loop power control adjust value for
 * compressed mode. There are 2 types of power control gain to be adjusted
 * for compressed mode.
 * 
 * deltaP_compression:
 *  For SF/2 it is 3 dB
 *  For higher layer scheduling it is 0 dB
 * deltaP_coding:
 *  This is the deltaP_SIR or deltaP_SIR_After based on GAP position and
 *  active TGPS state for the GAP. The detail of this is mentioned below.
 *  Each deltaP_SIR_xxx is specified by UTRAN ranges from 0 to 3 dB in step of 0.1 dB
 * Here is the detail of how the deltaP coding is calculated.
 * DeltaP coding is the combination of 2 deltaP SIR
 * 1. Previous Frame. This reflect the frame after GAP
 * 2. Current GAP
 *
 * For previous GAP (This frame is one after the GAP <gap num> has ended)
 *  If this frame is the frame after the GAP then apply deltaP_SIR_After<gap num>
 *  Following are the exception for above:
 *    The deltaP_SIR_After should not applied if the GAP that ended in previous
 *    frame was multiframe GAP.
 *
 * For current frame (This frame has a GAP <gap num>)
 *  If this is the first frame in the GAP (Gap start in this frame), apply
 *  deltaP_SIR_<gap num>
 *  If this is the frame has a GAP but GAP didn't started in this frame,
 *  apply deltaP_SIR_After<gap num>
 *  
 * Here <gap num> can be 1 or 2.
 * To apply the delta_P SIRs apply the delta that is the sum of all the
 * active TGPS at a given time.
 * 
 *  cmFpcAdjust =
 *   1024 * 10^(0.1 x [deltaP_compression + deltaP_coding])
 *   = 1024 * 10^(0.1 deltaP_compression) * 10^(0.1 deltaP_coding))
 *   
 * The max dB value possible is 10 x log10(65536 / 1024) = 18.06
 */
/*============================================================================*/
extern void mcalwcdma_cm_set_ebnt_adj_val(
  /** Total delta SIR based on gap parameters */
  uint16 total_delta);


/*============================================================================
FUNCTION mcalwcdma_cm_update_irat_tune_times

DESCRIPTION
  This function will call the correct rf interface in order to fill in IRAT's
  picture of how long tune times take from W2W, W2L, or W2G (both tuning away
  and back again).  Currently only does anything for W2W.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
============================================================================*/
void mcalwcdma_cm_update_irat_tune_times(void);

/*============================================================================
FUNCTION mcalwcdma_cm_set_pll_settling_time_cx8

DESCRIPTION
  This function stores the pll_settling time value in mcalwcdma_cm's global
  data fields.  This should be the only method of changing the value since FW
  must be notified each time the value changes.  This function takes care of 
  that.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  pll_settling_time is written to FW async params
============================================================================*/
void mcalwcdma_cm_set_pll_settling_time_cx8(
  /* the pll settling time in units of cx8 */
  uint32 settling_time);

/*============================================================================
FUNCTION mcalwcdma_cm_wrm_start_meas

DESCRIPTION
  1. This function is to call WRM MEAS START API to query current RF availability 
     to IRAT search.
  2. This wrapper function is shared by all IRAT search in idle/FACH/CM

DEPENDENCIES
  None

RETURN VALUE
  MCALWCDMA_TRM_REQ_SUCCESS,     RF Path get granted, measurement can start 
  MCALWCDMA_TRM_DIV_DENIED,      Diversity RF path get denied, measurement can not start
  MCALWCDMA_TRM_MEAS_DENIED,     Measure Req get denied, measurement can not start 
  MCALWCDMA_TRM_REQ_INVALID

SIDE EFFECTS
  None
============================================================================*/
extern mcalwcdma_cm_start_meas_enum_type mcalwcdma_cm_wrm_start_meas(
  /* Band for IRAT measurement */
  sys_band_class_e_type *band_info,  
  /* Duration of IRAT time to lock RF resource */
  uint32 dirat,
  /* Secondary chain (RxD) is used */
  boolean use_drx,
  /* IRAT RF ctrl buffer */
  mcalwcdma_cm_irat_rf_buf_ctrl_struct_type *rf_buf);

/*============================================================================
FUNCTION mcalwcdma_cm_wrm_complete_meas

DESCRIPTION
  This function is to call WRM MEAS COMPLETE to notify TRM that 
  IRAT search has completed

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
============================================================================*/
extern void mcalwcdma_cm_wrm_complete_meas(
  /* Secondary chain (RxD) is used */
  boolean use_drx,
  /* IRAT RF ctrl buffer */
  mcalwcdma_cm_irat_rf_buf_ctrl_struct_type *rf_buf);

#ifdef FEATURE_QSH_DUMP
/*============================================================================
FUNCTION mcalwcdma_cm_get_irat_rf_buffer_usage_info

DESCRIPTION
  This function is to collect the irat rf buffer usage info for debugging. 
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
============================================================================*/
extern void mcalwcdma_cm_get_irat_rf_buffer_usage_info(void *debug_buf);
#endif /* FEATURE_QSH_DUMP */

#endif /* MCALWCDMA_CM_H */
