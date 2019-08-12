#ifndef HDRSCMRMACCMN_H
#define HDRSCMRMACCMN_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

      H D R   S E S S I O N   C O N F I G U R A T I O N   M A N A G E R

       R E V E R S E  C H A N N E L  F L U I D  M A C  P R O T O C O L

                           H E A D E R  F I L E

       
GENERAL DESCRIPTION
  This module containing processing functions to handle the attributs
  processing for Reverse Channel MAC.  This module currently supports
  the processing of the common attributes between the Subtype 3 and 
  Subtype 4 RTCMAC protocols.
    
EXTERNALIZED FUNCTIONS
   
INITIALIZATION AND SEQUENCING REQUIREMENTS

  
  Copyright (c) 2007 - 2014 by Qualcomm Technologies, Inc.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                           EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/hdr/cp/api/hdrscmrmaccmn.h#1 $ $DateTime: 2016/12/13 07:58:05 $ $Author: mplcsds1 $
   
when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/11/14   tnp     Increase maximum number of supported MAC flows to 8
01/14/14   tnp     Reduce maximum number of supported MAC flows to 5
09/19/13   ukl     Reduce Code Duplication HDR-CP
03/30/07   cc      Created module.

===========================================================================*/

/*===========================================================================

                          INCLUDE FILES FOR MODULE
                     
===========================================================================*/

#include "comdef.h"

#include "hdrerrno.h"
#include "hdrscm.h"

/*===========================================================================

             DEFINITIONS  FROM  THE  HDR  AIR  INTERFACE  SPEC

===========================================================================*/

#define HDRSCMRMACCMN_ATTRIB_ID_SIZE                    16
#define HDRSCMRMACCMN_VALUE_ID_SIZE                     8

typedef enum
{
  HDRSCMRMACCMN_FIRST_PAYLOAD_SIZE  = 0x0,
  HDRSCMRMACCMN_0_BITS              = HDRSCMRMACCMN_FIRST_PAYLOAD_SIZE,
  HDRSCMRMACCMN_128_BITS            = 0x1,
  HDRSCMRMACCMN_256_BITS            = 0x2,
  HDRSCMRMACCMN_512_BITS            = 0x3,
  HDRSCMRMACCMN_768_BITS            = 0x4,
  HDRSCMRMACCMN_1024_BITS           = 0x5,
  HDRSCMRMACCMN_1536_BITS           = 0x6,
  HDRSCMRMACCMN_2048_BITS           = 0x7,
  HDRSCMRMACCMN_3072_BITS           = 0x8,
  HDRSCMRMACCMN_4096_BITS           = 0x9,
  HDRSCMRMACCMN_6144_BITS           = 0xA,
  HDRSCMRMACCMN_8192_BITS           = 0xB,
  HDRSCMRMACCMN_12288_BITS          = 0xC,
  HDRSCMRMACCMN_LAST_PAYLOAD_SIZE   = HDRSCMRMACCMN_12288_BITS,
  HDRSCMRMACCMN_MAX_NUM_PAYLOADS
} hdrscmrmaccmn_payload_size_enum_type;

 /* EJECT */
/*===========================================================================
 
                  DEFINITIONS AND DECLARATIONS FOR MODULE

  This section contains local definitions for constants, macros, types,
  variables and other items needed by this module.

===========================================================================*/
/* MaxMACFlows */
#define HDRSCMRMACCMN_DEFAULT_MAX_MAC_FLOWS               0x04
#define HDRSCMRMACCMN_MAX_NUM_MAC_FLOWS                   8

/* Max number of interlaces in the reverse link */
#define HDRSCMRMACCMN_MAX_NUM_INTERLACES                  3


#define HDRSCMRMACCMN_MAX_T2P_POINTS                      16
#define HDRSCMRMACCMN_MAX_FRAB_POINTS                     8
#define HDRSCMRMACCMN_MAX_APP_FLOWS                       8
#define HDRSCMRMACCMN_MAX_PS_POINTS                       15
#define HDRSCMRMACCMN_NUM_T2P_TRANS_POINTS                4

#define HDRSCMRMACCMN_MAX_NUM_SUBFRAME_HISTORY            3

/* Align attribute offset to the next octet boundary */
#define HDRSCMRMACCMN_ALIGN_TO_NEXT_OCTET(attrib_offset)                 \
              ((attrib_offset) = ((((attrib_offset) + 7) / 8) * 8))

/* Macros to get Flow ID from attrib ID */
#define HDRSCMRMACCMN_NN_ATTRIB_FLOW_ID_MASK              0x00FF
#define HDRSCMRMACCMN_GET_NN_ATTRIB_FLOW_ID(attrib_id)  \
          ((attrib_id) & HDRSCMRMACCMN_NN_ATTRIB_FLOW_ID_MASK)

typedef enum
{
  HDRSCMRMACCMN_TX_MODE_HI_CAP       = 0x0,
  HDRSCMRMACCMN_TX_MODE_LO_LAT       = 0x1,
  HDRSCMRMACCMN_NUM_TX_MODES         = 0x2 
} hdrscmrmaccmn_tx_mode_enum_type;

/* Max MergeThreshold for Flow 0 */
#define HDRSCMRMACCMN_INFINITE_MERGE_THRESHOLD_00      0x04

/* The type hdrscmrmaccmn_simple_attrib_valid_range_type defines a min and a 
 * max field to specify the Valid range of values that a simple attribute
 * could take. */
typedef struct
{
  uint8 min;    /* Minimum allowed value for the simple attribute */
  uint8 max;    /* Maximum allowed value for the simple attribute */

} hdrscmrmaccmn_simple_attrib_valid_range_type;

/* Complex attribute parsing routine type */
typedef hdrerrno_enum_type (*hdrscmrmaccmn_complex_attrib_handler_type)
(
  dsm_item_type                               *item_ptr,
  hdrscm_proposed_attrib_type   const *const   p_aip,
  hdrscm_accepted_attrib_type         *const   a_aip
);


