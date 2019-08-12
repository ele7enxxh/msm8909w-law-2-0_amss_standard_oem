/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

      H D R   S E S S I O N   C O N F I G U R A T I O N   M A N A G E R

          F O R W A R D  C H A N N E L  M A C  P R O T O C O L

       
GENERAL DESCRIPTION
  This module provides supporting API for SCM module to process
  ConfigurationRequest messages and AttributeUpdateRequest messages directed
  to Forward Traffic channel MAC protocol. 
    
EXTERNALIZED FUNCTIONS
  hdrscmfmac_process_complex_attrib 
    Processes all FMAC complex attributes 
    
  hdrscmfmac_is_attrib_simple 
    Returns true if an attribute is simple

  hdrscmfmac_is_simple_attrib_valid
    Verifies the validity of the simple attribute

  hdrscmfmac_get_attrib_offset_and_size
    Returns attribute size and offset into the config structure.

  hdrscmfmac_get_attrib_verno_index
    Returns the index of an attribute in the version number array. 
   
INITIALIZATION AND SEQUENCING REQUIREMENTS

  
  Copyright (c) 2005-2012 by Qualcomm Technologies, Inc.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                           EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/hdr/cp/src/hdrscmfmac.c#1 $ $DateTime: 2016/12/13 07:58:05 $ $Author: mplcsds1 $
   
when       who     what, where, why
--------   ---     ----------------------------------------------------------
12/20/12   vko     Fixed compiler warnings 
03/08/12   wsh     Merged NLB support
07/21/10   cnx     Added support for GAUP LoadInformationSupported attribute
06/07/10   wsh     Added support for Load Info Msg 
11/20/09   etv     Propose 0x7 for ShortPktEnabledThresh if
                   FEATURE_HDR_REVB_ENH_RATES is enabled
09/28/09   etv     Fixed Klocwork errors.
07/23/08   etv     Reject non-default attributes for MUPPreambleBase, 
                   SupportGAUPMUPPreambleBase, and SupportDeltaAckChanGainMUP.
                   Fixed bug due to which DRCGating was getting rejected.
06/11/08   etv     Added support for MaxOptionalPayloadSize and DataRate.
05/29/08   etv     Fixed TentativeDRCtoTxDRCMap validation. Added workaround
                   for standards bug.
05/22/08   pba     Klockwork fixes.
09/12/07   cc      Fixed Rev. A attribute offset calculation issue caused by Rev. B
                   addition.
08/22/07   etv     Fixed high lint warnings.
07/31/07   etv     Removed pragma and commented the code instead.
07/31/07   etv     Added pragma to suppress RVCT Compiler warning.
07/26/07   cc      Fixed bug in hdrscmfmac_get_prop_attrib_value.
06/20/07   cc      Added support for DOrB FTC MAC.
01/03/07   etv     Added support to handle unknown attributes.
08/16/06   etv     Change interface get_simple_attr_size to take attrib_id.
07/06/06   etv     Fixed L1 featurization issue for 7500.
06/08/06   etv     Send session close if AN GAUPs a non-GAUPable attribute.
05/16/06   etv     Accept non-default value for MinContinuationSpan attrib.
05/05/06   etv     Removed unused global variable.
05/01/06   etv     Accept default only for SingleUserMuxPktenabled attrib.
04/18/05   etv     Accept default only for MinContinuationSpan attrib.
03/14/06   etv     Fixed Valid range for ShortPktEnabledThresh attrib.
12/07/05   etv     Added function to get the simple attrib value size.
11/21/05   etv     Added header tags for perforce to fill in
11/02/05   etv     Changed SCM design to propogate subtype info.
10/05/05   etv     Replaced union of complex attribs with a separate instance
                   for each complex attribute.
09/09/05   etv     Added function to set default values.
09/06/05   etv     Made style corrections based on code review comments. 
08/22/05   etv     Created module.

===========================================================================*/

/*===========================================================================

                          INCLUDE FILES FOR MODULE
                     
===========================================================================*/
#include "hdr_variation.h"
#include "dsm.h"
#include "comdef.h"
#include "err.h"

#include "hdrdebug.h"
#include "hdrdsm.h"
#include "hdrscp.h"
#include "hdrhai.h"
#include "hdrtrace.h"

#ifdef FEATURE_HDR_REVC
#include "hdrfmac.h"
#endif /* FEATURE_HDR_REVC */
#include "hdrfmaci.h"

#include "hdrscmfmac.h"
#include "hdrscmdb.h"
#include "hdrscm.h"
#include "hdrutil.h"
#include "hdrhmp.h"

#include "hdrmc_v.h"

/*===========================================================================

             DEFINITIONS  FROM  THE  HDR  AIR  INTERFACE  SPEC

===========================================================================*/
#define HDRSCMFMAC_ATTRIB_ID_SIZE                       8
/* Size of the value ID for default FMAC */
#define HDRSCMFMAC_DEFAULT_FMAC_VALUE_ID_SIZE           16
/* Size of the value ID for Enhanced FMAC */
#define HDRSCMFMAC_ENH_FMAC_VALUE_ID_SIZE               8

/* Soft handoff delay attrbute size in bits */
#define HDRSCMFMAC_SOFT_HO_DELAY_ATTRIB_SIZE            8
/* Softer handoff delay attribute size in bits */
#define HDRSCMFMAC_SOFTER_HO_DELAY_ATTRIB_SIZE          8
/* DRC Lock period field attibute size in bits */
#define HDRSCMFMAC_DRC_LOCK_PERIOD_ATTRIB_SIZE          1
/* DRC Lock length field attribute size in bits */
#define HDRSCMFMAC_DRC_LOCK_LENGTH_ATTRIB_SIZE          2
/* DRC Lock Reserved field attribute size in bits */
#define HDRSCMFMAC_DRC_LOCK_RSVD_FIELD_SIZE             5
/* DRC Translation offset attribute size in bits */
#define HDRSCMFMAC_DRC_TRANS_OFFSET_ATTRIB_SIZE         45

/* Handoff Delays attribute size in bits */
#define HDRSCMFMAC_HO_DELAYS_ATTRIB_SIZE()      \
  ( HDRSCMFMAC_SOFTER_HO_DELAY_ATTRIB_SIZE +    \
    HDRSCMFMAC_SOFT_HO_DELAY_ATTRIB_SIZE )

/* DRC Lock attribute size in bits */
#define HDRSCMFMAC_DRC_LOCK_ATTRIB_SIZE()       \
  ( HDRSCMFMAC_DRC_LOCK_PERIOD_ATTRIB_SIZE +    \
    HDRSCMFMAC_DRC_LOCK_LENGTH_ATTRIB_SIZE +    \
    HDRSCMFMAC_DRC_LOCK_RSVD_FIELD_SIZE )

#ifdef FEATURE_HDR_REVC
/* LoadInfoSupported */
#define HDRSCMFMAC_LOAD_INFO_UNSUPPORTED                0
#define HDRSCMFMAC_LOAD_INFO_SUPPORTED                  1

/* Message priority for GAUP request message */
#define HDRSCMFMAC_ATTRIB_UPDATE_REQ_MSG_PRIO           40
#endif /* FEATURE_HDR_REVC */

#ifdef FEATURE_HDR_REVB

/* ATSupportedPacketFormats attribute sizes in bits */
#define HDRSCMFMAC_ASPF_ATTRIB_SIZE                     12
#define HDRSCMFMAC_ASPF_MAX_OPTIONAL_DATA_RATE_SIZE     4
#define HDRSCMFMAC_ASPF_MAX_OPTIONAL_PS_SIZE            4
#define HDRSCMFMAC_ASPF_SHORT_PKT_ENABLED_THRESH_SIZE   4

#define HDRSCMFMAC_ASPF_MIN_MAX_OPTIONAL_DATA_RATE      0x0
#define HDRSCMFMAC_ASPF_MAX_MAX_OPTIONAL_DATA_RATE      0x04

#define HDRSCMFMAC_ASPF_MIN_MAX_OPTIONAL_PS             0x04
#define HDRSCMFMAC_ASPF_MAX_MAX_OPTIONAL_PS             0x07

#define HDRSCMFMAC_ASPF_MIN_SHORT_PKTS_ENA_THRESH       0x0
#define HDRSCMFMAC_ASPF_MAX_SHORT_PKTS_ENA_THRESH       0x07

/* TentativeDRCtoTxDRCMap attribute sizes in bits */
#define HDRSCMFMAC_TDTTDM_ATTRIB_SIZE                   130
#define HDRSCMFMAC_TDTTDM_TX_DRC_MAP_SIZE               5

/* ExtendedSpans attribute sizes in bits */
#define HDRSCMFMAC_MIN_ES_ATTRIB_SIZE                   1
#define HDRSCMFMAC_ES_ENABLED_SIZE                      1
#define HDRSCMFMAC_ES_PER_TARGET_SPAN                   1

#endif /* FEATURE_HDR_REVB */

/*------------------------------------------------------------------
              Default Values for Configuration Attributes
-------------------------------------------------------------------*/
/* Default Subtype */
#define HDRSCMFMAC_DEFAULT_DRC_GATING                     0x00
#define HDRSCMFMAC_DEFAULT_NULL_RATE_DRC_384_ENABLED      0x00
#define HDRSCMFMAC_DEFAULT_DRC_LOCK_PERIOD                0x01
#define HDRSCMFMAC_DEFAULT_DRC_LOCK_LENGTH                0x01
#define HDRSCMFMAC_DEFAULT_SOFTER_HANDOFF_DELAYS          0x01
#define HDRSCMFMAC_DEFAULT_SOFT_HANDOFF_DELAYS            0x10

#ifdef FEATURE_HDR_REVA_L1
/* Enhanced Subtype */
#define HDRSCMFMAC_DEFAULT_ST1_DRC_LOCK_LENGTH            0x01
#define HDRSCMFMAC_DEFAULT_MUP_ENABLED                    0x00
#define HDRSCMFMAC_DEFAULT_DSC_LENGTH                     0x08
#define HDRSCMFMAC_DEFAULT_DELTA_ACK_CHAN_GAIN_MUP        0x0C
#define HDRSCMFMAC_DEFAULT_SHORT_PKT_ENABLED_THRESH       0x01
#define HDRSCMFMAC_DEFAULT_SU_MUX_PKTS_ENABLED            0x00
#define HDRSCMFMAC_DEFAULT_DRC_SUPERVISION_TIMER          0x00
#define HDRSCMFMAC_DEFAULT_MIN_CONTN_SPAN                 0x04
#define HDRSCMFMAC_DEFAULT_DSC_CHAN_GAIN_BOOST            0x00
#define HDRSCMFMAC_DEFAULT_DSC_BOOST_LENGTH               0x10
#define HDRSCMFMAC_DEFAULT_DRC_CHAN_GAIN_BOOST            0x00
#define HDRSCMFMAC_DEFAULT_DRC_BOOST_LENGTH               0x04
#define HDRSCMFMAC_DEFAULT_DRC_TRANS_OFFSET               0x00

#endif /* FEATURE_HDR_REVA_L1 */

#ifdef FEATURE_HDR_REVB
#define HDRSCMFMAC_DEFAULT_SUPPORT_GAUP_MUP_PREAMBLE_BASE           0x00
#define HDRSCMFMAC_DEFAULT_MUP_PREAMBLE_BASE                        0x42
#define HDRSCMFMAC_DEFAULT_SUPPORT_GAUP_DELTA_ACK_CHANNEL_GAIN_MUP  0x00
#define HDRSCMFMAC_DEFAULT_EXTENDED_SPANS_ENABLED                   0x00

/* TentativeDRCtoTxDRCMap Default Values */
#define HDRSCMFMAC_DEFAULT_TX_DRC_MAP_01                  0x01
#define HDRSCMFMAC_DEFAULT_TX_DRC_MAP_02                  0x02
#define HDRSCMFMAC_DEFAULT_TX_DRC_MAP_03                  0x03
#define HDRSCMFMAC_DEFAULT_TX_DRC_MAP_04                  0x04
#define HDRSCMFMAC_DEFAULT_TX_DRC_MAP_05                  0x05
#define HDRSCMFMAC_DEFAULT_TX_DRC_MAP_06                  0x06
#define HDRSCMFMAC_DEFAULT_TX_DRC_MAP_07                  0x07
#define HDRSCMFMAC_DEFAULT_TX_DRC_MAP_08                  0x08
#define HDRSCMFMAC_DEFAULT_TX_DRC_MAP_09                  0x09
#define HDRSCMFMAC_DEFAULT_TX_DRC_MAP_0A                  0x0a
#define HDRSCMFMAC_DEFAULT_TX_DRC_MAP_0B                  0x0b
#define HDRSCMFMAC_DEFAULT_TX_DRC_MAP_0C                  0x0c
#define HDRSCMFMAC_DEFAULT_TX_DRC_MAP_0D                  0x0d
#define HDRSCMFMAC_DEFAULT_TX_DRC_MAP_0E                  0x0e
#define HDRSCMFMAC_DEFAULT_TX_DRC_MAP_10                  0x05
#define HDRSCMFMAC_DEFAULT_TX_DRC_MAP_11                  0x07
#define HDRSCMFMAC_DEFAULT_TX_DRC_MAP_12                  0x07
#define HDRSCMFMAC_DEFAULT_TX_DRC_MAP_13                  0x08
#define HDRSCMFMAC_DEFAULT_TX_DRC_MAP_14                  0x08
#define HDRSCMFMAC_DEFAULT_TX_DRC_MAP_15                  0x0a
#define HDRSCMFMAC_DEFAULT_TX_DRC_MAP_16                  0x0b
#define HDRSCMFMAC_DEFAULT_TX_DRC_MAP_17                  0x0b
#define HDRSCMFMAC_DEFAULT_TX_DRC_MAP_18                  0x0c
#define HDRSCMFMAC_DEFAULT_TX_DRC_MAP_19                  0x0e
#define HDRSCMFMAC_DEFAULT_TX_DRC_MAP_1A                  0x0e
#define HDRSCMFMAC_DEFAULT_TX_DRC_MAP_1B                  0x0e

/* ATSupportedPacketFormats */
#define HDRSCMFMAC_DEFAULT_MAX_OPTIONAL_DATA_RATE         0x00
#define HDRSCMFMAC_DEFAULT_MAX_OPTIONAL_PAYLOAD_SIZE      0x04
#define HDRSCMFMAC_SHORT_PKT_ENABLED_THRESH_5120_BITS     0x04
#define HDRSCMFMAC_SHORT_PKT_ENABLED_THRESH_8192_BITS     0x07
#define HDRSCMFMAC_AT_SUPPORTED_PKT_FORMATS_BYTE_LENGTH   4

#define HDRSCMFMAC_DEFAULT_SU_MUX_PKTS_ENABLED            0x00
#define HDRSCMFMAC_DEFAULT_DRC_TRANS_OFFSET               0x00

#ifdef FEATURE_HDR_REVB_ENH_RATES
#define HDRSCMFMAC_PROP_MAX_OPTIONAL_DATA_RATE            0x4
#define HDRSCMFMAC_PROP_MAX_OPTIONAL_PAYLOAD_SIZE         0x7
#define HDRSCMFMAC_PROP_SHORT_PKT_ENABLED_THRESH          \
          HDRSCMFMAC_SHORT_PKT_ENABLED_THRESH_8192_BITS
#else
#define HDRSCMFMAC_PROP_MAX_OPTIONAL_DATA_RATE            \
          HDRSCMFMAC_DEFAULT_MAX_OPTIONAL_DATA_RATE
#define HDRSCMFMAC_PROP_MAX_OPTIONAL_PAYLOAD_SIZE         \
          HDRSCMFMAC_DEFAULT_MAX_OPTIONAL_PAYLOAD_SIZE
#define HDRSCMFMAC_PROP_SHORT_PKT_ENABLED_THRESH          \
          HDRSCMFMAC_SHORT_PKT_ENABLED_THRESH_5120_BITS
#endif /* FEATURE_HDR_REVB_ENH_RATES */

#endif /* FEATURE_HDR_REVB */

 /* EJECT */
/*===========================================================================
 
                  DEFINITIONS AND DECLARATIONS FOR MODULE

  This section contains local definitions for constants, macros, types,
  variables and other items needed by this module.

===========================================================================*/

/* The type hdrscmfmac_simple_attrib_valid_range_type defines a min and a 
 * max field to specify the Valid range of values that a simple attribute
 * could take. */
