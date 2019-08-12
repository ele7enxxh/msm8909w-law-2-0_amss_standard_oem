
#ifndef HDRSCMRMAC3_H
#define HDRSCMRMAC3_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

        H D R   S E S S I O N  C O N F I G U R A T I O N  M A N A G E R 

           R E V E R S E   C H A N N E L   M A C   P R O T O C O L
                                    
                            H E A D E R   F I L E       

GENERAL DESCRIPTION
  This module contains the configuration interface of the HDR Reverse Channel 
  Medium Access Control (MAC) Protocol.

EXTERNALIZED FUNCTIONS

  hdrscmrmac_process_complex_attrib 
    Processes all RMAC subtype-3 complex attributes 
    
  hdrscmrmac_is_attrib_simple 
    Returns true if an attribute is simple

  hdrscmrmac_is_simple_attrib_valid
    Verifies the validity of the simple attribute

  hdrscmrmac_get_attrib_offset_and_size
    Returns attribute size and offset into the config structure.

  hdrscmrmac_get_attrib_verno_index
    Returns the index of an attribute in the version number array. 

INITIALIZATION AND SEQUENCING REQUIREMENTS


  Copyright (c) 2005 - 2014 by Qualcomm Technologies, Inc.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                           EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/hdr/cp/api/hdrscmrmac3.h#1 $ $DateTime: 2016/12/13 07:58:05 $ $Author: mplcsds1 $
   
when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/11/14   tnp     Increase maximum number of supported MAC flows to 8
01/14/14   tnp     Reduce maximum number of supported MAC flows to 5
12/04/08   etv     Lint fixes.
02/17/07   wsh     Added value_id/attrib_id checking processing ConfigResp
01/03/07   etv     Added support to handle unknown attributes.
02/08/07   rmg     Added support for 8 MAC flows
06/08/06   etv     Send session close if AN GAUPs a non-GAUPable attribute.
04/10/06   etv     Bumped up HDRSCMRMAC3_MAX_APP_FLOWS from 4 to 8.
01/31/06   etv     Added support for AT-init Neg. for RMAC3.
11/21/05   etv     Added header tags for perforce to fill in
11/14/05   etv     Handled Flow zero's Mergethreshold as a special case
11/10/05   etv     Rearranged the order of per-flow simple attributes in the 
                   per-flow config struct.
11/08/05   etv     Merge error in per-flow attribute ID enum
11/04/05   etv     Changed power params struct to reflect RMAC3 defn and 
                   externed some typedefs.
11/02/05   etv     Bunped up the number of MAC flows to 4
08/23/05   etv     Created module.

===========================================================================*/

/*===========================================================================

                          INCLUDE FILES FOR MODULE
                     
===========================================================================*/

#include "dsm.h"
#include "comdef.h"
#include "err.h"

#include "hdrdsm.h"
#include "hdrscp.h"
#include "hdrhai.h"
#include "hdrtrace.h"

#include "hdrscm.h"
#include "hdrscmrmaccmn.h"

/*===========================================================================

             DEFINITIONS  FROM  THE  HDR  AIR  INTERFACE  SPEC

===========================================================================*/

/* EJECT */
/*===========================================================================
                   
                   DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains definitions and declarations for constants, macros,
types, variables and other items needed by this module.

===========================================================================*/
#define HDRSCMRMAC3_MAX_NUM_MAC_FLOWS                   8

#define HDRSCMRMAC3_MAX_T2P_POINTS                      16
#define HDRSCMRMAC3_MAX_FRAB_POINTS                     8
#define HDRSCMRMAC3_MAX_APP_FLOWS                       8
#define HDRSCMRMAC3_MAX_PS_POINTS                       15
#define HDRSCMRMAC3_NUM_T2P_TRANS_POINTS                4

#define HDRSCMRMAC3_MAX_NUM_SUBFRAME_HISTORY            3
#define HDRSCMRMAC3_TOTAL_NUM_SIMPLE_ATTRIBS            12
#define HDRSCMRMAC3_TOTAL_NUM_NN_SIMPLE_ATTRIBS         6
#define HDRSCMRMAC3_TOTAL_NUM_COMPLEX_ATTRIBS           21 
#define HDRSCMRMAC3_TOTAL_NUM_NN_COMPLEX_ATTRIBS        4

/* MaxMACFlows */
#define HDRSCMRMAC3_DEFAULT_MAX_MAC_FLOWS               0x04

/* Max number of interlaces in the reverse link */
#define HDRSCMRMAC3_MAX_NUM_INTERLACES                  3

