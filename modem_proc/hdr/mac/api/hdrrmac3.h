#ifndef HDRRMAC3_H
#define HDRRMAC3_H

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

       H D R   R E V E R S E   T R A F F I C   C H A N N E L   M A C   
                            
                             P R O T O C O L  (Subtype 3)      

GENERAL DESCRIPTION
  This module contains the implementation of the HDR Reverse Traffic Channel 
  Medium Access Control (MAC) Protocol for an HDR access terminal.

EXTERNALIZED FUNCTIONS
  hdrrmac_init
    Initialize the Reverse Traffic Channel MAC protocol.
    
  hdrrmac_activate
    Activate the Reverse Traffic Channel MAC protocol.

  hdrrmac_deactivate
    Deactivate the Reverse Traffic Channel MAC protocol.

  hdrrmac_is_active
    Determine if protocol is active.
    
  hdrrmac_process_cmd
    Process Forward Reverse Channel MAC commands from the task command queue.
    
  hdrrmac_process_sig
    Process Forward Reverse Channel MAC signals received at task level.
    
  hdrrmac_process_msg
    Process received Reverse Traffic Channel MAC messages.
    
  hdrrmac_msg_cb
    Callback function that queues received Reverse Traffic Channel MAC 
    messages onto the task message queue.
    

INITIALIZATION AND SEQUENCING REQUIREMENTS
  The protocol must be initialized after system startup prior to activating 
  the protocol.

  Copyright (c) 2005 - 2013 by Qualcomm Technologies, Inc.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/hdr/mac/api/hdrrmac3.h#1 $ $DateTime: 2016/12/13 07:58:05 $ $Author: mplcsds1 $
   
when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
06/04/14   arm     Changes to support DO to G QTA
11/25/13   rmg     Added changes to reduce idle subframes before tuneaway.
10/09/13   rmg     Featurized removal of redundant definitions.
10/02/13   rmg     Added support for RL rate estimate and RLP queue info for
                   modem API.
10/02/13   rmg     Removed some redundant definitions to use those defined 
                   in hdrrmaccmn files.
07/10/13   arm     Supported traffic DTX. 
05/15/13   rmv     Added updates for Cost of Modem Power API support 
10/03/12   rmg     Corrected tx power initialization during optimized handoff.
06/20/12   smd     Supported Device only DTX and FlexCONN.
10/27/11   kss     Added support for LTE to DO optimized handoff.
10/24/11   smd     Moved more to SU API header files.
09/23/11   smd     SU API cleanup.
06/21/10   cnx     Fixed compiler warnings.
05/18/10   rmg     Shifted DRC supervision timer management to FMAC: added 
                   APIs hdrrmac3_enter/exit_drc_supervision().
11/30/07   cc      Moved hdrrmac3_common pointer initialization from _init
                   to powerup_init function.
10/29/07   cc      Added hdrrmac3_set_target_ps_index function declaration.
10/15/07   cc      Added init_attrib argument in powerup_init function.
09/12/07   cc      Added "pri" field in hdrrmac3_gu_gd_struct_type.
09/05/07   kss     Added hdrrmac3_get_current_tx_packet_info() function.
08/08/07   cc      Replaced rmac3 flow definitions with common rmac definitions.
07/17/07   cc      Replaced hdrrmac3_mac_flow_set_elem_type by
                   hdrpcp_mac_flow_allocation_type.
03/08/07   rmg     Fixed RVCT compiler warnings
03/03/07   rmg     Added overhead channel gains compensation
02/08/07   rmg     Added support for 8 MAC flows
01/17/06   rmg     Added resume-from-HHO support in resume_connection()
08/22/06   kss     Added hdrrmac3_notify_suspend(). 
07/26/06   jyw     Added transmission check.
07/24/06   rmg     Added function hdrrmac3_get_mac_info()
04/25/06   etv     Removed T2P/FRAB, PS and T2pTrans #defines and used 
                   SCM's instead.
04/13/06   kss     Added hdrrmac3_update_throttling() function.
04/11/06   etv     Added support for sorting BucketFactor & GuGd.
04/10/06   etv     Replaced RMAC3_MAX_APP_FLOWS with SCMRMAC3_MAX_APP_FLOWS.
01/31/06   etv     Added support for AT-init Neg. for RMAC3.
11/17/05   etv     Added support for finding the flow mapping changes.
11/04/05   etv     Moved Config struct definition from hdrrmac3c.h
11/02/05   etv     Bumped up the Number of MAC flows to 4
10/13/05   kss     Added function declarations for logging.
10/05/05   sr/sy   Added HDRRMAC3_FLOW_ID_DEFAULT.
09/08/05   sr      Moved hdrrmac3_get_flow_mapping() declaration and associated
                   typedefs into this file
07/27/05   sr      Added interface for RETAP
05/04/05   sr      Created

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "hdr_variation.h"
#include "customer.h"


#ifdef FEATURE_HDR_REVA_L1

#include "comdef.h"
#include "dsm.h"
#include "hdrind.h"
#include "hdrhai.h"
#include "hdrrmaci.h"
#include "hdrrmac.h"
#ifdef FEATURE_HDR_USE_RMAC_CMN_DEFS
#include "hdrrmaccmn.h"
#endif /* FEATURE_HDR_USE_RMAC_CMN_DEFS */
#include "hdrscmrmac3.h"
#include "hdrl1_api.h"

/*===========================================================================

                LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

===========================================================================*/

#define HDRRMAC3_FLOW_ID_DEFAULT           HDRRMAC_FLOW_ID_DEFAULT
#define HDRRMAC3_MAX_NUM_SUBPACKETS        4
#define HDRRMAC3_MAX_AS_SIZE               6
#define HDRRMAC3_NUM_INTERLACES            3
#define HDRRMAC3_MAC_FLOW_ID_INVALID       0xFF

typedef hdrrmac_app_flow_struct_type      hdrrmac3_app_flow_struct_type;
typedef hdrrmac_flow_mapping_struct_type  hdrrmac3_flow_mapping_struct_type;