typedef struct
{
  uint8 min;    /* Minimum allowed value for the simple attribute */
  uint8 max;    /* Maximum allowed value for the simple attribute */

} hdrscmfmac_simple_attrib_valid_range_type;

#ifdef FEATURE_HDR_REVB
/* The structure type used to describe the DRC Rate Metric */
typedef struct
{
  uint16  rate;   /* Data rate, in unit of 0.1Kbps */
  uint16  ps;     /* Payload size, in unit of bits */
  uint8   tt;     /* Termination Target, in unit of slots */
} hdrscmfmac_rate_metric_struct_type;

#define HDRSCMFMAC_RATE_0_KBPS        0
#define HDRSCMFMAC_RATE_38_4_KBPS     384
#define HDRSCMFMAC_RATE_76_8_KBPS     768
#define HDRSCMFMAC_RATE_153_6_KBPS    1536
#define HDRSCMFMAC_RATE_307_2_KBPS    3072
#define HDRSCMFMAC_RATE_614_4_KBPS    6144
#define HDRSCMFMAC_RATE_921_6_KBPS    9216
#define HDRSCMFMAC_RATE_1228_8_KBPS   12288
#define HDRSCMFMAC_RATE_1843_2_KBPS   18432
#define HDRSCMFMAC_RATE_2457_6_KBPS   24576
#define HDRSCMFMAC_RATE_1536_0_KBPS   15360
#define HDRSCMFMAC_RATE_3072_0_KBPS   30720
#define HDRSCMFMAC_RATE_460_8_KBPS    4608
#define HDRSCMFMAC_RATE_768_0_KBPS    7680
#define HDRSCMFMAC_RATE_1075_2_KBPS   10752
#define HDRSCMFMAC_RATE_2150_4_KBPS   21504
#define HDRSCMFMAC_RATE_3686_4_KBPS   36864
#define HDRSCMFMAC_RATE_4300_8_KBPS   43008
#define HDRSCMFMAC_RATE_4915_2_KBPS   49152
#define HDRSCMFMAC_RATE_INVALID_MAX   50000
#endif /* FEATURE_HDR_REVB */

/* Max number of simple attributes */
#ifdef FEATURE_HDR_REVB
#define HDRSCMFMAC_MAX_NUM_SIMPLE_ATTRIBS               17

#define HDRSCMFMAC_SIMPLE_ATTRIB_ID_TO_IDX(attrib_id)   \
          ((attrib_id) -                                \
           (uint16)HDRSCMFMAC_ATTRIB_SERVING_SECTOR_LOCK_ACROSS_SUB_ACTIVE_SETS)
#else /* FEATURE_HDR_REVB */
#define HDRSCMFMAC_MAX_NUM_SIMPLE_ATTRIBS               13

#define HDRSCMFMAC_SIMPLE_ATTRIB_ID_TO_IDX(attrib_id)   \
          ((attrib_id) -                                \
           (uint16)HDRSCMFMAC_ATTRIB_DRC_BOOST_LENGTH)
#endif /* FEATURE_HDR_REVB */

#define HDRSCMFMAC_MAX_NUM_GAUPABLE_ATTRIBS             8 

#define HDRSCMFMAC_SIMPLE_ATTRIB_DATA_SIZE              8

/* Mask used to get table index from Attribute Id */
#define HDRSCMFMAC_ATTRIB_ID_TO_INDEX_M                 0x0F

/*------------------------------------------------------------------------
                            GLOBAL VARIABLES
 -----------------------------------------------------------------------*/
static struct
{
  hdrscmfmac_handoff_delays_struct_type handoff_delays;
    /* Handoff delays attribute */

  hdrscmfmac_drc_lock_struct_type drc_lock;
    /* DRC Lock attribute */

  hdrscmfmac_drc_translation_offset_type drc_trans_offset;
    /* DRC Translation Offset attribute */

#ifdef FEATURE_HDR_REVB

  hdrscmfmac_at_supported_packet_formats_struct_type 
    at_supported_packet_formats;
    /* ATSupportedPacketFormats attribute */

  hdrscmfmac_tentative_drc_to_tx_drc_map_struct_type 
    tentative_drc_to_tx_drc_map;
    /* TentativeDRCtoTxDRCMap*/

  hdrscmfmac_extended_spans_struct_type extended_spans;
    /* ExtendedSpans */

  uint8 trans_id;
    /* Transaction ID used for AT-init Session negotiation */

  uint8 at_supported_packet_formats_value_id;

#endif /* FEATURE_HDR_REVB */

#ifdef FEATURE_HDR_REVC
  uint8 load_info_supported;
  boolean load_info_attrib_rejected;
  uint8 load_info_gaup_tid;
#endif /* FEATURE_HDR_REVC */

} hdrscmfmac;

/*------------------------------------------------------------------------
                              LOOKUP TABLES
 -----------------------------------------------------------------------*/

#ifdef FEATURE_HDR_REVA_L1 

/* Size in bits of DRC Translation offset fields for each DRC from 1-14 in 
 * DRC Translation Offset Attribute */
static uint8 const 
  hdrscmfmac_drc_trans_offset_size[HDRHAI_MAX_NUM_REL_A_DRCS-1] =
    { 1, 2, 2, 3, 3, 3, 3, 4, 4, 4, 4, 4, 4, 4 };

/*------------------------------------------------------------------------
 * This table is a static definition of valid range of values for each 
 * of the simple attributes. The table could be indexed by masking off 
 * the Most significant 4 bits of Attribute ID and readjusting the origin
 * by 3. 
 * Note: Both the attributes Multi-user packets Enabled & Null Rate DRC 38.4
 * Enabled have the same attribute ID. Luckily, they have the same valid 
 * ranges too. Hence the valid range instance is shared and the table size
 * is one less than the max number of simple attributes.
 -----------------------------------------------------------------------*/
static hdrscmfmac_simple_attrib_valid_range_type const
  valid_range[HDRSCMFMAC_MAX_NUM_SIMPLE_ATTRIBS] =
{
#ifdef FEATURE_HDR_REVB
  {0x00, 0x01},  /* 0xEF ServingSectorLockAcrossSubActiveSets */
  {0x00, 0x00},  /* 0xF0 SupportGAUPDeltaACKChannelGainMUP */
  {0x00, 0x00},  /* 0xF1 MUPPreambleBase. Handled as an exceptional case. */
  {0x00, 0x00},  /* 0xF2 SupportGAUPMUPPreambleBase */
#endif /* FEATURE_HDR_REVB */
  {0x00, 0x20},  /* 0xF3 DRC Boost Length */
  {0x00, 0x06},  /* 0xF4 DRC Channel Gain Boost */
  {0x00, 0x40},  /* 0xF5 DSC Boost Length */
  {0x00, 0x06},  /* 0xF6 DSC Channel Gain Boost */
  {0x01, 0x10},  /* 0xF7 Minimum Continuation Span; */
  {0x00, 0xFF},  /* 0xF8 DRC Supervision Timer */
  {0x00, 0x00},  /* 0xF9 Single user Multiplex Packets Enabled; Default only */
  {0x00, 0x03},  /* 0xFA Short packets Enabled Threshold */
  {0x00, 0x12},  /* 0xFB Delta ACK Channel Gain MUP */
  {0x01, 0x20},  /* 0xFC DSC Length */
  {0x00, 0x01},  /* 0xFD Multi-user packets Enabled */
                 /* 0xFD Null Rate DRC 38.4 Enabled */
  {0x00, 0x03},  /* 0xFE DRC Lock Length */
  {0x00, 0x01}   /* 0xFF DRC Gating */

}; /* valid_range */

#endif /* FEATURE_HDR_REVA_L1 */

/* Table containing the IDs of all the attributes that are GAUPable */
static hdrscmfmac_attrib_id_enum_type const 
  hdrscmfmac_gaup_attrib_ids[HDRSCMFMAC_MAX_NUM_GAUPABLE_ATTRIBS] =
    {
      HDRSCMFMAC_ATTRIB_DRC_TRANSLATION_OFFSET,       /* 0x01 */
      HDRSCMFMAC_ATTRIB_DRC_BOOST_LENGTH,             /* 0xF3 */
      HDRSCMFMAC_ATTRIB_DRC_CHAN_GAIN_BOOST,          /* 0xF4 */
      HDRSCMFMAC_ATTRIB_DSC_BOOST_LENGTH,             /* 0xF5 */
      HDRSCMFMAC_ATTRIB_DSC_CHAN_GAIN_BOOST,          /* 0xF6 */
      HDRSCMFMAC_ATTRIB_DRC_SUPERVISION_TIMER,        /* 0xF8 */
      HDRSCMFMAC_ATTRIB_SHORT_PACKETS_ENABLED_THRESH, /* 0xFA */
      HDRSCMFMAC_ATTRIB_MUP_ENABLED                   /* 0xFD */
    };

#ifdef FEATURE_HDR_REVB
#define HDRSCMFMAC_SUBTYPE2_MAX_NUM_GAUPABLE_ATTRIBS            6

/* Table containing the IDs of all the attributes that are GAUPable for Subtype 2*/
static hdrscmfmac_attrib_id_enum_type const 
  hdrscmfmac_subtype2_gaup_attrib_ids[HDRSCMFMAC_SUBTYPE2_MAX_NUM_GAUPABLE_ATTRIBS] =
    {
      HDRSCMFMAC_ATTRIB_TENTATIVE_DRC_TO_TX_DRC_MAP,  /* 0x02 */
      HDRSCMFMAC_ATTRIB_DRC_BOOST_LENGTH,             /* 0xF3 */
      HDRSCMFMAC_ATTRIB_DRC_CHAN_GAIN_BOOST,          /* 0xF4 */
      HDRSCMFMAC_ATTRIB_DSC_BOOST_LENGTH,             /* 0xF5 */
      HDRSCMFMAC_ATTRIB_DSC_CHAN_GAIN_BOOST,          /* 0xF6 */
      HDRSCMFMAC_ATTRIB_DRC_SUPERVISION_TIMER         /* 0xF8 */
    };
#endif /* FEATURE_HDR_REVB */


/*===========================================================================

                         LOCAL FUNCTION DEFINITIONS
                    
===========================================================================*/

/*===========================================================================

FUNCTION HDRSCMFMAC_IS_HANDOFF_DELAYS_VALID                           LOCAL

DESCRIPTION
  Check if the hand-off delays Attribute is valid 

    0x00 - Handoff Delays Attribute ( Both Default & Enhanced FMAC )

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  E_SUCCESS - If the handoff delays Attribute is valid.
  E_OUT_OF_RANGE - otherwise. 

SIDE EFFECTS
  None.

===========================================================================*/
static hdrerrno_enum_type hdrscmfmac_is_handoff_delays_valid(void)
{
  /* All possible values are valid */
  return E_SUCCESS;

} /* hdrscmfmac_is_handoff_delays_valid */


 /* EJECT */
/*===========================================================================

FUNCTION HDRSCMFMAC_PROCESS_HANDOFF_DELAYS                            LOCAL

DESCRIPTION
  Processes the hand-off delays Attribute.

    0x00 - Handoff Delays Attribute ( Both Default & Enhanced FMAC )

DEPENDENCIES
  None.

PARAMETERS
  item_ptr - pointer to the DSM item holding the message to be parsed.
  p_aip    - proposed attribute information pointer.
  a_aip    - accepted attribute information pointer.

RETURN VALUE
  TRUE  - If a valid Handoff Delays attribute was parsed successfully.
  FALSE - otherwise. 

SIDE EFFECTS
  None.

===========================================================================*/
static hdrerrno_enum_type hdrscmfmac_process_handoff_delays
(
  dsm_item_type                               *item_ptr,
  hdrscm_proposed_attrib_type   const *const   p_aip,
  hdrscm_accepted_attrib_type         *const   a_aip
)
{
  uint16 attrib_offset;
    /* Offset to the current byte being processed from the start of DSM chain */

  uint16 value_id;
    /* Value ID of the complex attribute handoff delays */

  hdrerrno_enum_type err_no = E_NO_DATA;
    /* Error Code */

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */ 

  /* Initialize the attribute offset to first Value Id */
  attrib_offset = p_aip->attrib_len_size + a_aip->attrib_id_size;

  /* While there are enough bits left in the Attribute Record for one more
   * set of Handoff Delays complex Attribute Value */
  while ( ( attrib_offset + p_aip->value_id_size + 
            HDRSCMFMAC_HO_DELAYS_ATTRIB_SIZE() ) <= p_aip->attrib_len )
  {
    /*----------------------------------------------------------------------
     * There is at least one more attribute value in this attribute Record
     *--------------------------------------------------------------------*/


    /* Initialize the error code to success since we know that we have at least
     * one attrib value in the Attrib Record */
    err_no = E_SUCCESS;

    /* Unpack the Value ID into a uint16 */
    err_no = HDRBIT_UNPACK16( item_ptr, 
                              &(value_id), 
                              (p_aip->attrib_offset + attrib_offset), 
                              p_aip->value_id_size );

    /* If error unpacking value ID, break */
    if (err_no != E_SUCCESS)
    {
      ERR("Error unpacking Value ID of Handoff Delays Attribute, ignoring",
            0,0,0);
      break;
    }
    
    /* Since Value Id was parsed sucessfully, Update the attribute offset to 
     * start of softer handoff delay */ 
    attrib_offset += p_aip->value_id_size;
    
    /* Unpack softer handoff delay attrib */
    err_no = HDRBIT_UNPACK8( 
              item_ptr, 
              &(hdrscmfmac.handoff_delays.softer_handoff_delay), 
              (p_aip->attrib_offset + attrib_offset),
              HDRSCMFMAC_SOFTER_HO_DELAY_ATTRIB_SIZE );
    
    /* If error unpacking softer handoff delay, break */
    if (err_no != E_SUCCESS)
    {
      ERR("Error unpacking Softer handoff delay Attrib, ignoring", 0,0,0);
      break;
    }
    
    /* Since softer handoff delay was parsed sucessfully, Update the attribute 
     * offset to start of soft handoff delay */ 
    attrib_offset += HDRSCMFMAC_SOFTER_HO_DELAY_ATTRIB_SIZE;
    
    /* Unpack soft handoff delay attrib */
    err_no = HDRBIT_UNPACK8( 
              item_ptr, 
              &(hdrscmfmac.handoff_delays.soft_handoff_delay), 
              (p_aip->attrib_offset + attrib_offset), 
              HDRSCMFMAC_SOFT_HO_DELAY_ATTRIB_SIZE );
    
    /* If error unpacking soft handoff delay, break */
    if (err_no != E_SUCCESS)
    {
      ERR("Error unpacking Soft handoff delay Attrib, ignoring", 0,0,0);
      break;
    }
    
    /* If the handoff delays attribute, just parsed is valid, break out of 
     * loop. Otherwise continue looking for a valid one in the message */ 
    if ( (err_no = hdrscmfmac_is_handoff_delays_valid()) != E_OUT_OF_RANGE )
    {

      HDR_MSG_PROT_2( MSG_LEGACY_HIGH, " Handoff delays: %d %d ",
                      hdrscmfmac.handoff_delays.softer_handoff_delay,
                      hdrscmfmac.handoff_delays.soft_handoff_delay );
      
      /* Update the Accepted attribute information */
      a_aip->is_accepted = TRUE;
      a_aip->value_id = value_id;
      a_aip->complex_val_ptr = (void*) &(hdrscmfmac.handoff_delays);
      a_aip->complex_val_size = sizeof(hdrscmfmac_handoff_delays_struct_type);

      break;
    }
    
    /* Since we didn't find a valid attrib value, update the offset to start
     * of next value ID */
    attrib_offset += HDRSCMFMAC_SOFT_HO_DELAY_ATTRIB_SIZE;

  } /* While */

  return err_no;

} /* hdrscmfmac_process_handoff_delays */


 /* EJECT */
/*===========================================================================

FUNCTION HDRSCMFMAC_IS_DRC_LOCK_VALID                                 LOCAL

DESCRIPTION
  Check if the drc lock Attribute is valid 

    0x01 - DRC Lock Attribute ( Default FMAC )

  Note: The DRC lock attribute is applicable only to default FMAC subtype.

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  E_SUCCESS - If the drc lock Attribute is valid.
  E_OUT_OF_RANGE - otherwise. 

SIDE EFFECTS
  None.

===========================================================================*/
static hdrerrno_enum_type hdrscmfmac_is_drc_lock_valid (void)
{
  
  return ( ( ((hdrscmfmac.drc_lock.period == 0) || 
              (hdrscmfmac.drc_lock.period == 1) ) &&
             (hdrscmfmac.drc_lock.length <= 3) 
           )? E_SUCCESS: E_OUT_OF_RANGE );

} /* hdrscmfmac_is_drc_lock_valid */


 /* EJECT */
