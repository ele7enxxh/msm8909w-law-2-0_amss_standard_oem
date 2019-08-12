/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

      H D R   S E S S I O N   C O N F I G U R A T I O N   M A N A G E R

          S U B T Y P E - 4  R E V E R S E  C H A N N E L  M A C  

                              P R O T O C O L

       
GENERAL DESCRIPTION
  This module provides supporting API for SCM module to process
  ConfigurationRequest messages and AttributeUpdateRequest messages directed
  to Subtype-4 Reverse Traffic channel MAC protocol. 
    
EXTERNALIZED FUNCTIONS
  hdrscmrmac4_process_complex_attrib 
    Processes all RMAC ST-4 complex attributes 
    
  hdrscmrmac4_is_attrib_simple 
    Returns true if an attribute is simple

  hdrscmrmac4_is_simple_attrib_valid
    Verifies the validity of the simple attribute

  hdrscmrmac4_get_attrib_offset_and_size
    Returns attribute size and offset into the config structure.

  hdrscmrmac4_get_attrib_verno_index
    Returns the index of an attribute in the version number array. 
   
INITIALIZATION AND SEQUENCING REQUIREMENTS

  
  Copyright (c) 2007 - 2010 by Qualcomm Technologies, Inc.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                           EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/hdr/cp/src/hdrscmrmac4.c#1 $ $DateTime: 2016/12/13 07:58:05 $ $Author: mplcsds1 $
   
when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/30/13   ukl     Fix errors and KW errors in functions get index from attrib_id
09/19/13   ukl     Reduce Code Duplication HDR-CP
04/29/13   smd     Disable RevB code if FEATURE_HDR_REVB_SUPPORTED is not defined.
12/16/10   rmg     Fixed a compiler warning.
09/16/10   cnx     Fixed compiler warnings. 
06/21/10   cnx     Fixed compiler warnings.
11/18/09   rmg     Changed attrib id of MaxRLPilotTxPwrDiff to 0xFFF4.
09/28/09   etv     Fixed Klocwork errors.
09/10/09   etv     Added support for negotiation & GAUP of MaxRLPilotTxPwrDiff
12/04/08   etv     Lint fixes.
08/11/08   etv     Added support for accepting GAUP of RRIChannelPowerParams.
05/22/07   etv     Fixed bug with range checking for per-cc attribute macro.
10/06/07   cc      Initialized default values for AssociatedFlowConfigurationsNC
                   attributes for N != 1.
07/30/07   cc      Renamed HDRSCMRMAC4_MAX_NUM_REV_CHANNELS to
                   HDRSCMRMAC4_MAX_NUM_REV_LOG_CHANS.
03/30/07   cc      Created module.

===========================================================================*/

/*===========================================================================

                          INCLUDE FILES FOR MODULE
                     
===========================================================================*/

#ifdef FEATURE_HDR_REVB_SUPPORTED

#include "dsm.h"
#include "comdef.h"
#include "err.h"

#include "hdrdebug.h"
#include "hdrscp.h"
#include "hdrhai.h"
#include "hdrtrace.h"
#include "hdrbit.h"
#include "hdrutil.h"
#include "hdrhmp.h"

#include "hdrscmrmaccmn.h"
#include "hdrscmrmac4.h"
#include "hdrscmrmaci.h"
#include "hdrscm.h"
#include "hdrscmdb.h"

/*===========================================================================

             DEFINITIONS  FROM  THE  HDR  AIR  INTERFACE  SPEC

===========================================================================*/

/*---------------------------------------------------------------------------
          Sizes of the complex attributes and its fields in bits 
----------------------------------------------------------------------------*/

/* AssociatedFlowConfigurationsNC */
#define HDRSCMRMAC4_AFC_NC_SIZE                         8
#define HDRSCMRMAC4_AFC_T2P_TRANS_FUNC_SELECT_SIZE      4
#define HDRSCMRMAC4_AFC_T2P_INFLOW_RANGE_SELECT_SIZE    4

/* AssociatedATConfigurationsCC */
#define HDRSCMRMAC4_AAC_CC_SIZE                         8
#define HDRSCMRMAC4_AAC_PS_SELECT_SIZE                  4
#define HDRSCMRMAC4_AAC_TX_T2P_MAX_SELECT_SIZE          4

/* DataTokenBucketNN */
#define HDRSCMRMAC4_DTB_NN_SIZE                         16
#define HDRSCMRMAC4_DTB_DATA_TOKEN_INFLOW_SIZE          8
#define HDRSCMRMAC4_DTB_DATA_BUCKET_LEVEL_MAX_SIZE      8

/*---------------------------------------------------------------------------
     Valid Range definitions for each of the complex attribute fields
----------------------------------------------------------------------------*/

/* DataTokenInflo */
#define HDRSCMRMAC4_MIN_DATA_TOKEN_INFLOW_RESERVED      0x80
#define HDRSCMRMAC4_MAX_DATA_TOKEN_INFLOW_RESERVED      0xFE

/*---------------------------------------------------------------------------
                Default Values for Configuration Attributes
----------------------------------------------------------------------------*/
/* DataTokenBucketNN */
#define HDRSCMRMAC4_DEFAULT_NN_DATA_TOKEN_BUCKET_DATA_TOKEN_INFLOW      0xFF
#define HDRSCMRMAC4_DEFAULT_NN_DATA_TOKEN_BUCKET_DATA_BUCKET_LEVEL_MAX  0xFF

/* AssociatedFlowConfigurationsNC */
#define HDRSCMRMAC4_DEFAULT_NN_T2P_INFLOW_RANGE_SEL                     1
#define HDRSCMRMAC4_DEFAULT_NN_T2P_TRANSITION_FUNC_SEL                  1

 /* EJECT */
/*===========================================================================
 
                  DEFINITIONS AND DECLARATIONS FOR MODULE

  This section contains local definitions for constants, macros, types,
  variables and other items needed by this module.

===========================================================================*/

#define HDRSCMRMAC4_SIMPLE_ATTRIB_MSB_MASK              0xF000
#define HDRSCMRMAC4_SIMPLE_ATTRIB_MSB                   0xF000

#define HDRSCMRMAC4_NCX_ATTRIB_3RD_NIBBLE_MASK          0x0F00
#define HDRSCMRMAC4_NON_NCX_SIMPLE_ATTRIB_MASKED        0x0F00
#define HDRSCMRMAC4_NON_NCX_COMPLEX_ATTRIB_MASKED       0x0000

#define HDRSCMRMAC4_NN_SIMPLE_ATTRIB_INDEX_LSB          8
#define HDRSCMRMAC4_NCX_BASE_MASK                       0xFF00
#define HDRSCMRMAC4_NCX_ID_MASK                         0x00FF
#define HDRSCMRMAC4_NCX_NN_ID_MASK                      0x00F0
#define HDRSCMRMAC4_NCX_CC_ID_MASK                      0x000F
#define HDRSCMRMAC4_NCX_NN_ID_SHIFT                     4

#define HDRSCMRMAC4_IS_SIMPLE_ATTRIB_NON_NCX(attrib_id) \
          (((attrib_id) &                               \
            HDRSCMRMAC4_NCX_ATTRIB_3RD_NIBBLE_MASK) ==  \
           HDRSCMRMAC4_NON_NCX_SIMPLE_ATTRIB_MASKED)

#define HDRSCMRMAC4_IS_COMPLEX_ATTRIB_NON_NCX(attrib_id)\
          (((attrib_id) &                               \
            HDRSCMRMAC4_NCX_ATTRIB_3RD_NIBBLE_MASK) ==  \
           HDRSCMRMAC4_NON_NCX_COMPLEX_ATTRIB_MASKED)

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

#define HDRSCMRMAC4_NON_NCX_SIMPLE_ATTRIB_VERNO_INDEX(num_attribs_ahead)      \
          (HDRSCMRMAC4_NON_NN_SIMPLE_ATTRIB_INDEX_BASE + (num_attribs_ahead))

#define HDRSCMRMAC4_NON_NCX_COMPLEX_ATTRIB_VERNO_INDEX(num_attribs_ahead)     \
          (HDRSCMRMAC4_NON_NN_COMPLEX_ATTRIB_INDEX_BASE() + (num_attribs_ahead))

#define HDRSCMRMAC4_NN_SIMPLE_ATTRIB_VERNO_INDEX(num_attribs_ahead, flow_id)  \
          (HDRSCMRMAC4_NN_SIMPLE_ATTRIB_INDEX_BASE() +                          \
           (num_attribs_ahead) * HDRSCMRMACCMN_MAX_NUM_MAC_FLOWS + (flow_id))

#define HDRSCMRMAC4_NN_COMPLEX_ATTRIB_VERNO_INDEX(num_attribs_ahead, flow_id) \
          (HDRSCMRMAC4_NN_COMPLEX_ATTRIB_INDEX_BASE() +                         \
           (num_attribs_ahead) * HDRSCMRMACCMN_MAX_NUM_MAC_FLOWS + (flow_id))

#define HDRSCMRMAC4_CC_SIMPLE_ATTRIB_VERNO_INDEX(num_attribs_ahead, chan_id)  \
          (HDRSCMRMAC4_CC_SIMPLE_ATTRIB_INDEX_BASE() +                          \
           ((num_attribs_ahead) * HDRSCMRMAC4_MAX_NUM_REV_LOG_CHANS) + (chan_id))

#define HDRSCMRMAC4_CC_COMPLEX_ATTRIB_VERNO_INDEX(num_attribs_ahead, chan_id) \
          (HDRSCMRMAC4_CC_COMPLEX_ATTRIB_INDEX_BASE() +                         \
           ((num_attribs_ahead) * HDRSCMRMAC4_MAX_NUM_REV_LOG_CHANS) + (chan_id))

#define HDRSCMRMAC4_XX_COMPLEX_ATTRIB_VERNO_INDEX(num_attribs_ahead, config_ind)  \
          (HDRSCMRMAC4_XX_COMPLEX_ATTRIB_INDEX_BASE() +                             \
           ((num_attribs_ahead) * HDRSCMRMAC4_MAX_NUM_CONFIGS) + (config_id))

#define HDRSCMRMAC4_NC_COMPLEX_ATTRIB_VERNO_INDEX(flow_id, channel_id)  \
          (HDRSCMRMAC4_NC_COMPLEX_ATTRIB_INDEX_BASE() +                 \
           ((flow_id) * HDRSCMRMAC4_MAX_NUM_REV_LOG_CHANS) + (channel_id))

#define HDRSCMRMAC4_IS_ATTRIB_NON_NCX_SIMPLE(attrib_id)                 \
          (((attrib_id >= (uint16) HDRSCMRMAC4_FIRST_NON_NCX_SIMPLE_ATTRIB) && \
            (attrib_id <= (uint16) HDRSCMRMAC4_LAST_NON_NCX_SIMPLE_ATTRIB)) && \
           (attrib_id != (uint16) HDRSCMRMAC4_NON_NCX_SIMPLE_ATTRIB_RSVD1) &&  \
           (attrib_id != (uint16) HDRSCMRMAC4_NON_NCX_SIMPLE_ATTRIB_RSVD2) &&  \
           (attrib_id != (uint16) HDRSCMRMAC4_NON_NCX_SIMPLE_ATTRIB_RSVD3) &&  \
           (attrib_id != (uint16) HDRSCMRMAC4_NON_NCX_SIMPLE_ATTRIB_RSVD4))

/* commented out to avoid 
   #186-D: pointless comparison of unsigned integer with zero */
#define HDRSCMRMAC4_IS_ATTRIB_NON_NCX_COMPLEX(attrib_id)                \
          ((/* (attrib_id >= HDRSCMRMAC4_FIRST_NON_NCX_COMPLEX_ATTRIB) && */ \
           (attrib_id <= (uint16) HDRSCMRMAC4_LAST_NON_NCX_COMPLEX_ATTRIB)) && \
           (attrib_id != (uint16) HDRSCMRMAC4_NON_NCX_COMPLEX_ATTRIB_RSVD1) && \
           (attrib_id != (uint16) HDRSCMRMAC4_NON_NCX_COMPLEX_ATTRIB_RSVD2) && \
           (attrib_id != (uint16) HDRSCMRMAC4_NON_NCX_COMPLEX_ATTRIB_RSVD3))

#define HDRSCMRMAC4_IS_ATTRIB_NN_SIMPLE(attrib_id)                      \
            ((HDRSCMRMAC4_GET_NCX_ATTRIB_BASE(attrib_id) >=             \
              (uint16) HDRSCMRMAC4_FIRST_SIMPLE_NN_BASE)            &&  \
             (HDRSCMRMAC4_GET_NCX_ATTRIB_BASE(attrib_id) <=             \
              (uint16) HDRSCMRMAC4_LAST_SIMPLE_NN_BASE))

#define HDRSCMRMAC4_IS_ATTRIB_NN_COMPLEX(attrib_id)                     \
            ((HDRSCMRMAC4_GET_NCX_ATTRIB_BASE(attrib_id) ==             \
              (uint16) HDRSCMRMAC4_ASSOCIATED_FLOWS_NN_BASE)         || \
             (HDRSCMRMAC4_GET_NCX_ATTRIB_BASE(attrib_id) ==             \
              (uint16) HDRSCMRMAC4_BUCKET_FACTOR_NN_BASE)            || \
             (HDRSCMRMAC4_GET_NCX_ATTRIB_BASE(attrib_id) ==             \
              (uint16) HDRSCMRMAC4_DATA_TOKEN_BUCKET_NN_BASE))

#define HDRSCMRMAC4_IS_ATTRIB_CC_SIMPLE(attrib_id)                      \
            ((HDRSCMRMAC4_GET_NCX_ATTRIB_BASE(attrib_id) >=             \
              (uint16) HDRSCMRMAC4_FIRST_SIMPLE_CC_BASE)             && \
             (HDRSCMRMAC4_GET_NCX_ATTRIB_BASE(attrib_id) <=             \
              (uint16) HDRSCMRMAC4_LAST_SIMPLE_CC_BASE))

#define HDRSCMRMAC4_IS_ATTRIB_CC_COMPLEX(attrib_id)                     \
            ((HDRSCMRMAC4_GET_NCX_ATTRIB_BASE(attrib_id) ==             \
              (uint16) HDRSCMRMAC4_ASSOCIATED_AT_CONFIG_CC_BASE)     || \
             (HDRSCMRMAC4_GET_NCX_ATTRIB_BASE(attrib_id) ==             \
              (uint16) HDRSCMRMAC4_MAX_NUM_SUB_PACKETS_CC_BASE))

#define HDRSCMRMAC4_IS_ATTRIB_XX_COMPLEX(attrib_id)                         \
             ((HDRSCMRMAC4_GET_NCX_ATTRIB_BASE(attrib_id) ==                \
               (uint16) HDRSCMRMAC4_T2P_INFLOW_RANGE_CONFIG_XX_BASE)     || \
              (HDRSCMRMAC4_GET_NCX_ATTRIB_BASE(attrib_id) ==                \
               (uint16) HDRSCMRMAC4_T2P_TRANSITION_FUNC_CONFIG_XX_BASE)  || \
              (HDRSCMRMAC4_GET_NCX_ATTRIB_BASE(attrib_id) ==                \
               (uint16) HDRSCMRMAC4_PS_CONFIG_XX_BASE)                   || \
              (HDRSCMRMAC4_GET_NCX_ATTRIB_BASE(attrib_id) ==                \
               (uint16) HDRSCMRMAC4_TX_T2P_MAX_CONFIG_XX_BASE))

/* The type hdrscmrmac4_simple_non_ncx_attrib_offset_lut_type defines the
 * structure to store the LUT information required to determine the offset
 * of a given simple non-NN-CC-XX-NC attribute. */
typedef struct
{
  uint16  attrib_offset;      /* Offset from the beginning of the data 
                                 structure */
} hdrscmrmac4_simple_non_ncx_attrib_offset_lut_type;

/* The type hdrscmrmac4_simple_ncx_attrib_offset_lut_type defines the
 * structure to store the LUT information required to determine the offset
 * of a given simple per-NN-CC-XX-NC attribute. */
typedef struct
{
  uint16  attrib_offset;      /* Offset from the beginning of the data 
                                 structure */
  uint16  element_size;       /* The size of the per-NN-CC-XX-NC
                                 structure */
} hdrscmrmac4_simple_ncx_attrib_offset_lut_type;

/* The type hdrscmrmac4_complex_non_ncx_attrib_offset_size_lut_type
 * defines the structure to store the LUT information required to
 * determine the offset and size of a given non-NN-CC-XX-NC
 * complex attrib. */
typedef struct
{
  uint16  attrib_offset;      /* Offset from the beginning of the data 
                                 structure */
  uint16  attrib_size;        /* Attrib Size */
} hdrscmrmac4_complex_non_ncx_attrib_offset_size_lut_type;

/* The type hdrscmrmac4_complex_ncx_attrib_offset_size_lut_type
 * defines the structure to store the LUT information required to
 * determine the offset and size of a given per-NN-CC-XX-NC
 * complex attrib. */
typedef struct
{
  uint16  attrib_offset;      /* Offset from the beginning of the data 
                                 structure */
  uint16  attrib_size;        /* Attrib Size */
  uint16  element_size;       /* The size of the per-NN-CC-XX-NC
                                 structure */
} hdrscmrmac4_complex_ncx_attrib_offset_size_lut_type;

typedef enum
{
 SIMPLE_ATTRIB,
 COMPLX_ATTRIB
}hdrrmac4_attrib_type_enum_type;
/* enum for getiitng indecx for attributes */


/*------------------------------------------------------------------------
                    FUNCTION FORWARD DECLARATIONS
 -----------------------------------------------------------------------*/

/* AuxiliaryPilotChannelParameters Attribute */
static hdrerrno_enum_type hdrscmrmac4_process_aux_pilot_chan_params
(
  dsm_item_type                               *item_ptr,
  hdrscm_proposed_attrib_type   const *const   p_aip,
  hdrscm_accepted_attrib_type         *const   a_aip
);

/* CommonPowerParameters Attribute */
static hdrerrno_enum_type hdrscmrmac4_process_common_power_params
(
  dsm_item_type                               *item_ptr,
  hdrscm_proposed_attrib_type   const *const   p_aip,
  hdrscm_accepted_attrib_type         *const   a_aip
);

/* PermittedPayload Attribute */
static hdrerrno_enum_type hdrscmrmac4_process_permitted_payload
(
  dsm_item_type                               *item_ptr,
  hdrscm_proposed_attrib_type   const *const   p_aip,
  hdrscm_accepted_attrib_type         *const   a_aip
);

/* PowerParams Attribute */
static hdrerrno_enum_type hdrscmrmac4_process_power_params
(
  dsm_item_type                               *item_ptr,
  hdrscm_proposed_attrib_type   const *const   p_aip,
  hdrscm_accepted_attrib_type         *const   a_aip
);

/* RequestParams Attribute */
static hdrerrno_enum_type hdrscmrmac4_process_request_params
(
  dsm_item_type                               *item_ptr,
  hdrscm_proposed_attrib_type   const *const   p_aip,
  hdrscm_accepted_attrib_type         *const   a_aip
);

/* RRIChannelPowerParameters Attribute */
static hdrerrno_enum_type hdrscmrmac4_process_rri_chan_power_params
(
  dsm_item_type                               *item_ptr,
  hdrscm_proposed_attrib_type   const *const   p_aip,
  hdrscm_accepted_attrib_type         *const   a_aip
);

/* AssociatedFlowsNN Attribute */
static hdrerrno_enum_type hdrscmrmac4_process_associated_flows_nn
(
  dsm_item_type                               *item_ptr,
  hdrscm_proposed_attrib_type   const *const   p_aip,
  hdrscm_accepted_attrib_type         *const   a_aip
);

/* BucketFactorNN Attribute */
static hdrerrno_enum_type hdrscmrmac4_process_bucket_factor_nn
(
  dsm_item_type                               *item_ptr,
  hdrscm_proposed_attrib_type   const *const   p_aip,
  hdrscm_accepted_attrib_type         *const   a_aip
);

/* T2PInflowRangeConfigXX Attribute */
static hdrerrno_enum_type hdrscmrmac4_process_t2p_inflow_range_config_xx
(
  dsm_item_type                               *item_ptr,
  hdrscm_proposed_attrib_type   const *const   p_aip,
  hdrscm_accepted_attrib_type         *const   a_aip
);

/* T2PTransitionFunctionConfigXX Attribute */
static hdrerrno_enum_type hdrscmrmac4_process_t2p_trans_func_config_xx
(
  dsm_item_type                               *item_ptr,
  hdrscm_proposed_attrib_type   const *const   p_aip,
  hdrscm_accepted_attrib_type         *const   a_aip
);

/* AssociatedFlowConfigurationsNC Attribute */
static hdrerrno_enum_type hdrscmrmac4_process_associated_flow_config_nc
(
  dsm_item_type                               *item_ptr,
  hdrscm_proposed_attrib_type   const *const   p_aip,
  hdrscm_accepted_attrib_type         *const   a_aip
);

/* AssociatedATConfigurationsCC Attribute */
static hdrerrno_enum_type hdrscmrmac4_process_associated_at_config_cc
(
  dsm_item_type                               *item_ptr,
  hdrscm_proposed_attrib_type   const *const   p_aip,
  hdrscm_accepted_attrib_type         *const   a_aip
);

/* DataTokenBucketNN Attribute */
static hdrerrno_enum_type hdrscmrmac4_process_data_token_bucket_nn
(
  dsm_item_type                               *item_ptr,
  hdrscm_proposed_attrib_type   const *const   p_aip,
  hdrscm_accepted_attrib_type         *const   a_aip
);

/* MaxNumSubPacketsCC Attribute */
static hdrerrno_enum_type hdrscmrmac4_process_max_num_sub_pkts_cc
(
  dsm_item_type                               *item_ptr,
  hdrscm_proposed_attrib_type   const *const   p_aip,
  hdrscm_accepted_attrib_type         *const   a_aip
);

/* PilotStrengthConfigXX Attribute */
static hdrerrno_enum_type hdrscmrmac4_process_pilot_strength_config_xx
(
  dsm_item_type                               *item_ptr,
  hdrscm_proposed_attrib_type   const *const   p_aip,
  hdrscm_accepted_attrib_type         *const   a_aip
);

/* TxT2PmaxConfigXX Attribute */
static hdrerrno_enum_type hdrscmrmac4_process_tx_t2p_max_config_xx
(
  dsm_item_type                               *item_ptr,
  hdrscm_proposed_attrib_type   const *const   p_aip,
  hdrscm_accepted_attrib_type         *const   a_aip
);

/*------------------------------------------------------------------------
                            GLOBAL VARIABLES
 -----------------------------------------------------------------------*/

/*------------------------------------------------------------------------
                              LOOKUP TABLES
  All table entries are arranged in ascending attid_id order.  Make sure
  that the same ordering is maintained across all tables.
 -----------------------------------------------------------------------*/

/* This table is used to look up the index to be used to access non-NCX
   simple attrib information */