typedef enum
{
  HDRSCMRMAC3_FIRST_NON_NN_SIMPLE_ATTRIB        = 0xFFF2,
  HDRSCMRMAC3_SUPPORT_GAUP_MAX_NUM_SUB_PKTS = 
    HDRSCMRMAC3_FIRST_NON_NN_SIMPLE_ATTRIB,
  HDRSCMRMAC3_RATE_1M8_SUPPORTED                = 0xFFF3,
  HDRSCMRMAC3_NON_NN_SIMPLE_ATTRIB_RSVD1        = 0xFFF4,
  HDRSCMRMAC3_PS_QRAB_THRESH_DRC_UNLOCK         = 0xFFF5,
  HDRSCMRMAC3_PS_QRAB_THRESH_DRC_LOCK           = 0xFFF6,
  HDRSCMRMAC3_T2P_NO_TX_FILTER_TC               = 0xFFF7,
  HDRSCMRMAC3_NON_NN_SIMPLE_ATTRIB_RSVD2        = 0xFFF8,
  HDRSCMRMAC3_QRAB_FILTER_TC                    = 0xFFF9,
  HDRSCMRMAC3_PS_FILTER_TC                      = 0xFFFA,
  HDRSCMRMAC3_PAYLOAD_THRESH                    = 0xFFFB,
  HDRSCMRMAC3_MERGE_THRESH                      = 0xFFFC,
  HDRSCMRMAC3_FRAB_LOW                          = 0xFFFD,
  HDRSCMRMAC3_FRAB_FILTER_TC                    = 0xFFFE,
  HDRSCMRMAC3_ARQ_MODE                          = 0xFFFF,
  HDRSCMRMAC3_LAST_NON_NN_SIMPLE_ATTRIB =
    HDRSCMRMAC3_ARQ_MODE
} hdrscmrmac3_simple_attrib_id_enum_type;

/*-----------------------------------------------------------------------------
  This enum defines the attribute IDs of each of the RMAC ST-3 complex attribs.
------------------------------------------------------------------------------*/
typedef enum
{
  HDRSCMRMAC3_FIRST_COMPLEX_ATTRIB          = 0x0000,
  HDRSCMRMAC3_AUX_PILOT_CHAN_PARAMS_ATTRIB  = 
    HDRSCMRMAC3_FIRST_COMPLEX_ATTRIB,
  HDRSCMRMAC3_COMMON_PARAMS_ATTRIB          = 0x0001,
  HDRSCMRMAC3_MAX_NUM_SUB_PACKETS_ATTRIB    = 0x0002,
  HDRSCMRMAC3_PERMITTED_PAYLOAD_ATTRIB      = 0x0003,
  HDRSCMRMAC3_PILOT_STRENGTH_ATTRIB         = 0x0004,
  HDRSCMRMAC3_FIRST_POWER_PARAMS_ATTRIB     = 0x0005,
  HDRSCMRMAC3_POWER_PARAMS_128_ATTRIB       = 
    HDRSCMRMAC3_FIRST_POWER_PARAMS_ATTRIB,
  HDRSCMRMAC3_POWER_PARAMS_256_ATTRIB       = 0x0006,
  HDRSCMRMAC3_POWER_PARAMS_512_ATTRIB       = 0x0007,
  HDRSCMRMAC3_POWER_PARAMS_768_ATTRIB       = 0x0008,
  HDRSCMRMAC3_POWER_PARAMS_1024_ATTRIB      = 0x0009,
  HDRSCMRMAC3_POWER_PARAMS_1536_ATTRIB      = 0x000A,
  HDRSCMRMAC3_POWER_PARAMS_2048_ATTRIB      = 0x000B,
  HDRSCMRMAC3_POWER_PARAMS_3072_ATTRIB      = 0x000C,
  HDRSCMRMAC3_POWER_PARAMS_4096_ATTRIB      = 0x000D,
  HDRSCMRMAC3_POWER_PARAMS_6144_ATTRIB      = 0x000E,
  HDRSCMRMAC3_POWER_PARAMS_8192_ATTRIB      = 0x000F,
  HDRSCMRMAC3_POWER_PARAMS_12288_ATTRIB     = 0x0010,
  HDRSCMRMAC3_LAST_POWER_PARAMS_ATTRIB      = 
    HDRSCMRMAC3_POWER_PARAMS_12288_ATTRIB,
  HDRSCMRMAC3_REQUEST_PARAMS_ATTRIB         = 0x0011,
  HDRSCMRMAC3_RRI_CHAN_POWER_PARAMS_ATTRIB  = 0x0012,
  HDRSCMRMAC3_TX_T2P_MAX_ATTRIB             = 0x0013,
  HDRSCMRMAC3_MAX_MAC_FLOWS_ATTRIB          = 0x0014,
  HDRSCMRMAC3_LAST_COMPLEX_ATTRIB = HDRSCMRMAC3_MAX_MAC_FLOWS_ATTRIB

} hdrscmrmac3_complex_attrib_id_enum_type;

typedef enum
{
  HDRSCMRMAC3_TX_MODE_HI_CAP       = 0x0,
  HDRSCMRMAC3_TX_MODE_LO_LAT       = 0x1,
  HDRSCMRMAC3_NUM_TX_MODES         = 0x2 
} hdrscmrmac3_tx_mode_enum_type;


