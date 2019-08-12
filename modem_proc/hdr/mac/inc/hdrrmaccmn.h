#ifndef HDRRMACCMN_H
#define HDRRMACCMN_H

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

       H D R   R E V E R S E   T R A F F I C   C H A N N E L   M A C   

                              P R O T O C O L

                   C O M M O N   H E A D E R    F I L E

GENERAL DESCRIPTION
  This file contains definitions and declarations used by HDR Reverse Traffic
  Channel Medium Access Control (MAC).


  Copyright (c) 2007 - 2013 by Qualcomm Technologies, Inc.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/hdr/mac/inc/hdrrmaccmn.h#1 $ $DateTime: 2016/12/13 07:58:05 $ $Author: mplcsds1 $
   
when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
08/28/14   wsh     RTT:Updated RTT threshold, used seperate hysteresis flags
08/12/14   wsh     Disable RTT in TAP call; force HiCap if boost, new threshold
04/03/14   kss/wsh Added T2P boost for RTT reduction
04/17/14   kss/wsh Tuneaway/tuneback T2P boost tweaking 
10/25/13   rmg     Added T2P boost support.
11/07/11   vlc     Fixed Klocwork warnings. 
09/16/11   wsh     Reduce heap mem usage by moving memory back to static
01/26/10   wsh     Merged ZI memory reduction to main branch
09/08/08   etv     Added macro to sign extend 4 bits to 8 bits.
09/20/07   rmg     Created from original hdrrmaccmn.h.

===========================================================================*/

/*===========================================================================

                          INCLUDE FILES FOR MODULE

===========================================================================*/

#include "hdr_variation.h"
#include "customer.h"
#include "hdrrmac.h"
#include "hdrscmrmaccmn.h"

#ifdef FEATURE_HDR_REVB


/*===========================================================================

                        DEFINITIONS AND DECLARATIONS

===========================================================================*/
#define HDRRMACCMN_MAX_NUM_SUBPACKETS         4

/* Sign extend 4-bit number to a 8-bit number
   If the sign bit of the 4 bit 2s complement number is set, sign extend. */
#define HDRRMACCMN_SIGN_EXTEND_4_TO_8(val)      \
  (int8) ((((val) & 0x8) == 0)? (val): ((val)|0xF0))

typedef enum
{
  HDRRMACCMN_0_BITS       = 0x0,
  HDRRMACCMN_128_BITS     = 0x1,
  HDRRMACCMN_256_BITS     = 0x2,
  HDRRMACCMN_512_BITS     = 0x3,
  HDRRMACCMN_768_BITS     = 0x4,
  HDRRMACCMN_1024_BITS    = 0x5,
  HDRRMACCMN_1536_BITS    = 0x6,
  HDRRMACCMN_2048_BITS    = 0x7,
  HDRRMACCMN_3072_BITS    = 0x8,
  HDRRMACCMN_4096_BITS    = 0x9,
  HDRRMACCMN_6144_BITS    = 0xA,
  HDRRMACCMN_8192_BITS    = 0xB,
  HDRRMACCMN_12288_BITS   = 0xC,
  HDRRMACCMN_MAX_PAYLOADS = 0xD,
  HDRRMACCMN_INFINITE_BITS= 0xE,
  HDRRMACCMN_IS856_A_RRI  = 0xF  /* RMAC4 normal pkt size selection applies */
} hdrrmaccmn_payload_size_enum_type;

typedef enum
{
  HDRRMACCMN_TX_MODE_HI_CAP       = 0x0,
  HDRRMACCMN_TX_MODE_LO_LAT       = 0x1,
  HDRRMACCMN_NUM_TX_MODES         = 0x2 
} hdrrmaccmn_tx_mode_enum_type;

typedef enum
{
  HDRRMACCMN_QRAB_VAL_QRAB        = 0x0,
  HDRRMACCMN_QRAB_VAL_QRAB_PS     = 0x1
} hdrrmaccmn_qrab_select_enum_type;

typedef enum
{
  HDRRMACCMN_ARQ_MODE_BI_POLAR    = 0x0,
  HDRRMACCMN_ARQ_MODE_ON_OFF      = 0x1
} hdrrmaccmn_arq_mode_enum_type;