#ifndef FEATURE_HDR_USE_RMAC_CMN_DEFS
typedef enum
{
  HDRRMAC3_0_BITS       = 0x0,
  HDRRMAC3_128_BITS     = 0x1,
  HDRRMAC3_256_BITS     = 0x2,
  HDRRMAC3_512_BITS     = 0x3,
  HDRRMAC3_768_BITS     = 0x4,
  HDRRMAC3_1024_BITS    = 0x5,
  HDRRMAC3_1536_BITS    = 0x6,
  HDRRMAC3_2048_BITS    = 0x7,
  HDRRMAC3_3072_BITS    = 0x8,
  HDRRMAC3_4096_BITS    = 0x9,
  HDRRMAC3_6144_BITS    = 0xA,
  HDRRMAC3_8192_BITS    = 0xB,
  HDRRMAC3_12288_BITS   = 0xC,
  HDRRMAC3_MAX_PAYLOADS = 0xD,
  HDRRMAC3_INFINITE_BITS= 0xE,
  HDRRMAC3_IS856_A_RRI  = 0xF  /* RMAC3 normal pkt size selection applies */
} hdrrmac3_payload_size_enum_type;

typedef enum
{
  HDRRMAC3_SUBFRAME_N_MINUS_1   = 0x0,
  HDRRMAC3_SUBFRAME_N_MINUS_2   = 0x1,
  HDRRMAC3_SUBFRAME_N_MINUS_3   = 0x2,
  HDRRMAC3_MAX_SUBFRAME_HISTORY = 0x3,
} hdrrmac3_subframe_hist_enum_type;

typedef enum
{
  HDRRMAC3_TX_MODE_HI_CAP       = 0x0,
  HDRRMAC3_TX_MODE_LO_LAT       = 0x1,
  HDRRMAC3_NUM_TX_MODES         = 0x2 
} hdrrmac3_tx_mode_enum_type;

typedef enum
{
  HDRRMAC3_QRAB_VAL_QRAB        = 0x0,
  HDRRMAC3_QRAB_VAL_QRAB_PS     = 0x1
} hdrrmac3_qrab_select_enum_type;

typedef enum
{
  HDRRMAC3_ARQ_MODE_BI_POLAR    = 0x0,
  HDRRMAC3_ARQ_MODE_ON_OFF      = 0x1
} hdrrmac3_arq_mode_enum_type;



typedef struct
{
  int16                             gain_db;
    /* AuxPilotChannelGain: 5 bit 2's complement in 1 dB units; default=0x14 */

  uint16                            min_payload;
    /* Minimum payload size that requires Aux Pilot to be transmitted */

  uint16                            gain; 
    /* linear Q6.10 value from LUT in hdrrmac3math */
} hdrrmac3_aux_pilot_struct_type;

typedef struct
{
  uint8                             t2p_trans_subframes;
    /* 
     * Number of subframes to use the pre-transition T2P values for a given 
     * packet size. Out of the maximum 4 possible transmissions (subpackets) 
     * of a given packet this tells the AT at which subpacket it can 
     * transition its T2P.
     */

  uint8                             term_target;
    /* 
     * Expected number of subframes needed to achieve the target Physical
     * Layer erasure rate.
     */
  
  uint8                             t2p_pre_trans;
    /* 
     * T2P value to be used BEFORE transitioning to a possibly different 
     * (pre_transition) T2P value.
     */
  
  uint8                             t2p_post_trans;
    /* 
     * T2P value to be used AFTER transitioning to a possibly different 
     * (post_transition) T2P value.
     */

} hdrrmac3_power_params_struct_type;

/* The following data is derived from the PowerParams attribute */
typedef struct   /* Size = 4 + 16 + 4 + 4 + 1 = 29 */
{ 
  uint8       data_t2p_db[HDRRMAC3_MAX_NUM_SUBPACKETS];
    /* Data channel gains for each subpacket. */

  uint32      tx_t2p[HDRRMAC3_MAX_NUM_SUBPACKETS];
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

} hdrrmac3_tx_t2p_info_type;

typedef struct  /* Size = (29 * 13 + 2) = 379 */
{ 
  hdrrmac3_tx_t2p_info_type tx_t2p_info[HDRRMAC3_MAX_PAYLOADS];
    /* T2P info is needed for each rate. */

  uint16                    t2p_conv_factor;
    /* T2P conversion factor, in Q10.6 */
} hdrrmac3_t2p_info_type;

typedef struct
{
  uint16                             alloc_stagger;
  uint16                             tx_t2p_min_db;
  uint16                             rpc_step_db;
} hdrrmac3_common_pow_params_struct_type; 

typedef struct
{
  int8                             rri_chan_gain_pre_transition;
    /* RRI Channel gain for subpackets transmitted prior to T2P transition */
  int8                             rri_chan_gain_post_transition;
    /* RRI Channel gain for subpackets transmitted after T2P transition */
} hdrrmac3_rri_power_params_struct_type;

typedef struct
{
  uint8                             num_points;
    /* Number of values on the T2P axis */
  uint8                             values[HDRSCMRMAC3_MAX_T2P_POINTS];
    /* The values that define the T2P axis */
} hdrrmac3_t2p_axis_struct_type;

typedef struct
{
  uint8                             num_points;
    /* Number of values on the FRAB axis */
  int8                              values[HDRSCMRMAC3_MAX_FRAB_POINTS];
    /* The values that define the FRAB axis */
} hdrrmac3_frab_axis_struct_type;

typedef struct
{
  hdrrmac3_t2p_axis_struct_type     t2p_axis;
    /* The T2P (x) axis */
  hdrrmac3_frab_axis_struct_type    frab_axis;
    /* The FRAB (y) axis */
  int8                              grid[HDRSCMRMAC3_MAX_T2P_POINTS]
                                        [HDRSCMRMAC3_MAX_FRAB_POINTS];
    /* The values of the function, say, f(), at the grid points. f() could
     * be BucketFactor(), T2PUp or T2PDown 
     */
} hdrrmac3_t2p_frab_grid_struct_type;

typedef struct
{
 hdrrmac3_t2p_frab_grid_struct_type up;
   /* T2PUp interpolation grid */
 hdrrmac3_t2p_frab_grid_struct_type down;
   /* T2PDown interpolation grid */
 int32                              pri;
   /* flow priority by evaluating T2PUp/T2PDn at the left-most (T2P, FRAB) point */
} hdrrmac3_gu_gd_struct_type;