/*---------------------------------------------------------------------------
  Each NN attribute falls under a few major classifications and the attrib_id
  "XYZ00" can be used to identify the XYZ group of attribs. This enum defines 
  this base attrib Id for each class of NN attrib. Given an NN attrib_id, a 
  particular NN attribute class can be found by MASKING off the LSB using 
  HDRSCMRMAC3_NN_BASE_MASK 
----------------------------------------------------------------------------*/
typedef enum
{
  HDRSCMRMAC3_FIRST_SIMPLE_NN_ATTRIB_BASE   = 0xF900,
  HDRSCMRMAC3_T2P_FILTER_TC_NN_BASE         = 
    HDRSCMRMAC3_FIRST_SIMPLE_NN_ATTRIB_BASE,
  HDRSCMRMAC3_BURST_DURATION_FACT0R_NN_BASE = 0xFA00,
  HDRSCMRMAC3_QRAB_SELECT_NN_BASE           = 0xFB00,
  HDRSCMRMAC3_TRANSMISSION_MODE_NN_BASE     = 0xFC00,
  HDRSCMRMAC3_MERGE_THRESHOLD_NN_BASE       = 0xFD00,
  HDRSCMRMAC3_BUCKET_LEVEL_MAX_NN_BASE      = 0xFE00,
  HDRSCMRMAC3_LAST_SIMPLE_NN_ATTRIB_BASE =
    HDRSCMRMAC3_BUCKET_LEVEL_MAX_NN_BASE,
  /* Complex Attributes */
  HDRSCMRMAC3_ASSOCIATED_FLOWS_NN_BASE      = 0x0100,
  HDRSCMRMAC3_BUCKET_FACTOR_NN_BASE         = 0x0300,
  HDRSCMRMAC3_T2P_INFLOW_RANGE_NN_BASE      = 0x1500,
  HDRSCMRMAC3_T2P_TRANSITION_FUNC_NN_BASE   = 0x1600
} hdrscmrmac3_per_flow_attrib_id_base_type;

/* Max MergeThreshold for Flow 0 */
#define HDRSCMRMAC3_INFINITE_MERGE_THRESHOLD_00      0x04

/*---------------------------------------------------------------------------
                       VERSION NUMBER TYPE DECLARATIONS
----------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------

     Non NN       Non NN     NNSimple     NNSimple NNComplex   NNComplex
     Simple       Complex    Attrib1      AttribN  Attrib1     AttribN
   ----------------------------------------------------------------------
  |            |           | + + + + |..| + + + + | + + + + |..| + + + + |
   ----------------------------------------------------------------------

  The version number type is defined as a flat uint8 array with one byte for
  version number of each RMAC3 attribute. The Non-NN attributes are arranged
  in the beginning followed by NN attributes. The NN attributes are arranged
  in block of bytes for each attribute with HDRSCMRMAC3_MAX_NUM_MAC_FLOWS 
  bytes in each block with one byte for version number of each flow specific
  attribute as demonstrated by fig. above.
      
----------------------------------------------------------------------------*/
typedef uint8 
  hdrscmrmac3_verno_type[HDRSCMRMAC3_TOTAL_NUM_SIMPLE_ATTRIBS + 
                         HDRSCMRMAC3_TOTAL_NUM_COMPLEX_ATTRIBS +
                         (HDRSCMRMAC3_MAX_NUM_MAC_FLOWS * 
                           (HDRSCMRMAC3_TOTAL_NUM_NN_SIMPLE_ATTRIBS + 
                            HDRSCMRMAC3_TOTAL_NUM_NN_COMPLEX_ATTRIBS))];

/*---------------------------------------------------------------------------
                       PER-FLOW ATTRIBUTE TYPE DECLARATION
----------------------------------------------------------------------------*/

/* T2P Axis Struct type - X-axis */
typedef struct
{
  uint8                             num_points;
    /* Number of values on the T2P axis */

  uint8                             values[HDRSCMRMAC3_MAX_T2P_POINTS];
    /* The values that define the T2P axis. Units: 0.25dB */

} hdrscmrmac3_t2p_axis_struct_type;

/* FRAB Axis Struct type - Y-axis */
typedef struct
{
  uint8                             num_points;
    /* Number of values on the FRAB axis */

  uint8                             values[HDRSCMRMAC3_MAX_FRAB_POINTS];
    /* The values that define the FRAB axis. Range: (-1 ~ 7/8) */

} hdrscmrmac3_frab_axis_struct_type;

/* BucketFactorNN - T2PFRABAxis Grid type 
   The follwing struct defines the generic GRID with T2PAxis values on x-axis
   and FRAB axis values on y-axis. The function values at each (x,y) co-ordinate
   of (T2P, FRAB) combination is given by grid. The per-flow NN attribute 
   BucketFactorNN uses this data structure. 

              ^    bucket_factor()
              | + + + + + + + + + + + +
              | + + + + + + + + + + + +
              | + + + + + + + + + + + +
              | + + + + + + + + + + + +
      FRAB    | + + + + + + + + + + + +
      Axis    | + + + + + + + + + + + +
              | + + + + + + + + + + + +
              | + + + + + + + + + + + +
              +-+-+-+-+-+-+-+-+-+-+-+-+->
                  T2P Axis
*/
 