/*---------------------------------------------------------------------------
                       ATTRIBUTE TYPE DECLARATION
----------------------------------------------------------------------------*/

typedef struct
{
  uint16                            stream_id;
  uint8                             sub_stream_id;
} hdrscmrmaccmn_app_flow_struct_type;

/* AssociatedFlows */
typedef struct
{
  uint8                            flow_count;
  hdrscmrmaccmn_app_flow_struct_type app_flows[HDRSCMRMACCMN_MAX_APP_FLOWS];
} hdrscmrmaccmn_associated_flows_struct_type;

/* T2P Axis Struct type - X-axis */
typedef struct
{
  uint8                             num_points;
    /* Number of values on the T2P axis */

  uint8                             values[HDRSCMRMACCMN_MAX_T2P_POINTS];
    /* The values that define the T2P axis. Units: 0.25dB */

} hdrscmrmaccmn_t2p_axis_struct_type;

/* FRAB Axis Struct type - Y-axis */
typedef struct
{
  uint8                             num_points;
    /* Number of values on the FRAB axis */

  uint8                             values[HDRSCMRMACCMN_MAX_FRAB_POINTS];
    /* The values that define the FRAB axis. Range: (-1 ~ 7/8) */

} hdrscmrmaccmn_frab_axis_struct_type;

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
  hdrscmrmaccmn_t2p_axis_struct_type      t2p_axis;
    /* The T2P (x) axis */

  hdrscmrmaccmn_frab_axis_struct_type     frab_axis;
    /* The FRAB (y) axis */

  uint8                                     grid[HDRSCMRMACCMN_MAX_T2P_POINTS]
                                                [HDRSCMRMACCMN_MAX_FRAB_POINTS];
    /* The values of the function, say, f(), at the grid points. f() could
       be BucketFactor(), T2PUp or T2PDown. 
       BucketFactor: Range(1 ~ 32 7/8); Units: 1/8
       T2PUp/down: Units: 0.25dB */

} hdrscmrmaccmn_t2p_frab_grid_struct_type;

/* MaxNumSubPackets */
typedef struct
{
  uint8 interlace[HDRSCMRMACCMN_MAX_NUM_INTERLACES]; 
    /* Maximum number of subpackets for interlace 0 thru 2 */

} hdrscmrmaccmn_max_num_sub_pkts_struct_type;

/* T2PInflowRange */
typedef struct
{
  uint8                           min;
    /* T2PInflow minimum */

  uint8                           max;
    /* T2PInflow maximum */

} hdrscmrmaccmn_t2p_inflow_range_struct_type;

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
  hdrscmrmaccmn_t2p_axis_struct_type  t2p_axis;
    /* The T2P (x) axis */

  hdrscmrmaccmn_frab_axis_struct_type frab_axis;
    /* The FRAB (y) axis */

  uint8                                 up[HDRSCMRMACCMN_MAX_T2P_POINTS]
                                          [HDRSCMRMACCMN_MAX_FRAB_POINTS];
   /* T2PUp interpolation grid */

  uint8                                 down[HDRSCMRMACCMN_MAX_T2P_POINTS]
                                            [HDRSCMRMACCMN_MAX_FRAB_POINTS];
   /* T2PDown interpolation grid */

} hdrscmrmaccmn_gu_gd_struct_type;

/* PilotStrength */
typedef struct 
{
  uint8 num_ps_axis_values;
    /* Number of values on the pilot strength axis. */

  uint8 ps_axis[HDRSCMRMACCMN_MAX_PS_POINTS];
    /* Values that define the pilotstrength axis. 
       Units: -0.25dB Range: (0~63) */

  uint8 ps_ps_axis[HDRSCMRMACCMN_MAX_PS_POINTS];
    /* PilotStrength(ps) corresponding to pilotstrength values defined. 
       Units: 1dB */

} hdrscmrmaccmn_pilot_strength_grid_struct_type;

/* TxT2PMax */
typedef struct 
{
  uint8 num_ps_axis_values;
    /* Number of values on the pilot strength axis. */

  uint8  ps_axis[HDRSCMRMACCMN_MAX_PS_POINTS];
    /* Values that define the pilotstrength axis. 
       Range: (0 ~ 63); Units: -0.25 dB */

  uint8  txt2p_max_ps_axis[HDRSCMRMACCMN_MAX_PS_POINTS];
    /* T2Pmax(ps) values corresponding to pilotstrength values defined.
       Units: 0.5 dB */

} hdrscmrmaccmn_txt2p_max_grid_struct_type;

/* AuxiliaryPilotChannelGain */
typedef struct
{
  uint8                             gain_db;
    /* AuxPilotChannelGain.
       Range: (0 ~ 15); Units: -1dB; */

  uint8                             min_payload;
    /* Minimum payload size that requires Aux Pilot to be transmitted.
       Range: (0x0 ~ 0xB); Units: Enum */
    
} hdrscmrmaccmn_aux_pilot_struct_type;

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

} hdrscmrmaccmn_common_pow_params_struct_type; 

/* PermittedPayload */
typedef uint8 hdrscmrmaccmn_permitted_payload_type
                [HDRSCMRMACCMN_MAX_NUM_PAYLOADS] 
                [HDRSCMRMACCMN_MAX_NUM_SUBFRAME_HISTORY];

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

} hdrscmrmaccmn_power_params_struct_type;


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

} hdrscmrmaccmn_req_params_struct_type;

/* RRIChannelPowerParameters */
typedef struct
{
  uint8                             rri_chan_gain_pre_transition;
    /* RRI Channel gain for subpackets transmitted prior to T2P transition
       Units: 1dB */

  uint8                             rri_chan_gain_post_transition;
    /* RRI Channel gain for subpackets transmitted after T2P transition.
       Units: 1dB */

} hdrscmrmaccmn_rri_power_params_struct_type;