static hdrscmrmac4_simple_attrib_id_enum_type const
  hdrscmrmac4_simple_non_ncx_index_lut[HDRSCMRMAC4_TOTAL_NUM_SIMPLE_ATTRIBS] =
{
  HDRSCMRMAC4_MAX_PACKET_SIZE_ATTRIB,               /* 0xFFF1 */
  HDRSCMRMAC4_SUPPORT_GAUP_MAX_NUM_SUB_PKTS_ATTRIB, /* 0xFFF2 */
  HDRSCMRMAC4_MAX_RL_PILOT_TX_PWR_DIFF_ATTRIB,      /* 0xFFF4 */
  HDRSCMRMAC4_T2P_NO_TX_FILTER_TC_ATTRIB,           /* 0xFFF7 */
  HDRSCMRMAC4_QRAB_FILTER_TC_ATTRIB,                /* 0xFFF9 */
  HDRSCMRMAC4_PS_FILTER_TC_ATTRIB,                  /* 0xFFFA */
  HDRSCMRMAC4_PAYLOAD_THRESH_ATTRIB,                /* 0xFFFB */
  HDRSCMRMAC4_MERGE_THRESH_ATTRIB,                  /* 0xFFFC */
  HDRSCMRMAC4_FRAB_LOW_ATTRIB,                      /* 0xFFFD */
  HDRSCMRMAC4_FRAB_FILTER_TC_ATTRIB                 /* 0xFFFE */
};

/* This table is used to look up the index to be used to access per-NCX
   simple attrib information */
static hdrscmrmac4_per_ncx_attrib_id_base_type const 
  hdrscmrmac4_simple_ncx_index_lut[HDRSCMRMAC4_TOTAL_NUM_NCX_SIMPLE_ATTRIBS] =
{
  HDRSCMRMAC4_PS_QRAB_THRESH_DRC_UNLOCK_CC_BASE,    /* 0xF600 */
  HDRSCMRMAC4_PS_QRAB_THRESH_DRC_LOCK_CC_BASE,      /* 0xF700 */
  HDRSCMRMAC4_ARQ_MODE_CC_BASE,                     /* 0xF800 */
  HDRSCMRMAC4_T2P_FILTER_TC_NN_BASE,                /* 0xF900 */
  HDRSCMRMAC4_BURST_DURATION_FACT0R_NN_BASE,        /* 0xFA00 */
  HDRSCMRMAC4_QRAB_SELECT_NN_BASE,                  /* 0xFB00 */
  HDRSCMRMAC4_TRANSMISSION_MODE_NN_BASE,            /* 0xFC00 */
  HDRSCMRMAC4_MERGE_THRESHOLD_NN_BASE,              /* 0xFD00 */
  HDRSCMRMAC4_BUCKET_LEVEL_MAX_NN_BASE              /* 0xFE00 */
};

/* This table is used to look up the index to be used to access non-NCX
   complex attrib information */
static hdrscmrmac4_complex_attrib_id_enum_type const 
  hdrscmrmac4_complex_non_ncx_index_lut[HDRSCMRMAC4_TOTAL_NUM_COMPLEX_ATTRIBS] =
{
  HDRSCMRMAC4_AUX_PILOT_CHAN_PARAMS_ATTRIB,         /* 0x0000 */
  HDRSCMRMAC4_COMMON_PARAMS_ATTRIB,                 /* 0x0001 */
  HDRSCMRMAC4_PERMITTED_PAYLOAD_ATTRIB,             /* 0x0003 */
  HDRSCMRMAC4_POWER_PARAMS_128_ATTRIB,              /* 0x0005 */
  HDRSCMRMAC4_POWER_PARAMS_256_ATTRIB,              /* 0x0006 */
  HDRSCMRMAC4_POWER_PARAMS_512_ATTRIB,              /* 0x0007 */
  HDRSCMRMAC4_POWER_PARAMS_768_ATTRIB,              /* 0x0008 */
  HDRSCMRMAC4_POWER_PARAMS_1024_ATTRIB,             /* 0x0009 */
  HDRSCMRMAC4_POWER_PARAMS_1536_ATTRIB,             /* 0x000A */
  HDRSCMRMAC4_POWER_PARAMS_2048_ATTRIB,             /* 0x000B */
  HDRSCMRMAC4_POWER_PARAMS_3072_ATTRIB,             /* 0x000C */
  HDRSCMRMAC4_POWER_PARAMS_4096_ATTRIB,             /* 0x000D */
  HDRSCMRMAC4_POWER_PARAMS_6144_ATTRIB,             /* 0x000E */
  HDRSCMRMAC4_POWER_PARAMS_8192_ATTRIB,             /* 0x000F */
  HDRSCMRMAC4_POWER_PARAMS_12288_ATTRIB,            /* 0x0010 */
  HDRSCMRMAC4_REQUEST_PARAMS_ATTRIB,                /* 0x0011 */
  HDRSCMRMAC4_RRI_CHAN_POWER_PARAMS_ATTRIB,         /* 0x0012 */
  HDRSCMRMAC4_MAX_MAC_FLOWS_ATTRIB                  /* 0x0014 */
};

/* This table is used to look up the index to be used to access per-NCX
   complex attrib information */
static hdrscmrmac4_per_ncx_attrib_id_base_type const
  hdrscmrmac4_complex_ncx_index_lut[HDRSCMRMAC4_TOTAL_NUM_NCX_COMPLEX_ATTRIBS] =
{
  HDRSCMRMAC4_ASSOCIATED_FLOWS_NN_BASE,             /* 0x0100 */
  HDRSCMRMAC4_BUCKET_FACTOR_NN_BASE,                /* 0x0300 */
  HDRSCMRMAC4_T2P_INFLOW_RANGE_CONFIG_XX_BASE,      /* 0x1500 */
  HDRSCMRMAC4_T2P_TRANSITION_FUNC_CONFIG_XX_BASE,   /* 0x1600 */
  HDRSCMRMAC4_ASSOCIATED_FLOW_CONFIG_NC_BASE,       /* 0x1700 */
  HDRSCMRMAC4_ASSOCIATED_AT_CONFIG_CC_BASE,         /* 0x1800 */
  HDRSCMRMAC4_DATA_TOKEN_BUCKET_NN_BASE,            /* 0x1900 */
  HDRSCMRMAC4_MAX_NUM_SUB_PACKETS_CC_BASE,          /* 0x1A00 */
  HDRSCMRMAC4_PS_CONFIG_XX_BASE,                    /* 0x1B00 */
  HDRSCMRMAC4_TX_T2P_MAX_CONFIG_XX_BASE             /* 0x1C00 */
};

/* This table holds pointers to all Non-NCX complex attribute parsing 
   routines for ease of access */
static hdrscmrmaccmn_complex_attrib_handler_type const 
  hdrscmrmac4_complex_non_ncx_attrib_handler[] =
{
  hdrscmrmac4_process_aux_pilot_chan_params,  /* 0x0000 */
  hdrscmrmac4_process_common_power_params,    /* 0x0001 */
  hdrscmrmac4_process_permitted_payload,      /* 0x0003 */
  hdrscmrmac4_process_power_params,           /* 0x0005 */
  hdrscmrmac4_process_power_params,           /* 0x0006 */
  hdrscmrmac4_process_power_params,           /* 0x0007 */
  hdrscmrmac4_process_power_params,           /* 0x0008 */
  hdrscmrmac4_process_power_params,           /* 0x0009 */
  hdrscmrmac4_process_power_params,           /* 0x000A */
  hdrscmrmac4_process_power_params,           /* 0x000B */
  hdrscmrmac4_process_power_params,           /* 0x000C */
  hdrscmrmac4_process_power_params,           /* 0x000D */
  hdrscmrmac4_process_power_params,           /* 0x000E */
  hdrscmrmac4_process_power_params,           /* 0x000F */
  hdrscmrmac4_process_power_params,           /* 0x0010 */
  hdrscmrmac4_process_request_params,         /* 0x0011 */
  hdrscmrmac4_process_rri_chan_power_params,  /* 0x0012 */
  NULL                                        /* 0x0014, MaxMACFlows */
};

/* This table holds pointers to all NCX complex attribute parsing 
   routines for ease of access */
static hdrscmrmaccmn_complex_attrib_handler_type const 
  hdrscmrmac4_complex_ncx_attrib_handler[] =
{
  hdrscmrmac4_process_associated_flows_nn,            /* 0x0100 */
  hdrscmrmac4_process_bucket_factor_nn,               /* 0x0300 */
  hdrscmrmac4_process_t2p_inflow_range_config_xx,     /* 0x1500 */
  hdrscmrmac4_process_t2p_trans_func_config_xx,       /* 0x1600 */
  hdrscmrmac4_process_associated_flow_config_nc,      /* 0x1700 */
  hdrscmrmac4_process_associated_at_config_cc,        /* 0x1800 */
  hdrscmrmac4_process_data_token_bucket_nn,           /* 0x1900 */
  hdrscmrmac4_process_max_num_sub_pkts_cc,            /* 0x1A00 */
  hdrscmrmac4_process_pilot_strength_config_xx,       /* 0x1B00 */
  hdrscmrmac4_process_tx_t2p_max_config_xx            /* 0x1C00 */
};

/* This table defines the valid ranges for each of the simple attributes
   except the per-flow ones. This table could be indexed using the result
   from the hdrscmrmac4_simple_non_ncx_index_lut[]. */
static hdrscmrmaccmn_simple_attrib_valid_range_type const
  hdrscmrmac4_non_ncx_valid_range[HDRSCMRMAC4_TOTAL_NUM_SIMPLE_ATTRIBS] =
{
  {0x00, 0x04},   /* 0xFFF1 MaxPacketSize */
  {0x00, 0x00},   /* 0xFFF2 SupportGAUPMaxNumSubPkts; Accept default only */
  {0x08, 0x1F},   /* 0xFFF4 MaxRLPilotTxPwrDiff; 0 is also valid - handled as a special case */
  {0x00, 0x04},   /* 0xFFF7 T2PNoTxFilterTC */
  {0x00, 0x01},   /* 0xFFF9 QRABFilterTC */
  {0x00, 0x02},   /* 0xFFFA PilotStrengthFilterTC */
  {0x00, 0x03},   /* 0xFFFB PayloadThresh */
  {0x00, 0x05},   /* 0xFFFC MergeThreshold */
  {0x00, 0x04},   /* 0xFFFD FRABLow */
  {0x00, 0x03}    /* 0xFFFE FRABFilterTC */
};

/* This table defines the valid ranges for each of the per-flow, per-channel
   simple attributes.  This table could be indexed using the result
   from the hdrscmrmac4_simple_ncx_index_lut[]. */
static hdrscmrmaccmn_simple_attrib_valid_range_type const
  hdrscmrmac4_ncx_valid_range[HDRSCMRMAC4_TOTAL_NUM_NCX_SIMPLE_ATTRIBS] =
{
  {0x00, 0x3F},   /* 0xF600 PilotStrengthQRABThresholdDRCUnlockCC */
  {0x00, 0x3F},   /* 0xF700 PilotStrengthQRABThresholdDRCLockCC */
  {0x00, 0x01},   /* 0xF800 ARQModeCC */
  {0x00, 0x04},   /* 0xF9NN T2PFilterTCNN */
  {0x00, 0x03},   /* 0xFANN BurstDurationFactorNN */
  {0x00, 0x01},   /* 0xFBNN QRABSelect */
  {0x00, 0x01},   /* 0xFCNN TransmissionModeNN */
  {0x00, 0x06},   /* 0xFDNN MergeThresholdNN */
  {0x00, 0xFF}    /* 0xFENN BucketLevelMaxNN */
};

/* Attribute IDs of Non-NCX GAUPable Attributes */
static uint16 const 
  hdrscmrmac4_gaup_non_ncx_attrib_ids[HDRSCMRMAC4_NUM_GAUPABLE_NON_NCX_ATTRIBS] = 
{
  /* Simple Attributes */
  0xFFF4, /* 0.   MaxRLPilotTxPwrDiff */
  0xFFFC, /* 1.   MergeThreshold */
  0xFFFB, /* 2.   PayloadThresh */

  /* Complex Attributes */
  0x0000, /* 3.   AuxiliaryPilotChannelParameters */
  0x0003, /* 4.   PermittedPayload */
  0x0005, /* 5.   PowerParameters128 */
  0x0006, /* 6.   PowerParameters256 */
  0x0007, /* 7.   PowerParameters512 */
  0x0008, /* 8.   PowerParameters768 */
  0x0009, /* 9.   PowerParameters1024 */
  0x000A, /* 10.   PowerParameters1536 */
  0x000B, /* 11.  PowerParameters2048 */
  0x000C, /* 12.  PowerParameters3072 */
  0x000D, /* 13.  PowerParameters4096 */
  0x000E, /* 14.  PowerParameters6144 */
  0x000F, /* 15.  PowerParameters8192 */
  0x0010, /* 16.  PowerParameters12288 */
  0x0011, /* 17.  RequestParameters */
  0x0012  /* 18.  RRIChannelPowerParameters */
};

static uint16 const 
  hdrscmrmac4_gaup_ncx_attrib_ids
    [HDRSCMRMAC4_NUM_GAUPABLE_NCX_ATTRIBS] = 
{
  /* Per-Flow Simple Attributes */
  0xFE00, /* 0.   BucketLevelMaxNN */
  0xFD00, /* 1.   MergeThreshNN */
  0xFC00, /* 2.   TransmissionModeNN */
  0xFB00, /* 3.   QRABSelectNN */
  0xFA00, /* 4.   BurstDurationFactorNN */
  0xF900, /* 5.   T2PFilterTCNN */

  /* Per-Reverse Logical Channel Simple Attributes */
  0xF800, /* 6.   ARQModeCC */
  0xF700, /* 7.   PilotStrengthQRABThresholdDRCLockCC */
  0xF600, /* 8.   PilotStrengthQRABThresholdDRCUnlockCC */

  /* Per-Flow Complex Attributes */
  0x0100, /* 9.   AssociatedFlowsNN */
  0x0300, /* 10.  BucketFactorNN */
  0x1900, /* 11.  DataTokenBucketNN */

  /* Per-Flow Per-Reverse Logical Channel Complex Attributes */
  0x1700, /* 12.  AssociatedFlowConfigurationsNC */

  /* Per-Reverse Logical Channel Complex Attributes */
  0x1800, /* 13.  AssociatedATConfigurationsCC */

  /* Per-Configuration Complex Attributes */
  0x1B00, /* 14.  PilotStrengthConfigXX */
  0x1500, /* 15.  T2PInflowRangeConfigXX */
  0x1600, /* 16.  T2PTransitionFunctionConfigXX */
  0x1C00, /* 17.  TxT2PmaxConfigXX */
};

 /* EJECT */
/*===========================================================================

                         LOCAL FUNCTION DEFINITIONS
                    
===========================================================================*/
 /* EJECT */

/*===========================================================================

FUNCTION HDRSCMRMAC4_GET_INDEX_FROM_NON_NCX_ATTRIB_ID            LOCAL

DESCRIPTION
  This function is used to map the attribute ID of a Non-NCX 
  attribute to an index.
  
DEPENDENCIES
  The function assumes that the index is a Non-NCX attribute

PARAMETERS
  attrib_id - Attribute ID of the attribute.
  pindex    - Address where the computed index is stored.
  atype     - Type of attribute SIMPLE/COMPLEX

RETURN VALUE
  E_SUCCESS       - If the attrib_value is valid.
  E_DATA_INVALID  - If the attribute specified by attrib_id is not valid.

SIDE EFFECTS
  None.
  
===========================================================================*/
static hdrerrno_enum_type hdrscmrmac4_get_index_from_non_ncx_attrib_id
(
  uint16 attrib_id,
  uint16 *pindex,
  hdrrmac4_attrib_type_enum_type atype
)
{
  hdrerrno_enum_type err_no = E_SUCCESS;
    /* Error Code */

  uint16 index;
    /* attribute Index */

  boolean found=FALSE;
    /* If a matching entry is found */
	
  uint16 local_attrib_id;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */ 

  /* The for loop has been included in if else block to keep KW happy */
  if (atype == SIMPLE_ATTRIB)
  {
    for (index = 0;
         (!found && (index < HDRSCMRMAC4_TOTAL_NUM_SIMPLE_ATTRIBS));
         index++)
    {
     local_attrib_id = (uint16) hdrscmrmac4_simple_non_ncx_index_lut[index]; 
     if (attrib_id == local_attrib_id)
      {
        /* found a match */
        found = TRUE;
        *pindex = index;
      }
    }
  }
  else	/*COMPLX_ATTRIB*/
  {
    for (index = 0;
         (!found &&
          (index < HDRSCMRMAC4_TOTAL_NUM_COMPLEX_ATTRIBS));
         index++)
    {
     local_attrib_id = (uint16) hdrscmrmac4_complex_non_ncx_index_lut[index]; 
     if (attrib_id == local_attrib_id)
      {
        /* found a match */
        found = TRUE;
        *pindex = index;
      }
    }
  }


  if (!found)
  {
    err_no = E_DATA_INVALID;
  }

  return err_no;

} /* hdrscmrmac4_get_index_from_non_ncx_attrib_id */


 /* EJECT */
/*===========================================================================

FUNCTION HDRSCMRMAC4_GET_INDEX_FROM_NCX_ATTRIB_ID               LOCAL

DESCRIPTION
  This function is used to map the attribute ID of a per-NCX  
  attribute to an index.
  
DEPENDENCIES
  The function assumes that the index is a per-NCX attribute

PARAMETERS
  attrib_id - Attribute ID of the attribute.
  pindex    - Address where the computed index is stored.
  atype     - Type of attribute SIMPLE/COMPLEX

RETURN VALUE
  E_SUCCESS       - If the attrib_value is valid.
  E_DATA_INVALID  - If the attribute specified by attrib_id is not valid.

SIDE EFFECTS
  None.
  
===========================================================================*/
static hdrerrno_enum_type hdrscmrmac4_get_index_from_ncx_attrib_id
(
  uint16 attrib_id,
  uint16 *pindex,
  hdrrmac4_attrib_type_enum_type atype
)
{
  hdrerrno_enum_type err_no = E_SUCCESS;
    /* Error Code */

  uint16 index;
    /* Simple attribute Index */

  boolean found=FALSE;
    /* If a matching entry is found */

  uint16  attrib_id_base;
    /* Attrib ID base */

  uint16 comp_attrib_id_base;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */ 

  /* The for loop has been included in if else block to keep KW happy */
  if (atype == SIMPLE_ATTRIB)
  {
    attrib_id_base = HDRSCMRMAC4_GET_NCX_ATTRIB_BASE(attrib_id);
    for (index = 0;
         (!found && (index < HDRSCMRMAC4_TOTAL_NUM_NCX_SIMPLE_ATTRIBS));
         index++)
    {
       comp_attrib_id_base = (uint16) hdrscmrmac4_simple_ncx_index_lut[index];
       if (attrib_id_base == comp_attrib_id_base)
       {
         /* found a match */
         found = TRUE;
         *pindex = index;
       }
    }
  }
  else	/*COMPLX_ATTRIB*/
  {
    attrib_id_base = attrib_id & HDRSCMRMAC4_NCX_BASE_MASK;
    for (index = 0;
         (!found &&
          (index < HDRSCMRMAC4_TOTAL_NUM_NCX_COMPLEX_ATTRIBS));
         index++)
    {
      comp_attrib_id_base = (uint16) hdrscmrmac4_complex_ncx_index_lut[index];
      if (attrib_id_base == comp_attrib_id_base)
      {
        /* found a match */
        found = TRUE;
    
        *pindex = index;
      }
    }
  }


  if (!found)
  {
    err_no = E_DATA_INVALID;
  }

  return err_no;

} /* hdrscmrmac4_get_index_from_ncx_attrib_id */


 /* EJECT */
/*===========================================================================

FUNCTION HDRSCMRMAC4_PROCESS_AUX_PILOT_CHAN_PARAMS                      LOCAL

DESCRIPTION
  This is a generic interface to process the AuxiliaryPilotChannelParameters
  Attribute.

    0x0000 - AuxiliaryPilotChannelParameters Attribute 

DEPENDENCIES
  None.

PARAMETERS
  item_ptr    - pointer to the DSM item holding the message to be parsed.
  p_aip       - proposed attribute information pointer.
  a_aip       - accepted attribute information pointer.

RETURN VALUE
  E_SUCCESS       - If a valid attribute was parsed successfully.
  E_NO_DATA       - Record parsing error.
  E_OUT_OF_RANGE  - Invalid data value.

SIDE EFFECTS
  None.

===========================================================================*/
static hdrerrno_enum_type hdrscmrmac4_process_aux_pilot_chan_params
(
  dsm_item_type                               *item_ptr,
  hdrscm_proposed_attrib_type   const *const   p_aip,
  hdrscm_accepted_attrib_type         *const   a_aip
)
{

  return (hdrscmrmaccmn_process_aux_pilot_chan_params (
            item_ptr,
            p_aip,
            a_aip,
            &hdrscmrmaci.sub4.aux_pilot));

} /* hdrscmrmac4_process_aux_pilot_chan_params */


 /* EJECT */
/*===========================================================================

FUNCTION HDRSCMRMAC4_PROCESS_COMMON_POWER_PARAMS                    LOCAL

DESCRIPTION
  This is a generic interface to process the CommonPowerParameters 
  Attribute.

  0x0001 - CommonPowerParameters Attribute 

DEPENDENCIES
  None.

PARAMETERS
  item_ptr - pointer to the DSM item holding the message to be parsed.
  p_aip    - proposed attribute information pointer.
  a_aip    - accepted attribute information pointer.

RETURN VALUE
  E_SUCCESS       - If a valid attribute was parsed successfully.
  E_NO_DATA       - Record parsing error.
  E_OUT_OF_RANGE  - Invalid data value.

SIDE EFFECTS
  None.

===========================================================================*/
static hdrerrno_enum_type hdrscmrmac4_process_common_power_params
(
  dsm_item_type                               *item_ptr,
  hdrscm_proposed_attrib_type   const *const   p_aip,
  hdrscm_accepted_attrib_type         *const   a_aip
)
{

  return (hdrscmrmaccmn_process_common_power_params (
            item_ptr,
            p_aip,
            a_aip,
            &hdrscmrmaci.sub4.common_power_params));

} /* hdrscmrmac4_process_common_power_params */


 /* EJECT */
/*===========================================================================

FUNCTION HDRSCMRMAC4_PROCESS_PERMITTED_PAYLOAD                    LOCAL

DESCRIPTION
  This is a generic interface to process the PermittedPayload 
  Attribute.

  0x0003 - PermittedPayload Attribute

DEPENDENCIES
  None.

PARAMETERS
  item_ptr - pointer to the DSM item holding the message to be parsed.
  p_aip    - proposed attribute information pointer.
  a_aip    - accepted attribute information pointer.

RETURN VALUE
  E_SUCCESS       - If a valid attribute was parsed successfully.
  E_NO_DATA       - Record parsing error.
  E_OUT_OF_RANGE  - Invalid data value.

SIDE EFFECTS
  None.

===========================================================================*/
static hdrerrno_enum_type hdrscmrmac4_process_permitted_payload
(
  dsm_item_type                               *item_ptr,
  hdrscm_proposed_attrib_type   const *const   p_aip,
  hdrscm_accepted_attrib_type         *const   a_aip
)
{

  return (hdrscmrmaccmn_process_permitted_payload (
            item_ptr,
            p_aip,
            a_aip,
            hdrscmrmaci.sub4.permitted_payload) );

} /* hdrscmrmac4_process_permitted_payload */


 /* EJECT */