typedef struct
{
  hdrscmrmac3_t2p_axis_struct_type      t2p_axis;
    /* The T2P (x) axis */

  hdrscmrmac3_frab_axis_struct_type     frab_axis;
    /* The FRAB (y) axis */

  uint8                                 grid[HDRSCMRMAC3_MAX_T2P_POINTS]
                                            [HDRSCMRMAC3_MAX_FRAB_POINTS];
    /* The values of the function, say, f(), at the grid points. f() could
       be BucketFactor(), T2PUp or T2PDown. 
       BucketFactor: Range(1 ~ 32 7/8); Units: 1/8
       T2PUp/down: Units: 0.25dB */

} hdrscmrmac3_t2p_frab_grid_struct_type;


/* The following struct defines the GRID for T2PTransitionFunction. The struct
   defines two grids one for each of Up and Down T2PTransition functions.

              ^           Up()                 ^          Down()
              | + + + + + + + + + + + +        | + + + + + + + + + + + +        
              | + + + + + + + + + + + +        | + + + + + + + + + + + +        
              | + + + + + + + + + + + +        | + + + + + + + + + + + +        
              | + + + + + + + + + + + +        | + + + + + + + + + + + +        
      FRAB    | + + + + + + + + + + + +        | + + + + + + + + + + + +        
      Axis    | + + + + + + + + + + + +        | + + + + + + + + + + + +        
              | + + + + + + + + + + + +        | + + + + + + + + + + + +        
              | + + + + + + + + + + + +        | + + + + + + + + + + + +        
              +-+-+-+-+-+-+-+-+-+-+-+-+->      +-+-+-+-+-+-+-+-+-+-+-+-+->
                  T2P Axis                                T2P Axis
*/
typedef struct
{
  hdrscmrmac3_t2p_axis_struct_type t2p_axis;
    /* The T2P (x) axis */

  hdrscmrmac3_frab_axis_struct_type frab_axis;
    /* The FRAB (y) axis */

  uint8                                 up[HDRSCMRMAC3_MAX_T2P_POINTS]
                                          [HDRSCMRMAC3_MAX_FRAB_POINTS];
   /* T2PUp interpolation grid */

  uint8                                 down[HDRSCMRMAC3_MAX_T2P_POINTS]
                                            [HDRSCMRMAC3_MAX_FRAB_POINTS];
   /* T2PDown interpolation grid */

} hdrscmrmac3_gu_gd_struct_type;


/* T2PInflowRange */
typedef struct
{
  uint8                           min;
    /* T2PInflow minimum */

  uint8                           max;
    /* T2PInflow maximum */

} hdrscmrmac3_t2p_inflow_range_struct_type;


typedef struct
{
  uint16                            stream_id;
  uint8                             sub_stream_id;
} hdrscmrmac3_app_flow_struct_type;

/* AssociatedFlows */
typedef struct
{
  uint8                            flow_count;
  hdrscmrmac3_app_flow_struct_type app_flows[HDRSCMRMAC3_MAX_APP_FLOWS];
} hdrscmrmac3_associated_flows_struct_type;


/* Per-Flow Configuration Attributes.
   Note: The order to attributes in this struct has important implication in
   the way the attribute offset for simple attributes are computed. It is 
   important to maintain this order.
   TODO: Add the changes required if order needs to change or adding a new
   attribute. */
typedef struct
{

  uint8                                         t2p_filt_tc;
    /* T2PInflow IIR filter time constant for this MAC flow. 
       AttribId: 0xF900~0xF9NN Units: Enum Range: (0x00~0x04) Default: 0x01 */

  uint8                                         burst_duration_factor;
    /* Maximum duration this MAC flow can sustain a peak rate based on its T2P
       allocation. 
       AttribId: 0xFA00~0xFANN Units: Enum Range: (0x00~0x03) Default: 0x00 */

  uint8                                         qrab_select;
    /* Switch to control whether AT should use QRAB or QRABps as indication of
       short term sector loading for this MAC flow. 
       AttribId: 0xFB00~0xFBNN Units: Enum Range: (0x00(QRAB), 0x01(QRABps))
       Default: 0x00(QRAB) */ 

  uint8                                         tx_mode;
    /* LoLat or HiCap transmission mode. 
       AttribId: 0xFC00~0xFCNN Units: Enum Range: (0x00, 0x01) Default: 0x00 */

  uint8                                         merge_thresh;
    /* Merge threshold. If this HiCap MAC flow has more than MergeThreshold 
       octets to send then it is eligible for transmission in a new packet 
       under certain conditions (see comment under eligibility thresh struct)
       AttribId: 0xFD00~0xFDNN Units: Enum Range: (0x00~0x04) Default: 0x00 */

  uint8                                         bucket_level_max;
    /* Maximum T2P resource that can be accumulated by a flow. 0 means flow is 
       inactive; 
       AttribId: 0xFE00~0xFENN Units: Enum Range: (0x01~0xFF) Default: 0x50 */

  hdrscmrmac3_t2p_frab_grid_struct_type         bucket_factor;
    /* Standard allows for up to a 255x15 grid. 8 bit values in [1..32.875] 
       represented in 1/8 dB units, so actual value is (OTA value * 1/8) + 1 
       AttribId: 0x0300~0x03NN; */
  
  hdrscmrmac3_gu_gd_struct_type                 gu_gd;
    /* Used to determine T2PInflow up/down increments. They are aka Gu/Gd 
       functions. Values are in 8 bit 2's complement form in 1/8 dB units
       AttribId: 0x1600~0x16NN; */
  
  hdrscmrmac3_t2p_inflow_range_struct_type      t2p_inflow_range;
    /* T2PInflow min and max for this flow: 8 bit values in 1/8 dB units.
       AttribId: 0x1500~0x15NN; */

  hdrscmrmac3_associated_flows_struct_type      flow_mapping;
    /* AssociatedFlowsNN.
       AttribId: 0x0100~0x01NN; */

} hdrscmrmac3_flow_config_struct_type;