/* MaxMACFlows */
typedef struct
{
  uint8 max_num_mac_flows;
    /* Maximum total number of activated and deactivated MAC flows supported
       Range: (0x04 ~ 0x10) Default: 0x04 */

  uint8 max_num_active_mac_flows;
    /* Maximum number of active MAC flows supported
       Range: (0x04 - max_num_mac_flows); Default: 0x04*/

} hdrscmrmaccmn_max_mac_flows_struct_type;

#define HDRSCMRMACCMN_MAX_NUM_ACTIVE_MAC_FLOWS_SIZE     8
#define HDRSCMRMACCMN_MAX_NUM_MAC_FLOWS_SIZE            8
#define HDRSCMRMACCMN_MAX_MAC_FLOWS_BYTE_LENGTH         5 

/*---------------------------------------------------------------------------
                Default Values for Configuration Attributes
----------------------------------------------------------------------------*/
/* Non-Per_Flow Simple Attributes */
#define HDRSCMRMACCMN_DEFAULT_FRAB_FILTER_TC            0x02
#define HDRSCMRMACCMN_DEFAULT_FRAB_LOW                  0x03
#define HDRSCMRMACCMN_DEFAULT_MERGE_THRESHOLD           0x02
#define HDRSCMRMACCMN_DEFAULT_PAYLOAD_THRESHOLD         0x01
#define HDRSCMRMACCMN_DEFAULT_PILOT_STRENGTH_FILTER_TC  0x01
#define HDRSCMRMACCMN_DEFAULT_T2P_NO_TX_FILTER_TC       0x01
#define HDRSCMRMACCMN_DEFAULT_PS_QRAB_THRESH_DRC_LOCK   0x18
#define HDRSCMRMACCMN_DEFAULT_PS_QRAB_THRESH_DRC_UNLOCK 0x0C

/* Per-Flow Simple NN Attributes */
#define HDRSCMRMACCMN_DEFAULT_00_BUCKET_LEVEL_MAX       0x50
#define HDRSCMRMACCMN_DEFAULT_01_BUCKET_LEVEL_MAX       0x6C
#define HDRSCMRMACCMN_DEFAULT_NN_MERGE_THRESHOLD        0x02
#define HDRSCMRMACCMN_DEFAULT_NN_T2P_FILTER_TC          0x01

/* Non-Per-Flow complex Attributes */
/* AuxiliaryPilotChannelParameters */
#define HDRSCMRMACCMN_DEFAULT_AUX_PILOT_CHAN_GAIN         0x0C
#define HDRSCMRMACCMN_DEFAULT_AUX_PILOT_CHAN_MIN_PAYLOAD  0x07

/* CommonPowerParameters */
#define HDRSCMRMACCMN_DEFAULT_CPP_TX_T2P_MIN              0x0F
#define HDRSCMRMACCMN_DEFAULT_CPP_RPC_STEP                0x01

/* MaxNumSubPackets */
#define HDRSCMRMACCMN_DEFAULT_MAX_NUM_SUB_PACKETS         0x03

/* PermittedPayload */
#define HDRSCMRMACCMN_DEFAULT_PERMITTED_PAYLOAD_0_4       0x05
#define HDRSCMRMACCMN_DEFAULT_PERMITTED_PAYLOAD_5_6       0x07
#define HDRSCMRMACCMN_DEFAULT_PERMITTED_PAYLOAD_7_8       0x09
#define HDRSCMRMACCMN_DEFAULT_PERMITTED_PAYLOAD_9_A       0x0B
#define HDRSCMRMACCMN_DEFAULT_PERMITTED_PAYLOAD_B_C       0x0C

/* PilotStrength */
#define HDRSCMRMACCMN_DEFAULT_PS_NUM_PS_AXIS_VALS         0x02
#define HDRSCMRMACCMN_DEFAULT_PS_PS_AXIS_0                0x3C