/*===========================================================================

FUNCTION HDRSCMFMAC_PROCESS_DRC_LOCK                                  LOCAL

DESCRIPTION
  This is a generic interface to process the DRC Lock Attribute.

    0x01 - DRC Lock Attribute ( Default FMAC )

  Note: The DRC lock attribute is applicable only to default FMAC subtype.
  
DEPENDENCIES
  None.

PARAMETERS
  item_ptr - pointer to the DSM item holding the message to be parsed.
  p_aip    - proposed attribute information pointer.
  a_aip    - accepted attribute information pointer.

RETURN VALUE
  TRUE  - If a valid DRC Lock was parsed successfully.
  FALSE - otherwise. 

SIDE EFFECTS
  None.

===========================================================================*/
static hdrerrno_enum_type hdrscmfmac_process_drc_lock
(
  dsm_item_type                               *item_ptr,
  hdrscm_proposed_attrib_type   const *const   p_aip,
  hdrscm_accepted_attrib_type         *const   a_aip
)
{
  uint16 attrib_offset;
    /* Offset to the current byte being processed from the start of DSM chain */

  uint16 value_id;
    /* Value ID of the complex attribute DRC Lock */

  hdrerrno_enum_type err_no = E_NO_DATA;
    /* Status Code */

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */ 
  
  /* Initialize the attribute offset to first Value Id */
  attrib_offset = p_aip->attrib_len_size + a_aip->attrib_id_size;
  
  /* While there are enough bits left in the Attribute Record for one more
   * set of Handoff Delays complex Attribute Value */
  while ( ( attrib_offset + p_aip->value_id_size + 
            HDRSCMFMAC_DRC_LOCK_ATTRIB_SIZE() ) <= p_aip->attrib_len )
  {
    /*----------------------------------------------------------------------
     * There is at least one more attribute value in this attribute Record
     *--------------------------------------------------------------------*/

    /* Initialize the error code to success since we know that we have at least
     * one attrib value in the Attrib Record */
    err_no = E_SUCCESS;

    /* Unpack the Value ID into a uint16 */
    err_no = HDRBIT_UNPACK16( item_ptr, 
                              &(value_id), 
                              (p_aip->attrib_offset + attrib_offset), 
                              p_aip->value_id_size );

    /* If error unpacking value ID, break */
    if (err_no != E_SUCCESS)
    {
      ERR("Error unpacking Value ID of DRC Lock Attribute, ignoring",
            0,0,0);
      break;
    }
    
    /* Since Value Id was parsed sucessfully, Update the attribute offset to 
     * start of DRC Lock Period attribute */ 
    attrib_offset += p_aip->value_id_size;

    err_no = HDRBIT_UNPACK8( item_ptr,
                             &(hdrscmfmac.drc_lock.period),
                             (p_aip->attrib_offset + attrib_offset),
                             HDRSCMFMAC_DRC_LOCK_PERIOD_ATTRIB_SIZE );

    /* If error unpacking DRC Lock period, break */
    if (err_no != E_SUCCESS)
    {
      ERR("Error unpacking DRC Lock Period, ignoring", 0,0,0);
      break;
    }

    attrib_offset += HDRSCMFMAC_DRC_LOCK_PERIOD_ATTRIB_SIZE;

    err_no = HDRBIT_UNPACK8( item_ptr,
                             &(hdrscmfmac.drc_lock.length),
                             (p_aip->attrib_offset + attrib_offset),
                             HDRSCMFMAC_DRC_LOCK_LENGTH_ATTRIB_SIZE );

    /* If error unpacking DRC Lock length, break */
    if (err_no != E_SUCCESS)
    {
      ERR("Error unpacking DRC Lock Length, ignoring", 0,0,0);
      break;
    }

    if ( (err_no = hdrscmfmac_is_drc_lock_valid()) != E_OUT_OF_RANGE )
    {
      HDR_MSG_PROT_2( MSG_LEGACY_HIGH, " DRC Lock: %d %d ",
                      hdrscmfmac.drc_lock.period,
                      hdrscmfmac.drc_lock.length );
      
      /* Update the Accepted attribute information */
      a_aip->is_accepted = TRUE;
      a_aip->value_id = value_id;
      a_aip->complex_val_ptr = (void*) &(hdrscmfmac.drc_lock);
      a_aip->complex_val_size = sizeof(hdrscmfmac.drc_lock);

      break;
    }
    
    /* Since we didn't find a valid attrib value, update the offset to start
     * of next value ID. The DRC Lock Attribute has a Reserved field (5 bits)
     * at the end.  */ 
    attrib_offset += ( HDRSCMFMAC_DRC_LOCK_LENGTH_ATTRIB_SIZE + 
                       HDRSCMFMAC_DRC_LOCK_RSVD_FIELD_SIZE );

  } /* While */

  return err_no;

} /* hdrscmfmac_process_drc_lock */


#ifdef FEATURE_HDR_REVA_L1

 /* EJECT */
/*===========================================================================

FUNCTION HDRSCMFMAC_IS_DRC_TRANSLATION_OFFSET_VALID                   LOCAL

DESCRIPTION
  Check if the drc translation offset Attribute is valid 

    0x01 - DRC Translation Offset Attribute ( Enhanced FMAC )

  Note: The DRC Translation Offset attribute is applicable only to Enhanced 
  FMAC subtype.

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  E_SUCCESS  - If the drc translation offset Attribute is valid.
  E_OUT_OF_RANGE - otherwise. 

SIDE EFFECTS
  None.

===========================================================================*/
static hdrerrno_enum_type hdrscmfmac_is_drc_translation_offset_valid(void)
{
  uint8 drc_idx;
    /* DRC index */

  hdrerrno_enum_type err_no = E_SUCCESS;
    /* Is the attribute valid? */

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */ 
  
  /* Loop around and check the offsets one at a time for validity */
  for ( drc_idx = 0; (err_no == E_SUCCESS) && 
        (drc_idx < (HDRHAI_MAX_NUM_REL_A_DRCS - 2)); ++drc_idx)
  {
    /* for drcN Is the offset between 0 and drcN, inclusive? */
    err_no = ( (hdrscmfmac.drc_trans_offset[drc_idx] <= (drc_idx+1))? 
                E_SUCCESS: E_OUT_OF_RANGE );
  }
  
  return err_no;

} /* hdrscmfmac_is_drc_translation_offset_valid */


 /* EJECT */
/*===========================================================================

FUNCTION HDRSCMFMAC_PROCESS_DRC_TRANSLATION_OFFSET                    LOCAL

DESCRIPTION
  This is a generic interface to process the DRC Translation Offset Attribute.

    0x01 - DRC Translation Offset Attribute ( Enhanced FMAC )

  Note: The DRC Translation Offset attribute is applicable only to Enhanced 
  FMAC subtype.
  
DEPENDENCIES
  None.

PARAMETERS
  item_ptr - pointer to the DSM item holding the message to be parsed.
  p_aip    - proposed attribute information pointer.
  a_aip    - accepted attribute information pointer.

RETURN VALUE
  TRUE  - If a valid DRC Translation Offset was parsed successfully.
  FALSE - otherwise. 

SIDE EFFECTS
  None.

===========================================================================*/
static hdrerrno_enum_type hdrscmfmac_process_drc_translation_offset
(
  dsm_item_type                               *item_ptr,
  hdrscm_proposed_attrib_type   const *const   p_aip,
  hdrscm_accepted_attrib_type         *const   a_aip
)
{
  uint16 attrib_offset;
    /* Offset to the current byte being processed from the start of DSM chain */

  uint16 value_id;
    /* Value ID of the complex attribute DRC Lock */

  hdrerrno_enum_type err_no = E_NO_DATA;
    /* Status Code */

  uint8 drc_idx = 0;
    /* DRC index */

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */ 
  
  /* Initialize the attribute offset to first Value Id */
  attrib_offset = p_aip->attrib_len_size + a_aip->attrib_id_size;
  
  /* While there are enough bits left in the Attribute Record for one more
   * set of Handoff Delays complex Attribute Value */
  while ( ( attrib_offset + p_aip->value_id_size + 
            HDRSCMFMAC_DRC_TRANS_OFFSET_ATTRIB_SIZE ) <= p_aip->attrib_len )
  {
    /*----------------------------------------------------------------------
     * There is at least one more attribute value in this attribute Record
     *--------------------------------------------------------------------*/

    /* Initialize the error code to success since we know that we have at least
     * one attrib value in the Attrib Record */
    err_no = E_SUCCESS;

    /* Unpack the Value ID into a uint16 */
    err_no = HDRBIT_UNPACK16( item_ptr,
                              &(value_id),
                              (p_aip->attrib_offset + attrib_offset),
                              p_aip->value_id_size );

    /* If error unpacking value ID, break */
    if (err_no != E_SUCCESS)
    {
      ERR("Error unpacking Value ID of DRC Trans. Offset Attribute, ignoring",
            0,0,0);
      break;
    }
    
    /* Since Value Id was parsed sucessfully, Update the attribute offset to 
     * start of DRC Lock Period attribute */ 
    attrib_offset += p_aip->value_id_size;
    
    /* Loop around once for each DRC from 1-14, and unpack the offset */
    for ( drc_idx = 0; (err_no == E_SUCCESS) && 
          (drc_idx < HDRHAI_MAX_NUM_REL_A_DRCS-1); ++drc_idx )
    {
      /* Unpack the DRC translation offset for DRC "drc_idx + 1" */
      err_no = HDRBIT_UNPACK8( item_ptr,
                               &(hdrscmfmac.drc_trans_offset[drc_idx]),
                               (p_aip->attrib_offset + attrib_offset),
                               hdrscmfmac_drc_trans_offset_size[drc_idx] );

      /* Since we have successfully parsed DRC Translation Offset for 
       * DRC "drc_idx + 1", update attrib_offset to next offset */
      attrib_offset += hdrscmfmac_drc_trans_offset_size[drc_idx];

    } /* for */

    /* If error unpacking DRC Translation Offset, break */
    if (err_no != E_SUCCESS)
    {
      ERR("Error unpacking DRC Trans. Offset Attribute, ignoring", 0,0,0);
      break; /* break from while loop */
    }

    if ( (err_no = 
            hdrscmfmac_is_drc_translation_offset_valid()) != E_OUT_OF_RANGE )
    {
      /* Update the Accepted attribute information */
      a_aip->is_accepted = TRUE;
      a_aip->value_id = value_id;
      a_aip->complex_val_ptr = (void*) &(hdrscmfmac.drc_trans_offset[0]);
      a_aip->complex_val_size = sizeof(hdrscmfmac_drc_translation_offset_type);

      break;
    }

  } /* While */

  return err_no;

} /* hdrscmfmac_process_drc_translation_offset */

#endif /* FEATURE_HDR_REVA_L1 */


 /* EJECT */
#ifdef FEATURE_HDR_REVB
/*===========================================================================

FUNCTION HDRSCMFMAC_DUMP_TENTATIVE_DRC_TO_TX_DRC_MAP                    LOCAL

DESCRIPTION
  Dump the F3 message of TentativeDRCtoTxDRCMap Attribute

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
static void hdrscmfmac_dump_tentative_drc_to_tx_drc_map(void)
{

  uint8 tent_drc = 0;
    /* DRC index */

  uint8 tx_drc = 0;
    /* Transmitted DRC */

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */ 

  /* Loop through all the DRC mappings */
  for ( tent_drc = HDRHAI_DRC_38_4; 
        tent_drc < HDRHAI_MAX_NUM_REL_B_DRCS; ++tent_drc)
  {
    if (tent_drc != HDRHAI_DRC_REL_B_NA)
    {
      /* The tentative_drc_to_tx_drc_map does not include mapping for DRC 0 and
         Invalid DRC (0xF). Adjust the lookup accordingly */
      tx_drc = ( (tent_drc > HDRHAI_MAX_NUM_REL_A_DRCS)?  
                 hdrscmfmac.tentative_drc_to_tx_drc_map[tent_drc - 2]: 
                 hdrscmfmac.tentative_drc_to_tx_drc_map[tent_drc - 1] );

      HDR_MSG_PROT_2( MSG_LEGACY_HIGH, "Tent DRC 0x%x, Tx DRC 0x%x", tent_drc, tx_drc );

    }

  }

  return;

} /* hdrscmfmac_dump_tentative_drc_to_tx_drc_map */


 /* EJECT */