typedef enum
{
  HDRRMACCMN_SUBFRAME_N_MINUS_1   = 0x0,
  HDRRMACCMN_SUBFRAME_N_MINUS_2   = 0x1,
  HDRRMACCMN_SUBFRAME_N_MINUS_3   = 0x2,
  HDRRMACCMN_MAX_SUBFRAME_HISTORY = 0x3,
} hdrrmaccmn_subframe_hist_enum_type;

typedef struct
{
  uint8                             num_points;
    /* Number of values on the T2P axis */
  uint8                             values[HDRSCMRMACCMN_MAX_T2P_POINTS];
    /* The values that define the T2P axis */
} hdrrmaccmn_t2p_axis_struct_type;

typedef struct
{
  uint8                             num_points;
    /* Number of values on the FRAB axis */
  int8                              values[HDRSCMRMACCMN_MAX_FRAB_POINTS];
    /* The values that define the FRAB axis */
} hdrrmaccmn_frab_axis_struct_type;

typedef struct
{
  hdrrmaccmn_t2p_axis_struct_type   t2p_axis;
    /* The T2P (x) axis */
  hdrrmaccmn_frab_axis_struct_type  frab_axis;
    /* The FRAB (y) axis */
  int8                              grid[HDRSCMRMACCMN_MAX_T2P_POINTS]
                                        [HDRSCMRMACCMN_MAX_FRAB_POINTS];
    /* The values of the function, say, f(), at the grid points. f() could
     * be BucketFactor(), T2PUp or T2PDown 
     */
} hdrrmaccmn_t2p_frab_grid_struct_type;

typedef hdrrmac_flow_mapping_struct_type  hdrrmaccmn_flow_mapping_struct_type;

typedef struct 
{
  uint8 num_ps_axis_values;
    /* Number of values on the pilot strength axis. */

  int32  ps_axis[HDRSCMRMACCMN_MAX_PS_POINTS];
    /* Values that define the pilotstrength axis. */

  int32  pilotstrength_ps_axis[HDRSCMRMACCMN_MAX_PS_POINTS];
    /* PilotStrength(ps) corresponding to pilotstrength values defined. */

} hdrrmaccmn_pilot_strength_grid_struct_type;

typedef struct 
{
  uint8 num_ps_axis_values;
    /* Number of values on the pilot strength axis. */

  int32  ps_axis[HDRSCMRMACCMN_MAX_PS_POINTS];
    /* Values that define the pilotstrength axis. */

  int32  txt2p_max_ps_axis[HDRSCMRMACCMN_MAX_PS_POINTS];
    /* T2Pmax(ps) values corresponding to pilotstrength values defined. */

} hdrrmaccmn_txt2p_max_grid_struct_type;

typedef struct
{
 hdrrmaccmn_t2p_frab_grid_struct_type up;
   /* T2PUp interpolation grid */
 hdrrmaccmn_t2p_frab_grid_struct_type down;
   /* T2PDown interpolation grid */
 int32                                pri;
   /* flow priority by evaluating T2PUp/T2PDn at the left-most (T2P, FRAB) point */
} hdrrmaccmn_gu_gd_struct_type;

typedef struct
{
  uint32                           min;
    /* T2PInflow minimum */
  uint32                           max;
    /* T2PInflow maximum */
} hdrrmaccmn_t2p_inflow_range_struct_type;

typedef struct
{
  uint16                            drc_lock;
    /* 
     * Minimum PilotStrength required for QRAB from non-serving sector to be
     * included in QRABps computation when DRC is in lock: 0, 3, 6 (default)
     * or 9 dB
     */
  uint16                           drc_unlock;
    /* 
     * Minimum PilotStrength required for QRAB from non-serving sector to be
     * included in QRABps computation when DRC is not in lock: 0, 3 (default)
     * 6 or 9 dB
     */
} hdrrmaccmn_ps_qrab_thresh_struct_type;

typedef struct
{
  uint32                            frab;
    /* FRAB IIR filter time constant: 128, 256, 384 (default) or 512 slots */

  uint32                            pilot_strength;
    /* PilotStrength IIR filter time constant: 32,64 (default) or 128 slots */

  uint32                            qrab;
    /* QRAB IIR filter time constant: 4 (default) or 8 slots */

  uint32                            t2p_no_tx;
    /* 
     * T2PInflow IIR filter time constant when not transmitting: 16 or 
     * 24 (default) subframes
     */
} hdrrmaccmn_time_const_struct_type;