/*===========================================================================

FUNCTION HDRSCMRMAC4_PROCESS_POWER_PARAMS                    LOCAL

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
  item_ptr - pointer to the DSM item holding the message to be parsed.
  p_aip    - proposed attribute information pointer.
  a_aip    - accepted attribute information pointer.

RETURN VALUE
  E_SUCCESS       - If a valid attribute was parsed successfully.
  E_NO_DATA       - Record parsing error.
  E_OUT_OF_RANGE  - Invalid data value.

SIDE EFFECTS
  None.

===========================================================================*/
static hdrerrno_enum_type hdrscmrmac4_process_power_params
(
  dsm_item_type                               *item_ptr,
  hdrscm_proposed_attrib_type   const *const   p_aip,
  hdrscm_accepted_attrib_type         *const   a_aip
)
{

  uint16 pl_ind;
    /* Payload Index corresponding to this power parameter */

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */ 

  /* First power parameter is stored at index 1 */
  pl_ind = a_aip->attrib_id - (uint16)HDRSCMRMAC4_FIRST_POWER_PARAMS_ATTRIB + 1;

  ASSERT (pl_ind < HDRSCMRMACCMN_MAX_NUM_PAYLOADS);

  HDR_MSG_PROT_1( MSG_LEGACY_HIGH,
                  "PowerParameters pl_ind=%d",
                  pl_ind);

  return (hdrscmrmaccmn_process_power_params (
            item_ptr,
            p_aip,
            a_aip,
            &hdrscmrmaci.sub4.power_params[pl_ind][0]));

} /* hdrscmrmac4_process_power_params */


 /* EJECT */
/*===========================================================================

FUNCTION HDRSCMRMAC4_PROCESS_REQUEST_PARAMS                    LOCAL

DESCRIPTION
  This is a generic interface to process the RequestParameters Attribute.

  0x0011 - RequestParameters Attribute

DEPENDENCIES
  None.

PARAMETERS
  item_ptr - pointer to the DSM item holding the message to be parsed.
  p_aip    - proposed attribute information pointer.
  a_aip    - accepted attribute information pointer.

RETURN VALUE
  E_SUCCESS       - If a valid attribute was parsed successfully.
  E_NO_DATA       - Record parsing error.
  E_OUT_OF_RANGE  - Invalid data value.

SIDE EFFECTS
  None.

===========================================================================*/
static hdrerrno_enum_type hdrscmrmac4_process_request_params
(
  dsm_item_type                               *item_ptr,
  hdrscm_proposed_attrib_type   const *const   p_aip,
  hdrscm_accepted_attrib_type         *const   a_aip
)
{

  return (hdrscmrmaccmn_process_request_params (
            item_ptr,
            p_aip,
            a_aip,
            &hdrscmrmaci.sub4.req_params));

} /* hdrscmrma4_process_request_params */


 /* EJECT */
/*===========================================================================

FUNCTION HDRSCMRMAC4_PROCESS_RRI_CHAN_POWER_PARAMS                   LOCAL

DESCRIPTION
  This is a generic interface to process the RRIChannelPowerParameters 
  Attribute.

  0x0012 - RRIChannelPowerParameters Attribute

DEPENDENCIES
  None.

PARAMETERS
  item_ptr - pointer to the DSM item holding the message to be parsed.
  p_aip    - proposed attribute information pointer.
  a_aip    - accepted attribute information pointer.

RETURN VALUE
  E_SUCCESS       - If a valid attribute was parsed successfully.
  E_NO_DATA       - Record parsing error.
  E_OUT_OF_RANGE  - Invalid data value.

SIDE EFFECTS
  None.

===========================================================================*/
static hdrerrno_enum_type hdrscmrmac4_process_rri_chan_power_params
(
  dsm_item_type                               *item_ptr,
  hdrscm_proposed_attrib_type   const *const   p_aip,
  hdrscm_accepted_attrib_type         *const   a_aip
)
{

  return (hdrscmrmaccmn_process_rri_chan_power_params (
            item_ptr,
            p_aip,
            a_aip,
            hdrscmrmaci.sub4.rri_chan_power_params));

} /* hdrscmrmac4_process_rri_chan_power_params */


 /* EJECT */
/*===========================================================================

FUNCTION HDRSCMRMAC4_PROCESS_ASSOCIATED_FLOWS_NN                    LOCAL

DESCRIPTION
  This is a generic interface to process the AssociatedFlowsNN Attribute.

  0x0100 - 0x01NN - AssociatedFlowsNN Attributes

DEPENDENCIES
  None.

PARAMETERS
  item_ptr - pointer to the DSM item holding the message to be parsed.
  p_aip    - proposed attribute information pointer.
  a_aip    - accepted attribute information pointer.

RETURN VALUE
  E_SUCCESS       - If a valid attribute was parsed successfully.
  E_NO_DATA       - Record parsing error.
  E_OUT_OF_RANGE  - Invalid data value.

SIDE EFFECTS
  None.

===========================================================================*/
static hdrerrno_enum_type hdrscmrmac4_process_associated_flows_nn
(
  dsm_item_type                               *item_ptr,
  hdrscm_proposed_attrib_type   const *const   p_aip,
  hdrscm_accepted_attrib_type         *const   a_aip
)
{

  uint8 flow_id;
    /* Flow ID */

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */ 

  /* Get the flow ID from the attribute ID */
  flow_id = HDRSCMRMACCMN_GET_NN_ATTRIB_FLOW_ID(a_aip->attrib_id);

  HDR_MSG_PROT_1( MSG_LEGACY_HIGH,
                  "AssociatedFlowsNN %x",
                  flow_id);

  return (hdrscmrmaccmn_process_associated_flows_nn (
            item_ptr,
            p_aip,
            a_aip,
            &hdrscmrmaci.sub4.associated_flows[flow_id]));

} /* hdrscmrmac4_process_associated_flows_nn */


 /* EJECT */
/*===========================================================================

FUNCTION HDRSCMRMAC4_PROCESS_BUCKET_FACTOR_NN                         LOCAL

DESCRIPTION
  This is a generic interface to process the BucketFactorNN Attribute.

  0x0300 - 0x03NN - BucketFactorNN Attributes

DEPENDENCIES
  None.

PARAMETERS
  item_ptr - pointer to the DSM item holding the message to be parsed.
  p_aip    - proposed attribute information pointer.
  a_aip    - accepted attribute information pointer.

RETURN VALUE
  E_SUCCESS       - If a valid attribute was parsed successfully.
  E_NO_DATA       - Record parsing error.
  E_OUT_OF_RANGE  - Invalid data value.

SIDE EFFECTS
  None.

===========================================================================*/
static hdrerrno_enum_type hdrscmrmac4_process_bucket_factor_nn
(
  dsm_item_type                               *item_ptr,
  hdrscm_proposed_attrib_type   const *const   p_aip,
  hdrscm_accepted_attrib_type         *const   a_aip
)
{

  uint8 flow_id;
    /* Flow ID */

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */ 
  
  /* Get the flow ID from the attribute ID */
  flow_id = HDRSCMRMACCMN_GET_NN_ATTRIB_FLOW_ID(a_aip->attrib_id);

  HDR_MSG_PROT_1( MSG_LEGACY_HIGH,
                  "BucketFactorNN %x",
                  flow_id);

  return (hdrscmrmaccmn_process_bucket_factor_nn (
            item_ptr,
            p_aip,
            a_aip,
            &hdrscmrmaci.sub4.bucket_factor[flow_id]));

} /* hdrscmrmac4_process_bucket_factor_nn */


 /* EJECT */
/*===========================================================================

FUNCTION HDRSCMRMAC4_PROCESS_T2P_INFLOW_RANGE_CONFIG_XX                LOCAL

DESCRIPTION
  This is a generic interface to process the T2PInflowRangeConfigXX Attribute.

  0x1500 - T2PInflowRangeConfigXX Attributes

DEPENDENCIES
  None.

PARAMETERS
  item_ptr - pointer to the DSM item holding the message to be parsed.
  p_aip    - proposed attribute information pointer.
  a_aip    - accepted attribute information pointer.

RETURN VALUE
  E_SUCCESS       - If a valid attribute was parsed successfully.
  E_NO_DATA       - Record parsing error.
  E_OUT_OF_RANGE  - Invalid data value.

SIDE EFFECTS
  None.

===========================================================================*/
static hdrerrno_enum_type hdrscmrmac4_process_t2p_inflow_range_config_xx
(
  dsm_item_type                               *item_ptr,
  hdrscm_proposed_attrib_type   const *const   p_aip,
  hdrscm_accepted_attrib_type         *const   a_aip
)
{

  uint8 config_id;
    /* Configuration ID */

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */ 

  /* Get the config ID from the attribute ID */
  config_id = HDRSCMRMAC4_GET_XX_ATTRIB_CONFIG_ID(a_aip->attrib_id);

  HDR_MSG_PROT_1( MSG_LEGACY_HIGH,
                  "T2PInflowRangeConfigXX %x",
                  config_id);

  return (hdrscmrmaccmn_process_t2p_inflow_range_nn__config_xx (
            item_ptr,
            p_aip,
            a_aip,
            &hdrscmrmaci.sub4.t2p_inflow_range[config_id]));

} /* hdrscmrmac4_process_t2p_inflow_range_config_xx */


 /* EJECT */
/*===========================================================================

FUNCTION HDRSCMRMAC4_PROCESS_T2P_TRANS_FUNC_CONFIG_XX                   LOCAL

DESCRIPTION
  This is a generic interface to process the T2PTransitionFunctionConfigXX
  Attribute.

  0x1600 - T2PTransitionFunctionConfigXX Attributes

DEPENDENCIES
  None.

PARAMETERS
  item_ptr - pointer to the DSM item holding the message to be parsed.
  p_aip    - proposed attribute information pointer.
  a_aip    - accepted attribute information pointer.

RETURN VALUE
  E_SUCCESS       - If a valid attribute was parsed successfully.
  E_NO_DATA       - Record parsing error.
  E_OUT_OF_RANGE  - Invalid data value.

SIDE EFFECTS
  None.

===========================================================================*/
static hdrerrno_enum_type hdrscmrmac4_process_t2p_trans_func_config_xx
(
  dsm_item_type                               *item_ptr,
  hdrscm_proposed_attrib_type   const *const   p_aip,
  hdrscm_accepted_attrib_type         *const   a_aip
)
{


  uint8 config_id;
    /* Config ID */

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */ 

  /* Get the config ID from the attribute ID */
  config_id = HDRSCMRMAC4_GET_XX_ATTRIB_CONFIG_ID(a_aip->attrib_id);

  HDR_MSG_PROT_1( MSG_LEGACY_HIGH,
                  "T2PTransitionFunctionConfigXX %x",
                  config_id);

  return (hdrscmrmaccmn_process_t2p_trans_func_nn__config_xx(
            item_ptr,
            p_aip,
            a_aip,
            &hdrscmrmaci.sub4.gu_gd[config_id]));

} /* hdrscmrmac4_process_t2p_trans_func_config_xx */


 /* EJECT */
/*===========================================================================

FUNCTION HDRSCMRMAC4_IS_ASSOCIATED_FLOW_CONFIG_NC_VALID                 LOCAL

DESCRIPTION
  Check if the following attribute is valid 

  0x17NC - AssociatedFlowConfigurationsNC Attribute.

DEPENDENCIES
  None.

PARAMETERS
  flow_id - Flow ID of the attrib specified.
  channel_id - Reverse Channel ID of the attrib specified.

RETURN VALUE
  E_SUCCESS      - If the Attribute is valid.
  E_OUT_OF_RANGE - otherwise. 

SIDE EFFECTS
  None.

===========================================================================*/
static hdrerrno_enum_type hdrscmrmac4_is_associated_flow_config_nc_valid
(
  uint8 flow_id,
  uint8 channel_id
)
{
  hdrerrno_enum_type err_no = E_SUCCESS;
    /* Error code */

  /* Make sure the T2PTransitionFunction and T2PInflowRange are in valid range */
  if ((hdrscmrmaci.sub4.associated_flow_config[flow_id][channel_id].
       t2p_transition_func_select >= 
       HDRSCMRMAC4_MAX_NUM_CONFIGS) ||
      (hdrscmrmaci.sub4.associated_flow_config[flow_id][channel_id].
       t2p_inflow_range_select >=
       HDRSCMRMAC4_MAX_NUM_CONFIGS))
  {
    err_no = E_OUT_OF_RANGE;
  }

  return err_no;

} /* hdrscmrmac4_is_associated_flow_config_nc_valid */
 /* EJECT */
/*===========================================================================

FUNCTION HDRSCMRMAC4_PROCESS_ASSOCIATED_FLOW_CONFIG_NC                  LOCAL

DESCRIPTION
  This is a generic interface to process the AssociatedFlowConfigurationsNC
  Attribute.

  0x1800 - AssociatedFlowConfigurationsNC Attribute 

DEPENDENCIES
  None.

PARAMETERS
  item_ptr - pointer to the DSM item holding the message to be parsed.
  p_aip    - proposed attribute information pointer.
  a_aip    - accepted attribute information pointer.

RETURN VALUE
  E_SUCCESS       - If a valid attribute was parsed successfully.
  E_NO_DATA       - Record parsing error.
  E_OUT_OF_RANGE  - Invalid data value.

SIDE EFFECTS
  None.

===========================================================================*/
static hdrerrno_enum_type hdrscmrmac4_process_associated_flow_config_nc
(
  dsm_item_type                               *item_ptr,
  hdrscm_proposed_attrib_type   const *const   p_aip,
  hdrscm_accepted_attrib_type         *const   a_aip
)
{

  uint16 attrib_offset;
    /* Offset to the current byte being processed from the start of DSM chain */

  uint8 value_id;
    /* Value ID of the complex attribute */

  hdrerrno_enum_type err_no = E_NO_DATA;
    /* Status Code */

  uint8 flow_id;
    /* Flow ID */

  uint8 channel_id;
    /* Reverse Channel ID */

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */ 

  /* Get the Channel and Flow ID from the attrib ID */
  flow_id     = HDRSCMRMAC4_GET_NC_ATTRIB_FLOW_ID(a_aip->attrib_id);
  channel_id  = HDRSCMRMAC4_GET_NC_ATTRIB_CHAN_ID(a_aip->attrib_id);

  /* Initialize the attribute offset to start of first Value ID */
  attrib_offset = p_aip->attrib_len_size + a_aip->attrib_id_size;

  /* While there are enough bits left in the Attribute Record for one more
     set of AssociatedFlowConfigurationsNC complex Attribute Value */
  while ( (attrib_offset + p_aip->value_id_size + 
           HDRSCMRMAC4_AFC_NC_SIZE ) <= p_aip->attrib_len )
  {
    /*----------------------------------------------------------------------
       There is at least one more attribute value in this attribute Record
     *--------------------------------------------------------------------*/

    /* Initialize the error code to success since we know that we have at least
       one attrib value in the Attrib Record */
    err_no = E_SUCCESS;

    /* Unpack the Value ID into a uint8 */
    err_no = HDRBIT_UNPACK8( item_ptr,
                             &(value_id),
                             (p_aip->attrib_offset + attrib_offset),
                             p_aip->value_id_size );

    /* If error unpacking value ID, break */
    if (err_no != E_SUCCESS)
    {
      ERR("Error unpacking Value ID of AssociatedFlowConfigurationsNC Attrib",
           0,0,0);
      break;
    }

    /* Since Value Id was parsed sucessfully, Update the attribute offset to 
     * start of  field */ 
    attrib_offset += p_aip->value_id_size ;

    /* Unpack T2PTransitionFunction */
    err_no = HDRBIT_UNPACK8( item_ptr,
                             &(hdrscmrmaci.sub4.associated_flow_config
                               [flow_id][channel_id].t2p_transition_func_select),
                             (p_aip->attrib_offset + attrib_offset),
                             HDRSCMRMAC4_AFC_T2P_TRANS_FUNC_SELECT_SIZE );

    /* If error unpacking T2PTransitionFunction, break */
    if (err_no != E_SUCCESS)
    {
      ERR("Error unpacking T2PTransitionFunction for AssociatedFlowConfigurationsNC",
          0,0,0);
      break;
    }

    /* Shift the offset to next field */
    attrib_offset += HDRSCMRMAC4_AFC_T2P_TRANS_FUNC_SELECT_SIZE;

    /* Unpack T2PInflowRange */
    err_no = HDRBIT_UNPACK8( item_ptr,
                             &(hdrscmrmaci.sub4.associated_flow_config
                               [flow_id][channel_id].t2p_inflow_range_select),
                             (p_aip->attrib_offset + attrib_offset),
                             HDRSCMRMAC4_AFC_T2P_INFLOW_RANGE_SELECT_SIZE );

    /* If error unpacking T2PInflowRange, break */
    if (err_no != E_SUCCESS)
    {
      ERR("Error unpacking T2PInflowRange for AssociatedFlowConfigurationsNC",
          0,0,0);
      break;
    }

    /* Shift the offset to next field */
    attrib_offset += HDRSCMRMAC4_AFC_T2P_INFLOW_RANGE_SELECT_SIZE;

    /* Parsing done! :-) Is the parsed attribute valid? */
    if ( (err_no = 
          hdrscmrmac4_is_associated_flow_config_nc_valid(flow_id, channel_id)) 
         != E_OUT_OF_RANGE )
    {
      /* Attrib valid! Yea! :-) print F3 mesg */
      HDR_MSG_PROT_3( MSG_LEGACY_HIGH,
                      "AssociatedFlowConfigNC flow/channel: %x, t2p_trans: %x, t2_inflow_range: %x",
                      (a_aip->attrib_id & HDRSCMRMAC4_NCX_ID_MASK),
                      hdrscmrmaci.sub4.associated_flow_config
                        [flow_id][channel_id].t2p_transition_func_select,
                      hdrscmrmaci.sub4.associated_flow_config
                        [flow_id][channel_id].t2p_inflow_range_select );

      /* Update the accepted attribute information */
      a_aip->is_accepted = TRUE;
      a_aip->value_id = value_id;
      a_aip->complex_val_ptr = (void*) &(hdrscmrmaci.sub4.associated_flow_config
                                         [flow_id][channel_id]);
      a_aip->complex_val_size = sizeof(hdrscmrmac4_associated_flow_config_struct_type);
      break;
    }

    /* Not much luck :-( Let's try next set of values if there is one. 
       Shift the offset to start of next value ID. */

  } /* while */

  return err_no;

} /* hdrscmrmac4_process_associated_flow_config_nc */


 /* EJECT */
/*===========================================================================

FUNCTION HDRSCMRMAC4_IS_ASSOCIATED_AT_CONFIG_CC_VALID                  LOCAL

DESCRIPTION
  Check if the following attribute is valid 

  0x18CC - AssociatedATConfigCC Attribute.

DEPENDENCIES
  None.

PARAMETERS
  channel_id - Reverse Channel ID of the per-Channel attrib specified. 

RETURN VALUE
  E_SUCCESS      - If the Attribute is valid.
  E_OUT_OF_RANGE - otherwise. 

SIDE EFFECTS
  None.

===========================================================================*/
static hdrerrno_enum_type hdrscmrmac4_is_associated_at_config_cc_valid
(
  uint8 channel_id
)
{
  hdrerrno_enum_type err_no = E_SUCCESS;
    /* Error code */

  /* Make sure the PilotStrength and TxT2PMax Selects are in valid range */
  if ((hdrscmrmaci.sub4.associated_at_config[channel_id].ps_select >= 
       HDRSCMRMAC4_MAX_NUM_CONFIGS) ||
      (hdrscmrmaci.sub4.associated_at_config[channel_id].tx_t2p_max_select >=
       HDRSCMRMAC4_MAX_NUM_CONFIGS))
  {
    err_no = E_OUT_OF_RANGE;
  }

  return err_no;

} /* hdrscmrmac4_is_associated_at_config_cc_valid */


 /* EJECT */
/*===========================================================================

FUNCTION HDRSCMRMAC4_PROCESS_ASSOCIATED_AT_CONFIG_CC                   LOCAL

DESCRIPTION
  This is a generic interface to process the AssociatedATConfigurationsCC
  Attribute.

  0x1800 - AssociatedATConfigurationsCC Attribute 

DEPENDENCIES
  None.

PARAMETERS
  item_ptr - pointer to the DSM item holding the message to be parsed.
  p_aip    - proposed attribute information pointer.
  a_aip    - accepted attribute information pointer.

RETURN VALUE
  E_SUCCESS       - If a valid attribute was parsed successfully.
  E_NO_DATA       - Record parsing error.
  E_OUT_OF_RANGE  - Invalid data value.

SIDE EFFECTS
  None.

===========================================================================*/
static hdrerrno_enum_type hdrscmrmac4_process_associated_at_config_cc
(
  dsm_item_type                               *item_ptr,
  hdrscm_proposed_attrib_type   const *const   p_aip,
  hdrscm_accepted_attrib_type         *const   a_aip
)
{

  uint16 attrib_offset;
    /* Offset to the current byte being processed from the start of DSM chain */

  uint8 value_id;
    /* Value ID of the complex attribute */

  hdrerrno_enum_type err_no = E_NO_DATA;
    /* Status Code */

  uint8 channel_id;
    /* Reverse Channel ID */

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */ 

  /* Get the Channel ID from the attrib ID */
  channel_id = HDRSCMRMAC4_GET_CC_ATTRIB_CHAN_ID(a_aip->attrib_id);

  /* Initialize the attribute offset to start of first Value ID */
  attrib_offset = p_aip->attrib_len_size + a_aip->attrib_id_size;

  /* While there are enough bits left in the Attribute Record for one more
     set of AssociatedATConfigurationsCC complex Attribute Value */
  while ( (attrib_offset + p_aip->value_id_size + 
           HDRSCMRMAC4_AAC_CC_SIZE ) <= p_aip->attrib_len )
  {
    /*----------------------------------------------------------------------
       There is at least one more attribute value in this attribute Record
     *--------------------------------------------------------------------*/

    /* Initialize the error code to success since we know that we have at least
       one attrib value in the Attrib Record */
    err_no = E_SUCCESS;

    /* Unpack the Value ID into a uint8 */
    err_no = HDRBIT_UNPACK8( item_ptr,
                             &(value_id),
                             (p_aip->attrib_offset + attrib_offset),
                             p_aip->value_id_size );

    /* If error unpacking value ID, break */
    if (err_no != E_SUCCESS)
    {
      ERR("Error unpacking Value ID of AssociatedATConfigurationsCC Attrib",
            0,0,0);
      break;
    }

    /* Since Value Id was parsed sucessfully, Update the attribute offset to 
     * start of  field */ 
    attrib_offset += p_aip->value_id_size ;

    /* Unpack PilotStrength */
    err_no = HDRBIT_UNPACK8( item_ptr,
                             &(hdrscmrmaci.sub4.associated_at_config[channel_id].ps_select ),
                             (p_aip->attrib_offset + attrib_offset),
                             HDRSCMRMAC4_AAC_PS_SELECT_SIZE );

    /* If error unpacking PilotStrength, break */
    if (err_no != E_SUCCESS)
    {
      ERR("Error unpacking PilotStrength for AssociatedATConfigurationsCC",
          0,0,0);
      break;
    }

    /* Shift the offset to next field */
    attrib_offset += HDRSCMRMAC4_AAC_PS_SELECT_SIZE;

    /* Unpack TxT2PMax */
    err_no = HDRBIT_UNPACK8( item_ptr,
                             &(hdrscmrmaci.sub4.associated_at_config[channel_id].
                               tx_t2p_max_select ),
                             (p_aip->attrib_offset + attrib_offset),
                             HDRSCMRMAC4_AAC_TX_T2P_MAX_SELECT_SIZE );

    /* If error unpacking TxT2PMax, break */
    if (err_no != E_SUCCESS)
    {
      ERR("Error unpacking TxT2PMax for AssociatedATConfigurationsCC",
          0,0,0);
      break;
    }

    /* Shift the offset to next field */
    attrib_offset += HDRSCMRMAC4_AAC_TX_T2P_MAX_SELECT_SIZE;

    /* Parsing done! :-) Is the parsed attribute valid? */
    if ( (err_no = 
          hdrscmrmac4_is_associated_at_config_cc_valid(channel_id)) != E_OUT_OF_RANGE )
    {
      /* Attrib valid! Yea! :-) print F3 mesg */
      HDR_MSG_PROT_3( MSG_LEGACY_HIGH,
                      "AssociatedATConfigCC channel: %x,ps: %x, txt2pmax: %x",
                      channel_id,
                      hdrscmrmaci.sub4.associated_at_config[channel_id].ps_select,
                      hdrscmrmaci.sub4.associated_at_config[channel_id].
                      tx_t2p_max_select );

      /* Update the accepted attribute information */
      a_aip->is_accepted = TRUE;
      a_aip->value_id = value_id;
      a_aip->complex_val_ptr = (void*) &(hdrscmrmaci.sub4.associated_at_config[channel_id]);
      a_aip->complex_val_size = sizeof(hdrscmrmac4_associated_at_config_struct_type);
      break;
    }

    /* Not much luck :-( Let's try next set of values if there is one. 
       Shift the offset to start of next value ID. */

  } /* while */

  return err_no;

} /* hdrscmrmac4_process_associated_at_config_cc */


 /* EJECT */