/* PowerParameters128-12288 */
#define HDRSCMRMACCMN_DEFAULT_PP_LL_T2P_TRANS             0x01
#define HDRSCMRMACCMN_DEFAULT_PP_LL_TERM_TARGET           0x01
#define HDRSCMRMACCMN_DEFAULT_PP_HC_T2P_TRANS             0x03
#define HDRSCMRMACCMN_DEFAULT_PP_HC_TERM_TARGET           0x03
/* PowerParameters128 */
#define HDRSCMRMACCMN_DEFAULT_PP_LL_PRE_TRANS_128         0x1C
#define HDRSCMRMACCMN_DEFAULT_PP_LL_POST_TRANS_128        0x03
#define HDRSCMRMACCMN_DEFAULT_PP_HC_TRANS_128             0x03
/* PowerParameters256 */
#define HDRSCMRMACCMN_DEFAULT_PP_LL_PRE_TRANS_256         0x28
#define HDRSCMRMACCMN_DEFAULT_PP_LL_POST_TRANS_256        0x0F
#define HDRSCMRMACCMN_DEFAULT_PP_HC_TRANS_256             0x0F
/* PowerParameters512 */
#define HDRSCMRMACCMN_DEFAULT_PP_LL_PRE_TRANS_512         0x34
#define HDRSCMRMACCMN_DEFAULT_PP_LL_POST_TRANS_512        0x1C
#define HDRSCMRMACCMN_DEFAULT_PP_HC_TRANS_512             0x1C
/* PowerParameters768 */
#define HDRSCMRMACCMN_DEFAULT_PP_LL_PRE_TRANS_768         0x3B
#define HDRSCMRMACCMN_DEFAULT_PP_LL_POST_TRANS_768        0x23
#define HDRSCMRMACCMN_DEFAULT_PP_HC_TRANS_768             0x23
/* PowerParameters1024 */
#define HDRSCMRMACCMN_DEFAULT_PP_LL_PRE_TRANS_1024        0x41
#define HDRSCMRMACCMN_DEFAULT_PP_LL_POST_TRANS_1024       0x28
#define HDRSCMRMACCMN_DEFAULT_PP_HC_TRANS_1024            0x28
/* PowerParameters1536 */
#define HDRSCMRMACCMN_DEFAULT_PP_LL_PRE_TRANS_1536        0x49
#define HDRSCMRMACCMN_DEFAULT_PP_LL_POST_TRANS_1536       0x2E
#define HDRSCMRMACCMN_DEFAULT_PP_HC_TRANS_1536            0x2E
/* PowerParameters2048 */
#define HDRSCMRMACCMN_DEFAULT_PP_LL_PRE_TRANS_2048        0x4D
#define HDRSCMRMACCMN_DEFAULT_PP_LL_POST_TRANS_2048       0x34
#define HDRSCMRMACCMN_DEFAULT_PP_HC_TRANS_2048            0x34
/* PowerParameters3072 */
#define HDRSCMRMACCMN_DEFAULT_PP_LL_PRE_TRANS_3072        0x4D
#define HDRSCMRMACCMN_DEFAULT_PP_LL_POST_TRANS_3072       0x39
#define HDRSCMRMACCMN_DEFAULT_PP_HC_TRANS_3072            0x39
/* PowerParameters4096 */
#define HDRSCMRMACCMN_DEFAULT_PP_LL_PRE_TRANS_4096        0x52
#define HDRSCMRMACCMN_DEFAULT_PP_LL_POST_TRANS_4096       0x3E
#define HDRSCMRMACCMN_DEFAULT_PP_HC_TRANS_4096            0x3E
/* PowerParameters6144 */
#define HDRSCMRMACCMN_DEFAULT_PP_LL_PRE_TRANS_6144        0x57
#define HDRSCMRMACCMN_DEFAULT_PP_LL_POST_TRANS_6144       0x44
#define HDRSCMRMACCMN_DEFAULT_PP_HC_TRANS_6144            0x44
/* PowerParameters8192 */
#define HDRSCMRMACCMN_DEFAULT_PP_LL_PRE_TRANS_8192        0x5D
#define HDRSCMRMACCMN_DEFAULT_PP_LL_POST_TRANS_8192       0x4A
#define HDRSCMRMACCMN_DEFAULT_PP_HC_TRANS_8192            0x4A
/* PowerParameters12288 */
#define HDRSCMRMACCMN_DEFAULT_PP_LL_PRE_TRANS_12288       0x69
#define HDRSCMRMACCMN_DEFAULT_PP_LL_POST_TRANS_12288      0x55
#define HDRSCMRMACCMN_DEFAULT_PP_HC_TRANS_12288           0x55

/* RRIChannelPowerParameters */
#define HDRSCMRMACCMN_DEFAULT_RRI_CHAN_GAIN_TRANS         0x0A

/* TxTPMax or TxT2PMaxConfigXX */
#define HDRSCMRMACCMN_DEFAULT_TXT2P_NUM_PS_AXIS_VALS      0x02
#define HDRSCMRMACCMN_DEFAULT_PS_AXIS_0                   0x28
#define HDRSCMRMACCMN_DEFAULT_PS_AXIS_1                   0x14
#define HDRSCMRMACCMN_DEFAULT_TXT2P_PS_AXIS_0             0x18
#define HDRSCMRMACCMN_DEFAULT_TXT2P_PS_AXIS_1             0x36

/* Per-Flow Complex Attributes */
/* Associated FlowsNN */
#define HDRSCMRMACCMN_DEFAULT_NN_ASSO_FLOWS_FLOW_COUNT    0x01
#define HDRSCMRMACCMN_DEFAULT_NN_ASSO_FLOWS_STREAM        511

/* BucketFactor00 */
#define HDRSCMRMACCMN_DEFAULT_00_BF_NUM_T2P_AXIS_VALS     0x01
#define HDRSCMRMACCMN_DEFAULT_00_BF_NUM_FRAB_AXIS_VALS    0x00
#define HDRSCMRMACCMN_DEFAULT_00_BF_T2P_AXIS_VALS_0       0x10
#define HDRSCMRMACCMN_DEFAULT_00_BF_T2P_AXIS_VALS_1       0x2B
#define HDRSCMRMACCMN_DEFAULT_00_BUCKET_FACTOR_0_0        0x18
#define HDRSCMRMACCMN_DEFAULT_00_BUCKET_FACTOR_1_0        0x08
/* BucketFactorNN */
#define HDRSCMRMACCMN_DEFAULT_NN_BF_NUM_T2P_AXIS_VALS     0x01
#define HDRSCMRMACCMN_DEFAULT_NN_BF_NUM_FRAB_AXIS_VALS    0x02
#define HDRSCMRMACCMN_DEFAULT_NN_BF_T2P_AXIS_VALS_0       0x00
#define HDRSCMRMACCMN_DEFAULT_NN_BF_T2P_AXIS_VALS_1       0x48
#define HDRSCMRMACCMN_DEFAULT_NN_BF_FRAB_AXIS_VALS_0      0x08
#define HDRSCMRMACCMN_DEFAULT_NN_BF_FRAB_AXIS_VALS_1      0x0B
#define HDRSCMRMACCMN_DEFAULT_NN_BF_FRAB_AXIS_VALS_2      0x07
#define HDRSCMRMACCMN_DEFAULT_NN_BUCKET_FACTOR_0_0        0x28
#define HDRSCMRMACCMN_DEFAULT_NN_BUCKET_FACTOR_0_1        0x10
#define HDRSCMRMACCMN_DEFAULT_NN_BUCKET_FACTOR_0_2        0x10
#define HDRSCMRMACCMN_DEFAULT_NN_BUCKET_FACTOR_1_Y        0x08