/*---------------------------------------------------------------------------
                       NON-PER-FLOW ATTRIBUTE TYPE DECLARATION
----------------------------------------------------------------------------*/

/* AuxiliaryPilotChannelGain */
typedef struct
{
  uint8                             gain_db;
    /* AuxPilotChannelGain.
       Range: (0 ~ 15); Units: -1dB; */

  uint8                             min_payload;
    /* Minimum payload size that requires Aux Pilot to be transmitted.
       Range: (0x0 ~ 0xB); Units: Enum */
    
} hdrscmrmac3_aux_pilot_struct_type;

/* CommonPowerParameters */
typedef struct
{
  uint8                             alloc_stagger;
    /* T2P allocation stagger factor across MAC flows at an AT and across AT.
       Range: (0 ~ 15) Units: 1/16 steps. */

  uint8                             tx_t2p_min_db;
    /* Minimum TxT2P that the receiver is always allowed to xmit. 
       Units: 0.25dB */

  uint8                             rpc_step_db;
    /* Reverse power control step size that AT shall use when controlling the 
       power of the reverse link. Units: Enum. Range: (0x00, 0x01) */

} hdrscmrmac3_common_pow_params_struct_type; 

/* MaxNumSubPackets */
typedef struct
{
  uint8 interlace[HDRSCMRMAC3_MAX_NUM_INTERLACES]; 
    /* Maximum number of subpackets for interlace 0 thru 2 */

} hdrscmrmac3_max_num_sub_pkts_struct_type;

/* PermittedPayload */
typedef uint8 hdrscmrmac3_permitted_payload_type
                [HDRSCMRMACCMN_MAX_NUM_PAYLOADS] 
                [HDRSCMRMAC3_MAX_NUM_SUBFRAME_HISTORY];

/* PilotStrength */
typedef struct 
{
  uint8 num_ps_axis_values;
    /* Number of values on the pilot strength axis. */

  uint8 ps_axis[HDRSCMRMAC3_MAX_PS_POINTS];
    /* Values that define the pilotstrength axis. 
       Units: -1dB Range: (0~15) */

  uint8 ps_ps_axis[HDRSCMRMAC3_MAX_PS_POINTS];
    /* PilotStrength(ps) corresponding to pilotstrength values defined. 
       Units: 1dB */

} hdrscmrmac3_pilot_strength_grid_struct_type;

typedef struct
{
  uint8 t2p_transition;
    /* One less than the Number of subframes for which the receiver shall use 
       the pre-transition T2P values and the number of sub-frames after which 
       the receiver shall use the post-transition T2P values when xmitting a 
       Reverse traffic channel packet. Units: subframes */

  uint8 termination_target;
    /* One less than the expected number of sub-frames needed to acheive the
       target physical layer erasure rate for a reverse traffic channel packet
       Units: subframes */

  uint8 pre_transition;
    /* Data channel power relative to the pilot channel power when xmitting 
       prior to T2P transition. Units: 0.25 dB. Range: (0x00 ~ 0x80) */

  uint8 post_transition;
    /* Data channel power relative to the pilot when transmitting follwing 
       the T2P transition. Units: 0.25 dB. Range: (0x00 ~ 0x80) */

} hdrscmrmac3_power_params_struct_type;


/* RequestParameters */
typedef struct
{
  uint8                            req_ratio;
    /* Maximum permitted ratio of the request bits to traffic bits.
       Range: (0 ~ 15) Units: 0.005 */

  uint8                            max_req_interval;
    /* Maximum time duration that AT shall wait to xmit request pkt once
       it has xmitted a request pkt. 
       Range: (0 ~ 15) Units: Frames. Zero disables Request pkt xmission. */

} hdrscmrmac3_req_params_struct_type;

/* RRIChannelPowerParameters */
typedef struct
{
  uint8                             rri_chan_gain_pre_transition;
    /* RRI Channel gain for subpackets transmitted prior to T2P transition
       Units: 1dB */

  uint8                             rri_chan_gain_post_transition;
    /* RRI Channel gain for subpackets transmitted after T2P transition.
       Units: 1dB */

} hdrscmrmac3_rri_power_params_struct_type;