typedef struct
{
  int16                             frab_low;
    /* 
     * FRAB threshold. FRAB < FRABLow ==> sector with lightly loaded 
     * reverse link 
     */
  uint16                            merge_thresh;
    /* AT-wide Merge Threshold in octets; used for determining F & F* sets */
} hdrrmaccmn_elig_thresh_struct_type; 

typedef struct
{
  int16                             gain_db;
    /* AuxPilotChannelGain: 5 bit 2's complement in 1 dB units; default=0x14 */

  uint16                            min_payload;
    /* Minimum payload size that requires Aux Pilot to be transmitted */

  uint16                            gain; 
    /* linear Q6.10 value from LUT in hdrrmac3math */
} hdrrmaccmn_aux_pilot_struct_type;

/* The following data is derived from the PowerParams attribute */
typedef struct   /* Size = 4 + 16 + 4 + 4 + 1 = 29 */
{ 
  uint8       data_t2p_db[HDRRMACCMN_MAX_NUM_SUBPACKETS];
    /* Data channel gains for each subpacket. */

  uint32      tx_t2p[HDRRMACCMN_MAX_NUM_SUBPACKETS];
    /* T2P for data channel and auxiliary pilot (if applicable) in
       Q22.10 format. */

  uint32      max_db;
    /* Max of pre and post t2p values, in 1/4 dB */

  uint32      nominal;
    /* Nominal t2p value in Q22.10. */

  uint8       term_target;
    /* Termination target as specified in PowerParams + 1.
       Future use in deciding whether a frame can be sent before
       RLSI or 1x tuneaway. */

  uint8       trans_point;
    /* Transition point for each rate */

} hdrrmaccmn_tx_t2p_info_type;

typedef struct  /* Size = (29 * 13 + 2) = 379 */
{ 
  hdrrmaccmn_tx_t2p_info_type tx_t2p_info[HDRRMACCMN_MAX_PAYLOADS];
    /* T2P info is needed for each rate. */

  uint16                    t2p_conv_factor;
    /* T2P conversion factor, in Q10.6 */
} hdrrmaccmn_t2p_info_type;

#ifdef FEATURE_HDR_RMAC_TUNEAWAY_OPTIMIZATION_T2P_BOOST
#define HDRRMACCMN_T2P_BOOST_SFS               15
  /* Maximum number of subframes after tuneback for which T2P boost should
     be effective */

#define HDRRMACCMN_T2P_BOOST_POST_TB_RRI_THRESH 7
  /* Mininum RRI required to qualify for post-tuneback T2P boost. */

#define HDRRMACCMN_T2P_BOOST_PRE_TA_RRI_THRESH  8
  /* Mininum RRI required to qualify for pre-tuneaway T2P boost. */

#define HDRRMACCMN_T2P_BOOST_REPORT_RRI_THRESH 7
  /* Min RRI of the packet required to report as "boosted" */

#define HDRRMACCMN_T2P_BOOST_LOW_RRI_DELTA  12
  /* Value to decrease boost by for lower RRIs (6 and under).
     In 1/4 dB units. */

#define HDRRMACCMN_T2P_BOOST_RRI_CHECK          6
  /* First RRI rate to sanity check before enabling boost */

#define  HDRRMACCMN_T2P_BOOST_RRI_RAB_THRESH    -13107
  /* FRAB (-0.4) below which to enable lower RRI boost */

#ifdef FEATURE_HDR_RMAC_RTT_REDUCTION_T2P_BOOST
#define HDRRMACCMN_RTT_DEBOOST_DISABLED    0xFF
  /* Indicating RTT is inactive (vs. deboost = 0) */

#define HDRRMACCMN_T2P_BOOST_RRI_RAB_THRESH_START    -14746
  /* RAB below which RTT reduction boost will start (-0.45) */

#define HDRRMACCMN_T2P_BOOST_RRI_RAB_THRESH_STOP    -11469
  /* RAB above which RTT reduction boost will stop (-0.35) */

#define HDRRMACCMN_T2P_BOOST_TX_PWR_THRESH_STOP   80 /* 8dBm */
  /* Stop boost if TxPwr > MaxTXPwr - StopThres */
#endif /* FEATURE_HDR_RMAC_RTT_REDUCTION_T2P_BOOST */

extern const uint8 hdrrmaccmn_nv_to_t2p_boost_map[4];
  /* Table to translate T2P boost NV config to T2P boost gain value
     in Q.2 dB */