typedef struct 
{
  uint8 num_ps_axis_values;
    /* Number of values on the pilot strength axis. */

  int32  ps_axis[HDRSCMRMAC3_MAX_PS_POINTS];
    /* Values that define the pilotstrength axis. */

  int32  txt2p_max_ps_axis[HDRSCMRMAC3_MAX_PS_POINTS];
    /* T2Pmax(ps) values corresponding to pilotstrength values defined. */

} hdrrmac3_txt2p_max_grid_struct_type;

typedef struct 
{
  uint8 num_ps_axis_values;
    /* Number of values on the pilot strength axis. */

  int32  ps_axis[HDRSCMRMAC3_MAX_PS_POINTS];
    /* Values that define the pilotstrength axis. */

  int32  pilotstrength_ps_axis[HDRSCMRMAC3_MAX_PS_POINTS];
    /* PilotStrength(ps) corresponding to pilotstrength values defined. */

} hdrrmac3_pilot_strength_grid_struct_type;


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
} hdrrmac3_time_const_struct_type;

typedef struct
{
  int16                             frab_low;
    /* 
     * FRAB threshold. FRAB < FRABLow ==> sector with lightly loaded 
     * reverse link 
     */
  uint16                            merge_thresh;
    /* AT-wide Merge Threshold in octets; used for determining F & F* sets */
} hdrrmac3_elig_thresh_struct_type; 

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
} hdrrmac3_ps_qrab_thresh_struct_type;

typedef struct
{
  uint32                           min;
    /* T2PInflow minimum */
  uint32                           max;
    /* T2PInflow maximum */
} hdrrmac3_t2p_inflow_range_struct_type;

typedef struct
{
  uint8                            req_ratio;
  uint8                            max_req_interval;
} hdrrmac3_req_params_struct_type;

#else
/* Map hdrrmac3 enums and struct types to those defined hdrrmaccmn */
#define hdrrmac3_payload_size_enum_type  hdrrmaccmn_payload_size_enum_type
#define hdrrmac3_subframe_hist_enum_type  hdrrmaccmn_subframe_hist_enum_type
#define hdrrmac3_tx_mode_enum_type  hdrrmaccmn_tx_mode_enum_type
#define hdrrmac3_qrab_select_enum_type  hdrrmaccmn_qrab_select_enum_type
#define hdrrmac3_arq_mode_enum_type  hdrrmaccmn_arq_mode_enum_type
#define hdrrmac3_aux_pilot_struct_type  hdrrmaccmn_aux_pilot_struct_type
#define hdrrmac3_power_params_struct_type  hdrrmaccmn_power_params_struct_type
#define hdrrmac3_tx_t2p_info_type  hdrrmaccmn_tx_t2p_info_type
#define hdrrmac3_t2p_info_type  hdrrmaccmn_t2p_info_type
#define hdrrmac3_common_pow_params_struct_type  hdrrmaccmn_common_pow_params_struct_type
#define hdrrmac3_rri_power_params_struct_type  hdrrmaccmn_rri_power_params_struct_type
#define hdrrmac3_t2p_axis_struct_type  hdrrmaccmn_t2p_axis_struct_type
#define hdrrmac3_frab_axis_struct_type  hdrrmacmn_frab_axis_struct_type
#define hdrrmac3_t2p_frab_grid_struct_type  hdrrmaccmn_t2p_frab_grid_struct_type
#define hdrrmac3_gu_gd_struct_type  hdrrmaccmn_gu_gd_struct_type
#define hdrrmac3_txt2p_max_grid_struct_type  hdrrmaccmn_txt2p_max_grid_struct_type
#define hdrrmac3_pilot_strength_grid_struct_type  hdrrmaccmn_pilot_strength_grid_struct_type
#define hdrrmac3_time_const_struct_type  hdrrmaccmn_time_const_struct_type
#define hdrrmac3_elig_thresh_struct_type  hdrrmaccmn_elig_thresh_struct_type
#define hdrrmac3_ps_qrab_thresh_struct_type  hdrrmaccmn_ps_qrab_thresh_struct_type
#define hdrrmac3_t2p_inflow_range_struct_type  hdrrmaccmn_t2p_inflow_range_struct_type
#define hdrrmac3_req_params_struct_type  hdrrmaccmn_req_params_struct_type

/* Map enumerated entries */
#define HDRRMAC3_0_BITS  HDRRMACCMN_0_BITS
#define HDRRMAC3_128_BITS  HDRRMACCMN_128_BITS
#define HDRRMAC3_256_BITS  HDRRMACCMN_256_BITS
#define HDRRMAC3_512_BITS  HDRRMACCMN_512_BITS
#define HDRRMAC3_768_BITS  HDRRMACCMN_768_BITS
#define HDRRMAC3_1024_BITS  HDRRMACCMN_1024_BITS
#define HDRRMAC3_1536_BITS  HDRRMACCMN_1536_BITS
#define HDRRMAC3_2048_BITS  HDRRMACCMN_2048_BITS
#define HDRRMAC3_3072_BITS  HDRRMACCMN_3072_BITS
#define HDRRMAC3_4096_BITS  HDRRMACCMN_4096_BITS
#define HDRRMAC3_6144_BITS  HDRRMACCMN_6144_BITS
#define HDRRMAC3_8192_BITS  HDRRMACCMN_8192_BITS
#define HDRRMAC3_12288_BITS  HDRRMACCMN_12288_BITS
#define HDRRMAC3_MAX_PAYLOADS HDRRMACCMN_MAX_PAYLOADS
#define HDRRMAC3_INFINITE_BITS  HDRRMACCMN_INFINITE_BITS
#define HDRRMAC3_IS856_A_RRI  HDRRMACCMN_IS856_A_RRI
#define HDRRMAC3_SUBFRAME_N_MINUS_1  HDRRMACCMN_SUBFRAME_N_MINUS_1
#define HDRRMAC3_SUBFRAME_N_MINUS_2  HDRRMACCMN_SUBFRAME_N_MINUS_2
#define HDRRMAC3_SUBFRAME_N_MINUS_3  HDRRMACCMN_SUBFRAME_N_MINUS_3
#define HDRRMAC3_MAX_SUBFRAME_HISTORY  HDRRMACCMN_MAX_SUBFRAME_HISTORY
#define HDRRMAC3_TX_MODE_HI_CAP  HDRRMACCMN_TX_MODE_HI_CAP
#define HDRRMAC3_TX_MODE_LO_LAT  HDRRMACCMN_TX_MODE_LO_LAT
#define HDRRMAC3_NUM_TX_MODES  HDRRMACCMN_NUM_TX_MODES
#define HDRRMAC3_QRAB_VAL_QRAB  HDRRMACCMN_QRAB_VAL_QRAB
#define HDRRMAC3_QRAB_VAL_QRAB_PS  HDRRMACCMN_QRAB_VAL_QRAB_PS
#define HDRRMAC3_ARQ_MODE_BI_POLAR  HDRRMACCMN_ARQ_MODE_BI_POLAR
#define HDRRMAC3_ARQ_MODE_ON_OFF  HDRRMACCMN_ARQ_MODE_ON_OFF
#endif /* FEATURE_HDR_USE_RMAC_CMN_DEFS */ 