/*===========================================================================

FUNCTION HDRSCMFMAC_IS_TENTATIVE_DRC_TO_TX_DRC_MAP_VALID                LOCAL

DESCRIPTION
  Check if the TentativeDRCtoTxDRCMap Attribute is valid 

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  E_SUCCESS - If the Attribute is valid.
  E_OUT_OF_RANGE - otherwise. 

SIDE EFFECTS
  None.

===========================================================================*/
static hdrerrno_enum_type hdrscmfmac_is_tentative_drc_to_tx_drc_map_valid(void)
{

  hdrerrno_enum_type err_no = E_SUCCESS;
    /* Status Code */

  uint8 tent_drc = 0;
    /* DRC index */

  uint8 tx_drc;
    /* Mapped DRC index */

  static hdrscmfmac_rate_metric_struct_type const 
    rate_metric [HDRHAI_MAX_NUM_REL_B_DRCS] =
  {
    /* Rate,                      PayloadSize,  Termination Target */
    {HDRSCMFMAC_RATE_0_KBPS,      1024,         16},    /* DRC 0x00 */
    {HDRSCMFMAC_RATE_38_4_KBPS,   1024,         16},    /* DRC 0x01 */
    {HDRSCMFMAC_RATE_76_8_KBPS,   1024,         8},     /* DRC 0x02 */
    {HDRSCMFMAC_RATE_153_6_KBPS,  1024,         4},     /* DRC 0x03 */
    {HDRSCMFMAC_RATE_307_2_KBPS,  1024,         2},     /* DRC 0x04 */
    {HDRSCMFMAC_RATE_307_2_KBPS,  2048,         4},     /* DRC 0x05 */
    {HDRSCMFMAC_RATE_614_4_KBPS,  1024,         1},     /* DRC 0x06 */
    {HDRSCMFMAC_RATE_614_4_KBPS,  2048,         2},     /* DRC 0x07 */
    {HDRSCMFMAC_RATE_921_6_KBPS,  3072,         2},     /* DRC 0x08 */
    {HDRSCMFMAC_RATE_1228_8_KBPS, 2048,         1},     /* DRC 0x09 */
    {HDRSCMFMAC_RATE_1228_8_KBPS, 4096,         2},     /* DRC 0x0a */
    {HDRSCMFMAC_RATE_1843_2_KBPS, 3072,         1},     /* DRC 0x0b */
    {HDRSCMFMAC_RATE_2457_6_KBPS, 4096,         1},     /* DRC 0x0c */
    {HDRSCMFMAC_RATE_1536_0_KBPS, 5120,         2},     /* DRC 0x0d */
    {HDRSCMFMAC_RATE_3072_0_KBPS, 5120,         1},     /* DRC 0x0e */
    {HDRSCMFMAC_RATE_INVALID_MAX, 5120,         0},     /* DRC 0x0f */
      /* The rate metric for DRC F is set to an invalid rate which 
         is bigger than the max supported rate and Span is set to zero 
         which is smaller than all other spans. This would force the 
         conditions to fail if any DRC is mapped to the invalid DRC F */
    {HDRSCMFMAC_RATE_460_8_KBPS,  3072,         4},     /* DRC 0x10 */
    {HDRSCMFMAC_RATE_614_4_KBPS,  4096,         4},     /* DRC 0x11 */
    {HDRSCMFMAC_RATE_768_0_KBPS,  5120,         4},     /* DRC 0x12 */
    {HDRSCMFMAC_RATE_921_6_KBPS,  6144,         4},     /* DRC 0x13 */
    {HDRSCMFMAC_RATE_1075_2_KBPS, 7168,         4},     /* DRC 0x14 */
    {HDRSCMFMAC_RATE_1228_8_KBPS, 8192,         4},     /* DRC 0x15 */
    {HDRSCMFMAC_RATE_1843_2_KBPS, 6144,         2},     /* DRC 0x16 */
    {HDRSCMFMAC_RATE_2150_4_KBPS, 7168,         2},     /* DRC 0x17 */
    {HDRSCMFMAC_RATE_2457_6_KBPS, 8192,         2},     /* DRC 0x18 */
    {HDRSCMFMAC_RATE_3686_4_KBPS, 6144,         1},     /* DRC 0x19 */
    {HDRSCMFMAC_RATE_4300_8_KBPS, 7168,         1},     /* DRC 0x1a */
    {HDRSCMFMAC_RATE_4915_2_KBPS, 8192,         1}      /* DRC 0x1b */
  };

  static uint16 const max_optional_data_rate_encoding
                  [(HDRSCMFMAC_ASPF_MAX_MAX_OPTIONAL_DATA_RATE+1)] =
  {
    HDRSCMFMAC_RATE_0_KBPS,       /* 0 */
    HDRSCMFMAC_RATE_1228_8_KBPS,  /* 1 */
    HDRSCMFMAC_RATE_3686_4_KBPS,  /* 2 */
    HDRSCMFMAC_RATE_4300_8_KBPS,  /* 3 */
    HDRSCMFMAC_RATE_4915_2_KBPS   /* 4 */
  };

  static uint16 const max_optional_payload_size_encoding
                  [(HDRSCMFMAC_ASPF_MAX_MAX_OPTIONAL_PS-
                    HDRSCMFMAC_ASPF_MIN_MAX_OPTIONAL_PS+1)] =
  {
    5120,
    6144,
    7168,
    8192
  };

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */ 

  /* For each DRC from HDRHAI_DRC_38_4 (DRC 1) through HDRHAI_DRC_REL_B_4915_2
     (DRC 1B)... */
  for ( tent_drc = HDRHAI_DRC_38_4;
        ((err_no == E_SUCCESS) && (tent_drc < HDRHAI_MAX_NUM_REL_B_DRCS));
        ++tent_drc )
  {
    /* C.S0024-B v2.0 Section 9.8.7.2.2
       Rule 1: The sender shall not set this field to a DRC value whose 
               Rate Metric is greater than that of DRC index 0x01. 
       Rule 2: The sender shall not set this field to a DRC value whose 
               Span is less than that of DRC index 0x01.

       The TransmittedDRC default value specified in the spec for some of 
       the Rev-B TentativeDRCs (0x11, 0x12, 0x13, 0x14, 0x15) violates Rule 2 
       (Shorter span) above. In order to work-around this standards bug, 
       validating the attribute only for Rev-A DRCs */

    //if (drc_idx != HDRHAI_DRC_REL_B_NA)
    if (tent_drc < HDRHAI_MAX_NUM_REL_A_DRCS)
    {
      /* The tentative_drc_to_tx_drc_map does not include mapping for DRC 0 and
         Invalid DRC (0xF). Adjust the lookup accordingly */
      tx_drc = ( (tent_drc > HDRHAI_MAX_NUM_REL_A_DRCS)?  
                         hdrscmfmac.tentative_drc_to_tx_drc_map[tent_drc - 2]: 
                         hdrscmfmac.tentative_drc_to_tx_drc_map[tent_drc - 1] );

      /* C.S0024-B v2.0 Section 9.8.6.1.5
       *
       * The access terminal and the access network SHALL support all packet formats
       * associated with DRC indices 0x00 through 0x0e.  The access terminal and
       * access network SHOULD support all packet formats associated with DRC 
       * indices from 0x10 through 0x1b.  The access terminal and the access network
       * SHALL support all packet formats associated with DRC indices 0x10 through
       * 0x1b for which the data rate is less than or equal to that indicated by
       * the MaxOptionalDataRate parameter of the ATSupportedPacketFormats 
       * attribute, and for which the packet size is less than or equal to that
       * indicated by the MaxOptionalPayloadSize parameter of the 
       * ATSupportedPacketFormats attribute.
       *
       */

      if (tx_drc >= HDRHAI_MAX_NUM_REL_A_DRCS)
      {
        /* The mapped rate cannot be greater than MaxOptionalDataRate AND
           the mapped payload size cannot be greater than MaxOptionalPayloadSize. */
        if ( (tx_drc >= HDRHAI_MAX_NUM_REL_B_DRCS) ||
             (rate_metric[tx_drc].rate > 
              max_optional_data_rate_encoding
              [HDRSCMFMAC_DEFAULT_MAX_OPTIONAL_DATA_RATE]) ||
             (rate_metric[tx_drc].ps >
                 max_optional_payload_size_encoding
                  [(HDRSCMFMAC_DEFAULT_MAX_OPTIONAL_PAYLOAD_SIZE-
                    HDRSCMFMAC_ASPF_MIN_MAX_OPTIONAL_PS)]) )
        {
          err_no = E_OUT_OF_RANGE;
        }
      }

      if (err_no == E_SUCCESS)
      {
        /* The mapped rate cannot be greater than the original rate AND
           the mapped span (termination target) cannot be less than the original
           span. */
        if ( (tx_drc >= HDRHAI_MAX_NUM_REL_B_DRCS) ||
             (rate_metric[tx_drc].rate > rate_metric[tent_drc].rate) ||
             (rate_metric[tx_drc].tt < rate_metric[tent_drc].tt) )
        {
          err_no = E_OUT_OF_RANGE;
        }
      }

    } /* If DRC is valid */
  }

  return err_no;

} /* hdrscmfmac_is_tentative_drc_to_tx_drc_map_valid */


 /* EJECT */
/*===========================================================================

FUNCTION HDRSCMFMAC_PROCESS_TENTATIVE_DRC_TO_TX_DRC_MAP                 LOCAL

DESCRIPTION
  This is a generic interface to process the TentativeDRCtoTxDRCMap Attribute.

DEPENDENCIES
  None.

PARAMETERS
  item_ptr - pointer to the DSM item holding the message to be parsed.
  p_aip    - proposed attribute information pointer.
  a_aip    - accepted attribute information pointer.

RETURN VALUE
  TRUE  - If a valid attribute was parsed successfully.
  FALSE - otherwise. 

SIDE EFFECTS
  None.

===========================================================================*/
static hdrerrno_enum_type hdrscmfmac_process_tentative_drc_to_tx_drc_map
(
  dsm_item_type                               *item_ptr,
  hdrscm_proposed_attrib_type   const *const   p_aip,
  hdrscm_accepted_attrib_type         *const   a_aip
)
{
  uint16 attrib_offset;
    /* Offset to the current byte being processed from the start of DSM chain */

  uint16 value_id;
    /* Value ID of the complex attribute */

  hdrerrno_enum_type err_no = E_NO_DATA;
    /* Status Code */

  uint8 drc_idx = 0;
    /* DRC index */

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */ 
  
  /* Initialize the attribute offset to first Value Id */
  attrib_offset = p_aip->attrib_len_size + a_aip->attrib_id_size;
  
  /* While there are enough bits left in the Attribute Record for one more
   * set of TentativeDRCtoTxDRCMap complex Attribute Value */
  while ( ( attrib_offset + p_aip->value_id_size + 
            HDRSCMFMAC_TDTTDM_ATTRIB_SIZE ) <= p_aip->attrib_len )
  {
    /*----------------------------------------------------------------------
     * There is at least one more attribute value in this attribute Record
     *--------------------------------------------------------------------*/

    /* Initialize the error code to success since we know that we have at least
     * one attrib value in the Attrib Record */
    err_no = E_SUCCESS;

    /* Unpack the Value ID into a uint16 */
    err_no = HDRBIT_UNPACK16( item_ptr,
                              &(value_id),
                              (p_aip->attrib_offset + attrib_offset),
                              p_aip->value_id_size );

    /* If error unpacking value ID, break */
    if (err_no != E_SUCCESS)
    {
      ERR("Error unpacking Value ID of TentativeDRCtoTxDRCMap Attribute, ignoring",
            0,0,0);
      break;
    }

    /* Since Value Id was parsed sucessfully, Update the attribute offset to 
     * start of TentativeDRCtoTxDRCMap attribute */ 
    attrib_offset += p_aip->value_id_size;

    /* Loop around once for each DRC and unpack the map value */
    for ( drc_idx = 0;
          ((err_no == E_SUCCESS)&&
           (drc_idx < (HDRHAI_MAX_NUM_REL_B_DRCS-2)));
          ++drc_idx )
    {
      err_no = HDRBIT_UNPACK8( item_ptr,
                               &(hdrscmfmac.tentative_drc_to_tx_drc_map[drc_idx]),
                               (p_aip->attrib_offset + attrib_offset),
                               HDRSCMFMAC_TDTTDM_TX_DRC_MAP_SIZE);

      /* Update the offset to next field */
      attrib_offset += HDRSCMFMAC_TDTTDM_TX_DRC_MAP_SIZE;

    } /* for */

    /* If error unpacking TxDRCMap, break */
    if (err_no != E_SUCCESS)
    {
      ERR("Error unpacking TxDRCMap %d", (drc_idx-1), 0, 0);
      break;
    }

    /* Dump the contents regardless if the values are valid */
    hdrscmfmac_dump_tentative_drc_to_tx_drc_map();

    if ( (err_no =
            hdrscmfmac_is_tentative_drc_to_tx_drc_map_valid()) != E_OUT_OF_RANGE )
    {
      /* Update the Accepted attribute information */
      a_aip->is_accepted = TRUE;
      a_aip->value_id = value_id;
      a_aip->complex_val_ptr = 
        (void *) (hdrscmfmac.tentative_drc_to_tx_drc_map);
      a_aip->complex_val_size = 
        sizeof(hdrscmfmac_tentative_drc_to_tx_drc_map_struct_type);

      break;
    }
  } /* while */

  return err_no;

} /* hdrscmfmac_process_tentative_drc_to_tx_drc_map */


 /* EJECT */
/*===========================================================================

FUNCTION HDRSCMFMAC_IS_EXTENDED_SPANS_VALID                           LOCAL

DESCRIPTION
  Check if the ExtendedSpans Attribute is valid 

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  E_SUCCESS - If the Attribute is valid.
  E_OUT_OF_RANGE - otherwise. 

SIDE EFFECTS
  None.

===========================================================================*/
static hdrerrno_enum_type hdrscmfmac_is_extended_spans_valid(void)
{

  hdrerrno_enum_type err_no = E_SUCCESS;

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */ 

  /* Only accept the default value */
  if (hdrscmfmac.extended_spans.extended_spans_enabled)
  {
    err_no = E_OUT_OF_RANGE;
  }

  return err_no;

} /* hdrscmfmac_is_extended_spans_valid */


 /* EJECT */
/*===========================================================================

FUNCTION HDRSCMFMAC_PROCESS_EXTENDED_SPANS                              LOCAL

DESCRIPTION
  This is a generic interface to process the ExtendedSpans Attribute.

DEPENDENCIES
  None.

PARAMETERS
  item_ptr - pointer to the DSM item holding the message to be parsed.
  p_aip    - proposed attribute information pointer.
  a_aip    - accepted attribute information pointer.

RETURN VALUE
  TRUE  - If a valid attribute was parsed successfully.
  FALSE - otherwise. 

SIDE EFFECTS
  None.

===========================================================================*/
static hdrerrno_enum_type hdrscmfmac_process_extended_spans
(
  dsm_item_type                               *item_ptr,
  hdrscm_proposed_attrib_type   const *const   p_aip,
  hdrscm_accepted_attrib_type         *const   a_aip
)
{
  uint16 attrib_offset;
    /* Offset to the current byte being processed from the start of DSM chain */

  uint16 value_id;
    /* Value ID of the complex attribute */

  hdrerrno_enum_type err_no = E_NO_DATA;
    /* Status Code */

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */ 
  
  /* Initialize the attribute offset to first Value Id */
  attrib_offset = p_aip->attrib_len_size + a_aip->attrib_id_size;
  
  /* While there are enough bits left in the Attribute Record for one more
   * set of Handoff Delays complex Attribute Value */
  while ( ( attrib_offset + p_aip->value_id_size + 
            HDRSCMFMAC_MIN_ES_ATTRIB_SIZE ) <= p_aip->attrib_len )
  {
    /*----------------------------------------------------------------------
     * There is at least one more attribute value in this attribute Record
     *--------------------------------------------------------------------*/

    /* Initialize the error code to success since we know that we have at least
     * one attrib value in the Attrib Record */
    err_no = E_SUCCESS;

    /* Unpack the Value ID into a uint16 */
    err_no = HDRBIT_UNPACK16( item_ptr,
                              &(value_id),
                              (p_aip->attrib_offset + attrib_offset),
                              p_aip->value_id_size );

    /* If error unpacking value ID, break */
    if (err_no != E_SUCCESS)
    {
      ERR("Error unpacking Value ID of ExtendedSpans Attribute, ignoring",
            0,0,0);
      break;
    }

    /* Since Value Id was parsed sucessfully, Update the attribute offset to 
     * start of ExtendedSpansEnabled attribute */ 
    attrib_offset += p_aip->value_id_size;
    
    /* Unpack ExtendedSpansEnabled */
    err_no = HDRBIT_UNPACK8( item_ptr,
                             &(hdrscmfmac.extended_spans.extended_spans_enabled),
                             (p_aip->attrib_offset + attrib_offset),
                             HDRSCMFMAC_ES_ENABLED_SIZE );

    /* If error unpacking ExtendedSpansEnabled, break */
    if (err_no != E_SUCCESS)
    {
      ERR("Error unpacking ExtendedSpansEnabled", 0, 0, 0);
      break;  /* break from while loop */
    }

    /* Update the attribute offset to start of next field */ 
    attrib_offset += HDRSCMFMAC_ES_ENABLED_SIZE;

    /* Unpack PERTargetSpan if it exists */
    if (hdrscmfmac.extended_spans.extended_spans_enabled)
    {
      err_no = HDRBIT_UNPACK8( item_ptr,
                               &(hdrscmfmac.extended_spans.per_target_span),
                               (p_aip->attrib_offset + attrib_offset),
                               HDRSCMFMAC_ES_PER_TARGET_SPAN );

      /* If error unpacking PERTargetSpan, break */
      if (err_no != E_SUCCESS)
      {
        ERR("Error unpacking PERTargetSpan", 0, 0, 0);
        break;  /* break from while loop */
      }

      /* Update the attribute offset to start of next field */ 
      attrib_offset += HDRSCMFMAC_ES_PER_TARGET_SPAN;
    }

    if ( (err_no =
            hdrscmfmac_is_extended_spans_valid()) != E_OUT_OF_RANGE )
    {
      HDR_MSG_PROT_2(
        MSG_LEGACY_HIGH,
        "ExtendedSpans %d %d",
        hdrscmfmac.extended_spans.extended_spans_enabled,
        hdrscmfmac.extended_spans.per_target_span);

      /* Update the Accepted attribute information */
      a_aip->is_accepted = TRUE;
      a_aip->value_id = value_id;
      a_aip->complex_val_ptr = (void *) &(hdrscmfmac.extended_spans);
      a_aip->complex_val_size = sizeof(hdrscmfmac_extended_spans_struct_type);

      break;
    }
  } /* while */

  return err_no;

} /* hdrscmfmac_process_extended_spans */

/* EJECT */
/*===========================================================================

FUNCTION HDRSCMFMAC_GET_SUBTYPE2_ATTRIB_VERNO_INDEX

DESCRIPTION
  Returns the index of the given attribute in the version number array.
  This function is used when the negotiated subtype is MC.

DEPENDENCIES
  None

PARAMETERS
  attrib_id     - Attribute Id.
  index         - Pointer to the index storage.
  
RETURN VALUE
  E_SUCCESS - If the given attribute ID is valid.

SIDE EFFECTS
  None

===========================================================================*/
static hdrerrno_enum_type hdrscmfmac_get_subtype2_attrib_verno_index
(
  uint16 attrib_id,
  uint16 *index_ptr
)
{

  hdrerrno_enum_type err_no = E_SUCCESS;

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */ 

  if (hdrscmfmac_is_attrib_simple(attrib_id, HDRSCP_MC_FMAC))
  {
    /* Simple attribute verno's are placed after the Complex attribue, in the
     * order according to the assending attrib ID */
    *index_ptr = ( HDRSCMFMAC_SIMPLE_ATTRIB_ID_TO_IDX(attrib_id) +
                   HDRSCMFMAC_MAX_NUM_PRE_REL_B_CONFIG_ATTRIBS +
                   HDRSCMFMAC_SUBTYPE2_MAX_NUM_COMPLEX_CONFIG_ATTRIBS );
  }
  else
  {
    /* 
     * Complex attribute verno's are placed immediately after the Pre-Rev. B 
     * attribute, in the order directly mapped to attrib ID.
     */
    *index_ptr = ( attrib_id + 
                    HDRSCMFMAC_MAX_NUM_PRE_REL_B_CONFIG_ATTRIBS );
  }

  return err_no;

} /* hdrscmfmac_get_subtype2_attrib_verno_index */


