#ifndef HDRSCMRMAC4_H
#define HDRSCMRMAC4_H
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


  Copyright (c) 2007 - 2009 by Qualcomm Technologies, Inc.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                           EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/hdr/cp/api/hdrscmrmac4.h#1 $ $DateTime: 2016/12/13 07:58:05 $ $Author: mplcsds1 $
   
when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/29/13   smd     Disable RevB code if FEATURE_HDR_REVB_SUPPORTED is not defined.
11/18/09   rmg     Changed attrib id of MaxRLPilotTxPwrDiff to 0xFFF4.
11/01/09   etv     Fixed Klocwork errors.
09/10/09   etv     Added support for negotiation & GAUP of MaxRLPilotTxPwrDiff
08/11/08   etv     Added support for accepting GAUP of RRIChannelPowerParams.
07/30/07   cc      Renamed HDRSCMRMAC4_MAX_NUM_REV_CHANNELS to
                   HDRSCMRMAC4_MAX_NUM_REV_LOG_CHANS.
03/30/07   cc      Created module.

===========================================================================*/

/*===========================================================================

                          INCLUDE FILES FOR MODULE
                     
===========================================================================*/

#include "dsm.h"
#include "comdef.h"
#include "err.h"

#ifdef FEATURE_HDR_REVB_SUPPORTED
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
#define HDRSCMRMAC4_MAX_NUM_REV_LOG_CHANS               16
#define HDRSCMRMAC4_MAX_NUM_CONFIGS                     16

#define HDRSCMRMAC4_TOTAL_NUM_SIMPLE_ATTRIBS            10
#define HDRSCMRMAC4_TOTAL_NUM_NN_SIMPLE_ATTRIBS         6
#define HDRSCMRMAC4_TOTAL_NUM_CC_SIMPLE_ATTRIBS         3
#define HDRSCMRMAC4_TOTAL_NUM_COMPLEX_ATTRIBS           18
#define HDRSCMRMAC4_TOTAL_NUM_NN_COMPLEX_ATTRIBS        3
#define HDRSCMRMAC4_TOTAL_NUM_CC_COMPLEX_ATTRIBS        2
#define HDRSCMRMAC4_TOTAL_NUM_XX_COMPLEX_ATTRIBS        4
#define HDRSCMRMAC4_TOTAL_NUM_NC_COMPLEX_ATTRIBS        1

#define HDRSCMRMAC4_NCX_BASE_MASK                       0xFF00
#define HDRSCMRMAC4_NCX_ID_MASK                         0x00FF
#define HDRSCMRMAC4_NCX_NN_ID_MASK                      0x00F0
#define HDRSCMRMAC4_NCX_CC_ID_MASK                      0x000F
#define HDRSCMRMAC4_NCX_NN_ID_SHIFT                     4

#define HDRSCMRMAC4_GET_CC_ATTRIB_CHAN_ID(attrib_id)    \
          ((attrib_id) & HDRSCMRMAC4_NCX_ID_MASK)

#define HDRSCMRMAC4_GET_XX_ATTRIB_CONFIG_ID(attrib_id)  \
          ((attrib_id) & HDRSCMRMAC4_NCX_ID_MASK)

#define HDRSCMRMAC4_GET_NC_ATTRIB_FLOW_ID(attrib_id)    \
          (((attrib_id) & HDRSCMRMAC4_NCX_NN_ID_MASK) >>\
           HDRSCMRMAC4_NCX_NN_ID_SHIFT)

#define HDRSCMRMAC4_GET_NC_ATTRIB_CHAN_ID(attrib_id)    \
          ((attrib_id) & HDRSCMRMAC4_NCX_CC_ID_MASK)

#define HDRSCMRMAC4_GET_NCX_ATTRIB_BASE(attrib_id)      \
          ((attrib_id) & HDRSCMRMAC4_NCX_BASE_MASK)

#define HDRSCMRMAC4_GET_NCX_ATTRIB_NCX_ID(attrib_id)    \
          ((attrib_id) & HDRSCMRMAC4_NCX_ID_MASK)

#define HDRSCMRMAC4_TOTAL_NUM_NCX_SIMPLE_ATTRIBS        \
  (HDRSCMRMAC4_TOTAL_NUM_NN_SIMPLE_ATTRIBS+ \
   HDRSCMRMAC4_TOTAL_NUM_CC_SIMPLE_ATTRIBS)
#define HDRSCMRMAC4_TOTAL_NUM_NCX_COMPLEX_ATTRIBS       \
  (HDRSCMRMAC4_TOTAL_NUM_NN_COMPLEX_ATTRIBS+ \
   HDRSCMRMAC4_TOTAL_NUM_CC_COMPLEX_ATTRIBS+ \
   HDRSCMRMAC4_TOTAL_NUM_XX_COMPLEX_ATTRIBS+ \
   HDRSCMRMAC4_TOTAL_NUM_NC_COMPLEX_ATTRIBS)