/*-----------------*/
/* Flow attributes */
/*-----------------*/
typedef struct
{
 /*--------------------------------------------------------------------------*/
 /*----------------------------- Simple Attributes --------------------------*/
 /*--------------------------------------------------------------------------*/
  uint32                                bucket_level_max;
    /* 
     * Maximum T2P resource that can be accumulated by a flow. 0 means flow is
     * inactive
     */

  uint16                                merge_thresh;
    /* 
     * Per flow threshold in octets. If this HiCap MAC flow  has more than
     * MergeThreshold octets to send then it is eligible for transmission in a 
     * new packet under certain conditions (see comment under eligibility 
     * thresholds below
     */

  hdrrmac3_tx_mode_enum_type            tx_mode;
    /* LoLat or HiCap transmission mode. Default is HiCap */

  hdrrmac3_qrab_select_enum_type        qrab_select;
    /* 
     * Switch to control whether AT should use QRAB or QRABps as indication of
     * short term sector loading for this MAC flow
     */ 

  uint16                                burst_duration_factor;
    /* 
     * Maximum duration (in subframes) this MAC flow can sustain a peak rate 
     * based on its T2P allocation
     */

  uint16                                t2p_filt_tc;
    /* 
     * T2PInflow IIR filter time constant for this MAC flow: 16, 24 (default),
     * 32, 64 or 128 subframes
     */

 /*--------------------------------------------------------------------------*/
 /*---------------------------- Complex Attributes --------------------------*/
 /*--------------------------------------------------------------------------*/

  hdrrmac3_t2p_frab_grid_struct_type    bucket_factor;
    /* 
     * Standard allows for up to a 255x15 grid. 8 bit values in [1..32.875] 
     * represented in 1/8 dB units, so actual value is (OTA value * 1/8) + 1
     */
  
  hdrrmac3_gu_gd_struct_type            gu_gd;
    /* 
     * Used to determine T2PInflow up/down increments. They are aka Gu/Gd 
     * functions. Values are in 8 bit 2's complement form in 1/8 dB units 
     */
  
  hdrrmac3_t2p_inflow_range_struct_type t2p_inflow_range;
    /* T2PInflow min and max for this flow: 8 bit values in 1/8 dB units */

  hdrrmac3_flow_mapping_struct_type     flow_mapping;
    /* AssociatedFlowsNN */

} hdrrmac3_flow_config_struct_type;


/*---------------------------------------------------------------------------
                             PROTOCOL ATTRIBUTES
---------------------------------------------------------------------------*/

/*---------------------------*/
/* RMAC Subtype 3 Attributes */
/*---------------------------*/
typedef struct
{
 /*--------------------------------------------------------------------------*/
 /*---------------------------- Simple Attributes ---------------------------*/
 /*--------------------------------------------------------------------------*/

  boolean                                is_max_sub_pkts_gaupable;
    /* Is MaxNumSubpackets(0xFFF2) attribute GAUPable */

  hdrrmac3_arq_mode_enum_type            arq_mode;
    /* Bipolar or On/Off keying */

  
  hdrrmac3_time_const_struct_type        filt_time_const;

  hdrrmac3_elig_thresh_struct_type       flow_elig_thresh;
    /* 
     * These 2 thresholds are used to determine whether all active flows are
     * eligible for transmission in the new packet. If the transmission mode is
     * LoLat and FRAB >= FRABLow and SumQOutflow < MergeThreshold then only 
     * LoLat flows or HiCap flows with more octets than the MergeThreshold for
     * that flow are eligible for transmission in the new packet
     */

  hdrrmac3_payload_size_enum_type        payload_thresh;
    /* 
     * Minimum packets size permitted to be transmitted in LoLat mode when it
     * does not contain LoLat flows
     */

  hdrrmac3_ps_qrab_thresh_struct_type    ps_qrab_thresh;

  uint16                                 max_num_mac_flows;
    /* Maximum number of MAC flows supported:4, 5, 6, 7, 8 (default), ..., 16 */

  boolean                                rate_1m8_supported;
    /* 1.8 Mbps rate supported or not: default is FALSE */
  
  hdrrmac3_flow_config_struct_type       flows[HDRRMAC3_MAX_NUM_MAC_FLOWS];
    /* 8 flows supported for now */

 /*--------------------------------------------------------------------------*/
 /*---------------------------- Complex Attributes --------------------------*/
 /*--------------------------------------------------------------------------*/

  hdrrmac3_aux_pilot_struct_type         aux_pilot_info;
    /* 
     * minimum payload requiring transmission of Aux Pilot and its gain 
     * relative to Traffic Channel
     */

  // hdrrmac3_permitted_payload_struct_type permitted_payload[13][3];
  hdrrmac3_payload_size_enum_type        permitted_payload
                                           [HDRRMAC3_MAX_PAYLOADS]
                                           [HDRRMAC3_MAX_SUBFRAME_HISTORY];
    /*
     * 12 + 1 possible payload sizes, 0 meaning no packet was transmitted.
     * The second dimension stands for subframes n-1, n-2 and n-3,
     * respectively. So permitted_payload[1][2], which has a default value of
     * 5 (standing for a payload size of 1024) means that the maximum permitted
     * payload size the AT can transmit in subframe n is 1024 bits if it
     * transmitted a payload of 128 bits in subframe n-2
     */ 

  hdrrmac3_pilot_strength_grid_struct_type pilotstrength;
    /* 
     * Scaling factor in 4 bit 2's complement format used to scale T2PInflow
     * if filtered PilotStrength equals corresponding PilotStrength axis point
     */

  hdrrmac3_t2p_info_type                 t2p_info[HDRRMAC3_NUM_TX_MODES];
    /* Size =  2 * 379 = 758 bytes. We don't need to store PowerParams (above)
     * separately when we have this
     */

  hdrrmac3_req_params_struct_type        req_params;
    /* ReqRatio and MaxReqInterval */

  hdrrmac3_rri_power_params_struct_type  rri_chan_power_params
                                           [HDRSCMRMAC3_NUM_T2P_TRANS_POINTS];
    /* 
     * RRI channels gains corresponding to the various combinations of pre and 
     * post T2P transition points
     */

  hdrrmac3_txt2p_max_grid_struct_type    txt2p_max;
    /* 
     * Grid that defines the maximum TxT2P AT is allowed to transmit if 
     * filtered serving sector * PilotStrength equals corresponding 
     * PilotStrengthAxis value. 
     */

  hdrrmac3_common_pow_params_struct_type common_pow_params;

} hdrrmac3_config_struct_type;