/* EJECT */
/*===========================================================================

FUNCTION HDRSCMFMAC_PACK_AT_SUPPORTED_PACKET_FORMATS                    LOCAL

DESCRIPTION
  This function is used to pack ATSupportPacketFormats attribute as part 
  of AT-init session configuration.

DEPENDENCIES
  None

PARAMETERS
  None.
   
RETURN VALUE
  send_msg_ptr  - DSM item to pack into 
  attrib_offset - offset into DSM item

SIDE EFFECTS
  None

===========================================================================*/
static void hdrscmfmac_pack_at_supported_packet_formats 
( 
  dsm_item_type *send_msg_ptr,
  uint8 *attrib_offset
)
{
  uint8 value_id = 0;
    /* Value ID */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  HDR_MSG_PROT(MSG_LEGACY_HIGH, 
               "AT-init Proposal of ATSupportedPacketFormats(0x01)");

  hdrscmfmac.at_supported_packet_formats.max_optional_data_rate =
    HDRSCMFMAC_PROP_MAX_OPTIONAL_DATA_RATE;
  hdrscmfmac.at_supported_packet_formats.max_optional_payload_size =
    HDRSCMFMAC_PROP_MAX_OPTIONAL_PAYLOAD_SIZE;
  hdrscmfmac.at_supported_packet_formats.short_pkt_enabled_thresh =
    HDRSCMFMAC_PROP_SHORT_PKT_ENABLED_THRESH;

  /* Pack the AttributeLen */
  HDRBIT_PACK8(send_msg_ptr, 
               HDRSCMFMAC_AT_SUPPORTED_PKT_FORMATS_BYTE_LENGTH,
               *attrib_offset,
               HDRSCM_LENGTH_SIZE);

  /* Shift the offset to start of AttributeID field */
  *attrib_offset += HDRSCM_LENGTH_SIZE;

  /* Pack the AttributeID */
  HDRBIT_PACK8(send_msg_ptr,
               HDRSCMFMAC_ATTRIB_AT_SUPPORTED_PACKET_FORMATS,
               *attrib_offset,
               HDRSCMFMAC_ATTRIB_ID_SIZE);

  /* Shift the offset to start of Value ID field */
  *attrib_offset += HDRSCMFMAC_ATTRIB_ID_SIZE;

  /* Pack Value ID */
  HDRBIT_PACK8(send_msg_ptr,
               value_id,
               *attrib_offset,
               HDRSCMFMAC_ENH_FMAC_VALUE_ID_SIZE);

  /* Shift the offset to start of MaxOptionalDataRate field */
  *attrib_offset += HDRSCMFMAC_ENH_FMAC_VALUE_ID_SIZE;

  /* Pack MaxOptionalDataRate */
  HDRBIT_PACK8(send_msg_ptr,
               hdrscmfmac.at_supported_packet_formats.max_optional_data_rate,
               *attrib_offset,
               HDRSCMFMAC_ASPF_MAX_OPTIONAL_DATA_RATE_SIZE);

  /* Shift the offset to start of MaxOptionalPayloadSize field */
  *attrib_offset += HDRSCMFMAC_ASPF_MAX_OPTIONAL_DATA_RATE_SIZE;

  /* Pack MaxOptionalPayloadSize */
  HDRBIT_PACK8(send_msg_ptr,
               hdrscmfmac.at_supported_packet_formats.max_optional_payload_size,
               *attrib_offset,
               HDRSCMFMAC_ASPF_MAX_OPTIONAL_PS_SIZE);

  /* Shift the offset to start of ShortPacketsEnabledThresh field */
  *attrib_offset += HDRSCMFMAC_ASPF_MAX_OPTIONAL_PS_SIZE;

  /* Pack ShortPacketsEnabledThresh */
  HDRBIT_PACK8(send_msg_ptr,
               hdrscmfmac.at_supported_packet_formats.short_pkt_enabled_thresh,
               *attrib_offset,
               HDRSCMFMAC_ASPF_SHORT_PKT_ENABLED_THRESH_SIZE);

  *attrib_offset += HDRSCMFMAC_ASPF_SHORT_PKT_ENABLED_THRESH_SIZE;

  HDR_MSG_PROT_3(MSG_LEGACY_HIGH, 
                 "AT Proposed. ASPF MaxDR: %d, MaxPS: %d, ShortPktsEnaThresh: %d", 
                 hdrscmfmac.at_supported_packet_formats.max_optional_data_rate,
                 hdrscmfmac.at_supported_packet_formats.max_optional_payload_size,
                 hdrscmfmac.at_supported_packet_formats.short_pkt_enabled_thresh);

} /* hdrscmfmac_pack_at_supported_packet_formats */
#endif /* FEATURE_HDR_REVB */


#ifdef FEATURE_HDR_REVC
/* EJECT */
/*===========================================================================

FUNCTION HDRSCMFMAC_PACK_LOAD_INFO_SUPPORTED                    LOCAL

DESCRIPTION
  This function is used to pack LoadInformationSupported attribute as part 
  of AT-init session configuration.

DEPENDENCIES
  None

PARAMETERS
  send_msg_ptr  - DSM item to pack into 
  attrib_offset - offset into DSM item
   
RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
static void hdrscmfmac_pack_load_info_supported
( 
  dsm_item_type *send_msg_ptr,
  uint8 *attrib_offset
)
{
  HDR_MSG_PROT(MSG_LEGACY_HIGH, 
               "AT-init Proposal of LoadInfoSupported(0xe9)");

  /* Pack the AttributeLen */
  HDRBIT_PACK8(send_msg_ptr, 
               (HDRSCMFMAC_ATTRIB_ID_SIZE + HDRSCMFMAC_ENH_FMAC_VALUE_ID_SIZE) / 8,
               *attrib_offset,
               HDRSCM_LENGTH_SIZE);

  /* Shift the offset to start of AttributeID field */
  *attrib_offset += HDRSCM_LENGTH_SIZE;

  /* Pack the AttributeID */
  HDRBIT_PACK8(send_msg_ptr,
               HDRSCMFMAC_ATTRIB_LOAD_INFO_SUPPORTED,
               *attrib_offset,
               HDRSCMFMAC_ATTRIB_ID_SIZE);

  /* Shift the offset to start of Value ID field */
  *attrib_offset += HDRSCMFMAC_ATTRIB_ID_SIZE;

  /* Pack Value ID */
  HDRBIT_PACK8(send_msg_ptr,
               HDRSCMFMAC_LOAD_INFO_SUPPORTED,
               *attrib_offset,
               HDRSCMFMAC_ENH_FMAC_VALUE_ID_SIZE);

  /* Shift the offset past LoadInfoSupported ValueID */
  *attrib_offset += HDRSCMFMAC_ENH_FMAC_VALUE_ID_SIZE;

  hdrscmfmac.load_info_supported = HDRSCMFMAC_LOAD_INFO_SUPPORTED;
} /* hdrscmfmac_pack_at_supported_packet_formats */
#endif /* FEATURE_HDR_REVC */

 /* EJECT */
/*============================================================================

                        EXTERNAL FUNCTION DEFINITIONS

=============================================================================*/

/*===========================================================================

FUNCTION HDRSCMFMAC_PROCESS_COMPLEX_ATTRIB

DESCRIPTION
  This is a generic interface to process all complex attributes given below.

    0x00 - Handoff Delays Attribute
    0x01 - DRC Translation Offset Attribute ( Enhanced FMAC )
    0x01 - DRC Lock Attribute ( Default FMAC )

  Note: Attribute ID 0x01 is shared between default FMAC and Enhanced FMAC.
  
DEPENDENCIES
  None.

PARAMETERS
  item_ptr - pointer to the DSM item holding the message to be parsed.
  p_aip    - proposed attribute information pointer.
  a_aip    - accepted attribute information pointer.
  subtype  - Subtype negotiated for FMAC

RETURN VALUE
  TRUE  - If a valid complex attribute was parsed successfully.
  FALSE - otherwise. 

SIDE EFFECTS
  None.

===========================================================================*/
hdrerrno_enum_type hdrscmfmac_process_complex_attrib
(
  dsm_item_type                               *item_ptr,
  hdrscm_proposed_attrib_type   const *const   p_aip,
  hdrscm_accepted_attrib_type         *const   a_aip,
  uint16                        const          subtype
)
{
  hdrerrno_enum_type err_no = E_SUCCESS;
    /* Error code; Lets be optimistic! */

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */ 


  switch ( p_aip->attrib_id )
  {

    case HDRSCMFMAC_ATTRIB_HANDOFF_DELAYS:

      err_no = hdrscmfmac_process_handoff_delays( item_ptr, p_aip, a_aip );
      break;
  
#ifdef FEATURE_HDR_REVA_L1 

    //case HDRSCMFMAC_ATTRIB_DRC_TRANSLATION_OFFSET:

#endif /* FEATURE_HDR_REVA_L1 */

    case HDRSCMFMAC_ATTRIB_DRC_LOCK:
     
#ifdef FEATURE_HDR_REVB

    //case HDRSCMFMAC_ATTRIB_AT_SUPPORTED_PACKET_FORMATS:
      if (subtype == HDRSCP_MC_FMAC)
      {
        /* 
         * Section 9.6.5.1.
         * The access network shall not send a ConfigurationRequest message
         * containing the ATSupportedPacketFormats attribute.
         */
        err_no = E_NOT_SUPPORTED;
      }
      else
#endif /* FEATURE_HDR_REVB */

#ifdef FEATURE_HDR_REVA_L1 

      if (subtype == HDRSCP_ENH_FMAC)
      {
        err_no = hdrscmfmac_process_drc_translation_offset( item_ptr, p_aip, a_aip );
      }
      else if (subtype == HDRSCP_DEFAULT_FMAC)

#endif /* FEATURE_HDR_REVA_L1 */
      {
        err_no = hdrscmfmac_process_drc_lock( item_ptr, p_aip, a_aip );
      }
      break;

#ifdef FEATURE_HDR_REVB
    case HDRSCMFMAC_ATTRIB_TENTATIVE_DRC_TO_TX_DRC_MAP:
      err_no = hdrscmfmac_process_tentative_drc_to_tx_drc_map (
                item_ptr,
                p_aip,
                a_aip);
      break;
  
    case HDRSCMFMAC_ATTRIB_EXTENDED_SPANS:
      err_no = hdrscmfmac_process_extended_spans (
                item_ptr,
                p_aip,
                a_aip);
      break;
#endif /* FEATURE_HDR_REVB */

    default:
      HDR_MSG_PROT_1( MSG_LEGACY_HIGH, 
        "Invalid complex attrib: %d", p_aip->attrib_id );

      err_no = E_NOT_SUPPORTED;
  }

  return err_no;  

} /* hdrscmfmac_process_complex_attrib */


 /* EJECT */
/*===========================================================================

FUNCTION HDRSCMFMAC_IS_ATTRIB_SIMPLE

DESCRIPTION
  This function is used by SCM to query if an attribute is simple, given the
  attribute ID.
  
DEPENDENCIES
  None.

PARAMETERS
  attrib_id - Attribute ID 
  subtype   - Subtype negotiated for FMAC.

RETURN VALUE
  TRUE - if the attribute given by 'attrib_id' is simple.
  FALSE - otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
boolean hdrscmfmac_is_attrib_simple
(
  uint16 const attrib_id,
  uint16 const subtype
)
{
  
  return !((attrib_id == HDRSCMFMAC_ATTRIB_HANDOFF_DELAYS) ||
           (attrib_id == HDRSCMFMAC_ATTRIB_DRC_LOCK) 
#ifdef FEATURE_HDR_REVA_L1 
        || (attrib_id == HDRSCMFMAC_ATTRIB_DRC_TRANSLATION_OFFSET)
#endif 
#ifdef FEATURE_HDR_REVB
        || (attrib_id == HDRSCMFMAC_ATTRIB_AT_SUPPORTED_PACKET_FORMATS)
        || (attrib_id == HDRSCMFMAC_ATTRIB_TENTATIVE_DRC_TO_TX_DRC_MAP)
        || (attrib_id == HDRSCMFMAC_ATTRIB_EXTENDED_SPANS)
#endif /* FEATURE_HDR_REVB */
#ifdef FEATURE_HDR_REVC
        || (attrib_id == HDRSCMFMAC_ATTRIB_LOAD_INFO_SUPPORTED)
#endif /* FEATURE_HDR_REVC */
           );

} /* hdrscmfmac_is_attrib_simple */


 /* EJECT */
/*===========================================================================

FUNCTION HDRSCMFMAC_IS_SIMPLE_ATTRIB_VALID

DESCRIPTION
  This function is used by SCM to query if the given 'attrib_value' is a 
  valid value for the attribute specified by 'attrib_id'.
  
DEPENDENCIES
  None.

PARAMETERS
  attrib_id    - Attribute ID of the simple attribute.
  attrib_value - Value of the simple attribute. 
  subtype      - Subtype negotiated for FMAC.

RETURN VALUE
  E_SUCCESS          - If the attrib_value is valid.
  E_OUT_OF_RANGE     - Otherwise.

SIDE EFFECTS
  None.
  
===========================================================================*/
hdrerrno_enum_type hdrscmfmac_is_simple_attrib_valid
(
  uint16 const attrib_id,
  uint32 const attrib_value,
  uint16 const subtype
)
{
  uint8 table_index;
    /* Index into valid_range table */

  hdrerrno_enum_type err_no = E_SUCCESS;
    /* Error Code */
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */ 

  /* Check if the attribute is simple? */
  if (hdrscmfmac_is_attrib_simple(attrib_id, subtype) == FALSE)
  {
    err_no = E_DATA_INVALID;
  }

  if (err_no == E_SUCCESS)
  {

  #ifdef FEATURE_HDR_REVA_L1  
    if ((subtype == HDRSCP_ENH_FMAC )
#ifdef FEATURE_HDR_REVB
         || (subtype == HDRSCP_MC_FMAC)
#endif /* FEATURE_HDR_REVB */
       )
    { 

#ifdef FEATURE_HDR_REVB
      /* Handle the special case */
      if ( (attrib_id == HDRSCMFMAC_ATTRIB_MUP_PREAMBLE_BASE) &&
           (attrib_value != HDRSCMFMAC_DEFAULT_MUP_PREAMBLE_BASE) )
      {
        err_no = E_OUT_OF_RANGE;
      }
      else
#endif /* FEATURE_HDR_REVB */

#ifdef FEATURE_HDR_REVC
      if (attrib_id == HDRSCMFMAC_ATTRIB_LOAD_INFO_SUPPORTED)
      {
        if ((attrib_value == HDRSCMFMAC_LOAD_INFO_SUPPORTED) ||
            (attrib_value == HDRSCMFMAC_LOAD_INFO_UNSUPPORTED))
        {
          err_no = E_SUCCESS;
        }
        else
        {
          err_no = E_OUT_OF_RANGE;
        }
      }
      else
#endif /* FEATURE_HDR_REVC */
      {
        /* Get the table index */
        table_index = HDRSCMFMAC_SIMPLE_ATTRIB_ID_TO_IDX(attrib_id);

        if (table_index < HDRSCMFMAC_MAX_NUM_SIMPLE_ATTRIBS)
        {
          /* Return E_SUCCESS if the attribute value is between min and max defined 
             and E_OUT_OF_RANGE otherwise */ 
          err_no = ( (attrib_value >= valid_range[table_index].min)&&
                     (attrib_value <= valid_range[table_index].max)   
                   )? E_SUCCESS: E_OUT_OF_RANGE;
        }
        else
        {
          err_no = E_DATA_INVALID;
        }
      }
    } /* Subtype is enhanced or MC */
    else
#endif /* FEATURE_HDR_REVA_L1 */
    {

      /* The only simple attributes for default subtype are DRC Gating and 
         NullRateDRC38.4Enable. Since the Min and Max for both these attribs
         are 0x00 and 0x01 respectively, the following generic check should 
         suffice. */
      err_no = ( (attrib_value == 0x00) || ( attrib_value == 0x01) 
               )? E_SUCCESS: E_OUT_OF_RANGE;

    } /* default subtype */

  } /* err_no == E_SUCCESS */

  return err_no;

} /* hdrscmfmac_is_simple_attrib_valid */


 /* EJECT */