/* TxT2PMax */
typedef struct 
{
  uint8 num_ps_axis_values;
    /* Number of values on the pilot strength axis. */

  uint8  ps_axis[HDRSCMRMAC3_MAX_PS_POINTS];
    /* Values that define the pilotstrength axis. 
       Range: (0 ~ 15); Units: -1 dB */

  uint8  txt2p_max_ps_axis[HDRSCMRMAC3_MAX_PS_POINTS];
    /* T2Pmax(ps) values corresponding to pilotstrength values defined.
       Units: 0.5 dB */

} hdrscmrmac3_txt2p_max_grid_struct_type;

/* MaxMACFlows */
typedef struct
{
  uint8 max_num_mac_flows;
    /* Maximum total number of activated and deactivated MAC flows supported
       Range: (0x04 ~ 0x10) Default: 0x04 */

  uint8 max_num_active_mac_flows;
    /* Maximum number of active MAC flows supported
       Range: (0x04 - max_num_mac_flows); Default: 0x04*/

} hdrscmrmac3_max_mac_flows_struct_type;


/*---------------------------------------------------------------------------
                    ST-3 RMAC Configuration Attributes 
----------------------------------------------------------------------------*/
typedef struct
{
  
  uint8 gaup_max_num_sub_pkts;
    /* Is modification of MaxNumSubPackets attribute GAUPable? 
       AttribId: 0xFFF2; Units: Enum; Range: (0x00, 0x01); Default: 0x00 */

  uint8 rate_1m8_supported;
    /* Does AN support 1.8 Mbps rate transmission on Reverse channel? 
       AttribID: 0xFFF3; units: Enum; Range: (0x00, 0x01); Default: 0x00 */

  uint8 ps_qrab_thresh_drc_unlock;
    /* Minimum PilotStrength required for QRAB from non-serving sector to be
       included in QRABps computation when DRC is not in lock.
       AttribId: 0xFFF5; Units: Enum; Range: (0x00~0x03); Default: 0x01 */

  uint8 ps_qrab_thresh_drc_lock;
    /* Minimum PilotStrength required for QRAB from non-serving sector to be
       included in QRABps computation when DRC is in lock. 
       AttribId: 0xFFF6; Units: Enum; Range: (0x00~0x03); Default: 0x02 */

  uint8 t2p_no_tx_filter_tc;
    /* T2PInflow IIR filter time constant when not transmitting; 
       AttribId: 0xFFF7; Units: Enum; Range: (0x00~0x04); Default: 0x01 */

  uint8 qrab_filter_tc;
    /* QRAB IIR filter time constant; 
       AttribId: 0xFFF9; Units: Enum; Range: (0x00, 0x01); Default: 0x00 */

  uint8 ps_filter_tc;
    /* PilotStrength IIR filter time constant; 
       AttribId: 0xFFFA; Units: Enum; Range: (0x00~0x02); Default: 0x01 */

  uint8 payload_thresh;
    /* Minimum packets size permitted to be transmitted in LoLat mode when it
       does not contain LoLat flows. 
       AttribId: 0xFFFB; Units: Enum; Range: (0x00~0x03); Default: 0x01 */

  uint8 merge_thresh;
    /* AT-wide Merge Threshold. 
       AttribId: 0xFFFC; Units: Enum; Range: (0x00~0x05); Default: 0x02 */

  uint8 frab_low;
    /* Reverse Link Loading threshold. 
       AttribId: 0xFFFD; Units: Enum; Range: (0x00~0x04); Default: 0x03 */

  uint8 frab_filter_tc;
    /* FRAB IIR filter time constant; 
       AttribId: 0xFFFE; Units: Enum; Range: (0x00~0x03); Default: 0x02 */

  uint8 arq_mode;
    /* Reverse link ARQ mode (Bipolar or On/Off keying) 
       AttribId: 0xFFFF; Units: Enum; Range (0x00, 0x01); Default: 0x00 */
  
  hdrscmrmac3_flow_config_struct_type flows[HDRSCMRMAC3_MAX_NUM_MAC_FLOWS];
    /* 8 flows supported for now */

  hdrscmrmac3_aux_pilot_struct_type aux_pilot_info;
    /* Minimum payload requiring transmission of Aux Pilot and its gain 
       relative to Traffic Channel. AttribId: 0x0000; */

  hdrscmrmac3_common_pow_params_struct_type common_pow_params;
    /* Common Power Parameters Attributes. AttribId: 0x0001 */

  hdrscmrmac3_max_num_sub_pkts_struct_type max_num_sub_pkts;
    /* Maximum number of subpackets for each of the interlaces 
       AttribId: 0x0002 */

  hdrscmrmac3_permitted_payload_type permitted_payload;
    /* Maximum Packet size the the receiver can transmit in the subframe y if
       x bits were transmitted in subframe (n-1) is given by 
       permitted_payload(x,y). AttribId: 0x0003 */ 

  hdrscmrmac3_pilot_strength_grid_struct_type pilot_strength;
    /* Scaling factor in 4 bit 2's complement format used to scale T2PInflow
       if filtered PilotStrength equals corresponding PilotStrength axis point.
       AttribId: 0x0004 */

  hdrscmrmac3_power_params_struct_type      
    power_params [HDRSCMRMACCMN_MAX_NUM_PAYLOADS] [HDRSCMRMAC3_NUM_TX_MODES];
  /* Power Parameters Attribute.
     AttribId: 0x0005 ~ 0x0010. 12 attributes one for each payload size  */

  hdrscmrmac3_req_params_struct_type        req_params;
    /* ReqRatio and MaxReqInterval. AttribId: 0x0011 */

  hdrscmrmac3_rri_power_params_struct_type  rri_chan_power_params
                                           [HDRSCMRMAC3_NUM_T2P_TRANS_POINTS];
    /* RRI channels gains corresponding to the various combinations of pre and 
     * post T2P transition points. AttribId: 0x0012 */

  hdrscmrmac3_txt2p_max_grid_struct_type    txt2p_max;
    /* Grid that defines the maximum TxT2P AT is allowed to transmit if 
       filtered serving sector PilotStrength equals corresponding 
       PilotStrengthAxis value. AttribOd: 0x0013 */

  hdrscmrmac3_max_mac_flows_struct_type     max_mac_flows;
    /* Maximum totol number of MAC flows supported. AttribId: 0x0014 */

} hdrscmrmac3_config_struct_type;


 /* EJECT */