/*===========================================================================

                      FUNCTION  DECLARATIONS

===========================================================================*/


/*===========================================================================
FUNCTION HDRRMAC3_POWERUP_INIT

DESCRIPTION
  This function initializes RMAC session configuration.

DEPENDENCIES
  Note that this is run in HDRMC context.
  Should be run BEFORE hdrrmac3_init().
 
DEPENDENCIES
  None.

PARAMETERS
  init_attrib - TRUE- RMAC3 is the current InUse subtype.  Attributes need
                to be initialized; FALSE otherwies.
  common_parameters - Pointer to the higher level RMAC common parameter
                block.
  
RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
extern void hdrrmac3_powerup_init
(
  boolean init_attrib,
  hdrrmac_common_type * common_parameters
);


/*===========================================================================
FUNCTION HDRRMAC3_RESET_RTC_RATE_COUNT                           EXTERNAL

DESCRIPTION
  Wrapper function to reset the current rate count.  

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
extern void hdrrmac3_reset_rtc_rate_count( void );


/*===========================================================================
FUNCTION HDRRMAC3_LOG_RTC_RATE_COUNT                             EXTERNAL

DESCRIPTION
  Wrapper function for logging the current rate count.  

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
extern void hdrrmac3_log_rtc_rate_count( void );


/*===========================================================================
FUNCTION HDRRMAC3_CONFIG_EARLY_TERMINATION                       EXTERNAL

DESCRIPTION
  Configures early termination on the RL link. This is a test mode function.
  
DEPENDENCIES
  None.

PARAMETERS
  config_mode: 0 - Default behavior, RL early termination on.
               1 - Forced early termination after 1 subpacket.
               2 - Forced early termination after 2 subpackets.
               3 - Forced early termination after 3 subpackets.
               4 - Forced early termination after 4 subpackets.

RETURN VALUE
  TRUE (config mode accepted),
  FALSE (config mode not valid).

SIDE EFFECTS
  None.
===========================================================================*/
extern boolean hdrrmac3_config_early_termination
(
  int config_mode
);


/*===========================================================================

FUNCTION HDRRMAC3_COMPUTE_SILENCE_TIME

DESCRIPTION
  Computes the next Reverse Silence Interval time

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  hdrrmac3.silence_begin_time is updated with the frame time for the
  next silence interval.

===========================================================================*/
extern void hdrrmac3_compute_silence_time( void );

/*===========================================================================
FUNCTION HDRRMAC3_SET_FIXED_RRI                                      EXTERNAL

DESCRIPTION
  Sets the RRI to a fixed value for testing purposes. Since in IS856-A we deal
  with packet sizes rather than rates, rri actually refers to the payload size 
  index.

DEPENDENCIES
  None.

PARAMETERS
  rri      - The RRI to fix to. Use 0xf, or HDRRMAC3_IS856__A_RRI for
             normal operation.

RETURN VALUE
  Boolean status indicating whether input is valid or not.

SIDE EFFECTS
  None.
===========================================================================*/
extern boolean hdrrmac3_set_fixed_rri
(
  uint8 rri
);    

/*===========================================================================
FUNCTION HDRRMAC3_INIT                                               EXTERNAL

DESCRIPTION
  This function initializes the protocol prior to use.  It should be called
  once upon system startup.  The supervision timer is initialized and the
  protocol state is set to Inactive.

DEPENDENCIES
  Should be run in HDRTx context, since timers are defined.
  Should be run AFTER hdrrmac3_powerup_init().

PARAMETERS
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
extern void hdrrmac3_init(void);



/*===========================================================================
FUNCTION HDRRMAC3_IS_ACTIVE

DESCRIPTION
  This function queries the current state of the protocol.
  
DEPENDENCIES
  None.
  
PARAMETERS
  caller - The protocol calling this externalized function (for sw trace)

RETURN VALUE
  TRUE  = protocol is Active.
  FALSE = protocol is Inactive.

SIDE EFFECTS
  None.
===========================================================================*/
extern boolean hdrrmac3_is_active ( void );