/*===========================================================================

FUNCTION HDRSCMFMAC_GET_SIMPLE_ATTRIB_SIZE

DESCRIPTION
  This function is used by SCM to query the simple attrib size for negotiated 
  subtype of FMAC.
  
DEPENDENCIES
  None.

PARAMETERS
  subtype      - Subtype negotiated for FMAC.

RETURN VALUE
  Size of the value ID in bytes.

SIDE EFFECTS
  None.
  
===========================================================================*/
uint8 hdrscmfmac_get_simple_attrib_size
(
  uint16 const attrib_id,
  uint16 const subtype
)
{

  /* Simple attribute size in FMAC is subtype-specific. 
     Ignore the parameter attrib_id. */
  SYS_ARG_NOT_USED(attrib_id);

  /* ValueIDSize = 16 for Default FMAC
     ValueIDSize = 8 for either Enhanced FMAC or MC FMAC */
  return ( (subtype == HDRSCP_DEFAULT_FMAC)? 
           HDRSCMFMAC_DEFAULT_FMAC_VALUE_ID_SIZE:
           HDRSCMFMAC_ENH_FMAC_VALUE_ID_SIZE );

} /* hdrscmfmac_get_simple_attrib_size */


 /* EJECT */
/*===========================================================================

FUNCTION HDRSCMFMAC_GET_ATTRIB_OFFSET_AND_SIZE

DESCRIPTION
  This function computes the byte offset of the attribute specified by the 
  attrib_id in the struct hdrscmfmac_config_struct_type and stores it in 
  out-param attrib_offset. It also stores the size of the attribute in 
  attrib_size.

DEPENDENCIES
  None

PARAMETERS
  attrib_id     - Attribute Id.
  attrib_offset - Address where the Byte Offset from the start of the struct 
                  to the attribute is stored.
  attrib_size   - Address where the size of the attribute is stored.
  subtype       - Subtype negotiated for FMAC.
  
RETURN VALUE
  E_SUCCESS if the given attrib ID is valid.
  E_DATA_INVALID if the attrib ID is not valid

SIDE EFFECTS
  None

===========================================================================*/
hdrerrno_enum_type hdrscmfmac_get_attrib_offset_and_size
(
  uint16 attrib_id, 
  uint16 *attrib_offset,
  uint16 *attrib_size,
  uint16 subtype
)
{
  uint8 attrib_index;
    /* Index got by masking 4 MSBs of attrib id */

  hdrerrno_enum_type err_no = E_SUCCESS;
    /* Return value */

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */ 

  switch ( attrib_id )
  {
    /* 0x00 Hand-off Delays Attribute */
    case HDRSCMFMAC_ATTRIB_HANDOFF_DELAYS:
      *attrib_offset = FPOS(hdrscmfmac_config_struct_type, handoff_delays);
      *attrib_size = sizeof(hdrscmfmac_handoff_delays_struct_type);
      break;

#ifdef FEATURE_HDR_REVB
    /* 0x02 TentativeDRCtoTxDRCMap 
     * This attribute is applicable to multicarrier subtype only and we shouldn't
     * even get here if the negotiated subtype is not MC. */
    case HDRSCMFMAC_ATTRIB_TENTATIVE_DRC_TO_TX_DRC_MAP:
    
      /* If the subtype is not MC, set the err_no to FALSE, so the caller
       * would ignore the offset and size */
      err_no = ( (subtype == HDRSCP_MC_FMAC)? 
                  E_SUCCESS: E_DATA_INVALID );
    
      *attrib_offset = 
        FPOS(hdrscmfmac_config_struct_type, tentative_drc_to_tx_drc_map);
      *attrib_size = 
        sizeof(hdrscmfmac_tentative_drc_to_tx_drc_map_struct_type);
      break;
    
    /* 0x03 ExtendedSpans
     * This attribute is applicable to multicarrier subtype only and we shouldn't
     * even get here if the negotiated subtype is not MC. */
    case HDRSCMFMAC_ATTRIB_EXTENDED_SPANS:
    
      /* If the subtype is not MC, set the err_no to FALSE, so the caller
       * would ignore the offset and size */
      err_no = ( (subtype == HDRSCP_MC_FMAC)? 
                  E_SUCCESS: E_DATA_INVALID );
    
      *attrib_offset = 
        FPOS(hdrscmfmac_config_struct_type, extended_spans);
      *attrib_size = 
        sizeof(hdrscmfmac_extended_spans_struct_type);
      break;
#endif /* FEATURE_HDR_REVB */

    /* 0xFF DRC Gating Attribute */
    case HDRSCMFMAC_ATTRIB_DRC_GATING:
      *attrib_offset = FPOS(hdrscmfmac_config_struct_type, drc_gating);
      *attrib_size = (HDRSCMFMAC_SIMPLE_ATTRIB_DATA_SIZE / 8);
      break;

#ifdef FEATURE_HDR_REVA_L1
    /* 0xFE DRC Lock Length attribute
     * This attribute is applicable to enhanced and multicarrier subtype and we 
     * shouldn't even get here if the negotiated subtype is default. */
    case HDRSCMFMAC_ATTRIB_DRC_LOCK_LENGTH:

      /* If the subtype is Default, set the err_no to FALSE, so the caller
       * would ignore the offset and size */
#ifdef FEATURE_HDR_REVB
      err_no = ( ((subtype == HDRSCP_ENH_FMAC) ||
                  (subtype == HDRSCP_MC_FMAC) )? 
#else /* FEATURE_HDR_REVB */
      err_no = ( (subtype == HDRSCP_ENH_FMAC)? 
#endif /* FEATURE_HDR_REVB */
                  E_SUCCESS: E_DATA_INVALID );

      *attrib_offset = FPOS(hdrscmfmac_config_struct_type, drc_lock_length);
      *attrib_size = (HDRSCMFMAC_SIMPLE_ATTRIB_DATA_SIZE/8);
      break;

#endif /* FEATURE_HDR_REVA_L1 */

    /*----------------------------------------------------------------------
     *                    OVERLOADED ATTRIBUTE IDS
     * The attribute IDs 0x01 and 0xFD are overloaded in FMAC default, 
     * Enhanced, and Multicarrier subtypes to mean different attributes as 
     * given below. 
     * 0x01 - DRC Lock Attribute in Default subtype and
     *        DRC Translation Offset attribute in Enhanced Subtype.
     *        AT Supported Packet Formats in Multicarrier Subtype.
     * 0xFD - Null-Rate DRC 38.4 Enable Attribute in Default subtype.
     *        Multi-user packet Enabled Attribute in Enhanced subtype.
     * It is important to note that all these attributes are applicable only
     * to the corresponding subtypes as stated above.
     *--------------------------------------------------------------------*/ 

    /* 0x01 DRC Lock attribute */
    case HDRSCMFMAC_ATTRIB_DRC_LOCK:
#ifdef FEATURE_HDR_REVB
    /* 0x01 ATSupportedPacketFormats */
    // case HDRSCMFMAC_ATTRIB_AT_SUPPORTED_PACKET_FORMATS:

      /* If the subtype is Multicarrier */
      if (subtype == HDRSCP_MC_FMAC)
      {
        *attrib_offset = 
          FPOS( hdrscmfmac_config_struct_type, at_supported_packet_formats );
        *attrib_size = 
          sizeof(hdrscmfmac_at_supported_packet_formats_struct_type);
      }
      else
#endif /* FEATURE_HDR_REVB */

#ifdef FEATURE_HDR_REVA_L1
    /* 0x01 DRC Translation Offset attribute */
    //case HDRSCMFMAC_ATTRIB_DRC_TRANSLATION_OFFSET:
      
      /* If the subtype is enhanced */ 
      if ( subtype == HDRSCP_ENH_FMAC )
      {
        *attrib_offset = FPOS( hdrscmfmac_config_struct_type,
                               drc_translation_offset );
        *attrib_size = sizeof(hdrscmfmac_drc_translation_offset_type);
      }
      /* If the subtype is default */
      else
#endif /* FEATURE_HDR_REVA_L1 */
      {
        *attrib_offset = FPOS(hdrscmfmac_config_struct_type, drc_lock);
        *attrib_size = sizeof(hdrscmfmac_drc_lock_struct_type);
      }
      break;
    
    /* 0xFD Null-Rate DRC 38.4 Enabled attribute */
    case HDRSCMFMAC_ATTRIB_NULL_RATE_384_ENABLE:
#ifdef FEATURE_HDR_REVA_L1 
    /* 0xFD MUP Enabled attribute */
    //case HDRSCMFMAC_ATTRIB_MUP_ENABLED:
      if ( subtype == HDRSCP_ENH_FMAC )
      {
        *attrib_offset = FPOS(hdrscmfmac_config_struct_type, mup_enabled);
      }
      else
#endif /* FEATURE_HDR_REVA_L1 */
      {
        *attrib_offset = FPOS( hdrscmfmac_config_struct_type, 
                               null_rate_drc_38_4_enable );
      }
      *attrib_size = (HDRSCMFMAC_SIMPLE_ATTRIB_DATA_SIZE/8);
      break;

#ifdef FEATURE_HDR_REVC
    case HDRSCMFMAC_ATTRIB_LOAD_INFO_SUPPORTED:
      *attrib_offset = FPOS(hdrscmfmac_config_struct_type, 
                            load_info_supported);
      *attrib_size = (HDRSCMFMAC_SIMPLE_ATTRIB_DATA_SIZE/8);
      break;
#endif /* FEATURE_HDR_REVC */
    
    /* All attributes with attrib Id between 0xEF-0xFC are handled by 
     * default case */
    default:
#ifdef FEATURE_HDR_REVA_L1
      
      *attrib_size = (HDRSCMFMAC_SIMPLE_ATTRIB_DATA_SIZE/8);

      /* Attrib index is got by masking of the four MSBs */
      attrib_index = HDRSCMFMAC_SIMPLE_ATTRIB_ID_TO_IDX(attrib_id);

#ifdef FEATURE_HDR_REVB
      if ( attrib_index <= 13 )
      {
        /* Attrib offset */
        *attrib_offset = 
          ( FPOS(hdrscmfmac_config_struct_type, 
                 serving_sector_lock_across_sub_active_sets) +
            ( attrib_index * (HDRSCMFMAC_SIMPLE_ATTRIB_DATA_SIZE/8) ) );
      }
#else
      if (attrib_index <= 9)
      {
        /* Attrib offset */
        *attrib_offset = 
          ( FPOS(hdrscmfmac_config_struct_type, drc_boost_length) +
            ( attrib_index * (HDRSCMFMAC_SIMPLE_ATTRIB_DATA_SIZE/8) ) );
      }
#endif /* FEATURE_HDR_REVB */
      else
#endif /* FEATURE_HDR_REVA_L1 */
      {
        ERR("Invalid Attribute Id ", 0,0,0);
        err_no = E_DATA_INVALID;
      }

  } /* switch */
  
  return err_no;

} /* hdrscmfmac_get_attrib_offset_and_size */


/* EJECT */
/*===========================================================================

FUNCTION HDRSCMFMAC_GET_ATTRIB_VERNO_INDEX

DESCRIPTION
  Returns the index of the given attribute in the version number array.

DEPENDENCIES
  None

PARAMETERS
  attrib_id     - Attribute Id.
  
RETURN VALUE
  E_SUCCESS - If the given attribute ID is valid.

SIDE EFFECTS
  None

===========================================================================*/
hdrerrno_enum_type hdrscmfmac_get_attrib_verno_index
(
  uint16 attrib_id,
  uint16 *index,
  uint16 subtype
)
{
  hdrerrno_enum_type err_no = E_SUCCESS;

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */ 

#ifdef FEATURE_HDR_REVC
  if ((subtype != HDRSCP_DEFAULT_FMAC) && 
      (attrib_id == HDRSCMFMAC_ATTRIB_LOAD_INFO_SUPPORTED))
  {
    /* Make LoadInfoSupported a special case because:
       1. There is a gap between 0xE9 and 0xEF
       2. LoadInfoSupported is not limited subtype 2 FMAC
       3. minimum impact to existing logic
     */
    *index = HDRSCMFMAC_MAX_NUM_PRE_REL_B_CONFIG_ATTRIBS + 
             HDRSCMFMAC_SUBTYPE2_MAX_NUM_CONFIG_ATTRIBS - 1;
  }
  else
#endif /* FEATURE_HDR_REVC */
#ifdef FEATURE_HDR_REVB
  if (subtype == HDRSCP_MC_FMAC)
  {
    err_no = hdrscmfmac_get_subtype2_attrib_verno_index(attrib_id, index);
  }
  else
#endif /* FEATURE_HDR_REVB */

  if ( attrib_id == 0x01 )
  {
    *index = (subtype == HDRSCP_DEFAULT_FMAC)? 1: 2;
  }
  else if (attrib_id == 0xFD )
  {
    *index = (subtype == HDRSCP_DEFAULT_FMAC)? 15: 13;
  }
  else if (attrib_id == 0xFF)
  {
    *index = 16;
  }
  else
  {
    *index = attrib_id & HDRSCMFMAC_ATTRIB_ID_TO_INDEX_M;
  }

  return err_no;

} /* hdrscmfmac_get_attrib_verno_index */


/* EJECT */
/*===========================================================================

FUNCTION HDRSCMFMAC_SET_CONFIG_TO_DEFAULT

DESCRIPTION
  Set the configuration attributes to its default values.

DEPENDENCIES
  None

PARAMETERS
  hdrfmac_cfg - Pointer to the config structure.
  
RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
void hdrscmfmac_set_config_to_default
(
  hdrscmfmac_config_struct_type *hdrfmac_cfg
)
{
  uint8 drc;

#ifdef FEATURE_HDR_REVB
  static uint8 const tx_drc_map_default[(HDRHAI_MAX_NUM_REL_B_DRCS-2)] =
  {
    HDRSCMFMAC_DEFAULT_TX_DRC_MAP_01,
    HDRSCMFMAC_DEFAULT_TX_DRC_MAP_02,
    HDRSCMFMAC_DEFAULT_TX_DRC_MAP_03,
    HDRSCMFMAC_DEFAULT_TX_DRC_MAP_04,
    HDRSCMFMAC_DEFAULT_TX_DRC_MAP_05,
    HDRSCMFMAC_DEFAULT_TX_DRC_MAP_06,
    HDRSCMFMAC_DEFAULT_TX_DRC_MAP_07,
    HDRSCMFMAC_DEFAULT_TX_DRC_MAP_08,
    HDRSCMFMAC_DEFAULT_TX_DRC_MAP_09,
    HDRSCMFMAC_DEFAULT_TX_DRC_MAP_0A,
    HDRSCMFMAC_DEFAULT_TX_DRC_MAP_0B,
    HDRSCMFMAC_DEFAULT_TX_DRC_MAP_0C,
    HDRSCMFMAC_DEFAULT_TX_DRC_MAP_0D,
    HDRSCMFMAC_DEFAULT_TX_DRC_MAP_0E,
    HDRSCMFMAC_DEFAULT_TX_DRC_MAP_10,
    HDRSCMFMAC_DEFAULT_TX_DRC_MAP_11,
    HDRSCMFMAC_DEFAULT_TX_DRC_MAP_12,
    HDRSCMFMAC_DEFAULT_TX_DRC_MAP_13,
    HDRSCMFMAC_DEFAULT_TX_DRC_MAP_14,
    HDRSCMFMAC_DEFAULT_TX_DRC_MAP_15,
    HDRSCMFMAC_DEFAULT_TX_DRC_MAP_16,
    HDRSCMFMAC_DEFAULT_TX_DRC_MAP_17,
    HDRSCMFMAC_DEFAULT_TX_DRC_MAP_18,
    HDRSCMFMAC_DEFAULT_TX_DRC_MAP_19,
    HDRSCMFMAC_DEFAULT_TX_DRC_MAP_1A,
    HDRSCMFMAC_DEFAULT_TX_DRC_MAP_1B
  };
#endif /* FEATURE_HDR_REVB */

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */ 

  hdrfmac_cfg->drc_gating = HDRSCMFMAC_DEFAULT_DRC_GATING;
  hdrfmac_cfg->null_rate_drc_38_4_enable = 
    HDRSCMFMAC_DEFAULT_NULL_RATE_DRC_384_ENABLED;
  hdrfmac_cfg->drc_lock.period = HDRSCMFMAC_DEFAULT_DRC_LOCK_PERIOD;
  hdrfmac_cfg->drc_lock.length = HDRSCMFMAC_DEFAULT_DRC_LOCK_LENGTH;
  hdrfmac_cfg->handoff_delays.soft_handoff_delay = 
    HDRSCMFMAC_DEFAULT_SOFT_HANDOFF_DELAYS;
  hdrfmac_cfg->handoff_delays.softer_handoff_delay = 
    HDRSCMFMAC_DEFAULT_SOFTER_HANDOFF_DELAYS;

#ifdef FEATURE_HDR_REVA_L1
  hdrfmac_cfg->drc_lock_length = HDRSCMFMAC_DEFAULT_ST1_DRC_LOCK_LENGTH;
  hdrfmac_cfg->mup_enabled = HDRSCMFMAC_DEFAULT_MUP_ENABLED;
  hdrfmac_cfg->dsc_length = HDRSCMFMAC_DEFAULT_DSC_LENGTH;
  hdrfmac_cfg->delta_mup_ack_chan_gain = 
    HDRSCMFMAC_DEFAULT_DELTA_ACK_CHAN_GAIN_MUP;
  hdrfmac_cfg->short_pkt_enabled_thresh = 
    HDRSCMFMAC_DEFAULT_SHORT_PKT_ENABLED_THRESH;
  hdrfmac_cfg->su_mux_pkt_enabled = HDRSCMFMAC_DEFAULT_SU_MUX_PKTS_ENABLED;
  hdrfmac_cfg->drc_supervision_timer = HDRSCMFMAC_DEFAULT_DRC_SUPERVISION_TIMER;
  hdrfmac_cfg->min_cont_span = HDRSCMFMAC_DEFAULT_MIN_CONTN_SPAN;
  hdrfmac_cfg->dsc_chan_gain_boost = HDRSCMFMAC_DEFAULT_DSC_CHAN_GAIN_BOOST;
  hdrfmac_cfg->dsc_boost_length = HDRSCMFMAC_DEFAULT_DSC_BOOST_LENGTH;
  hdrfmac_cfg->drc_chan_gain_boost = HDRSCMFMAC_DEFAULT_DRC_CHAN_GAIN_BOOST;
  hdrfmac_cfg->drc_boost_length = HDRSCMFMAC_DEFAULT_DRC_BOOST_LENGTH;

#ifdef FEATURE_HDR_REVB

  /* SupportGAUPMUPPreambleBase */
  hdrfmac_cfg->support_gaup_mup_preamble_base = 
    HDRSCMFMAC_DEFAULT_SUPPORT_GAUP_MUP_PREAMBLE_BASE;

  /* MUPPreambleBase */
  hdrfmac_cfg->mup_preamble_base =
    HDRSCMFMAC_DEFAULT_MUP_PREAMBLE_BASE;

  /* SupportGAUPDeltaACKChannelGainMUP */
  hdrfmac_cfg->support_gaup_delta_ack_channel_gain_mup =
    HDRSCMFMAC_DEFAULT_SUPPORT_GAUP_DELTA_ACK_CHANNEL_GAIN_MUP;

  /************************
   *
   * Complex Attribute
   *
   ************************/

  /* ExtendedSpans */
  hdrfmac_cfg->extended_spans.extended_spans_enabled = 
    HDRSCMFMAC_DEFAULT_EXTENDED_SPANS_ENABLED;

  /* TentativeDRCtoTxDRCMap */
  for (drc = 0; drc < (HDRHAI_MAX_NUM_REL_B_DRCS-2); ++drc)
  {
    hdrfmac_cfg->tentative_drc_to_tx_drc_map[drc] = 
      tx_drc_map_default[drc];
  }

  /* ATSupportedPacketFormats */
  hdrfmac_cfg->at_supported_packet_formats.max_optional_data_rate =
    HDRSCMFMAC_DEFAULT_MAX_OPTIONAL_DATA_RATE;
  hdrfmac_cfg->at_supported_packet_formats.max_optional_payload_size =
    HDRSCMFMAC_DEFAULT_MAX_OPTIONAL_PAYLOAD_SIZE;
  hdrfmac_cfg->at_supported_packet_formats.short_pkt_enabled_thresh =
    HDRSCMFMAC_DEFAULT_SHORT_PKT_ENABLED_THRESH;

#endif /* FEATURE_HDR_REVB */

  for (drc = 0; drc < HDRHAI_MAX_NUM_REL_A_DRCS-1; ++drc)
  {
    hdrfmac_cfg->drc_translation_offset[drc] = 
      HDRSCMFMAC_DEFAULT_DRC_TRANS_OFFSET;
  }

#endif /* FEATURE_HDR_REVA_L1 */

} /* hdrscmfmac_set_config_to_default */


 /* EJECT */