/* T2PInflowRange00 or T2PInflowRangeConfig00 */
#define HDRSCMRMACCMN_DEFAULT_00_T2P_INFLOW_MIN           0x10
#define HDRSCMRMACCMN_DEFAULT_00_T2P_INFLOW_MAX           0x2B
/* T2PInflowRangeNN or T2PInflowRangeConfigXX */
#define HDRSCMRMACCMN_DEFAULT_XX_T2P_INFLOW_MIN           0x07
#define HDRSCMRMACCMN_DEFAULT_XX_T2P_INFLOW_MAX           0x78

/* T2PTransitionFunction00 or T2PTransitionFunctionConfig00 */
#define HDRSCMRMACCMN_DEFAULT_00_GUGD_NUM_T2P_VALS        0x01
#define HDRSCMRMACCMN_DEFAULT_00_GUGD_NUM_FRAB_VALS       0x00
#define HDRSCMRMACCMN_DEFAULT_00_T2P_AXIS_0               0x10
#define HDRSCMRMACCMN_DEFAULT_00_T2P_AXIS_1               0x2B
#define HDRSCMRMACCMN_DEFAULT_00_FRAB_AXIS_0              0x00
#define HDRSCMRMACCMN_DEFAULT_UP_0_0                      0xF4
#define HDRSCMRMACCMN_DEFAULT_UP_1_0                      0xBC
#define HDRSCMRMACCMN_DEFAULT_DOWN_X_Y                    0xD8

#define HDRSCMRMACCMN_DEFAULT_XX_GUGD_NUM_T2P_VALS        0x03
#define HDRSCMRMACCMN_DEFAULT_XX_GUGD_NUM_FRAB_VALS       0x03
#define HDRSCMRMACCMN_DEFAULT_XX_T2P_AXIS_0               0x00
#define HDRSCMRMACCMN_DEFAULT_XX_T2P_AXIS_1               0x22
#define HDRSCMRMACCMN_DEFAULT_XX_T2P_AXIS_2               0x47
#define HDRSCMRMACCMN_DEFAULT_XX_T2P_AXIS_3               0x5A
#define HDRSCMRMACCMN_DEFAULT_XX_FRAB_AXIS_0              0x08
#define HDRSCMRMACCMN_DEFAULT_XX_FRAB_AXIS_1              0x0B
#define HDRSCMRMACCMN_DEFAULT_XX_FRAB_AXIS_2              0x0E
#define HDRSCMRMACCMN_DEFAULT_XX_FRAB_AXIS_3              0x07
#define HDRSCMRMACCMN_DEFAULT_XX_UP_0_0                   0x1D
#define HDRSCMRMACCMN_DEFAULT_XX_UP_0_1                   0x0A
#define HDRSCMRMACCMN_DEFAULT_XX_UP_0_2                   0xFE
#define HDRSCMRMACCMN_DEFAULT_XX_UP_0_3                   0xFE
#define HDRSCMRMACCMN_DEFAULT_XX_UP_1_0                   0xFB
#define HDRSCMRMACCMN_DEFAULT_XX_UP_1_1                   0xE8
#define HDRSCMRMACCMN_DEFAULT_XX_UP_1_2                   0xDC
#define HDRSCMRMACCMN_DEFAULT_XX_UP_1_3                   0xDC
#define HDRSCMRMACCMN_DEFAULT_XX_UP_2_0                   0xFB
#define HDRSCMRMACCMN_DEFAULT_XX_UP_2_1                   0xE8
#define HDRSCMRMACCMN_DEFAULT_XX_UP_2_2                   0xDC
#define HDRSCMRMACCMN_DEFAULT_XX_UP_2_3                   0xDC
#define HDRSCMRMACCMN_DEFAULT_XX_UP_3_0                   0x18
#define HDRSCMRMACCMN_DEFAULT_XX_UP_3_1                   0x05
#define HDRSCMRMACCMN_DEFAULT_XX_UP_3_2                   0xF9
#define HDRSCMRMACCMN_DEFAULT_XX_UP_3_3                   0xF9
#define HDRSCMRMACCMN_DEFAULT_XX_DOWN_0_0                 0xE9
#define HDRSCMRMACCMN_DEFAULT_XX_DOWN_0_1                 0xE2
#define HDRSCMRMACCMN_DEFAULT_XX_DOWN_0_2                 0xD6
#define HDRSCMRMACCMN_DEFAULT_XX_DOWN_0_3                 0xD6
#define HDRSCMRMACCMN_DEFAULT_XX_DOWN_1_0                 0xE9
#define HDRSCMRMACCMN_DEFAULT_XX_DOWN_1_1                 0xE2
#define HDRSCMRMACCMN_DEFAULT_XX_DOWN_1_2                 0xD6
#define HDRSCMRMACCMN_DEFAULT_XX_DOWN_1_3                 0xD6
#define HDRSCMRMACCMN_DEFAULT_XX_DOWN_2_0                 0x0E
#define HDRSCMRMACCMN_DEFAULT_XX_DOWN_2_1                 0x07
#define HDRSCMRMACCMN_DEFAULT_XX_DOWN_2_2                 0xFB
#define HDRSCMRMACCMN_DEFAULT_XX_DOWN_2_3                 0xFB
#define HDRSCMRMACCMN_DEFAULT_XX_DOWN_3_0                 0x2D
#define HDRSCMRMACCMN_DEFAULT_XX_DOWN_3_1                 0x26
#define HDRSCMRMACCMN_DEFAULT_XX_DOWN_3_2                 0x1A
#define HDRSCMRMACCMN_DEFAULT_XX_DOWN_3_3                 0x1A

 /* EJECT */