#define HDRSCMRMAC4_NUM_GAUPABLE_NON_NCX_ATTRIBS        19
#define HDRSCMRMAC4_NUM_GAUPABLE_NN_ATTRIBS             9
#define HDRSCMRMAC4_NUM_GAUPABLE_CC_ATTRIBS             4
#define HDRSCMRMAC4_NUM_GAUPABLE_XX_ATTRIBS             4
#define HDRSCMRMAC4_NUM_GAUPABLE_NC_ATTRIBS             1
#define HDRSCMRMAC4_NUM_GAUPABLE_NCX_ATTRIBS            \
          (HDRSCMRMAC4_NUM_GAUPABLE_NN_ATTRIBS +        \
           HDRSCMRMAC4_NUM_GAUPABLE_CC_ATTRIBS +        \
           HDRSCMRMAC4_NUM_GAUPABLE_XX_ATTRIBS +        \
           HDRSCMRMAC4_NUM_GAUPABLE_NC_ATTRIBS)

typedef enum
{
  HDRSCMRMAC4_FIRST_NON_NCX_SIMPLE_ATTRIB           = 0xFFF1,
  HDRSCMRMAC4_MAX_PACKET_SIZE_ATTRIB =
    HDRSCMRMAC4_FIRST_NON_NCX_SIMPLE_ATTRIB,
  HDRSCMRMAC4_SUPPORT_GAUP_MAX_NUM_SUB_PKTS_ATTRIB  = 0xFFF2,
  HDRSCMRMAC4_NON_NCX_SIMPLE_ATTRIB_RSVD1           = 0xFFF3,
  HDRSCMRMAC4_MAX_RL_PILOT_TX_PWR_DIFF_ATTRIB       = 0xFFF4,
  HDRSCMRMAC4_NON_NCX_SIMPLE_ATTRIB_RSVD2           = 0xFFF5,
  HDRSCMRMAC4_NON_NCX_SIMPLE_ATTRIB_RSVD3           = 0xFFF6,
  HDRSCMRMAC4_T2P_NO_TX_FILTER_TC_ATTRIB            = 0xFFF7,
  HDRSCMRMAC4_NON_NCX_SIMPLE_ATTRIB_RSVD4           = 0xFFF8,
  HDRSCMRMAC4_QRAB_FILTER_TC_ATTRIB                 = 0xFFF9,
  HDRSCMRMAC4_PS_FILTER_TC_ATTRIB                   = 0xFFFA,
  HDRSCMRMAC4_PAYLOAD_THRESH_ATTRIB                 = 0xFFFB,
  HDRSCMRMAC4_MERGE_THRESH_ATTRIB                   = 0xFFFC,
  HDRSCMRMAC4_FRAB_LOW_ATTRIB                       = 0xFFFD,
  HDRSCMRMAC4_FRAB_FILTER_TC_ATTRIB                 = 0xFFFE,
  HDRSCMRMAC4_LAST_NON_NCX_SIMPLE_ATTRIB            =
    HDRSCMRMAC4_FRAB_FILTER_TC_ATTRIB
} hdrscmrmac4_simple_attrib_id_enum_type;

/*-----------------------------------------------------------------------------
  This enum defines the attribute IDs of each of the RMAC ST-4 complex attribs.
------------------------------------------------------------------------------*/
typedef enum
{
  HDRSCMRMAC4_FIRST_NON_NCX_COMPLEX_ATTRIB  = 0x0000,
  HDRSCMRMAC4_AUX_PILOT_CHAN_PARAMS_ATTRIB  = 
    HDRSCMRMAC4_FIRST_NON_NCX_COMPLEX_ATTRIB,
  HDRSCMRMAC4_COMMON_PARAMS_ATTRIB          = 0x0001,
  HDRSCMRMAC4_NON_NCX_COMPLEX_ATTRIB_RSVD1  = 0x0002,
  HDRSCMRMAC4_PERMITTED_PAYLOAD_ATTRIB      = 0x0003,
  HDRSCMRMAC4_NON_NCX_COMPLEX_ATTRIB_RSVD2  = 0x0004,
  HDRSCMRMAC4_FIRST_POWER_PARAMS_ATTRIB     = 0x0005,
  HDRSCMRMAC4_POWER_PARAMS_128_ATTRIB       = 
    HDRSCMRMAC4_FIRST_POWER_PARAMS_ATTRIB,
  HDRSCMRMAC4_POWER_PARAMS_256_ATTRIB       = 0x0006,
  HDRSCMRMAC4_POWER_PARAMS_512_ATTRIB       = 0x0007,
  HDRSCMRMAC4_POWER_PARAMS_768_ATTRIB       = 0x0008,
  HDRSCMRMAC4_POWER_PARAMS_1024_ATTRIB      = 0x0009,
  HDRSCMRMAC4_POWER_PARAMS_1536_ATTRIB      = 0x000A,
  HDRSCMRMAC4_POWER_PARAMS_2048_ATTRIB      = 0x000B,
  HDRSCMRMAC4_POWER_PARAMS_3072_ATTRIB      = 0x000C,
  HDRSCMRMAC4_POWER_PARAMS_4096_ATTRIB      = 0x000D,
  HDRSCMRMAC4_POWER_PARAMS_6144_ATTRIB      = 0x000E,
  HDRSCMRMAC4_POWER_PARAMS_8192_ATTRIB      = 0x000F,
  HDRSCMRMAC4_POWER_PARAMS_12288_ATTRIB     = 0x0010,
  HDRSCMRMAC4_LAST_POWER_PARAMS_ATTRIB      = 
    HDRSCMRMAC4_POWER_PARAMS_12288_ATTRIB,
  HDRSCMRMAC4_REQUEST_PARAMS_ATTRIB         = 0x0011,
  HDRSCMRMAC4_RRI_CHAN_POWER_PARAMS_ATTRIB  = 0x0012,
  HDRSCMRMAC4_NON_NCX_COMPLEX_ATTRIB_RSVD3  = 0x0013,
  HDRSCMRMAC4_MAX_MAC_FLOWS_ATTRIB          = 0x0014,
  HDRSCMRMAC4_LAST_NON_NCX_COMPLEX_ATTRIB = HDRSCMRMAC4_MAX_MAC_FLOWS_ATTRIB

} hdrscmrmac4_complex_attrib_id_enum_type;