/*===========================================================================

FUNCTION HDRSCMRMAC4_IS_DATA_TOKEN_BUCKET_NN_VALID                     LOCAL

DESCRIPTION
  Check if the following attribute is valid 

  0x19NN - DataTokenBucketNN Attribute.

DEPENDENCIES
  None.

PARAMETERS
  flow_id - MAC Flow ID of the per-flow attrib specified. 

RETURN VALUE
  E_SUCCESS      - If the Attribute is valid.
  E_OUT_OF_RANGE - otherwise. 

SIDE EFFECTS
  None.

===========================================================================*/
static hdrerrno_enum_type hdrscmrmac4_is_data_token_bucket_nn_valid
(
  uint8 flow_id
)
{
  hdrerrno_enum_type err_no = E_SUCCESS;
    /* Error code */

  /* Make sure the DataTokenInflow is not in the Reserved range */
  if ((hdrscmrmaci.sub4.data_token_bucket[flow_id].data_token_inflow >= 
       HDRSCMRMAC4_MIN_DATA_TOKEN_INFLOW_RESERVED) &&
      (hdrscmrmaci.sub4.data_token_bucket[flow_id].data_token_inflow <= 
       HDRSCMRMAC4_MAX_DATA_TOKEN_INFLOW_RESERVED))
  {
    err_no = E_OUT_OF_RANGE;
  }

  return err_no;

} /* hdrscmrmac4_is_data_token_bucket_nn_valid */


 /* EJECT */
/*===========================================================================

FUNCTION HDRSCMRMAC4_PROCESS_DATA_TOKEN_BUCKET_NN                    LOCAL

DESCRIPTION
  This is a generic interface to process the DataTokenBucketNN
  Attribute.

  0x1900 - DataTokenBucketNN Attribute 

DEPENDENCIES
  None.

PARAMETERS
  item_ptr - pointer to the DSM item holding the message to be parsed.
  p_aip    - proposed attribute information pointer.
  a_aip    - accepted attribute information pointer.

RETURN VALUE
  E_SUCCESS       - If a valid attribute was parsed successfully.
  E_NO_DATA       - Record parsing error.
  E_OUT_OF_RANGE  - Invalid data value.

SIDE EFFECTS
  None.

===========================================================================*/
static hdrerrno_enum_type hdrscmrmac4_process_data_token_bucket_nn
(
  dsm_item_type                               *item_ptr,
  hdrscm_proposed_attrib_type   const *const   p_aip,
  hdrscm_accepted_attrib_type         *const   a_aip
)
{
  uint16 attrib_offset;
    /* Offset to the current byte being processed from the start of DSM chain */

  uint8 value_id;
    /* Value ID of the complex attribute */

  hdrerrno_enum_type err_no = E_NO_DATA;
    /* Status Code */

  uint8 flow_id;
    /* Flow ID */

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */ 

  /* Get the flow ID from the attribute ID */
  flow_id = HDRSCMRMACCMN_GET_NN_ATTRIB_FLOW_ID(a_aip->attrib_id);

  /* Initialize the attribute offset to start of first Value ID */
  attrib_offset = p_aip->attrib_len_size + a_aip->attrib_id_size;

  /* While there are enough bits left in the Attribute Record for one moe
     set of DataTokenBucketNN complex Attribute Value */
  while ( (attrib_offset + p_aip->value_id_size +
           HDRSCMRMAC4_DTB_NN_SIZE) <= p_aip->attrib_len )
  {
    /*----------------------------------------------------------------------
       There is at least one more attribute value in this attribute Record
     *--------------------------------------------------------------------*/

    /* Initialize the error code to success since we know that we have at least
       one attrib value in the Attrib Record */
    err_no = E_SUCCESS;

    /* Unpack the Value ID into a uint8 */
    err_no = HDRBIT_UNPACK8( item_ptr,
                             &(value_id),
                             (p_aip->attrib_offset + attrib_offset),
                             p_aip->value_id_size );

    /* If error unpacking value ID, break */
    if (err_no != E_SUCCESS)
    {
      ERR("Error unpacking Value ID of DataTokenBucketNN Attrib", 0,0,0);
      break;
    }

    /*----------------------------------------------------------------------
                                DataTokenInflow
     *--------------------------------------------------------------------*/

    /* Shift the attribute offset to start of DataTokenInflow field */ 
    attrib_offset += p_aip->value_id_size ;

    /* Unpack DataTokenInflow */
    err_no = HDRBIT_UNPACK8( item_ptr,
                             &(hdrscmrmaci.sub4.data_token_bucket[flow_id].
                               data_token_inflow),
                             (p_aip->attrib_offset + attrib_offset),
                             HDRSCMRMAC4_DTB_DATA_TOKEN_INFLOW_SIZE );

    /* If error unpacking DataTokenInflow, break */
    if (err_no != E_SUCCESS)
    {
      ERR("Error unpacking DataTokenInflow of DataTokenBucketNN attrib",
          0,0,0);
      break;
    }

    /*----------------------------------------------------------------------
                             DataBucketLevelMax
     *--------------------------------------------------------------------*/

    /* Shift the offset to start of  DataBucketLevelMax field */
    attrib_offset += HDRSCMRMAC4_DTB_DATA_TOKEN_INFLOW_SIZE;

    /* Unpack DataBucketLevelMax field */
    err_no = HDRBIT_UNPACK8( item_ptr,
                             &(hdrscmrmaci.sub4.data_token_bucket[flow_id].
                               data_bucket_level_max),
                             (p_aip->attrib_offset + attrib_offset),
                             HDRSCMRMAC4_DTB_DATA_BUCKET_LEVEL_MAX_SIZE );

    /* If error unpacking DataTokenInflow, break */
    if (err_no != E_SUCCESS)
    {
      ERR("Error unpacking DataBucketLevelMax of DataTokenBucketNN attrib",
          0,0,0);
      break;
    }

    /* Parsing done! :-) Is the parsed attribute valid? */
    if ( (err_no =
          hdrscmrmac4_is_data_token_bucket_nn_valid(flow_id)) != 
          E_OUT_OF_RANGE )
    {
      /* Attrib value! Yea! :-) print F3 mesg */
      HDR_MSG_PROT_3(
        MSG_LEGACY_HIGH,
        "DTB FlowID: %x, DataTokenInflow: %x, DataBucketLevelMax: %x",
        flow_id,
        hdrscmrmaci.sub4.data_token_bucket[flow_id].data_token_inflow,
        hdrscmrmaci.sub4.data_token_bucket[flow_id].data_bucket_level_max );

      /* Update the accepted attribute information */
      a_aip->is_accepted = TRUE;
      a_aip->value_id = value_id;
      a_aip->complex_val_ptr = (void *) &(hdrscmrmaci.sub4.data_token_bucket[flow_id]);
      a_aip->complex_val_size = sizeof(hdrscmrmac4_data_token_bucket_struct_type);
      break;
    }

    /* Not much luck :-( Let's try next set of values if there is one.
       Shift the offset to start of next valud ID. */
    attrib_offset += HDRSCMRMAC4_DTB_DATA_BUCKET_LEVEL_MAX_SIZE;

  } /* while */

  return err_no;

} /* hdrscmrmac4_process_data_token_bucket_nn */


 /* EJECT */
/*===========================================================================

FUNCTION HDRSCMRMAC4_PROCESS_MAX_NUM_SUB_PKTS_CC                    LOCAL

DESCRIPTION
  This is a generic interface to process the MaxNumSubPacketsCC
  Attribute.

  0x1A00 - MaxNumSubPacketsCC Attribute 

DEPENDENCIES
  None.

PARAMETERS
  item_ptr - pointer to the DSM item holding the message to be parsed.
  p_aip    - proposed attribute information pointer.
  a_aip    - accepted attribute information pointer.

RETURN VALUE
  E_SUCCESS       - If a valid attribute was parsed successfully.
  E_NO_DATA       - Record parsing error.
  E_OUT_OF_RANGE  - Invalid data value.

SIDE EFFECTS
  None.

===========================================================================*/
static hdrerrno_enum_type hdrscmrmac4_process_max_num_sub_pkts_cc
(
  dsm_item_type                               *item_ptr,
  hdrscm_proposed_attrib_type   const *const   p_aip,
  hdrscm_accepted_attrib_type         *const   a_aip
)
{

  uint8 channel_id;
    /* Reverse Channel ID */

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */ 

  /* Get the Channel ID from the attrib ID */
  channel_id = HDRSCMRMAC4_GET_CC_ATTRIB_CHAN_ID(a_aip->attrib_id);

  HDR_MSG_PROT_1( MSG_LEGACY_HIGH,
                  "MaxNumSubPacketsCC %x",
                  channel_id);

  return (hdrscmrmaccmn_process_max_num_sub_pkts__cc (
            item_ptr,
            p_aip,
            a_aip,
            (void *) &hdrscmrmaci.sub4.max_num_sub_pkts[channel_id]));

} /* hdrscmrmac4_process_max_num_sub_pkts_cc */


 /* EJECT */
/*===========================================================================

FUNCTION HDRSCMRMAC4_PROCESS_PILOT_STRENGTH_CONFIG_XX                   LOCAL

DESCRIPTION This is a generic interface to process the PilotStrength Attribute.

  0x1B00 - PilotStrengthConfigXX Attribute

DEPENDENCIES
  None.

PARAMETERS
  item_ptr - pointer to the DSM item holding the message to be parsed.
  p_aip    - proposed attribute information pointer.
  a_aip    - accepted attribute information pointer.

RETURN VALUE
  E_SUCCESS       - If a valid attribute was parsed successfully.
  E_NO_DATA       - Record parsing error.
  E_OUT_OF_RANGE  - Invalid data value.

SIDE EFFECTS
  None.

===========================================================================*/
static hdrerrno_enum_type hdrscmrmac4_process_pilot_strength_config_xx
(
  dsm_item_type                               *item_ptr,
  hdrscm_proposed_attrib_type   const *const   p_aip,
  hdrscm_accepted_attrib_type         *const   a_aip
)
{

  uint8 config_id;
    /* Config ID */

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */ 

  /* Get the config ID from the attribute ID */
  config_id = HDRSCMRMAC4_GET_XX_ATTRIB_CONFIG_ID(a_aip->attrib_id);

  HDR_MSG_PROT_1( MSG_LEGACY_HIGH,
                  "PilotStrengthConfigXX %x",
                  config_id);

  return (hdrscmrmaccmn_process_pilot_strength__config_xx (
            item_ptr,
            p_aip,
            a_aip,
            (void *) &hdrscmrmaci.sub4.pilot_strength[config_id]));

} /* hdrscmrmac4_process_pilot_strength_config_xx */


 /* EJECT */
/*===========================================================================

FUNCTION HDRSCMRMAC4_PROCESS_TX_T2P_MAX_CONFIG_XX                       LOCAL

DESCRIPTION
  This is a generic interface to process the 
  Attribute.

  0x1C00 - TxT2PMaxConfigXX Attribute

DEPENDENCIES
  None.

PARAMETERS
  item_ptr - pointer to the DSM item holding the message to be parsed.
  p_aip    - proposed attribute information pointer.
  a_aip    - accepted attribute information pointer.

RETURN VALUE
  E_SUCCESS       - If a valid attribute was parsed successfully.
  E_NO_DATA       - Record parsing error.
  E_OUT_OF_RANGE  - Invalid data value.

SIDE EFFECTS
  None.

===========================================================================*/
static hdrerrno_enum_type hdrscmrmac4_process_tx_t2p_max_config_xx
(
  dsm_item_type                               *item_ptr,
  hdrscm_proposed_attrib_type   const *const   p_aip,
  hdrscm_accepted_attrib_type         *const   a_aip
)
{

  uint8 config_id;
    /* Config ID */

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */ 

  /* Get the config ID from the attribute ID */
  config_id = HDRSCMRMAC4_GET_XX_ATTRIB_CONFIG_ID(a_aip->attrib_id);

  HDR_MSG_PROT_1( MSG_LEGACY_HIGH,
                  "Tx2PMaxConfigXX %x",
                  config_id);

  return (hdrscmrmaccmn_process_tx_t2p_max__config_xx (
            item_ptr,
            p_aip,
            a_aip,
            (void *) &hdrscmrmaci.sub4.txt2p_max[config_id]));

} /* hdrscmrmac4_process_tx_t2p_max_config_xx */


 /* EJECT */
/*===========================================================================

FUNCTION HDRSCMRMAC4_GET_SIMPLE_ATTRIB_OFFSET_AND_SIZE

DESCRIPTION
  This function computes the byte offset of the simple attribute specified by 
  the attrib_id in the struct hdrscmrmac4_config_struct_type and stores it in 
  out-param attrib_offset. It also stores the size of the attribute in 
  attrib_size.

DEPENDENCIES
  The attrib_id should belong to a simple attribute.

PARAMETERS
  attrib_id     - Attribute Id.
  attrib_offset - Address where the Byte Offset from the start of the struct 
                  to the attribute is stored.
  attrib_size   - Address where the size of the attribute is stored.
  
RETURN VALUE
  E_SUCCESS       - If the given attrib ID is valid.
  E_DATA_INVALID  - Otherwise.

SIDE EFFECTS
  None

===========================================================================*/
hdrerrno_enum_type hdrscmrmac4_get_simple_attrib_offset_and_size
(
  uint16 attrib_id, 
  uint16 *attrib_offset,
  uint16 *attrib_size
)
{
  hdrerrno_enum_type err_no = E_DATA_INVALID;
    /* Error Code */

  uint16 index = 0;
    /* Simple attribute Index */

  uint8 ncx_id = 0;
    /* NN/CC/XX index */
#if defined(__ARMCC_VERSION) && (__ARMCC_VERSION > 210000)
#pragma diag_suppress 1296
#endif
  static hdrscmrmac4_simple_non_ncx_attrib_offset_lut_type const
                  non_ncx_lut[HDRSCMRMAC4_TOTAL_NUM_SIMPLE_ATTRIBS] =
  {
    {FPOS(hdrscmrmac4_config_struct_type, max_packet_size)},          /* 0xFFF1 */
    {FPOS(hdrscmrmac4_config_struct_type, gaup_max_num_sub_pkts)},    /* 0xFFF2 */
    {FPOS(hdrscmrmac4_config_struct_type, max_rl_pilot_tx_pwr_diff)}, /* 0xFFF4 */
    {FPOS(hdrscmrmac4_config_struct_type, t2p_no_tx_filter_tc)},      /* 0xFFF7 */
    {FPOS(hdrscmrmac4_config_struct_type, qrab_filter_tc)},           /* 0xFFF9 */
    {FPOS(hdrscmrmac4_config_struct_type, ps_filter_tc)},             /* 0xFFFA */
    {FPOS(hdrscmrmac4_config_struct_type, payload_thresh)},           /* 0xFFFB */
    {FPOS(hdrscmrmac4_config_struct_type, merge_thresh)},             /* 0xFFFC */
    {FPOS(hdrscmrmac4_config_struct_type, frab_low)},                 /* 0xFFFD */
    {FPOS(hdrscmrmac4_config_struct_type, frab_filter_tc)}            /* 0xFFFE */
  };

  static hdrscmrmac4_simple_ncx_attrib_offset_lut_type const
                ncx_lut[HDRSCMRMAC4_TOTAL_NUM_NCX_SIMPLE_ATTRIBS] =
  {
    {FPOS(hdrscmrmac4_config_struct_type, 
          reverse_channels[0].ps_qrab_thresh_drc_unlock),
     sizeof(hdrscmrmac4_reverse_channel_config_struct_type)},     /* 0xF600 */
    {FPOS(hdrscmrmac4_config_struct_type, 
          reverse_channels[0].ps_qrab_thresh_drc_lock),
     sizeof(hdrscmrmac4_reverse_channel_config_struct_type)},     /* 0xF700 */
    {FPOS(hdrscmrmac4_config_struct_type, 
          reverse_channels[0].arq_mode),
     sizeof(hdrscmrmac4_reverse_channel_config_struct_type)},     /* 0xF800 */
    {FPOS(hdrscmrmac4_config_struct_type, 
          flows[0].t2p_filt_tc),
     sizeof(hdrscmrmac4_flow_config_struct_type)},                /* 0xF900 */
    {FPOS(hdrscmrmac4_config_struct_type, 
          flows[0].burst_duration_factor),
     sizeof(hdrscmrmac4_flow_config_struct_type)},                /* 0xFA00 */
    {FPOS(hdrscmrmac4_config_struct_type, 
          flows[0].qrab_select),
     sizeof(hdrscmrmac4_flow_config_struct_type)},                /* 0xFB00 */
    {FPOS(hdrscmrmac4_config_struct_type, 
          flows[0].tx_mode),
     sizeof(hdrscmrmac4_flow_config_struct_type)},                /* 0xFC00 */
    {FPOS(hdrscmrmac4_config_struct_type, 
          flows[0].merge_thresh),
     sizeof(hdrscmrmac4_flow_config_struct_type)},                /* 0xFD00 */
    {FPOS(hdrscmrmac4_config_struct_type, 
          flows[0].bucket_level_max),
     sizeof(hdrscmrmac4_flow_config_struct_type)}                 /* 0xFE00 */
  };
#if defined(__ARMCC_VERSION) && (__ARMCC_VERSION > 210000)
#pragma diag_default 1296
#endif
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */ 

  /* Check if the simple attribute is a non-NCX simple attribute? */
  if (HDRSCMRMAC4_IS_SIMPLE_ATTRIB_NON_NCX(attrib_id))
  {
    err_no = hdrscmrmac4_get_index_from_non_ncx_attrib_id(
                attrib_id, &index, SIMPLE_ATTRIB);

    if (err_no == E_SUCCESS)
    {
      /* Found a match */
      *attrib_offset = non_ncx_lut[index].attrib_offset;
    }
  } /* Non-NCX */

  /* Per-NCX Simple Attribute */
  else
  {
    err_no = hdrscmrmac4_get_index_from_ncx_attrib_id (
                attrib_id, &index, SIMPLE_ATTRIB);

    if (err_no == E_SUCCESS)
    {
      /* Found a match */

      /* Find the per-NCX index */
      ncx_id = HDRSCMRMAC4_GET_NCX_ATTRIB_NCX_ID(attrib_id);

      *attrib_offset = (ncx_lut[index].attrib_offset +
                        (ncx_lut[index].element_size * ncx_id));
    }
  } /* Per-NCX */

  if (err_no == E_SUCCESS)
  {
    /* Each simple attribute is of size uint8 */
    *attrib_size = sizeof(uint8);
  }

  return err_no;

} /* hdrscmrmac4_get_simple_attrib_offset_and_size */


 /* EJECT */
/*===========================================================================

FUNCTION HDRSCMRMAC4_GET_COMPLEX_ATTRIB_OFFSET_AND_SIZE               LOCAL

DESCRIPTION
  This function computes the byte offset of the complex attribute specified by
  the attrib_id in the struct hdrscmrmac4_config_struct_type and stores it in 
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
static hdrerrno_enum_type hdrscmrmac4_get_complex_attrib_offset_and_size
(
  uint16 attrib_id, 
  uint16 *attrib_offset,
  uint16 *attrib_size
)
{
  hdrerrno_enum_type err_no = E_DATA_INVALID;
    /* Error code */

  uint16 index = 0;
    /* Complex attribute Index */

  uint16 attrib_id_base;
    /* NCX Attrib ID base */

  uint8 ncx_id;
    /* NCX Index */

  uint8 flow_id;
    /* flow index */

  uint8 channel_id;
    /* channel index */