#ifdef FEATURE_HDR_RMAC_RTT_REDUCTION_T2P_BOOST
extern const uint8 hdrrmaccmn_nv_to_t2p_deboost_map[4];
  /* Table to translate T2P deboost NV config to T2P deboost gain value
     in Q.2 dB */
extern const uint8 hdrrmaccmn_nv_to_y1_map[4];
  /* Table to translate TxTotal threshold NV config to threshold y1 value */
#endif /* FEATURE_HDR_RMAC_RTT_REDUCTION_T2P_BOOST */

typedef enum
{
  HDRRMACCMN_T2P_BOOST_INACTIVE,
    /* T2P boost inactive: T2P boost not being applied to any packets */

  HDRRMACCMN_T2P_BOOST_PRE_TA_ACTIVE,
    /* Pre-tuneaway T2P boost active:
       - All packets are transmitted in LoLat mode
       - If a packet with RRI > HDRRMACCMN_T2P_BOOST_PRE_TA_RRI_THRESH 
         has termination target > subpackets before tunaway, 
         T2P boost is applied to the packet */

  HDRRMACCMN_T2P_BOOST_POST_TB_ACTIVE
    /* Post-tuneback T2P boost active: T2P boost applied to new packets
       with RRI > HDRRMACCMN_T2P_BOOST_POST_TB_RRI_THRESH. */

#ifdef FEATURE_HDR_RMAC_RTT_REDUCTION_T2P_BOOST
  , HDRRMACCMN_T2P_BOOST_RTT_REDUCT_ACTIVE
    /* T2P boost for RTT reduction. If RTT reduction NV is enabled,
       we will not do TA/TB boost */
#endif /* FEATURE_HDR_RMAC_RTT_REDUCTION_T2P_BOOST */
} hdrrmaccmn_t2p_boost_state_enum_type;

typedef struct
{
  hdrrmaccmn_t2p_boost_state_enum_type  state;
    /* T2P boost state */

  hdrrmaccmn_tx_t2p_info_type           tx_t2p_info[HDRRMACCMN_MAX_PAYLOADS];
    /* Boosted TxT2P info */

  uint8                                 reset_counter;
    /* T2P boost is reset when this counter goes to zero */

  uint8                                 gain;
    /* T2P boost gain in Q.2 dB */

#ifdef FEATURE_HDR_RMAC_RTT_REDUCTION_T2P_BOOST
  uint8                                 deboost_gain;
    /* Deboost gain used for RTT Reduct after transition point */
  uint8                                 y1;
    /* If TxPwr < Pmax - y1, set TxPwr_limited flag to FALSE  */
  boolean                               frab_limited[3];
    /* 1 if RTT is frab limited */
  boolean                               txpwr_limited;
    /* 1 if RTT is TxPwr limited */
  boolean                               suspended;
    /* Whether RMAC is supended due to tuneaway */
#endif /* FEATURE_HDR_RMAC_RTT_REDUCTION_T2P_BOOST */

  uint16                                boosted_rri_mask;
    /* Mask with bit positions corresponding to T2P boosted RRIs
       in current subframe set */
} hdrrmaccmn_t2p_boost_struct_type;
#endif /* FEATURE_HDR_RMAC_TUNEAWAY_OPTIMIZATION_T2P_BOOST */

typedef struct
{
  uint8                            req_ratio;
  uint8                            max_req_interval;
} hdrrmaccmn_req_params_struct_type;

typedef struct
{
  int8                             rri_chan_gain_pre_transition;
    /* RRI Channel gain for subpackets transmitted prior to T2P transition */
  int8                             rri_chan_gain_post_transition;
    /* RRI Channel gain for subpackets transmitted after T2P transition */
} hdrrmaccmn_rri_power_params_struct_type;

typedef struct
{
  uint16                             alloc_stagger;
  uint16                             tx_t2p_min_db;
  uint16                             rpc_step_db;
} hdrrmaccmn_common_pow_params_struct_type;

typedef uint8 hdrrmaccmn_permitted_payload_size_type
                [HDRRMACCMN_MAX_PAYLOADS]
                [HDRRMACCMN_MAX_SUBFRAME_HISTORY];
    /* RMAC payload array */

#endif /* FEATURE_HDR_REVB*/
#endif /* HDRRMACCMN_H */