/*---------------------------------------------------------------------------
  Each NN (and CC, NC, XX) attribute falls under a few major classifications
  and the attrib_id "XYZ00" can be used to identify the XYZ group of attribs.
  This enum defines this base attrib Id for each class of NN attrib. Given an
  NN attrib_id, a particular NN attribute class can be found by MASKING off 
  the LSB using HDRSCMRMAC4_NN_BASE_MASK 
----------------------------------------------------------------------------*/
typedef enum
{
  /* Per-CC Simple Attribute */
  HDRSCMRMAC4_PS_QRAB_THRESH_DRC_UNLOCK_CC_BASE     = 0xF600,
  HDRSCMRMAC4_FIRST_SIMPLE_CC_BASE                  =
    HDRSCMRMAC4_PS_QRAB_THRESH_DRC_UNLOCK_CC_BASE,
  HDRSCMRMAC4_PS_QRAB_THRESH_DRC_LOCK_CC_BASE       = 0xF700,
  HDRSCMRMAC4_ARQ_MODE_CC_BASE                      = 0xF800,
  HDRSCMRMAC4_LAST_SIMPLE_CC_BASE                   =
    HDRSCMRMAC4_ARQ_MODE_CC_BASE,

  /* Per-NN Simple Attribute */
  HDRSCMRMAC4_T2P_FILTER_TC_NN_BASE                 = 0xF900,
  HDRSCMRMAC4_FIRST_SIMPLE_NN_BASE                  =
    HDRSCMRMAC4_T2P_FILTER_TC_NN_BASE,
  HDRSCMRMAC4_BURST_DURATION_FACT0R_NN_BASE         = 0xFA00,
  HDRSCMRMAC4_QRAB_SELECT_NN_BASE                   = 0xFB00,
  HDRSCMRMAC4_TRANSMISSION_MODE_NN_BASE             = 0xFC00,
  HDRSCMRMAC4_MERGE_THRESHOLD_NN_BASE               = 0xFD00,
  HDRSCMRMAC4_BUCKET_LEVEL_MAX_NN_BASE              = 0xFE00,
  HDRSCMRMAC4_LAST_SIMPLE_NN_BASE                   =
    HDRSCMRMAC4_BUCKET_LEVEL_MAX_NN_BASE,

  /* Complex Attributes */
  HDRSCMRMAC4_ASSOCIATED_FLOWS_NN_BASE              = 0x0100,
  HDRSCMRMAC4_BUCKET_FACTOR_NN_BASE                 = 0x0300,
  HDRSCMRMAC4_T2P_INFLOW_RANGE_CONFIG_XX_BASE       = 0x1500,
  HDRSCMRMAC4_T2P_TRANSITION_FUNC_CONFIG_XX_BASE    = 0x1600,
  HDRSCMRMAC4_ASSOCIATED_FLOW_CONFIG_NC_BASE        = 0x1700,
  HDRSCMRMAC4_ASSOCIATED_AT_CONFIG_CC_BASE          = 0x1800,
  HDRSCMRMAC4_DATA_TOKEN_BUCKET_NN_BASE             = 0x1900,
  HDRSCMRMAC4_MAX_NUM_SUB_PACKETS_CC_BASE           = 0x1A00,
  HDRSCMRMAC4_PS_CONFIG_XX_BASE                     = 0x1B00,
  HDRSCMRMAC4_TX_T2P_MAX_CONFIG_XX_BASE             = 0x1C00
} hdrscmrmac4_per_ncx_attrib_id_base_type;

/* AT supported/proposed MaxPacketSize Attribute */
#define HDRSCMRMAC4_MAX_PKT_SIZE_BYTE_LENGTH        3
#define HDRSCMRMAC4_MAX_PKT_SIZE_ATTRIB_VALUE       4 /* 12288 bits */

/* Default MaxRLPilotTxPwrDiff (18dB) */
#define HDRSCMRMAC4_DEFAULT_MAX_RL_PILOT_TX_PWR_DIFF    0x12