/*===========================================================================

FUNCTION HDRSCMFMAC_GET_PROP_ATTRIB_VALUE

DESCRIPTION
  The function is used by SCM to get the proposed complex attribute value.

DEPENDENCIES
  None

PARAMETERS
  trans_id - Transaction ID of the Configuration message.
  a_aip    - Accepted attribute information pointer.
  subtype  - Subtype of the protocol for which the message is destined.
  
RETURN VALUE
  E_SUCCESS - If the subtype of the procotol supports AT-init negotiation 
              and if there was a valid outstanding transaction.
  E_FAILURE - If the trans_id does not match to any outstanding transaction.
  E_NOT_SUPPORTED - If the subtype does not support AT-init Neg.

SIDE EFFECTS
  None

===========================================================================*/
hdrerrno_enum_type hdrscmfmac_get_prop_attrib_value
(
  uint16 const trans_id,
  hdrscm_accepted_attrib_type *a_aip,
  uint16 const subtype
)
{
  hdrerrno_enum_type err_no = E_FAILURE;
    /* Error Code */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  switch(subtype)
  {
#ifdef FEATURE_HDR_REVB
    /* Subtype 2 FTCMAC */
    case HDRSCP_MC_FMAC:
      if (trans_id == hdrscmfmac.trans_id-1)
      {
        switch(a_aip->attrib_id)
        {
          case HDRSCMFMAC_ATTRIB_AT_SUPPORTED_PACKET_FORMATS:
            a_aip->value_id = hdrscmfmac.at_supported_packet_formats_value_id;
            a_aip->complex_val_ptr = 
              (void *)&(hdrscmfmac.at_supported_packet_formats);
            a_aip->complex_val_size = 
              sizeof(hdrscmfmac.at_supported_packet_formats);
            a_aip->is_accepted = TRUE;
            err_no = E_SUCCESS;
            break;
  
#ifdef FEATURE_HDR_REVC
          case HDRSCMFMAC_ATTRIB_LOAD_INFO_SUPPORTED:
            a_aip->value_id = hdrscmfmac.load_info_supported;
            a_aip->is_accepted = TRUE;
            err_no = E_SUCCESS;
            break;
#endif /* FEATURE_HDR_REVC */
  
          default:
          /* unsupported attrib_id */
          HDR_MSG_PROT_1(MSG_LEGACY_ERROR, 
              "FMAC2: Attrib id %d not supported", a_aip->attrib_id);
          break;
        }
      }
      else
      {
        HDR_MSG_PROT_2(MSG_LEGACY_ERROR, 
            "FMAC2: trans_id %d mismatched proposed trans_id %d",
             trans_id, (uint8)(hdrscmfmac.trans_id-1));
      }
      break;

#ifdef FEATURE_HDR_REVC
    case HDRSCP_ENH_FMAC:
      if (trans_id == hdrscmfmac.trans_id-1)
      {
        switch(a_aip->attrib_id)
        {
          case HDRSCMFMAC_ATTRIB_LOAD_INFO_SUPPORTED:
            a_aip->value_id = hdrscmfmac.load_info_supported;
            a_aip->is_accepted = TRUE;
            err_no = E_SUCCESS;
            break;
          default:
          /* unsupported attrib_id */
            HDR_MSG_PROT_1(MSG_LEGACY_ERROR, 
              "FMAC2: Attrib id %d not supported", a_aip->attrib_id);
            err_no = E_NOT_SUPPORTED;
            break;
        }
      }
      else
      {
        HDR_MSG_PROT_2(MSG_LEGACY_ERROR, 
            "FMAC1: trans_id %d mismatched proposed trans_id %d",
             trans_id, (uint8)(hdrscmfmac.trans_id-1));
      }
      break;
#endif /* FEATURE_HDR_REVC */

#endif /* FEATURE_HDR_REVB */

    default:
      ERR("FMAC Sub: %d does not support AT-init proposal", subtype,0,0);
      err_no = E_NOT_SUPPORTED;
  }

  return err_no;

} /* hdrscmrmac_get_prop_attrib_value */


/* <EJECT> */
/*===========================================================================

FUNCTION HDRSCMFMAC_FAIL_CURR_CONFIG_NEG

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
void hdrscmfmac_fail_curr_config_neg
(
  hdrhai_session_close_reason_enum_type close_reason
)
{
  uint16 subtype;
    /* Protocol subtype */

  hdrind_ind_data_union_type ind_data;

  /* Allocate out of the heap because it is possible for this indication to
  be queued from a different task context. Otherwise, the payload may be
  corrupted which could cause a null pointer de-reference. */
  static hdrscp_scp_fail_ind_type session_fail_data;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  subtype = hdrscp_get_config_subtype(HDRHAI_FTC_MAC_PROTOCOL);

  switch(subtype)
  {
#ifdef FEATURE_HDR_REVB
    /* Subtype 2 FTCMAC */
    case HDRSCP_MC_RMAC:
      /* fill the indication data */
      session_fail_data.type = HDRHAI_FTC_MAC_PROTOCOL;
      session_fail_data.subtype = HDRSCP_MC_FMAC;
      session_fail_data.attribute_id_length = 
        HDRSCMFMAC_AT_SUPPORTED_PKT_FORMATS_BYTE_LENGTH;
      session_fail_data.attribute_id = 
        HDRSCMFMAC_ATTRIB_AT_SUPPORTED_PACKET_FORMATS;
      session_fail_data.reason = close_reason;
  
      ind_data.scp_fail_ptr = (void *)&session_fail_data;
  
      hdrind_give_ind(HDRIND_FMAC_PROT_NEG_FAILED,(void *)&ind_data);
      break;
#endif /* FEATURE_HDR_REVB */

    default:
      ERR("RMAC Sub: %d does not support AT-init proposal", subtype,0,0);
  }

} /* hdrscmrmac_fail_curr_config_neg */


 /* EJECT */
/*===========================================================================

FUNCTION HDRSCMFMAC_IS_ATTRIB_GAUPABLE

DESCRIPTION
  This function is used by SCM module to find out if an attribute is GAUPable.

DEPENDENCIES
  None.

PARAMETERS
  attrib_id - Attribute ID.
  subtype - Protocol subtype.
  
RETURN VALUE
  E_SUCCESS       - If the attribute is GAUPable.
  E_NOT_ALLOWED   - If AN is not allowed to GAUP this attribute.

SIDE EFFECTS
  None.
===========================================================================*/
hdrerrno_enum_type hdrscmfmac_is_attrib_gaupable
(
  uint16 const attrib_id,
  uint16 const subtype
)
{
  
  uint8 gaup_ind = 0;
    /* Loop index */

  hdrerrno_enum_type err_no = E_NOT_ALLOWED;
    /* Error code */

  boolean            support_gaup_delta_ack_channel_gain_mup;
    /* SupportGAUPDeltaACKChannelGainMUP Attribute Value */

  boolean            support_gaup_mup_preamble_base;
    /* SupportGAUPMUPPreambleBase Attribute Value */

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  switch (subtype)
  {
    case HDRSCP_ENH_FMAC:
      /* Loop around the GAUPable attribute ID LUT to check if the given 
         attrib_id is one of them */
      for (gaup_ind = 0; (gaup_ind < HDRSCMFMAC_MAX_NUM_GAUPABLE_ATTRIBS) &&
                         (err_no != E_SUCCESS); ++gaup_ind)
      {
        if (attrib_id == hdrscmfmac_gaup_attrib_ids[gaup_ind])
        {
          /* attrib_id is GAUPable */
          err_no = E_SUCCESS;
        }
      }
      break;

#ifdef FEATURE_HDR_REVB
    case HDRSCP_MC_FMAC:
      if (attrib_id == HDRSCMFMAC_ATTRIB_MUP_PREAMBLE_BASE)
      {
        /* Depending on the value of SupportGAUPMUPPreambleBase */
        (void) hdrscmdb_get_inuse_attrib(
          HDRHAI_FTC_MAC_PROTOCOL,
          HDRSCMFMAC_ATTRIB_SUPPORT_GAUP_MUP_PREAMBLE_BASE,
          &support_gaup_mup_preamble_base,
          HDRSCP_MC_FMAC);

        if (support_gaup_mup_preamble_base)
        {
          err_no = E_SUCCESS;
        }
      }
      else if (attrib_id == HDRSCMFMAC_ATTRIB_DELTA_ACK_CHAN_GAIN_MUP)
      {
        /* Depending on the value of SupportGAUPDeltaACKChannelGainMUP */
        (void) hdrscmdb_get_inuse_attrib(
          HDRHAI_FTC_MAC_PROTOCOL,
          HDRSCMFMAC_ATTRIB_SUPPORT_GAUP_DELTA_ACK_CHANNEL_GAIN_MUP,
          &support_gaup_delta_ack_channel_gain_mup,
          HDRSCP_MC_FMAC);

        if (support_gaup_delta_ack_channel_gain_mup)
        {
          err_no = E_SUCCESS;
        }
      }
      else /* all other attributes */
      {
        /* Loop around the GAUPable attribute ID LUT to check if the given 
           attrib_id is one of them */
        for (gaup_ind = 0; (gaup_ind < HDRSCMFMAC_SUBTYPE2_MAX_NUM_GAUPABLE_ATTRIBS) &&
                           (err_no != E_SUCCESS); ++gaup_ind)
        {
          if (attrib_id == hdrscmfmac_subtype2_gaup_attrib_ids[gaup_ind])
          {
            /* attrib_id is GAUPable */
            err_no = E_SUCCESS;
          }
        }
      }
      break;
#endif /* FEATURE_HDR_REVB */

    case HDRSCP_DEFAULT_FMAC:
    default:
      /* Nothing to be done. Just return E_NOT_ALLOWED */
      break;
  }

  return err_no;

} /* hdrscmfmac_is_attrib_gaupable */