#if defined(__ARMCC_VERSION) && (__ARMCC_VERSION > 210000)
#pragma diag_suppress 1296
#endif
  static hdrscmrmac4_complex_non_ncx_attrib_offset_size_lut_type const
                  non_ncx_lut[HDRSCMRMAC4_TOTAL_NUM_COMPLEX_ATTRIBS] =
  {
    {FPOS(hdrscmrmac4_config_struct_type,
     aux_pilot_info),
     sizeof(hdrscmrmaccmn_aux_pilot_struct_type)},          /* 0x0000 */
    {FPOS(hdrscmrmac4_config_struct_type,
     common_pow_params),
     sizeof(hdrscmrmaccmn_common_pow_params_struct_type)},  /* 0x0001 */
    {FPOS(hdrscmrmac4_config_struct_type,
     permitted_payload),
     sizeof(hdrscmrmaccmn_permitted_payload_type)},         /* 0x0003 */
    {FPOS(hdrscmrmac4_config_struct_type,
     power_params[HDRSCMRMACCMN_128_BITS][0]),
     (sizeof(hdrscmrmaccmn_power_params_struct_type) *
      (int) HDRSCMRMACCMN_NUM_TX_MODES)},                   /* 0x0005 */
    {FPOS(hdrscmrmac4_config_struct_type,
     power_params[HDRSCMRMACCMN_256_BITS][0]),
     (sizeof(hdrscmrmaccmn_power_params_struct_type) *
      (int) HDRSCMRMACCMN_NUM_TX_MODES)},                   /* 0x0006 */
    {FPOS(hdrscmrmac4_config_struct_type,
     power_params[HDRSCMRMACCMN_512_BITS][0]),
     (sizeof(hdrscmrmaccmn_power_params_struct_type) *
      (int) HDRSCMRMACCMN_NUM_TX_MODES)},                   /* 0x0007 */
    {FPOS(hdrscmrmac4_config_struct_type,
     power_params[HDRSCMRMACCMN_768_BITS][0]),
     (sizeof(hdrscmrmaccmn_power_params_struct_type) *
      (int) HDRSCMRMACCMN_NUM_TX_MODES)},                   /* 0x0008 */
    {FPOS(hdrscmrmac4_config_struct_type,
     power_params[HDRSCMRMACCMN_1024_BITS][0]),
     (sizeof(hdrscmrmaccmn_power_params_struct_type) *
      (int) HDRSCMRMACCMN_NUM_TX_MODES)},                   /* 0x0009 */
    {FPOS(hdrscmrmac4_config_struct_type,
     power_params[HDRSCMRMACCMN_1536_BITS][0]),
     (sizeof(hdrscmrmaccmn_power_params_struct_type) *
      (int) HDRSCMRMACCMN_NUM_TX_MODES)},                   /* 0x000A */
    {FPOS(hdrscmrmac4_config_struct_type,
     power_params[HDRSCMRMACCMN_2048_BITS][0]),
     (sizeof(hdrscmrmaccmn_power_params_struct_type) *
      (int) HDRSCMRMACCMN_NUM_TX_MODES)},                   /* 0x000B */
    {FPOS(hdrscmrmac4_config_struct_type,
     power_params[HDRSCMRMACCMN_3072_BITS][0]),
     (sizeof(hdrscmrmaccmn_power_params_struct_type) *
      (int) HDRSCMRMACCMN_NUM_TX_MODES)},                   /* 0x000C */
    {FPOS(hdrscmrmac4_config_struct_type,
     power_params[HDRSCMRMACCMN_4096_BITS][0]),
     (sizeof(hdrscmrmaccmn_power_params_struct_type) *
      (int) HDRSCMRMACCMN_NUM_TX_MODES)},                   /* 0x000D */
    {FPOS(hdrscmrmac4_config_struct_type,
     power_params[HDRSCMRMACCMN_6144_BITS][0]),
     (sizeof(hdrscmrmaccmn_power_params_struct_type) *
      (int) HDRSCMRMACCMN_NUM_TX_MODES)},                   /* 0x000E */
    {FPOS(hdrscmrmac4_config_struct_type,
     power_params[HDRSCMRMACCMN_8192_BITS][0]),
     (sizeof(hdrscmrmaccmn_power_params_struct_type) *
      (int) HDRSCMRMACCMN_NUM_TX_MODES)},                   /* 0x000F */
    {FPOS(hdrscmrmac4_config_struct_type,
     power_params[HDRSCMRMACCMN_12288_BITS][0]),
     (sizeof(hdrscmrmaccmn_power_params_struct_type) *
      (int) HDRSCMRMACCMN_NUM_TX_MODES)},                   /* 0x0010 */
    {FPOS(hdrscmrmac4_config_struct_type,
     req_params),
     sizeof(hdrscmrmaccmn_req_params_struct_type)},         /* 0x0011 */
    {FPOS(hdrscmrmac4_config_struct_type,
     rri_chan_power_params[0]),
     (sizeof(hdrscmrmaccmn_rri_power_params_struct_type) *
      HDRSCMRMACCMN_NUM_T2P_TRANS_POINTS)},                 /* 0x0012 */
    {FPOS(hdrscmrmac4_config_struct_type,
     max_mac_flows),
     sizeof(hdrscmrmaccmn_max_mac_flows_struct_type)}       /* 0x0014 */
  };

  static hdrscmrmac4_complex_ncx_attrib_offset_size_lut_type const
                  ncx_lut[HDRSCMRMAC4_TOTAL_NUM_NCX_COMPLEX_ATTRIBS] =
  {
    {FPOS(hdrscmrmac4_config_struct_type, flows[0].flow_mapping),
     sizeof(hdrscmrmaccmn_associated_flows_struct_type),
     sizeof(hdrscmrmac4_flow_config_struct_type)},            /* 0x0100 */
    {FPOS(hdrscmrmac4_config_struct_type, flows[0].bucket_factor),
     sizeof(hdrscmrmaccmn_t2p_frab_grid_struct_type),
     sizeof(hdrscmrmac4_flow_config_struct_type)},            /* 0x0300 */
    {FPOS(hdrscmrmac4_config_struct_type, configurations[0].t2p_inflow_range),
     sizeof(hdrscmrmaccmn_t2p_inflow_range_struct_type),
     sizeof(hdrscmrmac4_configuration_config_struct_type)},   /* 0x1500 */
    {FPOS(hdrscmrmac4_config_struct_type, configurations[0].gu_gd),
     sizeof(hdrscmrmaccmn_gu_gd_struct_type),
     sizeof(hdrscmrmac4_configuration_config_struct_type)},   /* 0x1600 */
    {FPOS(hdrscmrmac4_config_struct_type, associated_flow_config[0][0]),
     sizeof(hdrscmrmac4_associated_flow_config_struct_type),
     sizeof(hdrscmrmac4_associated_flow_config_struct_type)}, /* 0x1700, 
                                      NC Attribute, Handled as special case. */
    {FPOS(hdrscmrmac4_config_struct_type, reverse_channels[0].associated_at_config),
     sizeof(hdrscmrmac4_associated_at_config_struct_type),
     sizeof(hdrscmrmac4_reverse_channel_config_struct_type)}, /* 0x1800 */
    {FPOS(hdrscmrmac4_config_struct_type, flows[0].data_token_bucket),
     sizeof(hdrscmrmac4_data_token_bucket_struct_type),
     sizeof(hdrscmrmac4_flow_config_struct_type)},            /* 0x1900 */
    {FPOS(hdrscmrmac4_config_struct_type, reverse_channels[0].max_num_sub_pkts),
     sizeof(hdrscmrmaccmn_max_num_sub_pkts_struct_type),
     sizeof(hdrscmrmac4_reverse_channel_config_struct_type)}, /* 0x1A00 */
    {FPOS(hdrscmrmac4_config_struct_type, configurations[0].pilot_strength),
     sizeof(hdrscmrmaccmn_pilot_strength_grid_struct_type),
     sizeof(hdrscmrmac4_configuration_config_struct_type)},   /* 0x1B00 */
    {FPOS(hdrscmrmac4_config_struct_type, configurations[0].txt2p_max),
     sizeof(hdrscmrmaccmn_txt2p_max_grid_struct_type),
     sizeof(hdrscmrmac4_configuration_config_struct_type)}    /* 0x1C00 */
  };
#if defined(__ARMCC_VERSION) && (__ARMCC_VERSION > 210000)
#pragma diag_default 1296
#endif
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */ 

  /* Is it a non-NCX complex attrib? */
  if (HDRSCMRMAC4_IS_COMPLEX_ATTRIB_NON_NCX(attrib_id))
  {
    err_no = hdrscmrmac4_get_index_from_non_ncx_attrib_id (
                attrib_id, &index, COMPLX_ATTRIB);

    if (err_no == E_SUCCESS)
    {
      /* Found a match */

      *attrib_offset = non_ncx_lut[index].attrib_offset;
      *attrib_size = non_ncx_lut[index].attrib_size;
    }
  }
  else /* Per-NCX */
  {
    /* Extract the attrib ID base */
    attrib_id_base = HDRSCMRMAC4_GET_NCX_ATTRIB_BASE(attrib_id);

    /* First to see if it's a NC attrib */
    if (attrib_id_base == (uint16) HDRSCMRMAC4_ASSOCIATED_FLOW_CONFIG_NC_BASE)
    {
      /* Found a match */
      err_no = E_SUCCESS;

      /* Extract the channel and flow ids */
      flow_id = HDRSCMRMAC4_GET_NC_ATTRIB_FLOW_ID(attrib_id);
      channel_id = HDRSCMRMAC4_GET_NC_ATTRIB_CHAN_ID(attrib_id);

      *attrib_offset = FPOS(hdrscmrmac4_config_struct_type, 
                            associated_flow_config[flow_id][channel_id]);
      *attrib_size = sizeof(hdrscmrmac4_associated_flow_config_struct_type);
    }
    else
    {
      /* Handle other NN, CC, XX attributes through LUT */

      err_no = hdrscmrmac4_get_index_from_ncx_attrib_id (
                  attrib_id, &index, COMPLX_ATTRIB);

      if (err_no == E_SUCCESS)
      {
        /* Found a match */

        /* Find the per-NCX index */
        ncx_id = HDRSCMRMAC4_GET_NCX_ATTRIB_NCX_ID(attrib_id);

        *attrib_offset = (ncx_lut[index].attrib_offset +
                          (ncx_lut[index].element_size * ncx_id));

        *attrib_size = ncx_lut[index].attrib_size;
      }
    }
  }

  return err_no;

} /* hdrscmrmac4_get_complex_attrib_offset_and_size */


/* EJECT */
/*===========================================================================

FUNCTION HDRSCMRMAC4_PROP_MAX_MAC_FLOWS_MAX_PKT_SIZE                    LOCAL

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
static boolean hdrscmrmac4_prop_max_mac_flows_max_pkt_size( void )
{
  uint8 timer_id;
      /* Reserved timer Id */

  dsm_item_type * send_msg_ptr = NULL;
    /* Pointer to Config req. message to send */

  uint8 attrib_offset = 0;
    /* Attribute field offset from start of DSM item */

  boolean status = TRUE;
    /* Return code */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Reserve a Response timer for this Config Request message  */
  if ( (hdrscm_reserve_rsp_timer(HDRHAI_RTC_MAC_PROTOCOL, &timer_id, 
                           hdrscmrmaci.sub4.trans_id, FALSE)) != E_SUCCESS)
  {
    /* No free timer was available. */
    status = FALSE;
  }

  if (status)
  {
    HDR_MSG_PROT(MSG_LEGACY_HIGH, "AT-init Proposal of MaxMACFlows(0x0014)");

    hdrscmrmaci.sub4.max_mac_flows.max_num_mac_flows = HDRSCMRMACCMN_MAX_NUM_MAC_FLOWS;
    hdrscmrmaci.sub4.max_mac_flows.max_num_active_mac_flows = 
      HDRSCMRMACCMN_MAX_NUM_MAC_FLOWS;

    /* Get a new DSM buffer */
    send_msg_ptr = hdrutil_new_msg_buffer();

    /* Pack AttribUpdateRequest message's Message ID */
    HDRBIT_PACK8(send_msg_ptr, HDRSCM_CONFIG_REQ_MSG,
                 attrib_offset, HDRSCM_MESSAGE_ID_SIZE );

    /* Shift the offset to start of Transaction ID */
    attrib_offset += HDRSCM_MESSAGE_ID_SIZE;

    /* Pack the message's Transaction ID */
    HDRBIT_PACK8(send_msg_ptr, hdrscmrmaci.sub4.trans_id, attrib_offset, 
                 HDRSCM_TRANS_ID_SIZE);

    /******* MaxMACFlows ********/

    /* Shift the offset to start of Attribute Record's Length */
    attrib_offset += HDRSCM_TRANS_ID_SIZE;

    HDRBIT_PACK8(send_msg_ptr, HDRSCMRMACCMN_MAX_MAC_FLOWS_BYTE_LENGTH, 
                 attrib_offset, HDRSCM_LENGTH_SIZE);

    /* Shift the offset to start of AttributeID field */
    attrib_offset += HDRSCM_LENGTH_SIZE;

    HDRBIT_PACK16(send_msg_ptr, (uint16) HDRSCMRMAC4_MAX_MAC_FLOWS_ATTRIB, 
                  attrib_offset, HDRSCMRMACCMN_ATTRIB_ID_SIZE);

    /* Shift the offset to start of Value ID field */
    attrib_offset += HDRSCMRMACCMN_ATTRIB_ID_SIZE;

    /* Pack Value ID */
    hdrscmrmaci.sub4.max_mac_flow_value_id++;
    HDRBIT_PACK8(send_msg_ptr, hdrscmrmaci.sub4.max_mac_flow_value_id, attrib_offset, 
                 HDRSCMRMACCMN_VALUE_ID_SIZE);

    /* Shift the offset to start of MaxNumMACFlows field  */
    attrib_offset += HDRSCMRMACCMN_VALUE_ID_SIZE;

    /* Pack MaxNumMACFlows */
    HDRBIT_PACK8(send_msg_ptr, HDRSCMRMACCMN_MAX_NUM_MAC_FLOWS, attrib_offset, 
                 HDRSCMRMACCMN_MAX_NUM_MAC_FLOWS_SIZE);

    /* Shift the offset to start of MaxNumActiveMACFlows field */
    attrib_offset += HDRSCMRMACCMN_MAX_NUM_MAC_FLOWS_SIZE;

    /* Pack MaxNumActiveMACflows */
    HDRBIT_PACK8(send_msg_ptr, HDRSCMRMACCMN_MAX_NUM_MAC_FLOWS, attrib_offset, 
                 HDRSCMRMACCMN_MAX_NUM_ACTIVE_MAC_FLOWS_SIZE);

    attrib_offset += HDRSCMRMACCMN_MAX_NUM_ACTIVE_MAC_FLOWS_SIZE;

    /******* MaxPacketSize ********/

    HDRBIT_PACK8(send_msg_ptr, HDRSCMRMAC4_MAX_PKT_SIZE_BYTE_LENGTH, 
                 attrib_offset, HDRSCM_LENGTH_SIZE);

    /* Shift the offset to start of AttributeID field */
    attrib_offset += HDRSCM_LENGTH_SIZE;

    HDRBIT_PACK16(send_msg_ptr, (uint16) HDRSCMRMAC4_MAX_PACKET_SIZE_ATTRIB, 
                  attrib_offset, HDRSCMRMACCMN_ATTRIB_ID_SIZE);

    /* Shift the offset to start of Value ID field */
    attrib_offset += HDRSCMRMACCMN_ATTRIB_ID_SIZE;

    /* Pack Value ID */
    HDRBIT_PACK8(send_msg_ptr, 
                 HDRSCMRMAC4_MAX_PKT_SIZE_ATTRIB_VALUE, 
                 attrib_offset, 
                 HDRSCMRMACCMN_VALUE_ID_SIZE);

    /* Shift the offset to start of MaxNumMACFlows field  */
    attrib_offset += HDRSCMRMACCMN_VALUE_ID_SIZE;

    /* Send ConfigRequest msg on RTC. All config messages are reliable. */
    hdrhmp_send_msg(HDRHAI_RTC_MAC_PROTOCOL,
                    HDRHAI_RTC,
                    TRUE,
                    HDRSCM_CONFIG_REQ_MSG_PRI,
                    HDRHAI_IN_CONFIG,
                    send_msg_ptr,
                    hdrscm_tx_outcome_cb,
                    (void*) ((dword) timer_id) );

    HDR_MSG_PROT_2(MSG_LEGACY_HIGH, 
                   "AT Proposed. MaxNumMACFlows: %d, MaxNumActiveMACFlows: %d", 
                   HDRSCMRMACCMN_MAX_NUM_MAC_FLOWS,
                   HDRSCMRMACCMN_MAX_NUM_MAC_FLOWS);

    /* Increment the transaction ID for next transaction */ 
    ++hdrscmrmaci.sub4.trans_id;
  }

  return status;

} /* hdrscmrmac4_prop_max_mac_flows_max_pkt_size */


 /* EJECT */
/*============================================================================

                        EXTERNAL FUNCTION DEFINITIONS

=============================================================================*/

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
  E_SUCCESS      - If a valid complex attribute was parsed successfully.
  E_NO_DATA      - If the config request message does not have enough data.
  E_OUT_OF_RANGE - If the config request message does not have any valid data.
  E_DATA_INVALID - If the given attrib_id is not valid.
  E_NOT_SUPPORTED- If the given attrib_id should not be negotiated.

SIDE EFFECTS
  None.

===========================================================================*/
hdrerrno_enum_type hdrscmrmac4_process_complex_attrib
(
  dsm_item_type                               *item_ptr,
  hdrscm_proposed_attrib_type   const *const   p_aip,
  hdrscm_accepted_attrib_type         *const   a_aip
)
{
  hdrerrno_enum_type err_no = E_DATA_INVALID;
    /* Error code */

  uint16 attrib_id;
    /* Complex Attrib ID */

  uint16 index = 0;
    /* Complex attribute Index */

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */ 

  attrib_id = a_aip->attrib_id;

  /*---------------------------------------------------------------------------
    Each one of the complex attribute has one handler routine registered in 
    hdrscmrmac4_complex_non_ncx_attrib_handler and 
    hdrscmrmac4_complex_ncx_attrib_handler registration tables.
   --------------------------------------------------------------------------*/

  /* Is it a non-NCX complex attrib? */
  if (HDRSCMRMAC4_IS_COMPLEX_ATTRIB_NON_NCX(attrib_id))
  {
    /* Not NCX attrib */
    if (attrib_id == (uint16) HDRSCMRMAC4_MAX_MAC_FLOWS_ATTRIB)
    {
      /* 
       * Section 9.13.5.1
       * The access network shall not initiate negotiation of the MaxMACFlows
       * attribute.
       */
      err_no = E_NOT_SUPPORTED;
    }
    else
    {
      err_no = hdrscmrmac4_get_index_from_non_ncx_attrib_id(
                attrib_id, &index, COMPLX_ATTRIB);

      if (err_no == E_SUCCESS)
      {
        err_no = hdrscmrmac4_complex_non_ncx_attrib_handler[index]
                  (item_ptr, p_aip, a_aip);
      }
    }
  }
  else
  {
    /* NCX attrib */
    err_no = hdrscmrmac4_get_index_from_ncx_attrib_id(
              attrib_id, &index, COMPLX_ATTRIB);

    if (err_no == E_SUCCESS)
    {
      err_no = hdrscmrmac4_complex_ncx_attrib_handler[index]
                (item_ptr, p_aip, a_aip);
    }
  }

  return err_no;

} /* hdrscmrmac4_process_complex_attrib */


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
boolean hdrscmrmac4_is_attrib_simple
(
  uint16 const attrib_id
)
{

  return ((attrib_id & HDRSCMRMAC4_SIMPLE_ATTRIB_MSB_MASK) == 
            HDRSCMRMAC4_SIMPLE_ATTRIB_MSB )? TRUE : FALSE;

} /* hdrscmrmac4_is_attrib_simple */


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
  E_OUT_OF_RANGE     - If the attrib_value is not valid.
  E_DATA_INVALID     - If the attribute specified by attrib_id is not simple.
  E_NOT_SUPPORTED    - If the attrib_id should not be negotiated.

SIDE EFFECTS
  None.
  
===========================================================================*/
hdrerrno_enum_type hdrscmrmac4_is_simple_attrib_valid
(
  uint16 const attrib_id,
  uint32 const attrib_value 
)
{
  hdrerrno_enum_type err_no = E_SUCCESS;
    /* Error Code */

  uint16 index = 0;
    /* Index into the valid_range table */

  uint16 attrib_id_base;
    /* Attrib ID base for NCX attrib */

  uint8 ncx_id = 0;
    /* MAC flow or Channel ID */

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */ 

  /* Check if the attribute is simple */
  if (hdrscmrmac4_is_attrib_simple(attrib_id) == FALSE)
  {
    /* Not simple */
    err_no = E_DATA_INVALID;
  }
  /* Attribute is simple */
  else
  {
    /* Check if the simple attribute is a non-NCX simple attribute? */
    if (HDRSCMRMAC4_IS_SIMPLE_ATTRIB_NON_NCX(attrib_id))
    {

      if (attrib_id == (uint16) HDRSCMRMAC4_MAX_PACKET_SIZE_ATTRIB)
      {
        /* 
         * Section 9.13.5.1
         * The access network shall not initiate negotiation of the 
         * MaxPacketSize attribute.
         */
        err_no = E_NOT_SUPPORTED;
      }
      else
      {
        /* Get the simple attribute index from the attribute ID */
        if ( (err_no = 
                hdrscmrmac4_get_index_from_non_ncx_attrib_id
                  (attrib_id, &index, SIMPLE_ATTRIB))
              == E_SUCCESS )
        {
          /* Check the validity of the simple attribute */
          err_no = ( (attrib_value >= hdrscmrmac4_non_ncx_valid_range[index].min) &&
                     (attrib_value <= hdrscmrmac4_non_ncx_valid_range[index].max) )?
                     E_SUCCESS: E_OUT_OF_RANGE;

          /* Zero is valid for MaxRLPilotTxPwrDiff which falls outside the contiguous 
             range (0x08-0x1F) checked using LUT above. Handle as a special case. */
          if ( (attrib_id == HDRSCMRMAC4_MAX_RL_PILOT_TX_PWR_DIFF_ATTRIB) &&
               (err_no == E_OUT_OF_RANGE) &&
               (attrib_value == 0) )
          {
            err_no = E_SUCCESS;
          }
        }
      }

    } /* Non-Per-NCX */

    /* Per-NCX Simple Attribute */
    else
    {
      /* Get attrib base */
      attrib_id_base = HDRSCMRMAC4_GET_NCX_ATTRIB_BASE(attrib_id);

      /* Get the NCX ID */
      ncx_id = HDRSCMRMAC4_GET_NCX_ATTRIB_NCX_ID(attrib_id);

      /* Make sure that the NN and CC ID's are withing supported range */
      if ((attrib_id_base >= (uint16) HDRSCMRMAC4_FIRST_SIMPLE_NN_BASE) &&
          (attrib_id_base <= (uint16) HDRSCMRMAC4_LAST_SIMPLE_NN_BASE))
      {
        /* NN attrib */
        if (ncx_id >= HDRSCMRMACCMN_MAX_NUM_MAC_FLOWS)
        {
          /* NN greater than what AT supports */
          HDR_MSG_PROT_2(MSG_LEGACY_ERROR, 
                         "Attrib Id 0x%x for MAC flow %d not supported", 
                         attrib_id,
                         ncx_id);

          err_no = E_DATA_INVALID;
        }
      }
      else if ((attrib_id_base >= (uint16) HDRSCMRMAC4_FIRST_SIMPLE_CC_BASE) &&
               (attrib_id_base <= (uint16) HDRSCMRMAC4_LAST_SIMPLE_CC_BASE))
      {
        /* CC attrib */
        if (ncx_id >= HDRSCMRMAC4_MAX_NUM_REV_LOG_CHANS)
        {
          /* CC greater than what AT supports */
          HDR_MSG_PROT_2(MSG_LEGACY_ERROR,
                         "Attrib Id 0x%x for Rev. Ch. %d not supported",
                         attrib_id,
                         ncx_id);

          err_no = E_DATA_INVALID;
        }
      }
      else
      {
        /* Should never get here */
        err_no = E_DATA_INVALID;
      }

      if (err_no == E_SUCCESS)
      {
        /* The valid range for MergeThreshold Flow 0 is different from other flows
           Check that as a special case */
        if (attrib_id == (uint16) HDRSCMRMAC4_MERGE_THRESHOLD_NN_BASE)
        {
          /* Check the validity of NN simple attribute */
          err_no = ((attrib_value <= HDRSCMRMACCMN_INFINITE_MERGE_THRESHOLD_00)?
                    E_SUCCESS: E_OUT_OF_RANGE);
        }
        else
        {
          /* Get the simple attribute index from the attribute ID */
          if ( (err_no = hdrscmrmac4_get_index_from_ncx_attrib_id
                          (attrib_id, &index, SIMPLE_ATTRIB)) ==
               E_SUCCESS )
          {
            if (attrib_id == (uint16) HDRSCMRMAC4_BUCKET_LEVEL_MAX_NN_BASE)
            {
              /* BucketLevelMax for Flow 0 has to be greater than zero. */
              err_no = ( ((attrib_value >  hdrscmrmac4_ncx_valid_range[index].min) &&
                          (attrib_value <= hdrscmrmac4_ncx_valid_range[index].max) )? 
                         E_SUCCESS: E_OUT_OF_RANGE );
            }
            else
            {
              /* Check the validity of the simple attribute */
              err_no = ( ((attrib_value >= hdrscmrmac4_ncx_valid_range[index].min) &&
                          (attrib_value <= hdrscmrmac4_ncx_valid_range[index].max) )?
                           E_SUCCESS: E_OUT_OF_RANGE );
            }
          }
        }
      }
    }
  } /* Attribute is simple */

  if (err_no != E_SUCCESS)
  {
    HDR_MSG_PROT_2(MSG_LEGACY_ERROR, 
                   "Simple attrib %x proposed with invalid value %d", 
                   attrib_id,
                   attrib_value);
  }

  return err_no;

} /* hdrscmrmac4_is_simple_attrib_valid */


 /* EJECT */