/*===========================================================================

FUNCTION HDRSCMRMACCMN_PROCESS_AUX_PILOT_CHAN_PARAMS

DESCRIPTION
  This is a generic interface to process the AuxiliaryPilotChannelParameters
  Attribute.

    0x0000 - AuxiliaryPilotChannelParameters Attribute 

DEPENDENCIES
  None.

PARAMETERS
  item_ptr      - pointer to the DSM item holding the message to be parsed.
  p_aip         - proposed attribute information pointer.
  a_aip         - accepted attribute information pointer.
  aux_pilot_ptr - pointer to location to store the unpacked attribute.

RETURN VALUE
  E_SUCCESS       - If a valid attribute was parsed successfully.
  E_NO_DATA       - Record parsing error.
  E_OUT_OF_RANGE  - Invalid data value.

SIDE EFFECTS
  None.

===========================================================================*/
hdrerrno_enum_type hdrscmrmaccmn_process_aux_pilot_chan_params
(
  dsm_item_type                               *item_ptr,
  hdrscm_proposed_attrib_type   const *const   p_aip,
  hdrscm_accepted_attrib_type         *const   a_aip,
  hdrscmrmaccmn_aux_pilot_struct_type         *aux_pilot_ptr
);


 /* EJECT */
/*===========================================================================

FUNCTION HDRSCMRMACCMN_PROCESS_COMMON_POWER_PARAMS

DESCRIPTION
  This is a generic interface to process the CommonPowerParameters 
  Attribute.

  0x0001 - CommonPowerParameters Attribute 

DEPENDENCIES
  None.

PARAMETERS
  item_ptr              - pointer to the DSM item holding the message to be 
                          parsed.
  p_aip                 - proposed attribute information pointer.
  a_aip                 - accepted attribute information pointer.
  common_pow_params_ptr - pointer to location to store the unpacked attribute.

RETURN VALUE
  E_SUCCESS       - If a valid attribute was parsed successfully.
  E_NO_DATA       - Record parsing error.
  E_OUT_OF_RANGE  - Invalid data value.

SIDE EFFECTS
  None.

===========================================================================*/
hdrerrno_enum_type hdrscmrmaccmn_process_common_power_params
(
  dsm_item_type                               *item_ptr,
  hdrscm_proposed_attrib_type   const *const   p_aip,
  hdrscm_accepted_attrib_type         *const   a_aip,
  hdrscmrmaccmn_common_pow_params_struct_type *common_pow_params_ptr
);


 /* EJECT */
/*===========================================================================

FUNCTION HDRSCMRMACCMN_PROCESS_PERMITTED_PAYLOAD

DESCRIPTION
  This is a generic interface to process the PermittedPayload 
  Attribute.

  0x0003 - PermittedPayload Attribute

DEPENDENCIES
  None.

PARAMETERS
  item_ptr          - pointer to the DSM item holding the message to be parsed.
  p_aip             - proposed attribute information pointer.
  a_aip             - accepted attribute information pointer.
  permitted_payload - pointer to location to store the unpacked attribute.

RETURN VALUE
  E_SUCCESS       - If a valid attribute was parsed successfully.
  E_NO_DATA       - Record parsing error.
  E_OUT_OF_RANGE  - Invalid data value.

SIDE EFFECTS
  None.

===========================================================================*/
hdrerrno_enum_type hdrscmrmaccmn_process_permitted_payload
(
  dsm_item_type                               *item_ptr,
  hdrscm_proposed_attrib_type   const *const   p_aip,
  hdrscm_accepted_attrib_type         *const   a_aip,
  uint8                                        permitted_payload[]
                                                [HDRSCMRMACCMN_MAX_NUM_SUBFRAME_HISTORY]
);


 /* EJECT */
/*===========================================================================

FUNCTION HDRSCMRMACCMN_PROCESS_POWER_PARAMS

DESCRIPTION
  This is a generic interface to process the PowerParameters Attributes.

  0x0005 - PowerParameters128 Attribute
  0x0006 - PowerParameters256 Attribute 
  0x0007 - PowerParameters512 Attribute
  0x0008 - PowerParameters768 Attribute
  0x0009 - PowerParameters1024 Attribute
  0x000A - PowerParameters1536 Attribute
  0x000B - PowerParameters2048 Attribute
  0x000C - PowerParameters3072 Attribute
  0x000D - PowerParameters4096 Attribute
  0x000E - PowerParameters6144 Attribute
  0x000F - PowerParameters8192 Attribute
  0x0010 - PowerParameters12288 Attribute

DEPENDENCIES
  None.

PARAMETERS
  item_ptr          - pointer to the DSM item holding the message to be parsed.
  p_aip             - proposed attribute information pointer.
  a_aip             - accepted attribute information pointer.
  power_params_ptr  - pointer to location to store the unpacked attribute.

RETURN VALUE
  E_SUCCESS       - If a valid attribute was parsed successfully.
  E_NO_DATA       - Record parsing error.
  E_OUT_OF_RANGE  - Invalid data value.

SIDE EFFECTS
  None.

===========================================================================*/
hdrerrno_enum_type hdrscmrmaccmn_process_power_params
(
  dsm_item_type                               *item_ptr,
  hdrscm_proposed_attrib_type   const *const   p_aip,
  hdrscm_accepted_attrib_type         *const   a_aip,
  hdrscmrmaccmn_power_params_struct_type      *power_params_ptr
);


 /* EJECT */
/*===========================================================================

FUNCTION HDRSCMRMACCMN_PROCESS_REQUEST_PARAMS

DESCRIPTION
  This is a generic interface to process the RequestParameters Attribute.

  0x0011 - RequestParameters Attribute

DEPENDENCIES
  None.

PARAMETERS
  item_ptr        - pointer to the DSM item holding the message to be parsed.
  p_aip           - proposed attribute information pointer.
  a_aip           - accepted attribute information pointer.
  req_params_ptr  - pointer to location to store the unpacked attribute.

RETURN VALUE
  E_SUCCESS       - If a valid attribute was parsed successfully.
  E_NO_DATA       - Record parsing error.
  E_OUT_OF_RANGE  - Invalid data value.

SIDE EFFECTS
  None.

===========================================================================*/
hdrerrno_enum_type hdrscmrmaccmn_process_request_params
(
  dsm_item_type                               *item_ptr,
  hdrscm_proposed_attrib_type   const *const   p_aip,
  hdrscm_accepted_attrib_type         *const   a_aip,
  hdrscmrmaccmn_req_params_struct_type        *req_params_ptr
);


 /* EJECT */