/* EJECT */
/*===========================================================================

FUNCTION HDRSCMFMAC_ATTRIB_IS_SUPPORTED

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
boolean hdrscmfmac_attrib_is_supported
(
  uint16 const attrib_id,
  uint16 const subtype
)
{
  boolean known = FALSE;

/*- - - - - - - - - - - - -- - - - - - - - - - - - - - - - - - - - - - - - -*/

  switch(subtype)
  {
    case HDRSCP_DEFAULT_FMAC:

      known = ((attrib_id == HDRSCMFMAC_ATTRIB_DRC_GATING) ||
               (attrib_id == HDRSCMFMAC_ATTRIB_NULL_RATE_384_ENABLE) ||
               (attrib_id == HDRSCMFMAC_ATTRIB_DRC_LOCK) ||
               (attrib_id == HDRSCMFMAC_ATTRIB_HANDOFF_DELAYS));
      break;

    case HDRSCP_ENH_FMAC:

      known = (((attrib_id >= HDRSCMFMAC_ATTRIB_DRC_BOOST_LENGTH) &&
                (attrib_id <= HDRSCMFMAC_ATTRIB_DRC_GATING)) ||
               (attrib_id == HDRSCMFMAC_ATTRIB_HANDOFF_DELAYS) ||
               (attrib_id == HDRSCMFMAC_ATTRIB_DRC_TRANSLATION_OFFSET)
#ifdef FEATURE_HDR_REVC
                || (attrib_id == HDRSCMFMAC_ATTRIB_LOAD_INFO_SUPPORTED &&
                    hdrmc_feature_is_enabled(
                      HDRMC_FEATURE_NETWORK_LOAD_BALANCING))
#endif /* FEATURE_HDR_REVC */
              );

      break;

#ifdef FEATURE_HDR_REVB
    /* (attrib_id >= HDRSCMFMAC_ATTRIB_HANDOFF_DELAYS) commented out to avoid 
	     Warning: #186-D: pointless comparison of unsigned integer with zero  */
    case HDRSCP_MC_FMAC:
      known = (((attrib_id >= 
                 HDRSCMFMAC_ATTRIB_SERVING_SECTOR_LOCK_ACROSS_SUB_ACTIVE_SETS) &&
                (attrib_id <= HDRSCMFMAC_ATTRIB_DRC_SUPERVISION_TIMER)) ||
               (attrib_id == HDRSCMFMAC_ATTRIB_DELTA_ACK_CHAN_GAIN_MUP) ||
               (attrib_id == HDRSCMFMAC_ATTRIB_DSC_LENGTH)              ||
               (attrib_id == HDRSCMFMAC_ATTRIB_DRC_LOCK_LENGTH)         ||
               (attrib_id == HDRSCMFMAC_ATTRIB_DRC_GATING)              ||
               (/* (attrib_id >= HDRSCMFMAC_ATTRIB_HANDOFF_DELAYS) && */
                (attrib_id <= HDRSCMFMAC_ATTRIB_EXTENDED_SPANS))        
#ifdef FEATURE_HDR_REVC
                || (attrib_id == HDRSCMFMAC_ATTRIB_LOAD_INFO_SUPPORTED &&
                    hdrmc_feature_is_enabled(
                      HDRMC_FEATURE_NETWORK_LOAD_BALANCING))
#endif
              );
      break;
#endif /* FEATURE_HDR_REVB */

    default:
      HDR_MSG_PROT_1(MSG_LEGACY_ERROR, 
                     "FMAC subtype 0x%x not supported", subtype);
  }

  if (!known)
  {
    HDR_MSG_PROT_1(MSG_LEGACY_ERROR, 
                   "FMAC attribute 0x%x not supported", attrib_id);
  }

  return known;

} /* hdrscmfmac_attrib_is_supported */


 /* EJECT */
/*===========================================================================
FUNCTION HDRSCMFMAC_CONFIG_IS_NEEDED                                 EXTERNAL

DESCRIPTION
  This function is used to find out if an AT-initiated Configuration is 
  needed by FMAC2. 

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
boolean hdrscmfmac_config_is_needed( void )
{
  boolean config_is_needed = FALSE;
    /* AT-init config needed for FMAC? */

  uint16 inuse_st, inconfig_st;
    /* RMAC Subtype */

#ifdef FEATURE_HDR_REVC
  uint32 load_info_supported;
#endif /* FEATURE_HDR_REVC */
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* First check if the InConfig subtype is the same as the InUse one */
  inuse_st = hdrscp_get_current_subtype(HDRHAI_FTC_MAC_PROTOCOL);
  inconfig_st = hdrscp_get_config_subtype(HDRHAI_FTC_MAC_PROTOCOL);

  if (inconfig_st == HDRSCP_DEFAULT_FMAC)
  {
    config_is_needed = FALSE;
  }
  else if ((inuse_st != inconfig_st) &&
           (inconfig_st == HDRSCP_MC_FMAC))
  {
    /* 
     * If FTCMAC InUse subtype is the same as InConfig subtype, don't need
     * to initiate AT-init configuration; otherwise, we need to propose
     * ATSupportPacketFormats.
     */
    config_is_needed = TRUE;
  }
#ifdef FEATURE_HDR_REVC
  else
  {
    /* Propose LoadInfo if inconfig subtype is non-default,
     * and LoadInfoSupported is disabled.
     */
    hdrscmdb_get_inuse_attrib(HDRHAI_FTC_MAC_PROTOCOL,
                              HDRSCMFMAC_ATTRIB_LOAD_INFO_SUPPORTED,
                              &load_info_supported, inuse_st);

    if (load_info_supported != HDRSCMFMAC_LOAD_INFO_SUPPORTED &&
        hdrmc_feature_is_enabled(HDRMC_FEATURE_NETWORK_LOAD_BALANCING))
    {
      config_is_needed = TRUE;
    }
  }
#endif
  return config_is_needed;

} /* hdrscmfmac_config_is_needed */


 /* EJECT */
/*===========================================================================
FUNCTION HDRSCMFMAC_CONFIGURE                                        EXTERNAL

DESCRIPTION
  This function does the AT-init configuration of FMAC2 attributes. 

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
boolean hdrscmfmac_configure( void )
{
  uint8 timer_id;
      /* Reserved timer Id */

  dsm_item_type * send_msg_ptr = NULL;
    /* Pointer to Config req. message to send */

  uint8 attrib_offset = 0;
    /* Attribute field offset from start of DSM item */

  boolean status = TRUE;
    /* Return code */

  uint8 sub_type = hdrscp_get_config_subtype(HDRHAI_FTC_MAC_PROTOCOL);

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

#ifdef FEATURE_HDR_REVC
  if (sub_type == HDRSCP_DEFAULT_FMAC)
#else
  if (sub_type != HDRSCP_MC_FMAC)
#endif /* FEATURE_HDR_REVC */
  {
    return FALSE;
  }

  /* Reserve a Response timer for this Config Request message  */
  if ( (hdrscm_reserve_rsp_timer(HDRHAI_FTC_MAC_PROTOCOL, &timer_id, 
                           hdrscmfmac.trans_id, FALSE)) != E_SUCCESS)
  {
    /* No free timer was available. */
    status = FALSE;
  }

  if (status)
  {
    
    /* Get a new DSM buffer */
    send_msg_ptr = hdrutil_new_msg_buffer();    

    if (send_msg_ptr == NULL)
    {
      HDR_MSG_PROT(MSG_LEGACY_ERROR, "Can't alloc msg buf for FMAC ConReq");
      return FALSE;
    }

    /* Pack AttribUpdateRequest message's Message ID */
    HDRBIT_PACK8(send_msg_ptr, HDRSCM_CONFIG_REQ_MSG,
                 attrib_offset, HDRSCM_MESSAGE_ID_SIZE );

    /* Shift the offset to start of Transaction ID */
    attrib_offset += HDRSCM_MESSAGE_ID_SIZE;

    /* Pack the message's Transaction ID */
    HDRBIT_PACK8(send_msg_ptr, hdrscmfmac.trans_id, attrib_offset, 
                 HDRSCM_TRANS_ID_SIZE);

    /* Shift the offset to start of Attribute Record's Length */
    attrib_offset += HDRSCM_TRANS_ID_SIZE;

#ifdef FEATURE_HDR_REVC
    if (sub_type != HDRSCP_DEFAULT_FMAC &&
        hdrmc_feature_is_enabled(HDRMC_FEATURE_NETWORK_LOAD_BALANCING))
    {
      hdrscmfmac_pack_load_info_supported(send_msg_ptr,
                                          &attrib_offset);
    }
#endif /* FEATURE_HDR_REVC */

#ifdef FEATURE_HDR_REVB
    if (sub_type == HDRSCP_MC_FMAC)
    {
      hdrscmfmac_pack_at_supported_packet_formats(send_msg_ptr,
                                                  &attrib_offset);
    }
#endif /* FEATURE_HDR_REVB */

    /* Send ConfigRequest msg on RTC. All config messages are reliable. */
    hdrhmp_send_msg(HDRHAI_FTC_MAC_PROTOCOL,
                    HDRHAI_RTC,
                    TRUE,
                    HDRSCM_CONFIG_REQ_MSG_PRI,
                    HDRHAI_IN_CONFIG,
                    send_msg_ptr,
                    hdrscm_tx_outcome_cb,
                    (void*) ((dword) timer_id) );


    /* Increment the transaction ID for next transaction */ 
    ++hdrscmfmac.trans_id;
  } 

  return status;
} /* hdrscmfmac_configure */

#ifdef FEATURE_HDR_REVC
/*===========================================================================
 
FUNCTION HDRSCMFMAC_GAUP_LOAD_INFO_SUPPORTED

DESCRIPTION
  This function updates LoadInformationSupported attribute. 

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  E_SUCCESS       - If a GAUP Update was sent successfully.
  E_NOT_AVAILABLE - If we didn't have a free Rsp timer or free msg buffer
                    to send this GAUP update.
  E_FAILURE       - If NLB feature is not enabled.
 
SIDE EFFECTS
  None.

===========================================================================*/
hdrerrno_enum_type hdrscmfmac_gaup_load_info_supported( void )
{
  uint8 timer_id;
    /* Reserved timer Id */

  dsm_item_type * send_msg_ptr = NULL;
    /* Pointer to message buffer */

  uint8 attrib_offset = 0;
    /* Attribute field offset from start of DSM item */

  hdrerrno_enum_type err_no = E_SUCCESS;
    /* Error Code */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (!hdrmc_feature_is_enabled(HDRMC_FEATURE_NETWORK_LOAD_BALANCING))
  {
    return E_FAILURE;
  }

  HDR_MSG_PROT(MSG_LEGACY_HIGH, 
               "AT-init LoadInfoSupported(0xe9) update req");

  /* Reserve a Response timer for this Config Request message */
  if ( (err_no = hdrscm_reserve_rsp_timer(HDRHAI_FTC_MAC_PROTOCOL, &timer_id, 
                           ++hdrscmfmac.load_info_gaup_tid, FALSE)) != E_SUCCESS )
  {
    /* No free timer was available */
    HDR_MSG_PROT(MSG_LEGACY_ERROR, 
                 "Don't have a free Rsp timer for LoadInfoSupported update req");
    return err_no;
  }

  /* Get a new DSM buffer */
  send_msg_ptr = hdrutil_new_msg_buffer();    

  if (send_msg_ptr == NULL)
  {
    HDR_MSG_PROT(MSG_LEGACY_ERROR, 
                 "Can't alloc msg buffer for LoadInfoSupported update req");
    return E_NOT_AVAILABLE;
  }

  /* Pack AttribUpdateRequest message's Message ID */
  HDRBIT_PACK8(send_msg_ptr, HDRSCM_ATTRIB_UPDATE_REQ_MSG,
               attrib_offset, HDRSCM_MESSAGE_ID_SIZE);

  /* Shift the offset to start of Transaction ID */
  attrib_offset += HDRSCM_MESSAGE_ID_SIZE;

  /* Pack the message's Transaction ID */
  HDRBIT_PACK8(send_msg_ptr, hdrscmfmac.load_info_gaup_tid, attrib_offset, 
               HDRSCM_TRANS_ID_SIZE);

  /* Shift the offset to start of Attribute Record's Length */
  attrib_offset += HDRSCM_TRANS_ID_SIZE;

  /* Set LoadInformationSupported attribute record */
  hdrscmfmac_pack_load_info_supported(send_msg_ptr,
                                      &attrib_offset);

  /* Send AttributeUpdateRequest msg on RTC */
  hdrhmp_send_msg(HDRHAI_FTC_MAC_PROTOCOL,
                  HDRHAI_RTC,
                  TRUE,
                  HDRSCMFMAC_ATTRIB_UPDATE_REQ_MSG_PRIO,
                  HDRHAI_IN_USE,
                  send_msg_ptr,
                  hdrscm_tx_outcome_cb,
                  (void*) ((dword) timer_id) );

  return err_no;
}


/*===========================================================================
 
FUNCTION HDRSCMFMAC_SET_LOAD_INFO_ATTRIB_REJECTED

DESCRIPTION
  This function resets the indicator which shows whether
  LoadInformationSupported attribute is rejected by the current
  serving sector. 

DEPENDENCIES
  None.

PARAMETERS
  load_info_attrib_rejected - TRUE, if LoadInformationSupported attribute
                              proposed has been rejected by the current serving
                              sector. FALSE, if not.
 
RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void hdrscmfmac_set_load_info_attrib_rejected
(
  boolean load_info_attrib_rejected 
)
{
  hdrscmfmac.load_info_attrib_rejected = load_info_attrib_rejected;
}

/*===========================================================================
 
FUNCTION HDRSCMFMAC_IS_LOAD_INFO_ATTRIB_REJECTED

DESCRIPTION
  This function tells whether LoadInformationSupported attribute has been
  rejected by the current serving sector. 

DEPENDENCIES
  None.

PARAMETERS
  None.
 
RETURN VALUE
  TRUE  - LoadInformationSupported attribute request has been
          rejected by the current serving sector.
  FALSE - LoadInformationSupported attribute request has not
          been rejected by the current serving sector yet.

SIDE EFFECTS
  None.
 
===========================================================================*/
boolean hdrscmfmac_is_load_info_attrib_rejected( void )
{
  return hdrscmfmac.load_info_attrib_rejected;
}


/*=========================================================================== 

FUNCTION HDRSCMFMAC_RESET_LOAD_INFO_GAUP_CONDITIONS

DESCRIPTION
  Reset the variables which control the updating of
  LoadInformationSupported attribute.

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
 
===========================================================================*/
void hdrscmfmac_reset_load_info_gaup_conditions( void )
{
  HDR_MSG_PROT( MSG_LEGACY_MED, 
                "Reset LoadInfoSupported(0xe9) ctrl conditions" );

  hdrscmfmac_set_load_info_attrib_rejected(FALSE);

  hdrfmac_set_load_info_msg_rcvd(FALSE);
}

 /* EJECT */
/*===========================================================================

FUNCTION HDRSCMFMAC_GET_GAUPED_ATTRIB_VALUE

DESCRIPTION
  This function is used by SCM to get the attribute value proposed by
  AT as GAUP update.

DEPENDENCIES
  None.

PARAMETERS
  trans_id      - Transaction ID.
  a_aip         - Array of Accepted attribute information where the info about
                  each of the attributes in the transaction specified by 
                  trans_id are stored.
  attrib_count  - Number of attributes GAUPed in this transaction.
  subtype       - Subtype negotiated for FMAC.
 
RETURN VALUE
  E_SUCCESS       - If the given transaction ID is valid.
  E_NOT_SUPPORTED - If the current subtype is not allowed to receive any GAUP
                    update response message. Currently FMAC only GAUPs
                    LoadInfoSupported and only when RevC is enabled.

SIDE EFFECTS
  None.

===========================================================================*/
hdrerrno_enum_type hdrscmfmac_get_gauped_attrib_value
(
  uint8 const trans_id,
  dsm_item_type **aai_dsm,
  uint8 *const p_attrib_count,
  uint16 const subtype
)
{

  hdrerrno_enum_type err_no = E_NOT_SUPPORTED;
    /* Error Code */

  hdrscm_accepted_attrib_type gauped_attribs;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  switch(subtype)
  {
    case HDRSCP_MC_FMAC:
    case HDRSCP_ENH_FMAC:
      /* Currently the AT only GAUP LoadInfoSupported for FMAC, so some
         simplification can be done. If new GAUPable attributes are added,
         a more full-scale implementation would be needed. */
      if (trans_id == hdrscmfmac.load_info_gaup_tid)
      {
        gauped_attribs.attrib_id = HDRSCMFMAC_ATTRIB_LOAD_INFO_SUPPORTED;
        gauped_attribs.attrib_id_size = HDRSCMFMAC_ATTRIB_ID_SIZE;
        gauped_attribs.complex_val_ptr = &hdrscmfmac.load_info_supported;
        gauped_attribs.complex_val_size = HDRSCMFMAC_ENH_FMAC_VALUE_ID_SIZE;
        gauped_attribs.is_accepted = TRUE;
        *p_attrib_count = 1;

        (void) dsm_pushdown(aai_dsm, &gauped_attribs, 
                            sizeof(hdrscm_accepted_attrib_type), 
                            DSM_DS_SMALL_ITEM_POOL);

        err_no = E_SUCCESS;
      }
      else
      {
        HDR_MSG_PROT_2(MSG_LEGACY_ERROR, "FMAC GaupRsp trans_id mismatch %d != %d",
                       trans_id, hdrscmfmac.load_info_gaup_tid);
      }
      break;

    default:
      break;
  }

  return err_no;

} /* hdrscmrmac_get_gauped_complex_attrib_value */
#endif /* FEATURE_HDR_REVC */