/*===========================================================================

FUNCTION HDRSCMRMAC4_GET_ATTRIB_OFFSET_AND_SIZE

DESCRIPTION
  This function computes the byte offset of the attribute specified by the 
  attrib_id in the struct hdrscmrmac4_config_struct_type and stores it in 
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
hdrerrno_enum_type hdrscmrmac4_get_attrib_offset_and_size
(
  uint16 attrib_id, 
  uint16 *attrib_offset,
  uint16 *attrib_size
)
{

  hdrerrno_enum_type err_no = E_SUCCESS;
    /* Error code */

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */ 
  
  /* Check if the attribute is simple? */
  if (hdrscmrmac4_is_attrib_simple(attrib_id))
  {
    /* Get the offset and size of the simple attribute. */
    err_no = hdrscmrmac4_get_simple_attrib_offset_and_size(attrib_id,
                                                           attrib_offset,
                                                           attrib_size);
  }
  else
  {
    /* Get the offset and size of the complex attribute */
    err_no = hdrscmrmac4_get_complex_attrib_offset_and_size(attrib_id,
                                                            attrib_offset,
                                                            attrib_size);
  }

  return err_no;

} /* hdrscmrmac4_get_attrib_offset_and_size */


/* EJECT */
/*===========================================================================

FUNCTION HDRSCMRMAC4_GET_ATTRIB_VERNO_INDEX

DESCRIPTION
  Returns the index of the given attribute in the version number array.

DEPENDENCIES
  None

PARAMETERS
  attrib_id     - Attribute Id.
  pindex        - Address where the index is stored.
  
RETURN VALUE
  E_SUCCESS - If the given attribute ID is valid.
  E_OUT_OF_RANGE - If the given attribute ID is invalid.
  

SIDE EFFECTS
  None

===========================================================================*/
hdrerrno_enum_type hdrscmrmac4_get_attrib_verno_index
(
  uint16 attrib_id,
  uint16 *pindex
)
{
  hdrerrno_enum_type err_no = E_SUCCESS;
    /* Error Code */

  boolean is_simple = FALSE;
    /* Is attribute simple */

  uint16 attrib_base;
    /* The top 2-nibbles of the attrib ID */

  uint8 flow_id;
    /* Flow ID */

  uint8 channel_id;
    /* Reverse Logical Channel ID */

  uint8 config_id;
    /* Configuration ID */

  uint16  num_attribs_ahead=0;
    /* Number of attributes ahead of this one */

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */ 

  /* Check if attribute is simple? */
  is_simple = hdrscmrmac4_is_attrib_simple(attrib_id);

  /* Check if the attribute is non-NCX attribute */
  if ( (HDRSCMRMAC4_IS_SIMPLE_ATTRIB_NON_NCX(attrib_id)) ||
       (HDRSCMRMAC4_IS_COMPLEX_ATTRIB_NON_NCX(attrib_id)) )
  {
    /* No. Attribute is Not a NN, CC, XX, or NC!. Is it simple though? */
    if (is_simple)
    {
      switch(attrib_id)
      {
        case HDRSCMRMAC4_MAX_PACKET_SIZE_ATTRIB:
          /* MaxPacketSize, 0xFFF1 */
          num_attribs_ahead = 0;
          break;
  
        case HDRSCMRMAC4_SUPPORT_GAUP_MAX_NUM_SUB_PKTS_ATTRIB:
          /* SupportGAUPMaxNumSubPackets, 0xFFF2 */
          num_attribs_ahead = 1;
          break;

        case HDRSCMRMAC4_MAX_RL_PILOT_TX_PWR_DIFF_ATTRIB:
          /* MaxRLPilotTxPwrDiff 0xFFF4 */
          num_attribs_ahead = 2;
          break;

        case HDRSCMRMAC4_T2P_NO_TX_FILTER_TC_ATTRIB:
          /* T2PNoTxFilterTC, 0xFFF7 */
          num_attribs_ahead = 3;
          break;
  
        case HDRSCMRMAC4_QRAB_FILTER_TC_ATTRIB:
          /* QRABFilterTC, 0xFFF9 */
          num_attribs_ahead = 4;
          break;
  
        case HDRSCMRMAC4_PS_FILTER_TC_ATTRIB:
          /* PilotStrengthFilterTC, 0xFFFA */
          num_attribs_ahead = 5;
          break;
  
        case HDRSCMRMAC4_PAYLOAD_THRESH_ATTRIB:
          /* PayloadThresh, 0xFFFB */
          num_attribs_ahead = 6;
          break;
  
        case HDRSCMRMAC4_MERGE_THRESH_ATTRIB:
          /* MergeThresh, 0xFFFC */
          num_attribs_ahead = 7;
          break;
  
        case HDRSCMRMAC4_FRAB_LOW_ATTRIB:
          /* PilotStrengthFilterTC, 0xFFFD */
          num_attribs_ahead = 8;
          break;
  
        case HDRSCMRMAC4_FRAB_FILTER_TC_ATTRIB:
          /* PilotStrengthFilterTC, 0xFFFE */
          num_attribs_ahead = 9;
          break;
  
        default:
          /* Something unexpected happen */
          HDR_MSG_PROT_1(MSG_LEGACY_ERROR,
                         "Unrecognized simple attrib ID: %x",
                         attrib_id);
          err_no = E_OUT_OF_RANGE;
      }

      *pindex = HDRSCMRMAC4_NON_NCX_SIMPLE_ATTRIB_VERNO_INDEX(num_attribs_ahead);
    }
    else
    {
      /* Complex attribute */
      switch (attrib_id)
      {
        case HDRSCMRMAC4_AUX_PILOT_CHAN_PARAMS_ATTRIB:
          /* AuxiliaryPilotChannelParameters, 0x0000 */
          num_attribs_ahead = 0;
          break;
  
        case HDRSCMRMAC4_COMMON_PARAMS_ATTRIB:
          /* CommonPowerParameters, 0x0001 */
          num_attribs_ahead = 1;
          break;
  
        case HDRSCMRMAC4_PERMITTED_PAYLOAD_ATTRIB:
          /* PermittedPayload, 0x0003 */
          num_attribs_ahead = 2;
          break;
  
        case HDRSCMRMAC4_POWER_PARAMS_128_ATTRIB:
          /* PowerParameters128, 0x0005 */
          num_attribs_ahead = 3;
          break;
  
        case HDRSCMRMAC4_POWER_PARAMS_256_ATTRIB:
          /* PowerParameters256, 0x0006 */
          num_attribs_ahead = 4;
          break;
  
        case HDRSCMRMAC4_POWER_PARAMS_512_ATTRIB:
          /* PowerParameters512, 0x0007 */
          num_attribs_ahead = 5;
          break;
  
        case HDRSCMRMAC4_POWER_PARAMS_768_ATTRIB:
          /* PowerParameters768, 0x0008 */
          num_attribs_ahead = 6;
          break;
  
        case HDRSCMRMAC4_POWER_PARAMS_1024_ATTRIB:
          /* PowerParameters1024, 0x0009 */
          num_attribs_ahead = 7;
          break;
  
        case HDRSCMRMAC4_POWER_PARAMS_1536_ATTRIB:
          /* PowerParameters1536, 0x000A */
          num_attribs_ahead = 8;
          break;
  
        case HDRSCMRMAC4_POWER_PARAMS_2048_ATTRIB:
          /* PowerParameters2048, 0x000B */
          num_attribs_ahead = 9;
          break;
  
        case HDRSCMRMAC4_POWER_PARAMS_3072_ATTRIB:
          /* PowerParameters3072, 0x000C */
          num_attribs_ahead = 10;
          break;
  
        case HDRSCMRMAC4_POWER_PARAMS_4096_ATTRIB:
          /* PowerParameters4096, 0x000D */
          num_attribs_ahead = 11;
          break;
  
        case HDRSCMRMAC4_POWER_PARAMS_6144_ATTRIB:
          /* PowerParameters6144, 0x000E */
          num_attribs_ahead = 12;
          break;
  
        case HDRSCMRMAC4_POWER_PARAMS_8192_ATTRIB:
          /* PowerParameters8192, 0x000F */
          num_attribs_ahead = 13;
          break;
  
        case HDRSCMRMAC4_POWER_PARAMS_12288_ATTRIB:
          /* PowerParameters12288, 0x0010 */
          num_attribs_ahead = 14;
          break;
  
        case HDRSCMRMAC4_REQUEST_PARAMS_ATTRIB:
          /* RequestParameters, 0x0011 */
          num_attribs_ahead = 15;
          break;
  
        case HDRSCMRMAC4_RRI_CHAN_POWER_PARAMS_ATTRIB:
          /* RRIChannelPowerParameters, 0x0012 */
          num_attribs_ahead = 16;
          break;
  
        case HDRSCMRMAC4_MAX_MAC_FLOWS_ATTRIB:
          /* MaxMACFlows, 0x0014 */
          num_attribs_ahead = 17;
          break;
  
        default:
          /* Something unexpected happen */
          HDR_MSG_PROT_1(MSG_LEGACY_ERROR,
                         "Unrecognized complex attrib ID: %x",
                         attrib_id);
          err_no = E_OUT_OF_RANGE;
      }

      *pindex = HDRSCMRMAC4_NON_NCX_COMPLEX_ATTRIB_VERNO_INDEX(num_attribs_ahead);
    }
  }
  /* Per-Flow (or per-CC, per-XX, per-NC) attributes */
  else
  {
    attrib_base = HDRSCMRMAC4_GET_NCX_ATTRIB_BASE(attrib_id);

    /* Get the Flow (or channel, or config) ID from the attribute ID */
    flow_id = channel_id = config_id = HDRSCMRMAC4_GET_NCX_ATTRIB_NCX_ID(attrib_id);

    if (is_simple)
    {
      /* Simple per-NCX attribute */
      switch(attrib_base)
      {
        case HDRSCMRMAC4_T2P_FILTER_TC_NN_BASE:               
          /* T2PFilterTCNN, 0xF9NN */
          *pindex = HDRSCMRMAC4_NN_SIMPLE_ATTRIB_VERNO_INDEX(0, flow_id);
          break;
    
        case HDRSCMRMAC4_BURST_DURATION_FACT0R_NN_BASE:       
          /* BurstDurationFactorNN, 0xFANN */
          *pindex = HDRSCMRMAC4_NN_SIMPLE_ATTRIB_VERNO_INDEX(1, flow_id);
          break;
    
        case HDRSCMRMAC4_QRAB_SELECT_NN_BASE:                 
          /* QRABSelectNN, 0xFBNN */
          *pindex = HDRSCMRMAC4_NN_SIMPLE_ATTRIB_VERNO_INDEX(2, flow_id);
          break;
    
        case HDRSCMRMAC4_TRANSMISSION_MODE_NN_BASE:           
          /* TransmissionModeNN, 0xFCNN */
          *pindex = HDRSCMRMAC4_NN_SIMPLE_ATTRIB_VERNO_INDEX(3, flow_id);
          break;
    
        case HDRSCMRMAC4_MERGE_THRESHOLD_NN_BASE:             
          /* MergeThreshNN, 0xFDNN */
          *pindex = HDRSCMRMAC4_NN_SIMPLE_ATTRIB_VERNO_INDEX(4, flow_id);
          break;
    
        case HDRSCMRMAC4_BUCKET_LEVEL_MAX_NN_BASE:            
          /* BucketLevelMaxNN, 0xFENN */
          *pindex = HDRSCMRMAC4_NN_SIMPLE_ATTRIB_VERNO_INDEX(5, flow_id);
          break;
    
        case HDRSCMRMAC4_PS_QRAB_THRESH_DRC_UNLOCK_CC_BASE:   
          /* PilotStrengthQRABThresholdDRCUnlockCC, 0xF6CC */
          *pindex = HDRSCMRMAC4_CC_SIMPLE_ATTRIB_VERNO_INDEX(0, channel_id);
          break;
    
        case HDRSCMRMAC4_PS_QRAB_THRESH_DRC_LOCK_CC_BASE:     
          /* PilotStrengthQRABThresholdDRCLockCC, 0xF7CC */
          *pindex = HDRSCMRMAC4_CC_SIMPLE_ATTRIB_VERNO_INDEX(1, channel_id);
          break;
    
        case HDRSCMRMAC4_ARQ_MODE_CC_BASE:                    
          /* ARQModeCC, 0xF8CC */
          *pindex = HDRSCMRMAC4_CC_SIMPLE_ATTRIB_VERNO_INDEX(2, channel_id);
          break;
    
        default:
          /* Something unexpected happen */
          HDR_MSG_PROT_1(MSG_LEGACY_ERROR,
                         "Unrecognized simple NCX attrib ID: %x",
                         attrib_id);
          err_no = E_OUT_OF_RANGE;
      }
    }
    else
    {
      /* Complex per-NCX atteibute */
      switch(attrib_base)
      {
        case HDRSCMRMAC4_ASSOCIATED_FLOWS_NN_BASE:            
          /* AssociatedFlowsNN, 0x01NN */
          *pindex = HDRSCMRMAC4_NN_COMPLEX_ATTRIB_VERNO_INDEX(0, flow_id);
          break;

        case HDRSCMRMAC4_BUCKET_FACTOR_NN_BASE:               
          /* BucketFactorNN, 0x03NN */
          *pindex = HDRSCMRMAC4_NN_COMPLEX_ATTRIB_VERNO_INDEX(1, flow_id);
          break;

        case HDRSCMRMAC4_DATA_TOKEN_BUCKET_NN_BASE:           
          /* DataTokenBucketNN, 0x19NN */
          *pindex = HDRSCMRMAC4_NN_COMPLEX_ATTRIB_VERNO_INDEX(2, flow_id);
          break;

        case HDRSCMRMAC4_ASSOCIATED_AT_CONFIG_CC_BASE:        
          /* AssociatedATConfigurationsCC, 0x18CC */
          *pindex = HDRSCMRMAC4_CC_COMPLEX_ATTRIB_VERNO_INDEX(0, channel_id);
          break;

        case HDRSCMRMAC4_MAX_NUM_SUB_PACKETS_CC_BASE:         
          /* MaxNumSubPacketsCC, 0x1ACC */
          *pindex = HDRSCMRMAC4_CC_COMPLEX_ATTRIB_VERNO_INDEX(1, channel_id);
          break;

        case HDRSCMRMAC4_T2P_INFLOW_RANGE_CONFIG_XX_BASE:     
          /* T2PInflowRangeConfigXX, 0x15XX */
          *pindex = HDRSCMRMAC4_XX_COMPLEX_ATTRIB_VERNO_INDEX(0, config_id);
          break;

        case HDRSCMRMAC4_T2P_TRANSITION_FUNC_CONFIG_XX_BASE:  
          /* T2PTranistionFunctionConfigXX, 0x16XX */
          *pindex = HDRSCMRMAC4_XX_COMPLEX_ATTRIB_VERNO_INDEX(1, config_id);
          break;

        case HDRSCMRMAC4_PS_CONFIG_XX_BASE:                   
          /* PilotStrengthConfigXX, 0x1BXX */
          *pindex = HDRSCMRMAC4_XX_COMPLEX_ATTRIB_VERNO_INDEX(2, config_id);
          break;

        case HDRSCMRMAC4_TX_T2P_MAX_CONFIG_XX_BASE:           
          /* TxT2PmaxConfigXX, 0x1CXX */
          *pindex = HDRSCMRMAC4_XX_COMPLEX_ATTRIB_VERNO_INDEX(3, config_id);
          break;

        case HDRSCMRMAC4_ASSOCIATED_FLOW_CONFIG_NC_BASE:      
          /* AssociatedFlowConfigurationsNC, 0x17NC */
          flow_id     = HDRSCMRMAC4_GET_NC_ATTRIB_FLOW_ID(attrib_id);
          channel_id  = HDRSCMRMAC4_GET_NC_ATTRIB_CHAN_ID(attrib_id);
          *pindex = HDRSCMRMAC4_NC_COMPLEX_ATTRIB_VERNO_INDEX(flow_id, channel_id);
          break;

        default:
          /* Something unexpected happen */
          HDR_MSG_PROT_1(MSG_LEGACY_ERROR,
                         "Unrecognized complex NCX attrib ID: %x",
                         attrib_id);
          err_no = E_OUT_OF_RANGE;
      }
    }
  }

  return err_no;

} /* hdrscmrmac4_get_attrib_verno_index */


/* EJECT */
/*===========================================================================

FUNCTION HDRSCMRMAC4_SET_CONFIG_TO_DEFAULT

DESCRIPTION
  Set the ST-4 RMAC configuration attributes to its default values.

DEPENDENCIES
  None

PARAMETERS
  p_config - Pointer to hdrscmrmac3_config_struct_type
  
RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
void hdrscmrmac4_set_config_to_default
(
  hdrscmrmac4_config_struct_type *p_config
)
{

  hdrscmrmaccmn_payload_size_enum_type pl_ind;
    /* Payload index */

  uint8 sf_hist;
    /* Subframe history index */

  uint8 flow;
    /* Flow index */

  uint8 rev_channel;
    /* Reverse Logical Channel index, CC */

  uint8 configuration;
    /* configuration index, XX */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/*---------------------------------------------------------------------------
  Setting Non-per-flow simple configuration attributes to its default values. 
  The configuration attributes that has zero default values are not explicitly
  set. They are MaxPacketSize, SupportGAUPMaxNumSubPackets, and QRABFilterTC.
----------------------------------------------------------------------------*/
  p_config->max_rl_pilot_tx_pwr_diff = 
    HDRSCMRMAC4_DEFAULT_MAX_RL_PILOT_TX_PWR_DIFF;
  p_config->t2p_no_tx_filter_tc = HDRSCMRMACCMN_DEFAULT_T2P_NO_TX_FILTER_TC;
  p_config->ps_filter_tc = HDRSCMRMACCMN_DEFAULT_PILOT_STRENGTH_FILTER_TC;
  p_config->payload_thresh = HDRSCMRMACCMN_DEFAULT_PAYLOAD_THRESHOLD;
  p_config->merge_thresh = HDRSCMRMACCMN_DEFAULT_MERGE_THRESHOLD;
  p_config->frab_low = HDRSCMRMACCMN_DEFAULT_FRAB_LOW;
  p_config->frab_filter_tc = HDRSCMRMACCMN_DEFAULT_FRAB_FILTER_TC;

/*---------------------------------------------------------------------------
  Setting Per-flow simple configuration attributes to its default values. The 
  configuration attributes that has zero default values are not explicitly
  set. They are BurstDurationFactorNN, QRABSelectNN, TransmissionModeNN,
  MergeThreshold00, and BucketLevelMaxNN.
----------------------------------------------------------------------------*/
  p_config->flows[0].t2p_filt_tc = HDRSCMRMACCMN_DEFAULT_NN_T2P_FILTER_TC;
  p_config->flows[0].bucket_level_max = 
    HDRSCMRMACCMN_DEFAULT_00_BUCKET_LEVEL_MAX;
  p_config->flows[1].bucket_level_max = 
    HDRSCMRMACCMN_DEFAULT_01_BUCKET_LEVEL_MAX;

  for (flow = 0x01; flow < HDRSCMRMACCMN_MAX_NUM_MAC_FLOWS; ++flow)
  {
    p_config->flows[flow].t2p_filt_tc = 
      HDRSCMRMACCMN_DEFAULT_NN_T2P_FILTER_TC;
    p_config->flows[flow].merge_thresh = 
      HDRSCMRMACCMN_DEFAULT_NN_MERGE_THRESHOLD;
  } 