/*===========================================================================
FUNCTION HDRRMAC3_SHUTDOWN                                            EXTERNAL

DESCRIPTION
  This command tells the RTCMAC to shutdown after the current Txing frame 
  has completed.  It does NOT deactivate the protocol.

DEPENDENCIES
  None.

PARAMETERS
  sender - The protocol giving this command (only for software trace)

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
extern void hdrrmac3_shutdown ( void );


/*===========================================================================
FUNCTION HDRRMAC3_START_TX

DESCRIPTION
  This command starts the transmitter after a hdrrmac3_activate() command,
  or restarts the transmitter if it has been stopped.
  
DEPENDENCIES
  None.

PARAMETERS
  sender - The protocol giving this command (only for software trace)
  
RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
extern void hdrrmac3_start_tx( void );


/*===========================================================================
FUNCTION HDRRMAC3_STOP_TX

DESCRIPTION
  This command stops the transmitter.  If a data packet is being 
  transmitted at the time the command is processed, the protocol will
  wait until that packet tranmission is complete before shutting down.
  Otherwise, the PA is shut down immediately.  A TxStopped indication 
  is given once the transmitter has been shut down.
  
DEPENDENCIES
  None.

PARAMETERS
  sender - The protocol giving this command (only for software trace)
  
RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
extern void hdrrmac3_stop_tx( void );


/*===========================================================================
FUNCTION HDRRMAC3_PROCESS_CMD

DESCRIPTION
  This function processes the commands for this protocol that were queued
  on the task command queue from a different task context.
  
DEPENDENCIES
  None.

PARAMETERS
  cmd_ptr - pointer to received command buffer.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
extern void hdrrmac3_process_cmd 
( 
  hdrrmac_cmd_union_type * cmd_ptr                   
);


/*===========================================================================
FUNCTION HDRRMAC3_PROCESS_SIG

DESCRIPTION
  This function handles the task signals assigned to this protocol.
  
DEPENDENCIES
  None.

PARAMETERS
  signal - task signal to be processed.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
extern void hdrrmac3_process_sig 
( 
  rex_sigs_type signal  
);


/*===========================================================================
FUNCTION HDRRMAC3_PROCESS_MSG                                         EXTERNAL 

DESCRIPTION
  This function processes incoming messages for this protocol.
  
DEPENDENCIES
  None.

PARAMETERS
  rcv_msg_ptr - pointer to receive message buffer.
  protocol_instance - Instance of the protocol the message is for
  
RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
extern void hdrrmac3_process_msg 
( 
  dsm_item_type * rcv_msg_ptr,              
  hdrhai_protocol_instance_enum_type protocol_instance
);



/*===========================================================================

FUNCTION HDRRMAC3_SET_FRAME_OFFSET                                    EXTERNAL

DESCRIPTION
  This function sets the RMAC frame offset.

DEPENDENCIES
  The parameter must not be NULL.
  
PARAMETERS
  frame_offset - the Frame offset

RETURN VALUE
  None.
  
SIDE EFFECTS
  None.
===========================================================================*/
extern void hdrrmac3_set_frame_offset
(            
  uint8 frame_offset
);


/*===========================================================================

FUNCTION HDRRMAC3_SET_CHANNEL_GAINS                                  EXTERNAL

DESCRIPTION
  This function conveys DRC, ACK and DSC channel gain information to 
  hdrrmac3drv

DEPENDENCIES
  None.
  
PARAMETERS
  channel_gains - DRC, ACK and DSC channel gains

RETURN VALUE
  None.
  
SIDE EFFECTS
  None.
===========================================================================*/
extern void hdrrmac3_set_channel_gains
(            
  hdrrmac_channel_gains_struct_type *channel_gains
);


/*===========================================================================

FUNCTION HDRRMAC3_PROCESS_IND                                    

DESCRIPTION
  This function processes indications given to RMAC.
  
DEPENDENCIES
  None.

PARAMETERS
  ind_name - Name of the indication for RMAC to process
  ind_data_ptr - pointer to data associated with the indication
  
RETURN VALUE
  None.

SIDE EFFECTS
  None.
  
===========================================================================*/
extern void hdrrmac3_process_ind
(
  hdrind_ind_name_enum_type ind_name,
  hdrind_ind_data_union_type * ind_data_ptr
);


/*===========================================================================

FUNCTION HDRRMAC3_NOTIFY_SUSPEND

DESCRIPTION
  This function tells the RTC MAC about an upcoming tune away.
    
DEPENDENCIES
  None.

PARAMETERS
  cancel_suspend - TRUE when scheduled suspend is to be cancelled
  suspend_slot   - If cancel_suspend is FALSE, slot when link is scheduled
                   to be suspended.
    
RETURN VALUE
  None.

SIDE EFFECTS
  None.
  
===========================================================================*/
void hdrrmac3_notify_suspend
(
  boolean cancel_suspend,
  uint32  suspend_slot
);


/*===========================================================================

FUNCTION HDRRMAC3_SUSPEND_CONNECTION                            

DESCRIPTION
  This function tells the MAC layer to cease sending data over the reverse
  link.  After the currently Tx'ing frame is complete, the PA will be turned
  off.  
  
DEPENDENCIES
  None.

PARAMETERS
  None.
    
RETURN VALUE
  None.

SIDE EFFECTS
  None.
  
===========================================================================*/
void hdrrmac3_suspend_connection( void );

/*===========================================================================

FUNCTION HDRRMAC3_RESUME_CONNECTION                            

DESCRIPTION
  This function tells the MAC layer that it may resume sending data over the 
  reverse link.  
    
DEPENDENCIES
  None.

PARAMETERS
  ms - the time (in mS) that the link was off.
  hho_flag - FALSE -> resume from tune-away
             TRUE  -> resume from hard-handoff
    
RETURN VALUE
  None.

SIDE EFFECTS
  None.
  
===========================================================================*/
void hdrrmac3_resume_connection
( 
  dword   ms,
    /* time, in mS, that connection was suspended. */

  boolean hho_flag
    /* FALSE - resume from tune-away
     * TRUE  - resume from hard-handoff
     */
); 

/*===========================================================================

FUNCTION HDRRMAC3_POWERDOWN_TX

DESCRIPTION
  Powers down the Tx section of RF.  Must only be called when the RF is 
  needed while an HDR connection is up.
      
DEPENDENCIES
  None.

PARAMETERS
  None.
    
RETURN VALUE
  None.

SIDE EFFECTS
  None.
  
===========================================================================*/
void hdrrmac3_powerdown_tx
( 
#ifdef FEATURE_HDR_QTA 
   boolean is_qta /* Is power up tx after QTA? */
#else
  void
#endif /* FEATURE_HDR_QTA */
);        

/*===========================================================================

FUNCTION HDRRMAC3_POWERUP_TX

DESCRIPTION
  Powers up the Tx section of RF.  hdrrmac3_powerdown_tx() must have been 
  called first.
      
DEPENDENCIES
  Hdrrmac3_powerdown_tx() must have been called.
  
PARAMETERS
  None.
    
RETURN VALUE
  None.

SIDE EFFECTS
  None.
  
===========================================================================*/
void hdrrmac3_powerup_tx( void );