/*---------------------------------------------------------------------------
                       VERSION NUMBER TYPE DECLARATIONS
----------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------

     Non NN       Non NN     NNSimple     NNSimple NNComplex   NNComplex
     Simple       Complex    Attrib1      AttribN  Attrib1     AttribN
   ----------------------------------------------------------------------
  |            |           |         |  |         |         |  |         |
  |            |           | + + + + |..| + + + + | + + + + |..| + + + + |....
  |            |           |         |  |         |         |  |         |
   ----------------------------------------------------------------------

    CCSimple     CCSimple     CCComplex     CCComplex
    Attrib1      AttribN      Attrib1       AttribN
   ------------------------------------------------------
  |          |  |         |  |          |  |          |  |
  | ******** |..|******** |..| ******** |..| ******** |..|....
  |          |  |         |  |          |  |          |  |
   ------------------------------------------------------

    XXComplex    XXComplex
    Attrib1      AttribN
   ---------------------------
  |          |  |          |  |
  | ^^^^^^^^^|..|^^^^^^^^^ |..|....
  |          |  |          |  |
   ---------------------------

              NCComplex                             NCComplex               
              Attrib1                               AttribN
   ------------------------------------- -----------------------------------
  | N=0       |N=1      |  |N=y      |..| N=0       |N=1      |  |N=y      |
  | C=0~x     |C=0~x    |..|C=0~x    |..| C=0~x     |C=0~x    |..|C=0~x    |
  | %%%%%%%%%%|%%%%%%%%%|  |%%%%%%%%%|..| %%%%%%%%%%|%%%%%%%%%|  |%%%%%%%%%|
   ------------------------------------- -----------------------------------

  The version number type is defined as a flat uint8 array with one byte for
  version number of each RMAC4 attribute. The Non-NN attributes are arranged
  in the beginning followed by NN attributes, CC attributes, XX attributes,
  and NC attributs.

  The NN attributes are arranged in block of bytes for each attribute with
  HDRSCMRMACCMN_MAX_NUM_MAC_FLOWS bytes in each block with one byte for version
  number of each flow specific attribute.

  The CC attribute are arranged in block of bytes for each attribute with
  HDRSCMRMAC4_MAX_NUM_REV_LOG_CHANS bytes in each block with one byte for
  version number for each reverse logical channel specific attribute.

  The XX attribute are arranged in block of bytes for each attribute with
  HDRSCMRMAC4_MAX_NUM_CONFIGS bytes in each block with one byte for
  version number for each configuration specific attribute.

  The NC attribute are arranged in block of bytes for each attribute with
  (HDRSCMRMACCMN_MAX_NUM_MAC_FLOWS * HDRSCMRMAC4_MAX_NUM_REV_LOG_CHANS) bytes
  in each block.  Those bytes are divided in HDRSCMRMACCMN_MAX_NUM_MAC_FLOWS
  sub-blocks with each sub-blocks containing HDRSCMRMAC4_MAX_NUM_REV_LOG_CHANS
  bytes with one byte for version number for each per-flow-per-channel specific
  attribute.

  The layout is demonstrated by fig. above.
      
----------------------------------------------------------------------------*/

#define HDRSCMRMAC4_MAX_NUM_RMAC4_ATTRIBS                       \
          ( HDRSCMRMAC4_TOTAL_NUM_SIMPLE_ATTRIBS +              \
            HDRSCMRMAC4_TOTAL_NUM_COMPLEX_ATTRIBS +             \
            ( HDRSCMRMACCMN_MAX_NUM_MAC_FLOWS *                 \
              ( HDRSCMRMAC4_TOTAL_NUM_NN_SIMPLE_ATTRIBS +       \
                HDRSCMRMAC4_TOTAL_NUM_NN_COMPLEX_ATTRIBS ) ) +  \
            ( HDRSCMRMAC4_MAX_NUM_REV_LOG_CHANS *               \
              ( HDRSCMRMAC4_TOTAL_NUM_CC_SIMPLE_ATTRIBS +       \
                HDRSCMRMAC4_TOTAL_NUM_CC_COMPLEX_ATTRIBS ) ) +  \
            ( HDRSCMRMAC4_MAX_NUM_CONFIGS *                     \
              HDRSCMRMAC4_TOTAL_NUM_XX_COMPLEX_ATTRIBS ) +      \
            ( HDRSCMRMACCMN_MAX_NUM_MAC_FLOWS *                 \
              ( HDRSCMRMAC4_MAX_NUM_REV_LOG_CHANS *             \
                HDRSCMRMAC4_TOTAL_NUM_NC_COMPLEX_ATTRIBS ) ) )
typedef uint8 
  hdrscmrmac4_verno_type[HDRSCMRMAC4_MAX_NUM_RMAC4_ATTRIBS];

/*---------------------------------------------------------------------------
          Constants used to compute the attrib verno index
----------------------------------------------------------------------------*/
#define HDRSCMRMAC4_NON_NN_SIMPLE_ATTRIB_INDEX_BASE     0
#define HDRSCMRMAC4_NON_NN_COMPLEX_ATTRIB_INDEX_BASE()  \
  (HDRSCMRMAC4_NON_NN_SIMPLE_ATTRIB_INDEX_BASE + \
   HDRSCMRMAC4_TOTAL_NUM_SIMPLE_ATTRIBS)