/*---------------------------------------------------------------------------
  Setting Non-Per-Flow Complex attributes to default values.
----------------------------------------------------------------------------*/
  /* Setting AuxiliaryPilotChannelParameters to default values */
  p_config->aux_pilot_info.gain_db = HDRSCMRMACCMN_DEFAULT_AUX_PILOT_CHAN_GAIN;
  p_config->aux_pilot_info.min_payload = 
    HDRSCMRMACCMN_DEFAULT_AUX_PILOT_CHAN_MIN_PAYLOAD;

  /* Setting CommonPowerParameters Attribute to default value. Default value of
     AllocationStagger is '0000'. So not explicitly set */
  p_config->common_pow_params.tx_t2p_min_db = HDRSCMRMACCMN_DEFAULT_CPP_TX_T2P_MIN;
  p_config->common_pow_params.rpc_step_db = HDRSCMRMACCMN_DEFAULT_CPP_RPC_STEP;

  /* Setting PermittedPayload (0, 128, 256, 512, 768) to default values */
  for (pl_ind = HDRSCMRMACCMN_FIRST_PAYLOAD_SIZE; 
        pl_ind < HDRSCMRMACCMN_1024_BITS; ++pl_ind)
  {
    for (sf_hist = 0; sf_hist < HDRSCMRMACCMN_MAX_NUM_SUBFRAME_HISTORY; ++sf_hist)
    {
      p_config->permitted_payload[pl_ind][sf_hist] = 
        HDRSCMRMACCMN_DEFAULT_PERMITTED_PAYLOAD_0_4;
    }
  }

  /* Setting PermittedPayload (1024, 1536) Attribute to default values */
  for (pl_ind = HDRSCMRMACCMN_1024_BITS; 
        pl_ind < HDRSCMRMACCMN_2048_BITS; ++pl_ind)
  {
    for (sf_hist = 0; sf_hist < HDRSCMRMACCMN_MAX_NUM_SUBFRAME_HISTORY; ++sf_hist)
    {
      p_config->permitted_payload[pl_ind][sf_hist] = 
        HDRSCMRMACCMN_DEFAULT_PERMITTED_PAYLOAD_5_6;
    }
  }

  /* Setting PermittedPayload (2048, 3072) Attribute to default values */
  for (pl_ind = HDRSCMRMACCMN_2048_BITS; 
        pl_ind < HDRSCMRMACCMN_4096_BITS; ++pl_ind)
  {
    for (sf_hist = 0; sf_hist < HDRSCMRMACCMN_MAX_NUM_SUBFRAME_HISTORY; ++sf_hist)
    {
      p_config->permitted_payload[pl_ind][sf_hist] = 
        HDRSCMRMACCMN_DEFAULT_PERMITTED_PAYLOAD_7_8;
    }
  }

  /* Setting PermittedPayload (4096, 6144) Attribute to default values */
  for (pl_ind = HDRSCMRMACCMN_4096_BITS; 
        pl_ind < HDRSCMRMACCMN_8192_BITS; ++pl_ind)
  {
    for (sf_hist = 0; sf_hist < HDRSCMRMACCMN_MAX_NUM_SUBFRAME_HISTORY; ++sf_hist)
    {
      p_config->permitted_payload[pl_ind][sf_hist] = 
        HDRSCMRMACCMN_DEFAULT_PERMITTED_PAYLOAD_9_A;
    }
  }

  /* Setting PermittedPayload (8192, 12288) Attribute to default values */
  for (pl_ind = HDRSCMRMACCMN_8192_BITS; 
        pl_ind < HDRSCMRMACCMN_MAX_NUM_PAYLOADS; ++pl_ind)
  {
    for (sf_hist = 0; sf_hist < HDRSCMRMACCMN_MAX_NUM_SUBFRAME_HISTORY; ++sf_hist)
    {
      p_config->permitted_payload[pl_ind][sf_hist] = 
        HDRSCMRMACCMN_DEFAULT_PERMITTED_PAYLOAD_B_C;
    }
  }

  /* Set LoLat/HiCap T2PTransition and TerminationTarget fields for 
     PowerParameters128-12288 Attributes */
  for (pl_ind = HDRSCMRMACCMN_FIRST_PAYLOAD_SIZE; 
        pl_ind < HDRSCMRMACCMN_MAX_NUM_PAYLOADS; ++pl_ind)
  {
    p_config->power_params[pl_ind][HDRSCMRMACCMN_TX_MODE_LO_LAT].t2p_transition = 
      HDRSCMRMACCMN_DEFAULT_PP_LL_T2P_TRANS;
    p_config->power_params
      [pl_ind][HDRSCMRMACCMN_TX_MODE_LO_LAT].termination_target = 
        HDRSCMRMACCMN_DEFAULT_PP_LL_TERM_TARGET;
    p_config->power_params
      [pl_ind][HDRSCMRMACCMN_TX_MODE_HI_CAP].t2p_transition =
        HDRSCMRMACCMN_DEFAULT_PP_HC_T2P_TRANS;
    p_config->power_params
      [pl_ind][HDRSCMRMACCMN_TX_MODE_HI_CAP].termination_target =
        HDRSCMRMACCMN_DEFAULT_PP_HC_TERM_TARGET;
  }

  /* Set LoLat/HiCap Pre/Post Transition fields of PowerParameters128 */
  p_config->power_params
    [HDRSCMRMACCMN_128_BITS][HDRSCMRMACCMN_TX_MODE_LO_LAT].pre_transition = 
      HDRSCMRMACCMN_DEFAULT_PP_LL_PRE_TRANS_128;
  p_config->power_params
    [HDRSCMRMACCMN_128_BITS][HDRSCMRMACCMN_TX_MODE_LO_LAT].post_transition =
      HDRSCMRMACCMN_DEFAULT_PP_LL_POST_TRANS_128;
  p_config->power_params
    [HDRSCMRMACCMN_128_BITS][HDRSCMRMACCMN_TX_MODE_HI_CAP].pre_transition =
      HDRSCMRMACCMN_DEFAULT_PP_HC_TRANS_128;
  p_config->power_params
    [HDRSCMRMACCMN_128_BITS][HDRSCMRMACCMN_TX_MODE_HI_CAP].post_transition =
      HDRSCMRMACCMN_DEFAULT_PP_HC_TRANS_128;

  /* Set LoLat/HiCap Pre/Post Transition fields of PowerParameters256 */
  p_config->power_params
    [HDRSCMRMACCMN_256_BITS][HDRSCMRMACCMN_TX_MODE_LO_LAT].pre_transition = 
      HDRSCMRMACCMN_DEFAULT_PP_LL_PRE_TRANS_256;
  p_config->power_params
    [HDRSCMRMACCMN_256_BITS][HDRSCMRMACCMN_TX_MODE_LO_LAT].post_transition =
      HDRSCMRMACCMN_DEFAULT_PP_LL_POST_TRANS_256;
  p_config->power_params
    [HDRSCMRMACCMN_256_BITS][HDRSCMRMACCMN_TX_MODE_HI_CAP].pre_transition =
      HDRSCMRMACCMN_DEFAULT_PP_HC_TRANS_256;
  p_config->power_params
    [HDRSCMRMACCMN_256_BITS][HDRSCMRMACCMN_TX_MODE_HI_CAP].post_transition =
      HDRSCMRMACCMN_DEFAULT_PP_HC_TRANS_256;

  /* Set LoLat/HiCap Pre/Post Transition fields of PowerParameters512 */
  p_config->power_params
    [HDRSCMRMACCMN_512_BITS][HDRSCMRMACCMN_TX_MODE_LO_LAT].pre_transition = 
      HDRSCMRMACCMN_DEFAULT_PP_LL_PRE_TRANS_512;
  p_config->power_params
    [HDRSCMRMACCMN_512_BITS][HDRSCMRMACCMN_TX_MODE_LO_LAT].post_transition =
      HDRSCMRMACCMN_DEFAULT_PP_LL_POST_TRANS_512;
  p_config->power_params
    [HDRSCMRMACCMN_512_BITS][HDRSCMRMACCMN_TX_MODE_HI_CAP].pre_transition =
      HDRSCMRMACCMN_DEFAULT_PP_HC_TRANS_512;
  p_config->power_params
    [HDRSCMRMACCMN_512_BITS][HDRSCMRMACCMN_TX_MODE_HI_CAP].post_transition =
      HDRSCMRMACCMN_DEFAULT_PP_HC_TRANS_512;

  /* Set LoLat/HiCap Pre/Post Transition fields of PowerParameters768 */
  p_config->power_params
    [HDRSCMRMACCMN_768_BITS][HDRSCMRMACCMN_TX_MODE_LO_LAT].pre_transition = 
      HDRSCMRMACCMN_DEFAULT_PP_LL_PRE_TRANS_768;
  p_config->power_params
    [HDRSCMRMACCMN_768_BITS][HDRSCMRMACCMN_TX_MODE_LO_LAT].post_transition =
      HDRSCMRMACCMN_DEFAULT_PP_LL_POST_TRANS_768;
  p_config->power_params
    [HDRSCMRMACCMN_768_BITS][HDRSCMRMACCMN_TX_MODE_HI_CAP].pre_transition =
      HDRSCMRMACCMN_DEFAULT_PP_HC_TRANS_768;
  p_config->power_params
    [HDRSCMRMACCMN_768_BITS][HDRSCMRMACCMN_TX_MODE_HI_CAP].post_transition =
      HDRSCMRMACCMN_DEFAULT_PP_HC_TRANS_768;

  /* Set LoLat/HiCap Pre/Post Transition fields of PowerParameters1024 */
  p_config->power_params
    [HDRSCMRMACCMN_1024_BITS][HDRSCMRMACCMN_TX_MODE_LO_LAT].pre_transition = 
      HDRSCMRMACCMN_DEFAULT_PP_LL_PRE_TRANS_1024;
  p_config->power_params
    [HDRSCMRMACCMN_1024_BITS][HDRSCMRMACCMN_TX_MODE_LO_LAT].post_transition =
      HDRSCMRMACCMN_DEFAULT_PP_LL_POST_TRANS_1024;
  p_config->power_params
    [HDRSCMRMACCMN_1024_BITS][HDRSCMRMACCMN_TX_MODE_HI_CAP].pre_transition =
      HDRSCMRMACCMN_DEFAULT_PP_HC_TRANS_1024;
  p_config->power_params
    [HDRSCMRMACCMN_1024_BITS][HDRSCMRMACCMN_TX_MODE_HI_CAP].post_transition =
      HDRSCMRMACCMN_DEFAULT_PP_HC_TRANS_1024;

  /* Set LoLat/HiCap Pre/Post Transition fields of PowerParameters1536 */
  p_config->power_params
    [HDRSCMRMACCMN_1536_BITS][HDRSCMRMACCMN_TX_MODE_LO_LAT].pre_transition = 
      HDRSCMRMACCMN_DEFAULT_PP_LL_PRE_TRANS_1536;
  p_config->power_params
    [HDRSCMRMACCMN_1536_BITS][HDRSCMRMACCMN_TX_MODE_LO_LAT].post_transition =
      HDRSCMRMACCMN_DEFAULT_PP_LL_POST_TRANS_1536;
  p_config->power_params
    [HDRSCMRMACCMN_1536_BITS][HDRSCMRMACCMN_TX_MODE_HI_CAP].pre_transition =
      HDRSCMRMACCMN_DEFAULT_PP_HC_TRANS_1536;
  p_config->power_params
    [HDRSCMRMACCMN_1536_BITS][HDRSCMRMACCMN_TX_MODE_HI_CAP].post_transition =
      HDRSCMRMACCMN_DEFAULT_PP_HC_TRANS_1536;

  /* Set LoLat/HiCap Pre/Post Transition fields of PowerParameters2048 */
  p_config->power_params
    [HDRSCMRMACCMN_2048_BITS][HDRSCMRMACCMN_TX_MODE_LO_LAT].pre_transition = 
      HDRSCMRMACCMN_DEFAULT_PP_LL_PRE_TRANS_2048;
  p_config->power_params
    [HDRSCMRMACCMN_2048_BITS][HDRSCMRMACCMN_TX_MODE_LO_LAT].post_transition =
      HDRSCMRMACCMN_DEFAULT_PP_LL_POST_TRANS_2048;
  p_config->power_params
    [HDRSCMRMACCMN_2048_BITS][HDRSCMRMACCMN_TX_MODE_HI_CAP].pre_transition =
      HDRSCMRMACCMN_DEFAULT_PP_HC_TRANS_2048;
  p_config->power_params
    [HDRSCMRMACCMN_2048_BITS][HDRSCMRMACCMN_TX_MODE_HI_CAP].post_transition =
      HDRSCMRMACCMN_DEFAULT_PP_HC_TRANS_2048;

  /* Set LoLat/HiCap Pre/Post Transition fields of PowerParameters3072 */
  p_config->power_params
    [HDRSCMRMACCMN_3072_BITS][HDRSCMRMACCMN_TX_MODE_LO_LAT].pre_transition = 
      HDRSCMRMACCMN_DEFAULT_PP_LL_PRE_TRANS_3072;
  p_config->power_params
    [HDRSCMRMACCMN_3072_BITS][HDRSCMRMACCMN_TX_MODE_LO_LAT].post_transition =
      HDRSCMRMACCMN_DEFAULT_PP_LL_POST_TRANS_3072;
  p_config->power_params
    [HDRSCMRMACCMN_3072_BITS][HDRSCMRMACCMN_TX_MODE_HI_CAP].pre_transition =
      HDRSCMRMACCMN_DEFAULT_PP_HC_TRANS_3072;
  p_config->power_params
    [HDRSCMRMACCMN_3072_BITS][HDRSCMRMACCMN_TX_MODE_HI_CAP].post_transition =
      HDRSCMRMACCMN_DEFAULT_PP_HC_TRANS_3072;

  /* Set LoLat/HiCap Pre/Post Transition fields of PowerParameters4096 */
  p_config->power_params
    [HDRSCMRMACCMN_4096_BITS][HDRSCMRMACCMN_TX_MODE_LO_LAT].pre_transition = 
      HDRSCMRMACCMN_DEFAULT_PP_LL_PRE_TRANS_4096;
  p_config->power_params
    [HDRSCMRMACCMN_4096_BITS][HDRSCMRMACCMN_TX_MODE_LO_LAT].post_transition =
      HDRSCMRMACCMN_DEFAULT_PP_LL_POST_TRANS_4096;
  p_config->power_params
    [HDRSCMRMACCMN_4096_BITS][HDRSCMRMACCMN_TX_MODE_HI_CAP].pre_transition =
      HDRSCMRMACCMN_DEFAULT_PP_HC_TRANS_4096;
  p_config->power_params
    [HDRSCMRMACCMN_4096_BITS][HDRSCMRMACCMN_TX_MODE_HI_CAP].post_transition =
      HDRSCMRMACCMN_DEFAULT_PP_HC_TRANS_4096;

  /* Set LoLat/HiCap Pre/Post Transition fields of PowerParameters6144 */
  p_config->power_params
    [HDRSCMRMACCMN_6144_BITS][HDRSCMRMACCMN_TX_MODE_LO_LAT].pre_transition = 
      HDRSCMRMACCMN_DEFAULT_PP_LL_PRE_TRANS_6144;
  p_config->power_params
    [HDRSCMRMACCMN_6144_BITS][HDRSCMRMACCMN_TX_MODE_LO_LAT].post_transition =
    HDRSCMRMACCMN_DEFAULT_PP_LL_POST_TRANS_6144;
  p_config->power_params
    [HDRSCMRMACCMN_6144_BITS][HDRSCMRMACCMN_TX_MODE_HI_CAP].pre_transition =
      HDRSCMRMACCMN_DEFAULT_PP_HC_TRANS_6144;
  p_config->power_params
    [HDRSCMRMACCMN_6144_BITS][HDRSCMRMACCMN_TX_MODE_HI_CAP].post_transition =
      HDRSCMRMACCMN_DEFAULT_PP_HC_TRANS_6144;

  /* Set LoLat/HiCap Pre/Post Transition fields of PowerParameters8192 */
  p_config->power_params
    [HDRSCMRMACCMN_8192_BITS][HDRSCMRMACCMN_TX_MODE_LO_LAT].pre_transition = 
      HDRSCMRMACCMN_DEFAULT_PP_LL_PRE_TRANS_8192;
  p_config->power_params
    [HDRSCMRMACCMN_8192_BITS][HDRSCMRMACCMN_TX_MODE_LO_LAT].post_transition =
      HDRSCMRMACCMN_DEFAULT_PP_LL_POST_TRANS_8192;
  p_config->power_params
    [HDRSCMRMACCMN_8192_BITS][HDRSCMRMACCMN_TX_MODE_HI_CAP].pre_transition =
      HDRSCMRMACCMN_DEFAULT_PP_HC_TRANS_8192;
  p_config->power_params
    [HDRSCMRMACCMN_8192_BITS][HDRSCMRMACCMN_TX_MODE_HI_CAP].post_transition =
      HDRSCMRMACCMN_DEFAULT_PP_HC_TRANS_8192;

  /* Set LoLat/HiCap Pre/Post Transition fields of PowerParameters12288 */
  p_config->power_params
    [HDRSCMRMACCMN_12288_BITS][HDRSCMRMACCMN_TX_MODE_LO_LAT].pre_transition = 
      HDRSCMRMACCMN_DEFAULT_PP_LL_PRE_TRANS_12288;
  p_config->power_params
    [HDRSCMRMACCMN_12288_BITS][HDRSCMRMACCMN_TX_MODE_LO_LAT].post_transition =
      HDRSCMRMACCMN_DEFAULT_PP_LL_POST_TRANS_12288;
  p_config->power_params
    [HDRSCMRMACCMN_12288_BITS][HDRSCMRMACCMN_TX_MODE_HI_CAP].pre_transition =
      HDRSCMRMACCMN_DEFAULT_PP_HC_TRANS_12288;
  p_config->power_params
    [HDRSCMRMACCMN_12288_BITS][HDRSCMRMACCMN_TX_MODE_HI_CAP].post_transition =
      HDRSCMRMACCMN_DEFAULT_PP_HC_TRANS_12288;

  /* RequestParameters Default values are zero. So not explicitly set */

  /* RRIChannelPowerParameters Attribute. PreTransition0 and PreTransition1 has
     zero default values and hence not explicitly set. */
  p_config->rri_chan_power_params[0].rri_chan_gain_post_transition =
    HDRSCMRMACCMN_DEFAULT_RRI_CHAN_GAIN_TRANS;
  p_config->rri_chan_power_params[1].rri_chan_gain_post_transition =
    HDRSCMRMACCMN_DEFAULT_RRI_CHAN_GAIN_TRANS;
  p_config->rri_chan_power_params[2].rri_chan_gain_pre_transition =
    HDRSCMRMACCMN_DEFAULT_RRI_CHAN_GAIN_TRANS;
  p_config->rri_chan_power_params[2].rri_chan_gain_post_transition =
    HDRSCMRMACCMN_DEFAULT_RRI_CHAN_GAIN_TRANS;
  p_config->rri_chan_power_params[3].rri_chan_gain_pre_transition =
    HDRSCMRMACCMN_DEFAULT_RRI_CHAN_GAIN_TRANS;

  /* MaxMACFlows */
  p_config->max_mac_flows.max_num_mac_flows = HDRSCMRMACCMN_DEFAULT_MAX_MAC_FLOWS;
  p_config->max_mac_flows.max_num_active_mac_flows = 
    HDRSCMRMACCMN_DEFAULT_MAX_MAC_FLOWS;

/*---------------------------------------------------------------------------
  Setting Per-Flow Complex attributes to default values.
----------------------------------------------------------------------------*/

  /* BucketFactor00 */
  p_config->flows[0].bucket_factor.t2p_axis.num_points =
    HDRSCMRMACCMN_DEFAULT_00_BF_NUM_T2P_AXIS_VALS;
  p_config->flows[0].bucket_factor.frab_axis.num_points = 
    HDRSCMRMACCMN_DEFAULT_00_BF_NUM_FRAB_AXIS_VALS;
  p_config->flows[0].bucket_factor.t2p_axis.values[0] =
    HDRSCMRMACCMN_DEFAULT_00_BF_T2P_AXIS_VALS_0;
  p_config->flows[0].bucket_factor.t2p_axis.values[1] = 
    HDRSCMRMACCMN_DEFAULT_00_BF_T2P_AXIS_VALS_1;
  p_config->flows[0].bucket_factor.grid[0][0] = 
    HDRSCMRMACCMN_DEFAULT_00_BUCKET_FACTOR_0_0;
  p_config->flows[0].bucket_factor.grid[1][0] = 
    HDRSCMRMACCMN_DEFAULT_00_BUCKET_FACTOR_1_0;

  /* DataTokenBucket00 */
  p_config->flows[0].data_token_bucket.data_token_inflow =
    HDRSCMRMAC4_DEFAULT_NN_DATA_TOKEN_BUCKET_DATA_TOKEN_INFLOW;
  p_config->flows[0].data_token_bucket.data_bucket_level_max =
    HDRSCMRMAC4_DEFAULT_NN_DATA_TOKEN_BUCKET_DATA_BUCKET_LEVEL_MAX;

  for (flow = 1; flow < HDRSCMRMACCMN_MAX_NUM_MAC_FLOWS; ++flow)
  {
    /* Set AssociatedFlows only for Flow 1 */
    if (flow == 1)
    {
      /* AssociatedFlowsNN SubStream has zero default value and hence not set 
         explicitly */
      p_config->flows[flow].flow_mapping.flow_count = 
        HDRSCMRMACCMN_DEFAULT_NN_ASSO_FLOWS_FLOW_COUNT;
      /* Flow count is 1. So just set the 0th higher layer flow's stream ID */
      p_config->flows[flow].flow_mapping.app_flows[0].stream_id = 
        HDRSCMRMACCMN_DEFAULT_NN_ASSO_FLOWS_STREAM;
    }

    /* BucketFactorNN */
    p_config->flows[flow].bucket_factor.t2p_axis.num_points = 
      HDRSCMRMACCMN_DEFAULT_NN_BF_NUM_T2P_AXIS_VALS;
    p_config->flows[flow].bucket_factor.frab_axis.num_points =
      HDRSCMRMACCMN_DEFAULT_NN_BF_NUM_FRAB_AXIS_VALS;
    p_config->flows[flow].bucket_factor.t2p_axis.values[0] =
      HDRSCMRMACCMN_DEFAULT_NN_BF_T2P_AXIS_VALS_0;
    p_config->flows[flow].bucket_factor.t2p_axis.values[1] =
      HDRSCMRMACCMN_DEFAULT_NN_BF_T2P_AXIS_VALS_1;
    p_config->flows[flow].bucket_factor.frab_axis.values[0] =
      HDRSCMRMACCMN_DEFAULT_NN_BF_FRAB_AXIS_VALS_0;
    p_config->flows[flow].bucket_factor.frab_axis.values[1] =
      HDRSCMRMACCMN_DEFAULT_NN_BF_FRAB_AXIS_VALS_1;
    p_config->flows[flow].bucket_factor.frab_axis.values[2] =
      HDRSCMRMACCMN_DEFAULT_NN_BF_FRAB_AXIS_VALS_2;
    p_config->flows[flow].bucket_factor.grid[0][0] = 
      HDRSCMRMACCMN_DEFAULT_NN_BUCKET_FACTOR_0_0;
    p_config->flows[flow].bucket_factor.grid[0][1] = 
      HDRSCMRMACCMN_DEFAULT_NN_BUCKET_FACTOR_0_1;
    p_config->flows[flow].bucket_factor.grid[0][2] = 
      HDRSCMRMACCMN_DEFAULT_NN_BUCKET_FACTOR_0_2;
    p_config->flows[flow].bucket_factor.grid[1][0] = 
      HDRSCMRMACCMN_DEFAULT_NN_BUCKET_FACTOR_1_Y;
    p_config->flows[flow].bucket_factor.grid[1][1] = 
      HDRSCMRMACCMN_DEFAULT_NN_BUCKET_FACTOR_1_Y;
    p_config->flows[flow].bucket_factor.grid[1][2] = 
      HDRSCMRMACCMN_DEFAULT_NN_BUCKET_FACTOR_1_Y;

    /* DataTokenBucketNN */
    p_config->flows[flow].data_token_bucket.data_token_inflow =
      HDRSCMRMAC4_DEFAULT_NN_DATA_TOKEN_BUCKET_DATA_TOKEN_INFLOW;
    p_config->flows[flow].data_token_bucket.data_bucket_level_max =
      HDRSCMRMAC4_DEFAULT_NN_DATA_TOKEN_BUCKET_DATA_BUCKET_LEVEL_MAX;
  } 