/*===========================================================================

FUNCTION HDRSCMRMACCMN_PROCESS_RRI_CHAN_POWER_PARAMS

DESCRIPTION
  This is a generic interface to process the RRIChannelPowerParameters 
  Attribute.

  0x0012 - RRIChannelPowerParameters Attribute

DEPENDENCIES
  None.

PARAMETERS
  item_ptr              - pointer to the DSM item holding the message to be 
                          parsed.
  p_aip                 - proposed attribute information pointer.
  a_aip                 - accepted attribute information pointer.
  rri_power_params_ptr  - pointer to location to store the unpacked attribute.

RETURN VALUE
  E_SUCCESS       - If a valid attribute was parsed successfully.
  E_NO_DATA       - Record parsing error.
  E_OUT_OF_RANGE  - Invalid data value.

SIDE EFFECTS
  None.

===========================================================================*/
hdrerrno_enum_type hdrscmrmaccmn_process_rri_chan_power_params
(
  dsm_item_type                               *item_ptr,
  hdrscm_proposed_attrib_type   const *const   p_aip,
  hdrscm_accepted_attrib_type         *const   a_aip,
  hdrscmrmaccmn_rri_power_params_struct_type  *rri_power_params_ptr
);


 /* EJECT */
/*===========================================================================

FUNCTION HDRSCMRMACCMN_PROCESS_ASSOCIATED_FLOWS_NN

DESCRIPTION
  This is a generic interface to process the AssociatedFlowsNN Attribute.

  0x0100 - 0x01NN - AssociatedFlowsNN Attributes

DEPENDENCIES
  None.

PARAMETERS
  item_ptr              - pointer to the DSM item holding the message to be 
                          parsed.
  p_aip                 - proposed attribute information pointer.
  a_aip                 - accepted attribute information pointer.
  associated_flows_ptr  - pointer to location to store the unpacked attribute.

RETURN VALUE
  E_SUCCESS       - If a valid attribute was parsed successfully.
  E_NO_DATA       - Record parsing error.
  E_OUT_OF_RANGE  - Invalid data value.

SIDE EFFECTS
  None.

===========================================================================*/
hdrerrno_enum_type hdrscmrmaccmn_process_associated_flows_nn
(
  dsm_item_type                               *item_ptr,
  hdrscm_proposed_attrib_type   const *const   p_aip,
  hdrscm_accepted_attrib_type         *const   a_aip,
  hdrscmrmaccmn_associated_flows_struct_type  *associated_flows_ptr
);


 /* EJECT */
/*===========================================================================

FUNCTION HDRSCMRMACCMN_PROCESS_BUCKET_FACTOR_NN

DESCRIPTION
  This is a generic interface to process the BucketFactorNN Attribute.

  0x0300 - 0x03NN - BucketFactorNN Attributes

DEPENDENCIES
  None.

PARAMETERS
  item_ptr          - pointer to the DSM item holding the message to be parsed.
  p_aip             - proposed attribute information pointer.
  a_aip             - accepted attribute information pointer.
  bucket_factor_ptr - pointer to location to store the unpacked attribute.

RETURN VALUE
  E_SUCCESS       - If a valid attribute was parsed successfully.
  E_NO_DATA       - Record parsing error.
  E_OUT_OF_RANGE  - Invalid data value.

SIDE EFFECTS
  None.

===========================================================================*/
hdrerrno_enum_type hdrscmrmaccmn_process_bucket_factor_nn
(
  dsm_item_type                               *item_ptr,
  hdrscm_proposed_attrib_type   const *const   p_aip,
  hdrscm_accepted_attrib_type         *const   a_aip,
  hdrscmrmaccmn_t2p_frab_grid_struct_type     *bucket_factor_ptr
);


 /* EJECT */
/*===========================================================================

FUNCTION HDRSCMRMACCMN_PROCESS_T2P_INFLOW_RANGE_NN__CONFIG_XX

DESCRIPTION
  This is a generic interface to process the T2PInflowRangeNN or 
  T2PInflowRangeConfigXX Attributes.

  Subtype 3
    0x15NN  - T2PInflowRangeNN

  Subtype 4
    0x15XX  - T2PInflowRangeConfigXX

DEPENDENCIES
  None.

PARAMETERS
  item_ptr              - pointer to the DSM item holding the message to be 
                          parsed.
  p_aip                 - proposed attribute information pointer.
  a_aip                 - accepted attribute information pointer.
  t2p_inflow_range_ptr  - pointer to location to store the unpacked attribute.

RETURN VALUE
  E_SUCCESS       - If a valid attribute was parsed successfully.
  E_NO_DATA       - Record parsing error.
  E_OUT_OF_RANGE  - Invalid data value.

SIDE EFFECTS
  None.

===========================================================================*/
hdrerrno_enum_type hdrscmrmaccmn_process_t2p_inflow_range_nn__config_xx
(
  dsm_item_type                               *item_ptr,
  hdrscm_proposed_attrib_type   const *const   p_aip,
  hdrscm_accepted_attrib_type         *const   a_aip,
  hdrscmrmaccmn_t2p_inflow_range_struct_type  *t2p_inflow_range_ptr
);


 /* EJECT */