#define HDRSCMRMAC4_NN_SIMPLE_ATTRIB_INDEX_BASE()       \
  (HDRSCMRMAC4_NON_NN_COMPLEX_ATTRIB_INDEX_BASE() + \
   HDRSCMRMAC4_TOTAL_NUM_COMPLEX_ATTRIBS)
#define HDRSCMRMAC4_NN_COMPLEX_ATTRIB_INDEX_BASE()      \
  (HDRSCMRMAC4_NN_SIMPLE_ATTRIB_INDEX_BASE() + \
   (HDRSCMRMACCMN_MAX_NUM_MAC_FLOWS * \
    HDRSCMRMAC4_TOTAL_NUM_NN_SIMPLE_ATTRIBS))
#define HDRSCMRMAC4_CC_SIMPLE_ATTRIB_INDEX_BASE()       \
  (HDRSCMRMAC4_NN_COMPLEX_ATTRIB_INDEX_BASE() + \
   (HDRSCMRMACCMN_MAX_NUM_MAC_FLOWS * \
    HDRSCMRMAC4_TOTAL_NUM_NN_COMPLEX_ATTRIBS))
#define HDRSCMRMAC4_CC_COMPLEX_ATTRIB_INDEX_BASE()      \
  (HDRSCMRMAC4_CC_SIMPLE_ATTRIB_INDEX_BASE() + \
   (HDRSCMRMAC4_MAX_NUM_REV_LOG_CHANS *                 \
    HDRSCMRMAC4_TOTAL_NUM_CC_SIMPLE_ATTRIBS))
#define HDRSCMRMAC4_XX_COMPLEX_ATTRIB_INDEX_BASE()      \
  (HDRSCMRMAC4_CC_COMPLEX_ATTRIB_INDEX_BASE() + \
   (HDRSCMRMAC4_MAX_NUM_REV_LOG_CHANS *                 \
    HDRSCMRMAC4_TOTAL_NUM_CC_COMPLEX_ATTRIBS))
#define HDRSCMRMAC4_NC_COMPLEX_ATTRIB_INDEX_BASE()      \
  (HDRSCMRMAC4_XX_COMPLEX_ATTRIB_INDEX_BASE() + \
   (HDRSCMRMAC4_MAX_NUM_CONFIGS * \
    HDRSCMRMAC4_TOTAL_NUM_XX_COMPLEX_ATTRIBS))

#define HDRSCMRMAC4_DATA_TOKEN_INFLOW_INFINITE      0xFF
#define HDRSCMRMAC4_DATA_BUCKET_LEVEL_MAX_INFINITE  0xFF
/* DataTokenBucket */
typedef struct
{
  uint8                           data_token_inflow;
  uint8                           data_bucket_level_max;
} hdrscmrmac4_data_token_bucket_struct_type;

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

  hdrscmrmaccmn_associated_flows_struct_type      flow_mapping;
    /* AssociatedFlowsNN.
       AttribId: 0x0100~0x01NN; */

  hdrscmrmaccmn_t2p_frab_grid_struct_type         bucket_factor;
    /* Standard allows for up to a 255x15 grid. 8 bit values in [1..32.875] 
       represented in 1/8 dB units, so actual value is (OTA value * 1/8) + 1 
       AttribId: 0x0300~0x03NN; */

  hdrscmrmac4_data_token_bucket_struct_type     data_token_bucket;
    /* DataTokenBucketNN.
       AttribID: 0x1900~0x19NN; */

} hdrscmrmac4_flow_config_struct_type;


/*---------------------------------------------------------------------------
             PER-REVERSE LOGICAL CHANNEL ATTRIBUTE TYPE DECLARATION
----------------------------------------------------------------------------*/

/* AssociatedATConfigurations */
typedef struct
{
  uint8   ps_select;
  uint8   tx_t2p_max_select;
} hdrscmrmac4_associated_at_config_struct_type;

/* Per-Reverse Logical Channel Configuration Attributes.
   Note: The order to attributes in this struct has important implication in
   the way the attribute offset for simple attributes are computed. It is 
   important to maintain this order.
   TODO: Add the changes required if order needs to change or adding a new
   attribute. */
typedef struct
{

  uint8                                         ps_qrab_thresh_drc_unlock;
    /* Minimum PilotStrength required for QRAB from non-serving sector to be
       included in QRABps computation when DRC is not in lock.
       AttribId: 0xF600~0xF6CC; Units: -0.25dB; Range: (0x00~0x3F); Default: 0x0C */

  uint8                                         ps_qrab_thresh_drc_lock;
    /* Minimum PilotStrength required for QRAB from non-serving sector to be
       included in QRABps computation when DRC is in lock. 
       AttribId: 0xF700~0xF7CC; Units: -0.25dB; Range: (0x00~0x3F); Default: 0x18 */

  uint8                                         arq_mode;
    /* Reverse link ARQ mode (Bipolar or On/Off keying) 
       AttribId: 0xF800~0xF8CC; Units: Enum; Range (0x00, 0x01); Default: 0x00 */

  hdrscmrmac4_associated_at_config_struct_type  associated_at_config;
    /* AssociatedATConfigurations
       AttribId: 0x1800~0x18CC */

  hdrscmrmaccmn_max_num_sub_pkts_struct_type      max_num_sub_pkts;
    /* Maximum number of subpackets for each of the interlaces
       AttribId: 0x1A00~0x1ACC */

} hdrscmrmac4_reverse_channel_config_struct_type;