/*---------------------------------------------------------------------------
  Setting Per-Reverse Logical Channel Simple attributes to default values.
  ARQModeCC has zero default values are not explicitly set.
----------------------------------------------------------------------------*/
  for (rev_channel = 0; 
       rev_channel < HDRSCMRMAC4_MAX_NUM_REV_LOG_CHANS;
       ++rev_channel)
  {
    /* PilotStrengthQRABThresholdDRCUnlockCC */
    p_config->reverse_channels[rev_channel].ps_qrab_thresh_drc_unlock = 
      HDRSCMRMACCMN_DEFAULT_PS_QRAB_THRESH_DRC_UNLOCK;

    /* PilotStrengthQRABThresholdDRCLockCC */
    p_config->reverse_channels[rev_channel].ps_qrab_thresh_drc_lock = 
      HDRSCMRMACCMN_DEFAULT_PS_QRAB_THRESH_DRC_LOCK;
  }

/*---------------------------------------------------------------------------
  Setting Per-Reverse Logical Channel Complex attributes to default values.
  AssociatedATConfigurationsCC has zero default values are not explicitly set.
----------------------------------------------------------------------------*/
  for (rev_channel = 0; 
       rev_channel < HDRSCMRMAC4_MAX_NUM_REV_LOG_CHANS;
       ++rev_channel)
  {
    /* Setting MaxNumSubPackets Attribute to default values */
    p_config->reverse_channels[rev_channel].max_num_sub_pkts.interlace[0] =
      HDRSCMRMACCMN_DEFAULT_MAX_NUM_SUB_PACKETS;
    p_config->reverse_channels[rev_channel].max_num_sub_pkts.interlace[1] =
      HDRSCMRMACCMN_DEFAULT_MAX_NUM_SUB_PACKETS;
    p_config->reverse_channels[rev_channel].max_num_sub_pkts.interlace[2] =
      HDRSCMRMACCMN_DEFAULT_MAX_NUM_SUB_PACKETS;
  }

/*---------------------------------------------------------------------------
  Setting Per-Configuration Complex attributes to default values.
----------------------------------------------------------------------------*/
  /* T2PInflowRangeConfig00 */
  p_config->configurations[0].t2p_inflow_range.min =
    HDRSCMRMACCMN_DEFAULT_00_T2P_INFLOW_MIN;
  p_config->configurations[0].t2p_inflow_range.max =
    HDRSCMRMACCMN_DEFAULT_00_T2P_INFLOW_MAX;

  /* T2PTranistionFunctionConfig00 */
  p_config->configurations[0].gu_gd.t2p_axis.num_points = 
    HDRSCMRMACCMN_DEFAULT_00_GUGD_NUM_T2P_VALS;
  p_config->configurations[0].gu_gd.frab_axis.num_points =
    HDRSCMRMACCMN_DEFAULT_00_GUGD_NUM_FRAB_VALS;
  p_config->configurations[0].gu_gd.t2p_axis.values[0] =
    HDRSCMRMACCMN_DEFAULT_00_T2P_AXIS_0;
  p_config->configurations[0].gu_gd.t2p_axis.values[1] =
    HDRSCMRMACCMN_DEFAULT_00_T2P_AXIS_1;
  p_config->configurations[0].gu_gd.frab_axis.values[0] =
    HDRSCMRMACCMN_DEFAULT_00_FRAB_AXIS_0;
  p_config->configurations[0].gu_gd.up[0][0] = HDRSCMRMACCMN_DEFAULT_UP_0_0;
  p_config->configurations[0].gu_gd.up[1][0] = HDRSCMRMACCMN_DEFAULT_UP_1_0;
  p_config->configurations[0].gu_gd.down[0][0] = HDRSCMRMACCMN_DEFAULT_DOWN_X_Y;
  p_config->configurations[0].gu_gd.down[1][0] = HDRSCMRMACCMN_DEFAULT_DOWN_X_Y;
  
  /* PilotStrengthConfig00. ps_axis[1] and ps_ps_axis
     have default values of zero. So not explicitly set! */
  p_config->configurations[0].pilot_strength.num_ps_axis_values = 
    HDRSCMRMACCMN_DEFAULT_PS_NUM_PS_AXIS_VALS;
  p_config->configurations[0].pilot_strength.ps_axis[0] = HDRSCMRMACCMN_DEFAULT_PS_PS_AXIS_0;

  /* TxT2PmaxConfig00 Attribute */
  p_config->configurations[0].txt2p_max.num_ps_axis_values = 
    HDRSCMRMACCMN_DEFAULT_TXT2P_NUM_PS_AXIS_VALS;
  p_config->configurations[0].txt2p_max.ps_axis[0] = HDRSCMRMACCMN_DEFAULT_PS_AXIS_0;
  p_config->configurations[0].txt2p_max.ps_axis[1] = HDRSCMRMACCMN_DEFAULT_PS_AXIS_1;
  p_config->configurations[0].txt2p_max.txt2p_max_ps_axis[0] = HDRSCMRMACCMN_DEFAULT_TXT2P_PS_AXIS_0;
  p_config->configurations[0].txt2p_max.txt2p_max_ps_axis[1] = HDRSCMRMACCMN_DEFAULT_TXT2P_PS_AXIS_1; 

  for (configuration = 1;
       configuration < HDRSCMRMAC4_MAX_NUM_CONFIGS;
       ++configuration)
  {
    /* T2PInflowRangeConfigXX */
    p_config->configurations[configuration].t2p_inflow_range.min =
      HDRSCMRMACCMN_DEFAULT_XX_T2P_INFLOW_MIN;
    p_config->configurations[configuration].t2p_inflow_range.max =
      HDRSCMRMACCMN_DEFAULT_XX_T2P_INFLOW_MAX;

    /* T2PTranistionFunctionConfigXX */
    p_config->configurations[configuration].gu_gd.t2p_axis.num_points = 
      HDRSCMRMACCMN_DEFAULT_XX_GUGD_NUM_T2P_VALS;
    p_config->configurations[configuration].gu_gd.frab_axis.num_points =
      HDRSCMRMACCMN_DEFAULT_XX_GUGD_NUM_FRAB_VALS;
    p_config->configurations[configuration].gu_gd.t2p_axis.values[0] =
      HDRSCMRMACCMN_DEFAULT_XX_T2P_AXIS_0;
    p_config->configurations[configuration].gu_gd.t2p_axis.values[1] =
      HDRSCMRMACCMN_DEFAULT_XX_T2P_AXIS_1;
    p_config->configurations[configuration].gu_gd.t2p_axis.values[2] =
      HDRSCMRMACCMN_DEFAULT_XX_T2P_AXIS_2;
    p_config->configurations[configuration].gu_gd.t2p_axis.values[3] =
      HDRSCMRMACCMN_DEFAULT_XX_T2P_AXIS_3;
    p_config->configurations[configuration].gu_gd.frab_axis.values[0] =
      HDRSCMRMACCMN_DEFAULT_XX_FRAB_AXIS_0;
    p_config->configurations[configuration].gu_gd.frab_axis.values[1] =
      HDRSCMRMACCMN_DEFAULT_XX_FRAB_AXIS_1;
    p_config->configurations[configuration].gu_gd.frab_axis.values[2] =
      HDRSCMRMACCMN_DEFAULT_XX_FRAB_AXIS_2;
    p_config->configurations[configuration].gu_gd.frab_axis.values[3] =
      HDRSCMRMACCMN_DEFAULT_XX_FRAB_AXIS_3;
    p_config->configurations[configuration].gu_gd.up[0][0] =
      HDRSCMRMACCMN_DEFAULT_XX_UP_0_0;
    p_config->configurations[configuration].gu_gd.up[0][1] =
      HDRSCMRMACCMN_DEFAULT_XX_UP_0_1;
    p_config->configurations[configuration].gu_gd.up[0][2] =
      HDRSCMRMACCMN_DEFAULT_XX_UP_0_2;
    p_config->configurations[configuration].gu_gd.up[0][3] =
      HDRSCMRMACCMN_DEFAULT_XX_UP_0_3;
    p_config->configurations[configuration].gu_gd.up[1][0] = 
      HDRSCMRMACCMN_DEFAULT_XX_UP_1_0;
    p_config->configurations[configuration].gu_gd.up[1][1] = 
      HDRSCMRMACCMN_DEFAULT_XX_UP_1_1;
    p_config->configurations[configuration].gu_gd.up[1][2] = 
      HDRSCMRMACCMN_DEFAULT_XX_UP_1_2;
    p_config->configurations[configuration].gu_gd.up[1][3] = 
      HDRSCMRMACCMN_DEFAULT_XX_UP_1_3;
    p_config->configurations[configuration].gu_gd.up[2][0] = 
      HDRSCMRMACCMN_DEFAULT_XX_UP_2_0;
    p_config->configurations[configuration].gu_gd.up[2][1] = 
      HDRSCMRMACCMN_DEFAULT_XX_UP_2_1;
    p_config->configurations[configuration].gu_gd.up[2][2] = 
      HDRSCMRMACCMN_DEFAULT_XX_UP_2_2;
    p_config->configurations[configuration].gu_gd.up[2][3] = 
      HDRSCMRMACCMN_DEFAULT_XX_UP_2_3;
    p_config->configurations[configuration].gu_gd.up[3][0] = 
      HDRSCMRMACCMN_DEFAULT_XX_UP_3_0;
    p_config->configurations[configuration].gu_gd.up[3][1] = 
      HDRSCMRMACCMN_DEFAULT_XX_UP_3_1;
    p_config->configurations[configuration].gu_gd.up[3][2] = 
      HDRSCMRMACCMN_DEFAULT_XX_UP_3_2;
    p_config->configurations[configuration].gu_gd.up[3][3] = 
      HDRSCMRMACCMN_DEFAULT_XX_UP_3_3;
    p_config->configurations[configuration].gu_gd.down[0][0] = 
      HDRSCMRMACCMN_DEFAULT_XX_DOWN_0_0;
    p_config->configurations[configuration].gu_gd.down[0][1] = 
      HDRSCMRMACCMN_DEFAULT_XX_DOWN_0_1;
    p_config->configurations[configuration].gu_gd.down[0][2] = 
      HDRSCMRMACCMN_DEFAULT_XX_DOWN_0_2;
    p_config->configurations[configuration].gu_gd.down[0][3] = 
      HDRSCMRMACCMN_DEFAULT_XX_DOWN_0_3;
    p_config->configurations[configuration].gu_gd.down[1][0] = 
      HDRSCMRMACCMN_DEFAULT_XX_DOWN_1_0;
    p_config->configurations[configuration].gu_gd.down[1][1] = 
      HDRSCMRMACCMN_DEFAULT_XX_DOWN_1_1;
    p_config->configurations[configuration].gu_gd.down[1][2] = 
      HDRSCMRMACCMN_DEFAULT_XX_DOWN_1_2;
    p_config->configurations[configuration].gu_gd.down[1][3] = 
      HDRSCMRMACCMN_DEFAULT_XX_DOWN_1_3;
    p_config->configurations[configuration].gu_gd.down[2][0] = 
      HDRSCMRMACCMN_DEFAULT_XX_DOWN_2_0;
    p_config->configurations[configuration].gu_gd.down[2][1] = 
      HDRSCMRMACCMN_DEFAULT_XX_DOWN_2_1;
    p_config->configurations[configuration].gu_gd.down[2][2] = 
      HDRSCMRMACCMN_DEFAULT_XX_DOWN_2_2;
    p_config->configurations[configuration].gu_gd.down[2][3] = 
      HDRSCMRMACCMN_DEFAULT_XX_DOWN_2_3;
    p_config->configurations[configuration].gu_gd.down[3][0] = 
      HDRSCMRMACCMN_DEFAULT_XX_DOWN_3_0;
    p_config->configurations[configuration].gu_gd.down[3][1] = 
      HDRSCMRMACCMN_DEFAULT_XX_DOWN_3_1;
    p_config->configurations[configuration].gu_gd.down[3][2] = 
      HDRSCMRMACCMN_DEFAULT_XX_DOWN_3_2;
    p_config->configurations[configuration].gu_gd.down[3][3] = 
      HDRSCMRMACCMN_DEFAULT_XX_DOWN_3_3;

    /* PilotStrengthConfigXX. ps_axis[1] and ps_ps_axis
       have default values of zero. So not explicitly set! */
    p_config->configurations[configuration].pilot_strength.num_ps_axis_values = 
      HDRSCMRMACCMN_DEFAULT_PS_NUM_PS_AXIS_VALS;
    p_config->configurations[configuration].pilot_strength.ps_axis[0] = HDRSCMRMACCMN_DEFAULT_PS_PS_AXIS_0;

    /* TxT2PmaxConfigXX Attribute */
    p_config->configurations[configuration].txt2p_max.num_ps_axis_values = 
      HDRSCMRMACCMN_DEFAULT_TXT2P_NUM_PS_AXIS_VALS;
    p_config->configurations[configuration].txt2p_max.ps_axis[0] = HDRSCMRMACCMN_DEFAULT_PS_AXIS_0;
    p_config->configurations[configuration].txt2p_max.ps_axis[1] = HDRSCMRMACCMN_DEFAULT_PS_AXIS_1;
    p_config->configurations[configuration].txt2p_max.txt2p_max_ps_axis[0] = HDRSCMRMACCMN_DEFAULT_TXT2P_PS_AXIS_0;
    p_config->configurations[configuration].txt2p_max.txt2p_max_ps_axis[1] = HDRSCMRMACCMN_DEFAULT_TXT2P_PS_AXIS_1; 
  }

/*---------------------------------------------------------------------------
  Setting Per-Flow/Per-Reverse Logical Channel Complex attributes to default 
  values.  AssociatedFlowConfigurationsNC attributes have zero default values 
  for N=0 are not explicitly set. 
----------------------------------------------------------------------------*/
  for (flow = 1; flow < HDRSCMRMACCMN_MAX_NUM_MAC_FLOWS; ++flow)
  {
    for (rev_channel = 0; 
         rev_channel < HDRSCMRMAC4_MAX_NUM_REV_LOG_CHANS;
         ++rev_channel)
    {
      p_config->associated_flow_config[flow][rev_channel].t2p_inflow_range_select =
        HDRSCMRMAC4_DEFAULT_NN_T2P_INFLOW_RANGE_SEL;
      p_config->associated_flow_config[flow][rev_channel].t2p_transition_func_select =
        HDRSCMRMAC4_DEFAULT_NN_T2P_TRANSITION_FUNC_SEL;
    }
  }

} /* hdrscmrmac4_set_config_to_default */


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
hdrerrno_enum_type hdrscmrmac4_get_prop_attrib_value
(
  uint16 const trans_id,
  hdrscm_accepted_attrib_type *a_aip
)
{

  hdrerrno_enum_type err_no = E_FAILURE;
    /* Error Code */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Make sure Attribute Information pointer is not NULL */
  ASSERT(a_aip != NULL);

  if (trans_id == hdrscmrmaci.sub4.trans_id-1)
  {
    switch(a_aip->attrib_id)
    {
      case HDRSCMRMAC4_MAX_MAC_FLOWS_ATTRIB:
        a_aip->value_id = hdrscmrmaci.sub4.max_mac_flow_value_id;
        a_aip->complex_val_ptr = (void *) &(hdrscmrmaci.sub4.max_mac_flows);
        a_aip->complex_val_size = sizeof(hdrscmrmaci.sub4.max_mac_flows);
        a_aip->is_accepted = TRUE;
        err_no = E_SUCCESS;
        break;

      case HDRSCMRMAC4_MAX_PACKET_SIZE_ATTRIB:
        a_aip->value_id = HDRSCMRMAC4_MAX_PKT_SIZE_ATTRIB_VALUE;
        a_aip->complex_val_ptr = (void *)NULL;  /* simple attribute */
        a_aip->is_accepted = TRUE;
        err_no = E_SUCCESS;
        break;
  
      default:
        /* unsupported attrib_id */
        HDR_MSG_PROT_1(MSG_LEGACY_ERROR, 
            "RMAC4: Attrib id %d not supported", a_aip->attrib_id);
        break;
    }
  }
  else
  {
    HDR_MSG_PROT_2(MSG_LEGACY_ERROR, 
        "RMAC4: trans_id %d mismatched proposed trans_id %d",
         trans_id, (uint8)(hdrscmrmaci.sub4.trans_id-1));
  }
  return err_no;
} /* hdrscmrmac4_get_prop_attrib_value */


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
void hdrscmrmac4_fail_curr_config_neg
(
  hdrhai_session_close_reason_enum_type close_reason
)
{

  hdrind_ind_data_union_type ind_data;

  /* Allocate out of the heap because it is possible for this indication to
  be queued from a different task context. Otherwise, the payload may be
  corrupted which could cause a null pointer de-reference. */
  static hdrscp_scp_fail_ind_type session_fail_data;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* fill the indication data */
  session_fail_data.type = (uint8) HDRHAI_RTC_MAC_PROTOCOL;
  session_fail_data.subtype = HDRSCP_MC_RMAC;
  session_fail_data.attribute_id_length = HDRSCMRMACCMN_MAX_MAC_FLOWS_BYTE_LENGTH;
  session_fail_data.attribute_id = (uint16) HDRSCMRMAC4_MAX_MAC_FLOWS_ATTRIB;
  session_fail_data.reason = close_reason;

  ind_data.scp_fail_ptr = (void *)&session_fail_data;

  hdrind_give_ind(HDRIND_RMAC_PROT_NEG_FAILED,(void *)&ind_data);

} /* hdrscmrmac4_fail_curr_config_neg */


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
hdrerrno_enum_type hdrscmrmac4_is_attrib_gaupable
(
  uint16 const attrib_id
)
{
  uint8 gaup_ind = 0;
    /* Loop index */

  hdrerrno_enum_type err_no = E_NOT_ALLOWED;
    /* Error code */

  uint16 nn_attrib_base;
    /* Base of per-flow attribute */

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Check if the attribute is a non-NCX attribute? */
  if ( (HDRSCMRMAC4_IS_SIMPLE_ATTRIB_NON_NCX(attrib_id)) ||
       (HDRSCMRMAC4_IS_COMPLEX_ATTRIB_NON_NCX(attrib_id)) )
  {
    /* Loop around the non-NCX GAUPable attribute ID LUT to check if the given 
     attrib_id is one of them */
    for (gaup_ind = 0; (gaup_ind < HDRSCMRMAC4_NUM_GAUPABLE_NON_NCX_ATTRIBS) &&
                     (err_no != E_SUCCESS); ++gaup_ind)
    {
      if (attrib_id == hdrscmrmac4_gaup_non_ncx_attrib_ids[gaup_ind])
      {
        /* attrib_id is GAUPable */
        err_no = E_SUCCESS;
      }
    }
  }
  else
  /* Attribute is per-NCX */
  {
    /* Find the NCX-attribute base */
    nn_attrib_base = HDRSCMRMAC4_GET_NCX_ATTRIB_BASE(attrib_id);

    /* Loop around the NCX GAUPable attribute ID LUT to check if the given 
       attrib_id is one of them */
    for (gaup_ind = 0; (gaup_ind < HDRSCMRMAC4_NUM_GAUPABLE_NCX_ATTRIBS) &&
                       (err_no != E_SUCCESS); ++gaup_ind)
    {
      if (nn_attrib_base == hdrscmrmac4_gaup_ncx_attrib_ids[gaup_ind])
      {
        /* attrib_id is GAUPable */
        err_no = E_SUCCESS;
      }
    }
  }

  return err_no;

} /* hdrscmrmac4_is_attrib_gaupable */


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
)
{
  boolean known = FALSE;

/*- - - - - - - - - - - - -- - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Check if the attribute is a non-NN simple attribute */
  known = HDRSCMRMAC4_IS_ATTRIB_NON_NCX_SIMPLE(attrib_id);

  /* If the attribute is not known, it could either be a Non-NCX complex, 
     an NN, CC, XX, NC attribute or an Unknown attribute */
  if (!known)
  {
    /* Check if this is a non-NCX complex attribute */
    known = HDRSCMRMAC4_IS_ATTRIB_NON_NCX_COMPLEX(attrib_id);
  }

  /* If it's still unknown, check if attribute is per-NN */
  if (!known)
  {
    known = (HDRSCMRMAC4_IS_ATTRIB_NN_SIMPLE(attrib_id) ||
             HDRSCMRMAC4_IS_ATTRIB_NN_COMPLEX(attrib_id));

    if (known)
    {
      /* The attribute base is per NN, check the ID */
      if (HDRSCMRMAC4_GET_NCX_ATTRIB_NCX_ID(attrib_id) >=
          HDRSCMRMACCMN_MAX_NUM_MAC_FLOWS)
      {
        known = FALSE;
      }
    }
    else
    {
      /* If it's still unknown, check if attribute is per-CC */
      known = (HDRSCMRMAC4_IS_ATTRIB_CC_SIMPLE(attrib_id) ||
               HDRSCMRMAC4_IS_ATTRIB_CC_COMPLEX(attrib_id));

      if (known)
      {
        /* The attribute base is per CC, check the ID */
        if (HDRSCMRMAC4_GET_NCX_ATTRIB_NCX_ID(attrib_id) >=
            HDRSCMRMAC4_MAX_NUM_REV_LOG_CHANS)
        {
          known = FALSE;
        }
      }
      else
      {
        /* If it's still unknown, check if attribute is per-XX */
        known = HDRSCMRMAC4_IS_ATTRIB_XX_COMPLEX(attrib_id);

        if (known)
        {
          /* The attribute base is per XX, check the ID */
          if(HDRSCMRMAC4_GET_NCX_ATTRIB_NCX_ID(attrib_id) >=
             HDRSCMRMAC4_MAX_NUM_CONFIGS)
          {
            known = FALSE;
          }
        }
        else
        {
          /* Is this the only NC attribute?*/
          if (HDRSCMRMAC4_GET_NCX_ATTRIB_BASE(attrib_id) ==
              HDRSCMRMAC4_ASSOCIATED_FLOW_CONFIG_NC_BASE)
          {
            /* The macro HDRSCMRMAC4_GET_NC_ATTRIB_CHAN_ID returns a value with
               nibble-range (0-15) which will always be less than 
               HDRSCMRMAC4_MAX_NUM_REV_LOG_CHANS (16). Commenting out the redundant
               check to avoid lint warning */
            /* if (HDRSCMRMAC4_GET_NC_ATTRIB_CHAN_ID(attrib_id) <
                (HDRSCMRMAC4_MAX_NUM_REV_LOG_CHANS)) */
            {
              if (HDRSCMRMAC4_GET_NC_ATTRIB_FLOW_ID(attrib_id) <
                  HDRSCMRMACCMN_MAX_NUM_MAC_FLOWS)
              {
                known = TRUE;
              }
            }
          }
        }
      }
    }
  }

  return known;

} /* hdrscmrmac4_attrib_is_supported */


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
boolean hdrscmrmac4_config_is_needed( void )
{
  return ( hdrscm_config_is_needed(HDRHAI_RTC_MAC_PROTOCOL) );
} /* hdrscmrmac4_config_is_needed */

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
boolean hdrscmrmac4_configure( void )
{

  return hdrscmrmac4_prop_max_mac_flows_max_pkt_size();

} /* hdrscmrmac4_configure */

#endif /* FEATURE_HDR_REVB_SUPPORTED */