/*===========================================================================

FUNCTION HDRSCMRMAC3_PROCESS_COMPLEX_ATTRIB

DESCRIPTION
  This is a generic interface to process all complex attributes given below.

  1.  0x0000 - AuxillaryPilotChannelParameters Attribute 
  2.  0x0001 - CommonPowerParameters Attribute 
  3.  0x0002 - MaxNumSubPackets Attribute 
  4.  0x0003 - PermittedPayload Attribute
  5.  0x0004 - PilotStrength Attribute
  6.  0x0005 - PowerParameters128 Attribute
      0x0006 - PowerParameters256 Attribute 
      ...
      0x0010 - PowerParameters12288 Attribute
  7.  0x0011 - RequestParameters Attribute
  8.  0x0012 - RRIChannelPowerParameters Attribute
  9.  0x0013 - TxT2PMax Attribute
  10. 0x0014 - MaxMACFlows Attribute 

  Per-Flow Attributes:
  ===================
  11. 0x0100 - 0x01NN - AssociatedFlowsNN Attributes
  12. 0x0300 - 0x03NN - BucketFactorNN Attributes
  13. 0x1500 - 0x15NN - T2PInflowRangeNN Attributes
  14. 0x1600 - 0x16NN - T2PTransitionFunctionNN Attributes

DEPENDENCIES
  None.

PARAMETERS
  item_ptr - pointer to the DSM item holding the message to be parsed.
  p_aip    - proposed attribute information pointer.
  a_aip    - The pointer points to the accepted attribute information
             struct that gets filled in after the complex attribute is parsed.

RETURN VALUE
  TRUE  - If a valid complex attribute was parsed successfully.
  FALSE - otherwise. 

SIDE EFFECTS
  None.

===========================================================================*/
extern hdrerrno_enum_type hdrscmrmac3_process_complex_attrib
(
  dsm_item_type                               *item_ptr,
  hdrscm_proposed_attrib_type   const *const   p_aip,
  hdrscm_accepted_attrib_type         *const   a_aip
);

 /* EJECT */
/*===========================================================================

FUNCTION HDRSCMRMAC3_IS_ATTRIB_SIMPLE

DESCRIPTION
  This function is used by SCM to query if an attribute is simple, given the
  attribute ID.
  
DEPENDENCIES
  None.

PARAMETERS
  attrib_id - Attribute ID 

RETURN VALUE
  TRUE - if the attribute given by 'attrib_id' is simple.
  FALSE - otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
extern boolean hdrscmrmac3_is_attrib_simple
(
  uint16 const attrib_id
);


 /* EJECT */
/*===========================================================================

FUNCTION HDRSCMRMAC3_IS_SIMPLE_ATTRIB_VALID

DESCRIPTION
  This function is used by SCM to query if the given 'attrib_value' is a 
  valid value for the attribute specified by 'attrib_id'.
  
DEPENDENCIES
  None.

PARAMETERS
  attrib_id    - Attribute ID of the simple attribute.
  attrib_value - Value of the simple attribute. 

RETURN VALUE
  E_SUCCESS          - If the attrib_value is valid.
  E_OUT_OF_RANGE     - Otherwise.

SIDE EFFECTS
  None.
  
===========================================================================*/
extern hdrerrno_enum_type hdrscmrmac3_is_simple_attrib_valid
(
  uint16 const attrib_id,
  uint32 const attrib_value 
);
 /* EJECT */