/*---------------------------------------------------------------------------
             PER-CONFIGURATION ATTRIBUTE TYPE DECLARATION
----------------------------------------------------------------------------*/

/* Per-Configuration Configuration Attributes.
   Note: The order to attributes in this struct has important implication in
   the way the attribute offset for simple attributes are computed. It is 
   important to maintain this order.
   TODO: Add the changes required if order needs to change or adding a new
   attribute. */
typedef struct
{

  hdrscmrmaccmn_t2p_inflow_range_struct_type      t2p_inflow_range;
    /* T2PInflow min and max for this flow: 8 bit values in 1/8 dB units.
       AttribId: 0x1500~0x15XX; */

  hdrscmrmaccmn_gu_gd_struct_type                 gu_gd;
    /* Used to determine T2PInflow up/down increments. They are aka Gu/Gd 
       functions. Values are in 8 bit 2's complement form in 1/8 dB units
       AttribId: 0x1600~0x16XX; */

  hdrscmrmaccmn_pilot_strength_grid_struct_type pilot_strength;
    /* Scaling factor in 4 bit 2's complement format used to scale T2PInflow
       if filtered PilotStrength equals corresponding PilotStrength axis point.
       AttribId: 0x1B00~1BXX */

  hdrscmrmaccmn_txt2p_max_grid_struct_type    txt2p_max;
    /* Grid that defines the maximum TxT2P AT is allowed to transmit if 
       filtered serving sector PilotStrength equals corresponding 
       PilotStrengthAxis value. AttribOd: 0x1C00~0x1CXX */

} hdrscmrmac4_configuration_config_struct_type;


/*---------------------------------------------------------------------------
       PER-FLOW/PER-REVERSE LOGICAL CHANNEL ATTRIBUTE TYPE DECLARATION
----------------------------------------------------------------------------*/
typedef struct
{
  uint8   t2p_transition_func_select;
    /* 4-bits value selecting one of the T2PTransitionFunctionConfig's */

  uint8   t2p_inflow_range_select;
    /* 4-bits value selecting one of the T2PInflowRangeConfig's */
} hdrscmrmac4_associated_flow_config_struct_type;


/*---------------------------------------------------------------------------
                       NON-PER-FLOW ATTRIBUTE TYPE DECLARATION
----------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------
                    ST-4 RMAC Configuration Attributes 
----------------------------------------------------------------------------*/
typedef struct
{

  uint8 max_packet_size;
    /* The largest packet size (in bits) AT can transmit in any sub-frame in
       any CDMA Channel.
       AttribId: 0xFFF1; Units: Enum; Range: (0x00~0x04); Default: 0x00 */

  uint8 gaup_max_num_sub_pkts;
    /* Is modification of MaxNumSubPackets attribute GAUPable? 
       AttribId: 0xFFF2; Units: Enum; Range: (0x00, 0x01); Default: 0x00 */

  uint8 max_rl_pilot_tx_pwr_diff;
    /* MaxRLPilotTxPowerDiff in units of 1dB 
       AttribId: 0xFFF4; Units: 1dB; Range: (0x00, 0x08-0x1F); Default: 0x12 */

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

  hdrscmrmac4_flow_config_struct_type flows[HDRSCMRMACCMN_MAX_NUM_MAC_FLOWS];
    /* 5 flows supported for now */

  hdrscmrmac4_reverse_channel_config_struct_type
      reverse_channels[HDRSCMRMAC4_MAX_NUM_REV_LOG_CHANS];
    /* 16 Reverse Logical Channels supported */

  hdrscmrmac4_configuration_config_struct_type
      configurations[HDRSCMRMAC4_MAX_NUM_CONFIGS];
    /* 16 Configurations supported */

  hdrscmrmac4_associated_flow_config_struct_type
      associated_flow_config[HDRSCMRMACCMN_MAX_NUM_MAC_FLOWS]
                            [HDRSCMRMAC4_MAX_NUM_REV_LOG_CHANS];
    /* (8 * 16) possible (flow,channel) combinations supported for now */

  hdrscmrmaccmn_aux_pilot_struct_type aux_pilot_info;
    /* Minimum payload requiring transmission of Aux Pilot and its gain 
       relative to Traffic Channel. AttribId: 0x0000; */

  hdrscmrmaccmn_common_pow_params_struct_type common_pow_params;
    /* Common Power Parameters Attributes. AttribId: 0x0001 */

  hdrscmrmaccmn_permitted_payload_type permitted_payload;
    /* Maximum Packet size the the receiver can transmit in the subframe y if
       x bits were transmitted in subframe (n-1) is given by 
       permitted_payload(x,y). AttribId: 0x0003 */ 

  hdrscmrmaccmn_power_params_struct_type      
    power_params [HDRSCMRMACCMN_MAX_NUM_PAYLOADS] [HDRSCMRMACCMN_NUM_TX_MODES];
  /* Power Parameters Attribute.
     AttribId: 0x0005 ~ 0x0010. 12 attributes one for each payload size  */

  hdrscmrmaccmn_req_params_struct_type        req_params;
    /* ReqRatio and MaxReqInterval. AttribId: 0x0011 */

  hdrscmrmaccmn_rri_power_params_struct_type  rri_chan_power_params
                                           [HDRSCMRMACCMN_NUM_T2P_TRANS_POINTS];
    /* RRI channels gains corresponding to the various combinations of pre and 
     * post T2P transition points. AttribId: 0x0012 */

  hdrscmrmaccmn_max_mac_flows_struct_type     max_mac_flows;
    /* Maximum totol number of MAC flows supported. AttribId: 0x0014 */

} hdrscmrmac4_config_struct_type;