/*===========================================================================
FUNCTION HDRRMAC3_PA_IS_AVAILABLE                                    EXTERNAL

DESCRIPTION
  This function queries the current availability of the PA.
  
DEPENDENCIES
  None.
  
PARAMETERS
  caller - The protocol calling this externalized function (for sw trace)

RETURN VALUE
  TRUE  = PA is available
  FALSE = PA is not currently available, due to 1) RTC MAC is not active or
          2) RTC MAC is in DRC supervision.

SIDE EFFECTS
  None.
===========================================================================*/
extern boolean hdrrmac3_pa_is_available
(
  hdrhai_protocol_name_enum_type caller
);


/*===========================================================================
FUNCTION HDRRMAC3_DISABLE_THROTTLING                                 EXTERNAL

DESCRIPTION
  This function allows the caller to request that Reverse Link throttling
  be disabled.  If the RTC MAC is not active at the time of the call, the
  throttle state will be changed the next time it becomes active.
  
  A caller ID is taken as a parameter, along with a boolean to indicate 
  whether the caller is registering the request or de-registering the 
  request.
  
  Note that since this function takes a boolean to vote on whether
  throttling is disables or not, there is no separate 
  "hdrrmac3_enable_throttling" function.

DEPENDENCIES
  None.

PARAMETERS
  caller_id       - The entity requesting that throttling be disabled.
  disable_request - TRUE indicates the caller is requesting disable.
                  - FALSE indicates the caller is ending the request. 
                    (RL throttling may be re-enabled).
RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
extern void hdrrmac3_disable_throttling
(
  hdrrmac_caller_id_enum_type caller_id,
  boolean                      disable_request
);

/*===========================================================================
FUNCTION HDRRMAC3_UPDATE_THROTTLING                                 INTERNAL

DESCRIPTION
  This function checks whether Reverse Link throttling should be disabled
  and enables/disables based on this. (Note that normal operation has RL 
  throttling on as per standard.)

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
extern void hdrrmac3_update_throttling( void );


/*===========================================================================
FUNCTION HDRRMAC3_ENTER_OPEN_STATE                                    INTERNAL

DESCRIPTION
  Enter the Open state.

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
extern void hdrrmac3_enter_open_state (void);


/*===========================================================================
FUNCTION HDRRMAC3_ENTER_INACTIVE_STATE                              INTERNAL

DESCRIPTION
  Enter the inactive state.

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
extern void hdrrmac3_enter_inactive_state (void);

/*===========================================================================
FUNCTION HDRRMAC3_START_RMAC

DESCRIPTION
  This function is invoked by either hdrrmac3_start_tx() or the RF tx complete
  callback function to initialize and start the RMAC isr.

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
extern void hdrrmac3_start_rmac( void );

#ifdef FEATURE_HDR_IS890A
/*===========================================================================
FUNCTION HDRRMAC3_SET_TARGET_PS_INDEX

DESCRIPTION
  This function is invoked by RETAP to set the maximum RRI value.

DEPENDENCIES
  None.

PARAMETERS
  max_rri       - maximum RRI value.

RETURN VALUE
  rri_valid - TRUE if the specified RRI is valid; FALSE otherwise.

SIDE EFFECTS
  None.
===========================================================================*/
boolean hdrrmac3_set_target_ps_index
(
  uint8 max_rri
);

 /* EJECT */
/*===========================================================================
FUNCTION HDRRMAC3_GET_SELECTED_PS_INDEX

DESCRIPTION
  This function is invoked by RETAP to get the payload size index selected 
  by RTCMAC3.

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  payload size index in the range [1..12]

SIDE EFFECTS
  None.
===========================================================================*/
extern uint8 hdrrmac3_get_selected_ps_index( void );

/*===========================================================================
FUNCTION HDRRMAC3_TRANSMISSION_CHECK

DESCRIPTION
  This function check if the transmission is optimal. 

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  If optimal. 

SIDE EFFECTS
  None.
===========================================================================*/
extern boolean hdrrmac3_transmission_check( void );
#endif /* FEATURE_HDR_IS890A */


/*===========================================================================
FUNCTION HDRRMAC3_GET_FLOW_MAPPING                                   EXTERNAL

DESCRIPTION
  Returns the AssociatedFlowsNN attribute to caller. 

DEPENDENCIES
  None.

PARAMETERS
  mapping - Pointer to structure giving the set of upper layer flows mapped 
            to a given MAC flow. 
  flow_id - MAC Flow Id 

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void hdrrmac3_get_flow_mapping
( 
  hdrrmac3_flow_mapping_struct_type *mapping,
  uint8 flow_id
);


/*===========================================================================
FUNCTION HDRRMAC3_IS_FLOW_ACTIVE                                     EXTERNAL

DESCRIPTION
  Function to find out if a given flow is active. A flow is considered active
  if its BucketLevelMax attribute is > 0

DEPENDENCIES
  None.

PARAMETERS
  MAC flow ID in the range [0x0 - 0xF]

RETURN VALUE
  TRUE if flow is active; false otherwise.

SIDE EFFECTS
  None.
===========================================================================*/
boolean hdrrmac3_is_flow_active( uint8 flow );

 /* EJECT */
/*=========================================================================

FUNCTION     : HDRRMAC3_GET_CHANGED_FLOW_MAPPING

DESCRIPTION  : 
  This function finds a set of App flows that are either bound or unbound from
  a particular MAC flow and stores it in the arrays bound_flows and 
  unbound_flows respectively.

Note: This function is called from DS task.

DEPENDENCIES : None

PARAMETERS   :
  unbound_flows - Array of Flow mapping structs with a set of App flows for 
                  each MAC flow that are unbound from this MAC flow due to 
                  recent GAUPing
  bound_flows   - Array of Flow mapping structs with a set of App flows for 
                  each MAC flow that are bound due to recent GAUPing

RETURN VALUE : 

SIDE EFFECTS : None

=========================================================================*/
extern void hdrrmac3_get_changed_flow_mapping
(
  hdrrmac3_flow_mapping_struct_type *unbound_flows,
  hdrrmac3_flow_mapping_struct_type *bound_flows
);