/*===========================================================================

FUNCTION HDRSCMRMACCMN_PROCESS_T2P_TRANS_FUNC_NN__CONFIG_XX

DESCRIPTION
  This is a generic interface to process the T2PTransitionFunctionNN or
  T2PTransitionFunctionConfigXX Attributes.

  Subtype 3
    0x16NN  - T2PTransitionFunctionNN

  Subtype 4
    0x16XX  - T2PTransitionFunctionConfigXX

DEPENDENCIES
  None.

PARAMETERS
  item_ptr  - pointer to the DSM item holding the message to be parsed.
  p_aip     - proposed attribute information pointer.
  a_aip     - accepted attribute information pointer.
  gu_gd_ptr - pointer to location to store the unpacked attribute.

RETURN VALUE
  E_SUCCESS       - If a valid attribute was parsed successfully.
  E_NO_DATA       - Record parsing error.
  E_OUT_OF_RANGE  - Invalid data value.

SIDE EFFECTS
  None.

===========================================================================*/
hdrerrno_enum_type hdrscmrmaccmn_process_t2p_trans_func_nn__config_xx
(
  dsm_item_type                               *item_ptr,
  hdrscm_proposed_attrib_type   const *const   p_aip,
  hdrscm_accepted_attrib_type         *const   a_aip,
  hdrscmrmaccmn_gu_gd_struct_type             *gu_gd_ptr
);


 /* EJECT */
/*===========================================================================

FUNCTION HDRSCMRMACCMN_PROCESS_MAX_NUM_SUB_PKTS__CC

DESCRIPTION
  This is a generic interface to process the MaxNumSubPackets or
  MaxNumSubPacketsCC Attributes.

  Subtype 3
    0x0002  - MaxNumSubpackets

  Subtype 4
    0x1ACC  - MaxNumSubPacketsCC

DEPENDENCIES
  None.

PARAMETERS
  item_ptr              - pointer to the DSM item holding the message to be 
                          parsed.
  p_aip                 - proposed attribute information pointer.
  a_aip                 - accepted attribute information pointer.
  max_num_sub_pkts_ptr  - pointer to location to store the unpacked attribute.

RETURN VALUE
  E_SUCCESS       - If a valid attribute was parsed successfully.
  E_NO_DATA       - Record parsing error.
  E_OUT_OF_RANGE  - Invalid data value.

SIDE EFFECTS
  None.

===========================================================================*/
hdrerrno_enum_type hdrscmrmaccmn_process_max_num_sub_pkts__cc
(
  dsm_item_type                               *item_ptr,
  hdrscm_proposed_attrib_type   const *const   p_aip,
  hdrscm_accepted_attrib_type         *const   a_aip,
  hdrscmrmaccmn_max_num_sub_pkts_struct_type  *max_num_sub_pkts_ptr
);


 /* EJECT */
/*===========================================================================

FUNCTION HDRSCMRMACCMN_PROCESS_PILOT_STRENGTH__CONFIG_XX

DESCRIPTION
  This is a generic interface to process the PilotStrength or
  PilotStrengthConfigXX Attributes.

  Subtype 3
    0x0004  - PilotStrength

  Subtype 4
    0x1BXX  - PilotStrengthConfigXX

DEPENDENCIES
  None.

PARAMETERS
  item_ptr            - pointer to the DSM item holding the message to be 
                        parsed.
  p_aip               - proposed attribute information pointer.
  a_aip               - accepted attribute information pointer.
  pilot_strength_ptr  - pointer to location to store the unpacked attribute.

RETURN VALUE
  E_SUCCESS       - If a valid attribute was parsed successfully.
  E_NO_DATA       - Record parsing error.
  E_OUT_OF_RANGE  - Invalid data value.

SIDE EFFECTS
  None.

===========================================================================*/
hdrerrno_enum_type hdrscmrmaccmn_process_pilot_strength__config_xx
(
  dsm_item_type                                 *item_ptr,
  hdrscm_proposed_attrib_type   const *const     p_aip,
  hdrscm_accepted_attrib_type         *const     a_aip,
  hdrscmrmaccmn_pilot_strength_grid_struct_type *pilot_strength_ptr
);


 /* EJECT */
/*===========================================================================

FUNCTION HDRSCMRMACCMN_PROCESS_TX_T2P_MAX__CONFIG_XX

DESCRIPTION
  This is a generic interface to process the TxT2PMax or TxT2PMaxConfigXX
  Attributes.

  Subtype 3
    0x0013  - TxT2PMax

  Subtype 4
    0x1CXX  - TxT2PMaxConfigXX

DEPENDENCIES
  None.

PARAMETERS
  item_ptr      - pointer to the DSM item holding the message to be parsed.
  p_aip         - proposed attribute information pointer.
  a_aip         - accepted attribute information pointer.
  txt2p_max_ptr - pointer to location to store the unpacked attribute.

RETURN VALUE
  E_SUCCESS       - If a valid attribute was parsed successfully.
  E_NO_DATA       - Record parsing error.
  E_OUT_OF_RANGE  - Invalid data value.

SIDE EFFECTS
  None.

===========================================================================*/
hdrerrno_enum_type hdrscmrmaccmn_process_tx_t2p_max__config_xx
(
  dsm_item_type                               *item_ptr,
  hdrscm_proposed_attrib_type   const *const   p_aip,
  hdrscm_accepted_attrib_type         *const   a_aip,
  hdrscmrmaccmn_txt2p_max_grid_struct_type    *txt2p_max_ptr
);

/*===========================================================================

FUNCTION HDRSCMRMACCMN_AXIS_HAS_DUP_POINTS                     LOCAL

DESCRIPTION
  The RMAC ST-4 grid attributes cannot have two points in the axes with
  same values. This routine is used to check if the axes has duplicate points.

DEPENDENCIES
  None.

PARAMETERS
  axis - Array of uint8s containing the axis points
  num_points - Number of points in the axis.

RETURN VALUE
  TRUE  - If the axis has duplicate points
  FALSE - otherwise. 

SIDE EFFECTS
  None.

===========================================================================*/
boolean hdrscmrmaccmn_axis_has_dup_points
(
  uint8 axis[],
  uint8 num_points
);

#endif /* HDRSCMRMACCMN_H */