typedef struct
{
  hdrscmrmaccmn_aux_pilot_struct_type aux_pilot;
  hdrscmrmaccmn_common_pow_params_struct_type common_power_params;
  hdrscmrmaccmn_permitted_payload_type permitted_payload;
  hdrscmrmaccmn_power_params_struct_type 
    power_params[HDRSCMRMACCMN_MAX_NUM_PAYLOADS]
                [HDRSCMRMACCMN_NUM_TX_MODES];
  hdrscmrmaccmn_req_params_struct_type req_params;
  hdrscmrmaccmn_rri_power_params_struct_type 
    rri_chan_power_params[HDRSCMRMACCMN_NUM_T2P_TRANS_POINTS];
  hdrscmrmaccmn_max_mac_flows_struct_type max_mac_flows;

  /* Per-Flow attributes */
  hdrscmrmaccmn_t2p_frab_grid_struct_type 
    bucket_factor[HDRSCMRMACCMN_MAX_NUM_MAC_FLOWS];
  hdrscmrmac4_data_token_bucket_struct_type 
    data_token_bucket[HDRSCMRMACCMN_MAX_NUM_MAC_FLOWS];
  hdrscmrmaccmn_associated_flows_struct_type 
    associated_flows[HDRSCMRMACCMN_MAX_NUM_MAC_FLOWS];

  /* Per Reverse Channel Attribute */
  hdrscmrmaccmn_max_num_sub_pkts_struct_type  
    max_num_sub_pkts[HDRSCMRMAC4_MAX_NUM_REV_LOG_CHANS];
  hdrscmrmac4_associated_at_config_struct_type 
    associated_at_config[HDRSCMRMAC4_MAX_NUM_REV_LOG_CHANS];

  /* Per-Config Attribute */
  hdrscmrmaccmn_txt2p_max_grid_struct_type  
    txt2p_max[HDRSCMRMAC4_MAX_NUM_CONFIGS];
  hdrscmrmaccmn_pilot_strength_grid_struct_type 
    pilot_strength[HDRSCMRMAC4_MAX_NUM_CONFIGS];
  hdrscmrmaccmn_gu_gd_struct_type 
    gu_gd[HDRSCMRMAC4_MAX_NUM_CONFIGS];
  hdrscmrmaccmn_t2p_inflow_range_struct_type  
    t2p_inflow_range[HDRSCMRMAC4_MAX_NUM_CONFIGS];

  /* Per-Flow, Per-Channel Attribute */
  hdrscmrmac4_associated_flow_config_struct_type
    associated_flow_config[HDRSCMRMACCMN_MAX_NUM_MAC_FLOWS]
                          [HDRSCMRMAC4_MAX_NUM_REV_LOG_CHANS];

  uint8 trans_id;
    /* Transaction ID used for AT-init Session negotiation */

  uint8 max_mac_flow_value_id;

} hdrscmrmac4_complex_config_struct_type;

 /* EJECT */
/*===========================================================================

FUNCTION HDRSCMRMAC4_PROCESS_COMPLEX_ATTRIB

DESCRIPTION
  This is a generic interface to process all complex attributes given below.

  1.  0x0000 - AuxillaryPilotChannelParameters Attribute 
  2.  0x0001 - CommonPowerParameters Attribute 
  3.  0x0003 - PermittedPayload Attribute
  4.  0x0005 - PowerParameters128 Attribute
  5.  0x0006 - PowerParameters256 Attribute 
  6.  0x0007 - PowerParameters512 Attribute 
  7.  0x0008 - PowerParameters768 Attribute 
  8.  0x0009 - PowerParameters1024 Attribute 
  9.  0x000A - PowerParameters1536 Attribute 
  10. 0x000B - PowerParameters2048 Attribute 
  11. 0x000C - PowerParameters3072 Attribute 
  12. 0x000D - PowerParameters4096 Attribute 
  13. 0x000E - PowerParameters6144 Attribute 
  14. 0x000F - PowerParameters8192 Attribute 
  15. 0x0010 - PowerParameters12288 Attribute 
  16. 0x0011 - RequestParameters Attribute
  17. 0x0012 - RRIChannelPowerParameters Attribute
  18. 0x0014 - MaxMACFlows Attribute 

  Per-NCX Attributes:
  ===================
  1.  0x0100 - 0x01NN - AssociatedFlowsNN Attributes
  2.  0x0300 - 0x03NN - BucketFactorNN Attributes
  3.  0x1500 - 0x15XX - T2PInflowRangeConfigXX Attributes
  4.  0x1600 - 0x16XX - T2PTransitionFunctionConfigXX Attributes
  5.  0x1700 - 0x17NC - AssociatedFlowConfigurationsNC Attributes
  6.  0x1800 - 0x18CC - AssociatedATConfigurationsCC Attributes
  7.  0x1900 - 0x19NN - DataTokenBucketNN Attributes
  8.  0x1A00 - 0x1ACC - MaxNumSubPacketsCC Attributes
  9.  0x1B00 - 0x1BXX - PilotStrengthConfigXX
  10. 0x1C00 - 0x1CXX - TxT2PmaxConfigXX

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
extern hdrerrno_enum_type hdrscmrmac4_process_complex_attrib (
  dsm_item_type                               *item_ptr,
  hdrscm_proposed_attrib_type   const *const   p_aip,
  hdrscm_accepted_attrib_type         *const   a_aip
);


 /* EJECT */