/*===========================================================================
FUNCTION HDRRMAC3_IS_CONFIG_NEEDED                                   EXTERNAL

DESCRIPTION
  This function is used to find out if an AT-initiated Configuration is 
  needed by RMAC3. 

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  TRUE - If AT-init config is required.
  FALSE - Otherwise

SIDE EFFECTS
  None.
===========================================================================*/
boolean hdrrmac3_is_config_needed( void );


/*===========================================================================
FUNCTION HDRRMAC3_CONFIGURE                                   EXTERNAL

DESCRIPTION
  This function does the AT-init configuration of RMAC3 attributes. 

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  TRUE - If the configuration succeeded.
  FALSE - Otherwise.

SIDE EFFECTS
  None.
===========================================================================*/
boolean hdrrmac3_configure( void );



/*===========================================================================
FUNCTION HDRRMAC3_GET_MAC_INFO                                       EXTERNAL

DESCRIPTION
  Returns MAC payload size equivalents (in bytes) for PA headroom, bucket
  level and T2PInflow to caller. 

DEPENDENCIES
  None.

PARAMETERS
  flow     - MAC Flow Id
  mac_info - Pointer to structure with a set of MAC payload size equivalents 
             for PA headroom, bucket level and T2PInflow

RETURN VALUE
  TRUE  - If the fuction succeeds in getting the requested values
  FALSE - Otherwise

SIDE EFFECTS
  None.
===========================================================================*/
extern boolean hdrrmac3_get_mac_info
( 
  uint8                  mac_flow_id, 
  hdrrmac3_mac_info_type *mac_info 
);


/*===========================================================================
FUNCTION HDRRMAC3_GET_CURRENT_TX_PACKET_INFO

DESCRIPTION
  Returns the currently transmitting RRI or packet size. 
  
DEPENDENCIES
  None.

PARAMETERS
  Point to packet info structure to be filled out.

RETURN VALUE
  TRUE if valid data was filled in. 
  FALSE if there was no data (due to PA not on, or RTCMAC not 
    active, for instance).

SIDE EFFECTS
  None.
===========================================================================*/
extern boolean hdrrmac3_get_current_tx_packet_info
(
  hdrrmac_current_tx_packet_info_type * pkt_info
);


/*===========================================================================
FUNCTION HDRRMAC3_ENTER_DRC_SUPERVISION

DESCRIPTION
  Performs drc supervision entry processing - disables RL transmission and 
  reverse power control.

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
extern void hdrrmac3_enter_drc_supervision( void );


/*===========================================================================
FUNCTION HDRRMAC3_EXIT_DRC_SUPERVISION

DESCRIPTION
  Performs drc supervision exit processing - re-enables RL transmission and
  reverse power control.

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
extern void hdrrmac3_exit_drc_supervision( void );

#ifdef FEATURE_HDR_DEVICE_ONLY_DTX
/*===========================================================================
FUNCTION HDRRMAC3_GET_FRAME_OFFSET                                   EXTERNAL

DESCRIPTION
  This function gets the RMAC frame offset.

DEPENDENCIES
  None
 
PARAMETERS
  None

RETURN VALUE
  uint8 frame_offset.

SIDE EFFECTS
  None.
===========================================================================*/
uint8 hdrrmac3_get_frame_offset ( void );
#endif /* FEATURE_HDR_DEVICE_ONLY_DTX */

#ifdef FEATURE_HDR_MODEM_API
/*==========================================================================

FUNCTION  HDRRMAC3_REPORT_COST_OF_MODEM_POWER_EST

DESCRIPTION
  This function calculates and reports estimated cost of modem power
    
DEPENDENCIES
  None

INPUTS
  None

RETURN VALUE
  Estimated cost of modem power LOW, MEDIUM, HIGH

SIDE EFFECTS
  None

==========================================================================*/
hdrrmac_cost_of_power_est_enum_type
hdrrmac3_report_cost_of_modem_power_est( void );
#endif /* FEATURE_HDR_MODEM_API */


#ifdef FEATURE_HDR_MODEM_API_RL_INFO
/*===========================================================================
FUNCTION HDRRMAC3_GET_RL_RATE_INFO

DESCRIPTION
  This function populates the following in the passed struct:
  1. Estimated reverse link throughput in kbps
  2. Flag indicating if throughput greater than estimated rate may be possible
  3. Current reverse link throughput in kbps
  4. RLP queue length in octets
  
DEPENDENCIES
  None
 
PARAMETERS
  Pointer to location where reverse link rate info is to be populated

RETURN VALUE 
  None

SIDE EFFECTS
  None
 
NOTE 
  This API should not be called directly by external entities.
  hdrrmac_get_rl_rate_info() should be called instead.
===========================================================================*/
void hdrrmac3_get_rl_rate_info
(
  hdrrmac_rl_rate_info_struct_type * rl_info_ptr
);
#endif /* FEATURE_HDR_MODEM_API_RL_INFO */


#ifdef FEATURE_HDR_DEVICE_ONLY_DTX
/*==========================================================================

FUNCTION  HDRRMAC3_UPDATE_DTX_MODE

DESCRIPTION
  This function call driver to send a message to FW to enable or disable
  DTX mode
    
DEPENDENCIES
  None

INPUTS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

==========================================================================*/
void hdrrmac3_update_dtx_mode( void );
#endif /* FEATURE_HDR_DEVICE_ONLY_DTX */

#ifdef FEATURE_HDR_FLEX_CONN
/*==========================================================================

FUNCTION  HDRRMAC3_SET_FLEXCONN_MODE

DESCRIPTION
  This function calls appropriate driver function to send a message to FW to
  enable or disable FLEXCONN mode
 
DEPENDENCIES
  None

INPUTS
  flexconn_mode:  TRUE means enabling flexconn
  
RETURN VALUE
  None

SIDE EFFECTS
  None

==========================================================================*/
void hdrrmac3_set_flexconn_mode( boolean flexconn_mode );
#endif /* FEATURE_HDR_FLEX_CONN */

#endif /* FEATURE_HDR_REVA_L1 */

#endif /* HDRRMAC3_H */