/*===========================================================================

FUNCTION HDRSCMRMAC3_GET_ATTRIB_OFFSET_AND_SIZE

DESCRIPTION
  This function computes the byte offset of the attribute specified by the 
  attrib_id in the struct hdrscmrmac3_config_struct_type and stores it in 
  out-param attrib_offset. It also stores the size of the attribute in 
  attrib_size.

DEPENDENCIES
  None

PARAMETERS
  attrib_id     - Attribute Id.
  attrib_offset - Address where the Byte Offset from the start of the struct 
                  to the attribute is stored.
  attrib_size   - Address where the size of the attribute is stored.
  
RETURN VALUE
  E_SUCCESS - If the given attrib ID is valid.
  E_DATA_INVALID - Otherwise.

SIDE EFFECTS
  None

===========================================================================*/
extern hdrerrno_enum_type hdrscmrmac3_get_attrib_offset_and_size
(
  uint16 attrib_id, 
  uint16 *attrib_offset,
  uint16 *attrib_size
);

/* EJECT */
/*===========================================================================

FUNCTION HDRSCMRMAC3_GET_ATTRIB_VERNO_INDEX

DESCRIPTION
  Returns the index of the given attribute in the version number array.

DEPENDENCIES
  None

PARAMETERS
  attrib_id     - Attribute Id.
  index         - Address where the index is stored.
  
RETURN VALUE
  E_SUCCESS - If the given attribute ID is valid.
  

SIDE EFFECTS
  None

===========================================================================*/
extern hdrerrno_enum_type hdrscmrmac3_get_attrib_verno_index
(
  uint16 attrib_id,
  uint16 *index
);

/* EJECT */
/*===========================================================================

FUNCTION HDRSCMRMAC3_SET_CONFIG_TO_DEFAULT

DESCRIPTION
  Set the ST-3 RMAC configuration attributes to its default values.

DEPENDENCIES
  None

PARAMETERS
  p_config - Pointer to hdrscmrmac3_config_struct_type
  
RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
extern void hdrscmrmac3_set_config_to_default
(
  hdrscmrmac3_config_struct_type *p_config
);

/* EJECT */
/*===========================================================================

FUNCTION HDRSCMRMAC3_PROP_MAX_MAC_FLOWS

DESCRIPTION
  This function is used to propose MaxNumMACFlows attribute as part of AT-init
  session configuration.

DEPENDENCIES
  None

PARAMETERS
  None.
   
RETURN VALUE
  TRUE - If the a Rsp timer was available for use.
  FALSE - otherwise.

SIDE EFFECTS
  None

===========================================================================*/
boolean hdrscmrmac3_prop_max_mac_flows( void );

/* EJECT */
/*===========================================================================

FUNCTION HDRSCMRMAC3_GET_PROP_ATTRIB_VALUE

DESCRIPTION
  This function is used to get the proposed complex attribute value

DEPENDENCIES
  None

PARAMETERS
  trans_id - Transaction ID
  a_aip - Attribute Information pointer 
     
RETURN VALUE
  E_SUCCESS - If the transaction ID is valid.
  E_FAILURE - Otherwise.

SIDE EFFECTS
  None

===========================================================================*/
hdrerrno_enum_type hdrscmrmac3_get_prop_attrib_value
(
  uint16 const trans_id,
  hdrscm_accepted_attrib_type *a_aip
);

/* <EJECT> */
/*===========================================================================

FUNCTION HDRSCMRMAC3_FAIL_CURR_CONFIG_NEG

DESCRIPTION
  This function fails the current config negotiation

DEPENDENCIES
  None

PARAMETERS
  close_reason - Session Close reason

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void hdrscmrmac3_fail_curr_config_neg
(
  hdrhai_session_close_reason_enum_type close_reason
);


 /* EJECT */
/*===========================================================================

FUNCTION HDRSCMRMAC3_IS_ATTRIB_GAUPABLE

DESCRIPTION
  This function is used by SCM module to find out if an attribute is GAUPable.

DEPENDENCIES
  None.

PARAMETERS
  attrib_id - Attribute ID.
  
RETURN VALUE
  E_SUCCESS       - If the attribute is GAUPable.
  E_NOT_ALLOWED   - If AN is not allowed to GAUP this attribute.

SIDE EFFECTS
  None.
===========================================================================*/
hdrerrno_enum_type hdrscmrmac3_is_attrib_gaupable
(
  uint16 const attrib_id
);


/* EJECT */
/*===========================================================================

FUNCTION HDRSCMRMAC3_ATTRIB_IS_SUPPORTED

DESCRIPTION
  Is this a known attribute?

DEPENDENCIES
  None

PARAMETERS 
  attrib_id: Attribute ID in question.
  subtype:   Subtype of the protocol that received this attribute.

RETURN VALUE
  TRUE - If the attribute is supported.
  FALSE - Otherwise

SIDE EFFECTS
  None

===========================================================================*/
boolean hdrscmrmac3_attrib_is_supported
(
  uint16 const attrib_id
);

#endif /* HDRSCMRMAC3_H */