/*===========================================================================

FUNCTION HDRSCMRMAC4_IS_ATTRIB_SIMPLE

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
extern boolean hdrscmrmac4_is_attrib_simple (
  uint16 const attrib_id
);


 /* EJECT */
/*===========================================================================

FUNCTION HDRSCMRMAC4_IS_SIMPLE_ATTRIB_VALID

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
extern hdrerrno_enum_type hdrscmrmac4_is_simple_attrib_valid (
  uint16 const attrib_id,
  uint32 const attrib_value 
);


 /* EJECT */
/*===========================================================================

FUNCTION HDRSCMRMAC4_GET_ATTRIB_OFFSET_AND_SIZE

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
extern hdrerrno_enum_type hdrscmrmac4_get_attrib_offset_and_size (
  uint16 attrib_id, 
  uint16 *attrib_offset,
  uint16 *attrib_size
);


/* EJECT */
/*===========================================================================

FUNCTION HDRSCMRMAC4_GET_ATTRIB_VERNO_INDEX

DESCRIPTION
  Returns the index of the given attribute in the version number array.

DEPENDENCIES
  None

PARAMETERS
  attrib_id     - Attribute Id.
  index         - Address where the index is stored.
  
RETURN VALUE
  E_SUCCESS - If the given attribute ID is valid.
  E_OUT_OF_RANGE - If the given attribute ID is invalid.
  

SIDE EFFECTS
  None

===========================================================================*/
extern hdrerrno_enum_type hdrscmrmac4_get_attrib_verno_index (
  uint16 attrib_id,
  uint16 *index
);


/* EJECT */
/*===========================================================================

FUNCTION HDRSCMRMAC4_SET_CONFIG_TO_DEFAULT

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
extern void hdrscmrmac4_set_config_to_default (
  hdrscmrmac4_config_struct_type *p_config
);


/* EJECT */
/*===========================================================================

FUNCTION HDRSCMRMAC4_PROP_MAX_MAC_FLOWS

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
boolean hdrscmrmac4_prop_max_mac_flows( void );


/* EJECT */
/*===========================================================================

FUNCTION HDRSCMRMAC4_GET_PROP_ATTRIB_VALUE

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
hdrerrno_enum_type hdrscmrmac4_get_prop_attrib_value (
  uint16 const trans_id,
  hdrscm_accepted_attrib_type *a_aip
);


/* <EJECT> */
/*===========================================================================

FUNCTION HDRSCMRMAC4_FAIL_CURR_CONFIG_NEG

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
void hdrscmrmac4_fail_curr_config_neg (
  hdrhai_session_close_reason_enum_type close_reason
);


 /* EJECT */
/*===========================================================================

FUNCTION HDRSCMRMAC4_IS_ATTRIB_GAUPABLE

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
hdrerrno_enum_type hdrscmrmac4_is_attrib_gaupable (
  uint16 const attrib_id
);


/* EJECT */
/*===========================================================================

FUNCTION HDRSCMRMAC4_ATTRIB_IS_SUPPORTED

DESCRIPTION
  Is this a known attribute?

DEPENDENCIES
  None

PARAMETERS 
  attrib_id: Attribute ID in question.

RETURN VALUE
  TRUE - If the attribute is supported.
  FALSE - Otherwise

SIDE EFFECTS
  None

===========================================================================*/
boolean hdrscmrmac4_attrib_is_supported
(
  uint16 const attrib_id
);


 /* EJECT */
/*===========================================================================
FUNCTION HDRSCMRMAC4_CONFIG_IS_NEEDED                               EXTERNAL

DESCRIPTION
  This function is used to find out if an AT-initiated Configuration is 
  needed by RMAC4. 

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
extern boolean hdrscmrmac4_config_is_needed( void );


 /* EJECT */
/*===========================================================================
FUNCTION HDRSCMRMAC4_CONFIGURE                                   EXTERNAL

DESCRIPTION
  This function does the AT-init configuration of RMAC4 attributes. 

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
extern boolean hdrscmrmac4_configure( void );

#endif /* FEATURE_HDR_REVB_SUPPORTED */
#endif /* HDRSCMRMAC4_H */
