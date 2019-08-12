/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

      H D R   S E S S I O N   C O N F I G U R A T I O N   M A N A G E R

          S U B T Y P E - 3  R E V E R S E  C H A N N E L  M A C  

                              P R O T O C O L

       
GENERAL DESCRIPTION
  This module provides supporting API for SCM module to process
  ConfigurationRequest messages and AttributeUpdateRequest messages directed
  to Subtype-3 Reverse Traffic channel MAC protocol. 
    
EXTERNALIZED FUNCTIONS
  hdrscmrmac3_process_complex_attrib 
    Processes all RMAC ST-3 complex attributes 
    
  hdrscmrmac3_is_attrib_simple 
    Returns true if an attribute is simple

  hdrscmrmac3_is_simple_attrib_valid
    Verifies the validity of the simple attribute

  hdrscmrmac3_get_attrib_offset_and_size
    Returns attribute size and offset into the config structure.

  hdrscmrmac3_get_attrib_verno_index
    Returns the index of an attribute in the version number array. 
   
INITIALIZATION AND SEQUENCING REQUIREMENTS

  
  Copyright (c) 2005 - 2016 by Qualcomm Technologies, Inc.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                           EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/hdr/cp/src/hdrscmrmac3.c#1 $ $DateTime: 2016/12/13 07:58:05 $ $Author: mplcsds1 $
   
when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/09/16   vko     Reduce excessive F3 logging
04/07/14   vko     Fixed klocwork issues
01/30/14   vko     Fixed improper handling of the Flow Count values by AT
09/19/13   ukl     Reduce Code Duplication HDR-CP
12/04/08   etv     Lint fixes.
12/04/08   pba     Lint and Compiler warning fixes
08/22/07   etv     Fixed high lint warnings.
08/01/07   rmg     Fixed lint warnings.
06/14/07   wsh     Moved value_id comparision to SCM instead of individual 
                   modules when handling get_prop_attrib()
02/22/07   etv     Fixed reserved bits calculation for complex attributes.
02/15/07   etv     Fixed bug with Pilot Strength default value initialization.
02/17/07   wsh     Added value_id/attrib_id checking processing ConfigResp
02/05/07   etv     Fixed parsing error with MaxNumSubPackets attribute.
02/05/07   etv     Moved memset config to zero from hdrscmrmac3 to hdrscmrmac
01/03/07   etv     Added support to handle unknown attributes.
09/11/06   etv     Added support for DUP checking in axis attribs.
06/08/06   etv     Send session close if AN GAUPs a non-GAUPable attribute.
05/19/06   etv     Removed ASSERTs for graceful degradation.
05/11/06   etv     Allow only a max of five for MaxMACFlows attribute.
04/24/06   etv     Increased the resolution of PSQRABThresholdDRCLock/UnLock.
                   Increased PilotStrength axis resolution.
04/18/06   etv     Accept default only for MaxNumSubPkts and SupportGAUP attrib.
04/07/06   hal     Changed hdrutil_new_buffer() to use hdrutil_new_msg_buffer()
03/17/06   etv     Fixed bug in RRIChannelPowerParams parsing routine.
02/02/06   etv     Initialize MaxNumActiveMACFlows to right default.
01/31/06   etv     Added support for AT-init Neg. for RMAC3.
01/23/06   etv     Fixed PowerParams attrib initialization error. 
01/04/06   etv     Changed AssociatedFlows attribute minimum size to 8.
12/21/05   etv     Fixed BucketLevelMax00 valid range check.
12/15/05   etv     Fixed offset by 1 error in cmplx attrib handler table.
12/14/05   etv     Fixed bug becos of operator precedence
11/21/05   etv     Fixed PowerParam index Mask to mask a byte
                   Changed some F3 messages variable format to Hex.
11/21/05   etv     Added header tags for perforce to fill in
11/17/05   etv     One instance of per-flow complex attribute.
11/14/05   etv     Code review comment and minor cleanup
11/11/05   etv     Fixed the Valid range for MergeThresholdNN
11/10/05   etv     Fixed bug in get_attrib_offset routine for simple attribs
11/08/05   etv     ASSERT if the flow index goes over allowed no. of MAC flows.
11/04/05   etv     Made updates to fit RMAC3 implementation
11/02/05   etv     Fixed bugs in complex attrib validity check functions
10/10/05   etv     Replaced Union of complex attribs with separate instances.
08/22/05   etv     Created module.

===========================================================================*/

/*===========================================================================

                          INCLUDE FILES FOR MODULE
                     
===========================================================================*/

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

#include "hdrscmrmac3.h"
#include "hdrscmrmaccmn.h"
#include "hdrscm.h"
#include "hdrscmdb.h"

/*===========================================================================

             DEFINITIONS  FROM  THE  HDR  AIR  INTERFACE  SPEC

===========================================================================*/

#define HDRSCMRMAC3_ATTRIB_ID_SIZE                      16
#define HDRSCMRMAC3_VALUE_ID_SIZE                       8

/*---------------------------------------------------------------------------
          Sizes of the complex attributes and its fields in bits 
----------------------------------------------------------------------------*/
/* AuxiliaryPilotChannelParameters */
#define HDRSCMRMAC3_AUX_PILOT_CHAN_GAIN_SIZE            4
#define HDRSCMRMAC3_AUX_PILOT_CHAN_MIN_PAYLOAD_SIZE     4
#define HDRSCMRMAC3_AUX_PILOT_CHAN_PARAMS_SIZE          8

/* CommonPowerParameters */
#define HDRSCMRMAC3_ALLOCATION_STAGGER_SIZE             4
#define HDRSCMRMAC3_TX_T2P_MIN_SIZE                     8
#define HDRSCMRMAC3_RPC_STEP_SIZE                       2
#define HDRSCMRMAC3_RSVD_COM_POW_PARAM_SIZE             2
#define HDRSCMRMAC3_COMM_POW_PARAMS_SIZE                16

/* MaxNumSubPackets */
#define HDRSCMRMAC3_MAX_NUM_SUB_PKTS_N_SIZE             2
#define HDRSCMRMAC3_RSVD_MAX_NUM_SUB_PKTS_SIZE          2
#define HDRSCMRMAC3_MAX_NUM_SUB_PKTS_SIZE               8

/* PermittedPayload */
#define HDRSCMRMAC3_PERMITTED_PAYLOAD_X_Y_SIZE          4
#define HDRSCMRMAC3_RSVD_PERMITTED_PAYLOAD_SIZE         4
#define HDRSCMRMAC3_PERMITTED_PAYLOAD_SIZE              160

/* PilotStrength */
#define HDRSCMRMAC3_PS_NUM_PS_AXIS_VALS_SIZE            4
#define HDRSCMRMAC3_PS_AXIS_SIZE                        6
#define HDRSCMRMAC3_PS_PS_AXIS_SIZE                     4
#define HDRSCMRMAC3_RSVD_PS_SIZE                        4
#define HDRSCMRMAC3_PS_ATTRIB_MIN_SIZE                  16

/* PowerParameters */
#define HDRSCMRMAC3_PP_T2P_TRANSITION_SIZE              2
#define HDRSCMRMAC3_PP_TERMINATION_TARGET_SIZE          2
#define HDRSCMRMAC3_PP_T2P_PRE_TRANSITION_SIZE          8
#define HDRSCMRMAC3_PP_T2P_POST_TRANSITION_SIZE         8
#define HDRSCMRMAC3_POWER_PARAMS_SIZE                   40

/* RequestParameters */
#define HDRSCMRMAC3_RP_REQ_RATIO_SIZE                   4
#define HDRSCMRMAC3_RP_MAX_REQ_INTERVAL_SIZE            4
#define HDRSCMRMAC3_REQUEST_PARAMS_SIZE                 8

/* RRIChannelPowerParameters */
#define HDRSCMRMAC3_RRI_CHAN_GAIN_TRANS_SIZE            4
#define HDRSCMRMAC3_RSVD_RRI_CHAN_POW_PARAMS_SIZE       4
#define HDRSCMRMAC3_RRI_CHAN_POW_PARAMS_SIZE            32  

/* TxT2PMax */
#define HDRSCMRMAC3_TX_T2P_NUM_PS_AXIS_VALS_SIZE        3
#define HDRSCMRMAC3_TX_T2P_PS_AXIS_SIZE                 6
#define HDRSCMRMAC3_TX_T2P_MAX_PS_AXIS_SIZE             7
#define HDRSCMRMAC3_MIN_TX_T2P_MAX_SIZE                 16

/* MaxMACFlows */
#define HDRSCMRMAC3_MAX_NUM_MAC_FLOWS_SIZE              8
#define HDRSCMRMAC3_MAX_NUM_ACTIVE_MAC_FLOWS_SIZE       8
#define HDRSCMRMAC3_MAX_MAC_FLOWS_SIZE                  16
#define HDRSCMRMAC3_MAX_MAC_FLOWS_BYTE_LENGTH           5 

/* AssociatedFlowsNN */
#define HDRSCMRMAC3_AF_FLOW_COUNT_SIZE                  8
#define HDRSCMRMAC3_AF_STREAM_SIZE                      9
#define HDRSCMRMAC3_AF_SUB_STREAM_SIZE                  8
#define HDRSCMRMAC3_MIN_ASSOCIATED_FLOWS_NN_SIZE        8

/* BucketFactorNN */
#define HDRSCMRMAC3_BF_NUM_T2P_AXIS_VALS_SIZE           4
#define HDRSCMRMAC3_BF_NUM_FRAB_AXIS_VALS_SIZE          3
#define HDRSCMRMAC3_BF_T2P_AXIS_SIZE                    8
#define HDRSCMRMAC3_BF_FRAB_AXIS_SIZE                   4
#define HDRSCMRMAC3_BF_T2P_AXIS_FRAB_AXIS_SIZE          8
#define HDRSCMRMAC3_MIN_BUCKET_FACTOR_NN_SIZE           32

/* T2PInflowRangeNN */
#define HDRSCMRMAC3_T2P_INFLOW_MIN_SIZE                 8
#define HDRSCMRMAC3_T2P_INFLOW_MAX_SIZE                 8
#define HDRSCMRMAC3_T2P_INFLOW_RANGE_SIZE               16

/* T2PTransitionFunctionNN */
#define HDRSCMRMAC3_GUGD_NUM_T2P_AXIS_VALS_SIZE         4
#define HDRSCMRMAC3_GUGD_NUM_FRAB_AXIS_VALS_SIZE        3
#define HDRSCMRMAC3_GUGD_T2P_AXIS_SIZE                  8
#define HDRSCMRMAC3_GUGD_FRAB_AXIS_SIZE                 4
#define HDRSCMRMAC3_GUGD_T2P_UP_T2P_FRAB_AXIS_SIZE      8
#define HDRSCMRMAC3_GUGD_T2P_DOWN_T2P_FRAB_AXIS_SIZE    8
#define HDRSCMRMAC3_MIN_GUGD_SIZE                       40 

/*---------------------------------------------------------------------------
     Valid Range definitions for each of the complex attribute fields
----------------------------------------------------------------------------*/

/* AuxiliaryPilotChannelParameters */
#define HDRSCMRMAC3_MAX_AUX_PILOT_CHAN_GAIN             0x0F
#define HDRSCMRMAC3_MAX_AUX_PILOT_CHAN_PAYLOAD          0x0B

/* CommonPowerParameters */
#define HDRSCMRMAC3_MAX_ALLOCATION_STAGGER              0x0F
#define HDRSCMRMAC3_MIN_RPC_STEP                        0x00
#define HDRSCMRMAC3_MAX_RPC_STEP                        0x03

/* MaxNumSubPackets */
#define HDRSCMRMAC3_MAX_MAX_NUM_SUB_PKTS                0x03

/* PilotStrength */
#define HDRSCMRMAC3_MIN_PS_NUM_PS_AXIS_VALUES           0x01 
#define HDRSCMRMAC3_MAX_PS_AXIS                         0x3F

/* PowerParameters */
#define HDRSCMRMAC3_MAX_PP_PRE_TRANSITION               0x80
#define HDRSCMRMAC3_MAX_PP_POST_TRANSITION              0x80

/* RequestParameters */
#define HDRSCMRMAC3_RP_MAX_REQ_RATIO                    0x0F
#define HDRSCMRMAC3_RP_MAX_MAX_REQ_INTERVAL             0x0F

/* TxT2PMax */
#define HDRSCMRMAC3_MIN_TX_T2P_NUM_PS_AXIS_VALS         0x01
#define HDRSCMRMAC3_MAX_TX_T2P_PS_AXIS                  0x3F

/* MaxMACFlows */
#define HDRSCMRMAC3_MIN_MAX_NUM_MAC_FLOWS               0x04
#define HDRSCMRMAC3_MIN_MAX_NUM_ACTIVE_MAC_FLOWS        0x04

/*---------------------------------------------------------------------------
                Default Values for Configuration Attributes
----------------------------------------------------------------------------*/
/* Non-Per_Flow Simple Attributes */
#define HDRSCMRMAC3_DEFAULT_FRAB_FILTER_TC              0x02
#define HDRSCMRMAC3_DEFAULT_FRAB_LOW                    0x03
#define HDRSCMRMAC3_DEFAULT_MERGE_THRESHOLD             0x02
#define HDRSCMRMAC3_DEFAULT_PAYLOAD_THRESHOLD           0x01
#define HDRSCMRMAC3_DEFAULT_PILOT_STRENGTH_FILTER_TC    0x01
#define HDRSCMRMAC3_DEFAULT_T2P_NO_TX_FILTER_TC         0x01
#define HDRSCMRMAC3_DEFAULT_PS_QRAB_THRESH_DRC_LOCK     0x18
#define HDRSCMRMAC3_DEFAULT_PS_QRAB_THRESH_DRC_UNLOCK   0x0C

/* Per-Flow Simple NN Attributes */
#define HDRSCMRMAC3_DEFAULT_00_BUCKET_LEVEL_MAX         0x50
#define HDRSCMRMAC3_DEFAULT_01_BUCKET_LEVEL_MAX         0x6C
#define HDRSCMRMAC3_DEFAULT_NN_MERGE_THRESHOLD          0x02
#define HDRSCMRMAC3_DEFAULT_NN_T2P_FILTER_TC            0x01

/* Non-Per-Flow complex Attributes */
/* AuxiliaryPilotChannelParameters */
#define HDRSCMRMAC3_DEFAULT_AUX_PILOT_CHAN_GAIN         0x0C
#define HDRSCMRMAC3_DEFAULT_AUX_PILOT_CHAN_MIN_PAYLOAD  0x07

/* CommonPowerParameters */
#define HDRSCMRMAC3_DEFAULT_CPP_TX_T2P_MIN              0x0F
#define HDRSCMRMAC3_DEFAULT_CPP_RPC_STEP                0x01

/* MaxNumSubPackets */
#define HDRSCMRMAC3_DEFAULT_MAX_NUM_SUB_PACKETS         0x03

/* PermittedPayload */
#define HDRSCMRMAC3_DEFAULT_PERMITTED_PAYLOAD_0_4       0x05
#define HDRSCMRMAC3_DEFAULT_PERMITTED_PAYLOAD_5_6       0x07
#define HDRSCMRMAC3_DEFAULT_PERMITTED_PAYLOAD_7_8       0x09
#define HDRSCMRMAC3_DEFAULT_PERMITTED_PAYLOAD_9_A       0x0B
#define HDRSCMRMAC3_DEFAULT_PERMITTED_PAYLOAD_B_C       0x0C

/* PilotStrength */
#define HDRSCMRMAC3_DEFAULT_PS_NUM_PS_AXIS_VALS         0x02
#define HDRSCMRMAC3_DEFAULT_PS_PS_AXIS_0                0x3C

/* PowerParameters128-12288 */
#define HDRSCMRMAC3_DEFAULT_PP_LL_T2P_TRANS             0x01
#define HDRSCMRMAC3_DEFAULT_PP_LL_TERM_TARGET           0x01
#define HDRSCMRMAC3_DEFAULT_PP_HC_T2P_TRANS             0x03
#define HDRSCMRMAC3_DEFAULT_PP_HC_TERM_TARGET           0x03
/* PowerParameters128 */
#define HDRSCMRMAC3_DEFAULT_PP_LL_PRE_TRANS_128         0x1C
#define HDRSCMRMAC3_DEFAULT_PP_LL_POST_TRANS_128        0x03
#define HDRSCMRMAC3_DEFAULT_PP_HC_TRANS_128             0x03
/* PowerParameters256 */
#define HDRSCMRMAC3_DEFAULT_PP_LL_PRE_TRANS_256         0x28
#define HDRSCMRMAC3_DEFAULT_PP_LL_POST_TRANS_256        0x0F
#define HDRSCMRMAC3_DEFAULT_PP_HC_TRANS_256             0x0F
/* PowerParameters512 */
#define HDRSCMRMAC3_DEFAULT_PP_LL_PRE_TRANS_512         0x34
#define HDRSCMRMAC3_DEFAULT_PP_LL_POST_TRANS_512        0x1C
#define HDRSCMRMAC3_DEFAULT_PP_HC_TRANS_512             0x1C
/* PowerParameters768 */
#define HDRSCMRMAC3_DEFAULT_PP_LL_PRE_TRANS_768         0x3B
#define HDRSCMRMAC3_DEFAULT_PP_LL_POST_TRANS_768        0x23
#define HDRSCMRMAC3_DEFAULT_PP_HC_TRANS_768             0x23
/* PowerParameters1024 */
#define HDRSCMRMAC3_DEFAULT_PP_LL_PRE_TRANS_1024        0x41
#define HDRSCMRMAC3_DEFAULT_PP_LL_POST_TRANS_1024       0x28
#define HDRSCMRMAC3_DEFAULT_PP_HC_TRANS_1024            0x28
/* PowerParameters1536 */
#define HDRSCMRMAC3_DEFAULT_PP_LL_PRE_TRANS_1536        0x49
#define HDRSCMRMAC3_DEFAULT_PP_LL_POST_TRANS_1536       0x2E
#define HDRSCMRMAC3_DEFAULT_PP_HC_TRANS_1536            0x2E
/* PowerParameters2048 */
#define HDRSCMRMAC3_DEFAULT_PP_LL_PRE_TRANS_2048        0x4D
#define HDRSCMRMAC3_DEFAULT_PP_LL_POST_TRANS_2048       0x34
#define HDRSCMRMAC3_DEFAULT_PP_HC_TRANS_2048            0x34
/* PowerParameters3072 */
#define HDRSCMRMAC3_DEFAULT_PP_LL_PRE_TRANS_3072        0x4D
#define HDRSCMRMAC3_DEFAULT_PP_LL_POST_TRANS_3072       0x39
#define HDRSCMRMAC3_DEFAULT_PP_HC_TRANS_3072            0x39
/* PowerParameters4096 */
#define HDRSCMRMAC3_DEFAULT_PP_LL_PRE_TRANS_4096        0x52
#define HDRSCMRMAC3_DEFAULT_PP_LL_POST_TRANS_4096       0x3E
#define HDRSCMRMAC3_DEFAULT_PP_HC_TRANS_4096            0x3E
/* PowerParameters6144 */
#define HDRSCMRMAC3_DEFAULT_PP_LL_PRE_TRANS_6144        0x57
#define HDRSCMRMAC3_DEFAULT_PP_LL_POST_TRANS_6144       0x44
#define HDRSCMRMAC3_DEFAULT_PP_HC_TRANS_6144            0x44
/* PowerParameters8192 */
#define HDRSCMRMAC3_DEFAULT_PP_LL_PRE_TRANS_8192        0x5D
#define HDRSCMRMAC3_DEFAULT_PP_LL_POST_TRANS_8192       0x4A
#define HDRSCMRMAC3_DEFAULT_PP_HC_TRANS_8192            0x4A
/* PowerParameters12288 */
#define HDRSCMRMAC3_DEFAULT_PP_LL_PRE_TRANS_12288       0x69
#define HDRSCMRMAC3_DEFAULT_PP_LL_POST_TRANS_12288      0x55
#define HDRSCMRMAC3_DEFAULT_PP_HC_TRANS_12288           0x55

/* RRIChannelPowerParameters */
#define HDRSCMRMAC3_DEFAULT_RRI_CHAN_GAIN_TRANS         0x0A

/* TxT2PMax */
#define HDRSCMRMAC3_DEFAULT_TXT2P_NUM_PS_AXIS_VALS      0x02
#define HDRSCMRMAC3_DEFAULT_PS_AXIS_0                   0x28
#define HDRSCMRMAC3_DEFAULT_PS_AXIS_1                   0x14
#define HDRSCMRMAC3_DEFAULT_TXT2P_PS_AXIS_0             0x18
#define HDRSCMRMAC3_DEFAULT_TXT2P_PS_AXIS_1             0x36

/* MaxMACFlows */
#define HDRSCMRMAC3_DEFAULT_MAX_MAC_FLOWS               0x04

/* Per-Flow Complex Attributes */
/* Associated FlowsNN */
#define HDRSCMRMAC3_DEFAULT_NN_ASSO_FLOWS_FLOW_COUNT    0x01
#define HDRSCMRMAC3_DEFAULT_NN_ASSO_FLOWS_STREAM        511

/* BucketFactor00 */
#define HDRSCMRMAC3_DEFAULT_00_BF_NUM_T2P_AXIS_VALS     0x01
#define HDRSCMRMAC3_DEFAULT_00_BF_NUM_FRAB_AXIS_VALS    0x00
#define HDRSCMRMAC3_DEFAULT_00_BF_T2P_AXIS_VALS_0       0x10
#define HDRSCMRMAC3_DEFAULT_00_BF_T2P_AXIS_VALS_1       0x2B
#define HDRSCMRMAC3_DEFAULT_00_BUCKET_FACTOR_0_0        0x18
#define HDRSCMRMAC3_DEFAULT_00_BUCKET_FACTOR_1_0        0x08
/* BucketFactorNN */
#define HDRSCMRMAC3_DEFAULT_NN_BF_NUM_T2P_AXIS_VALS     0x01
#define HDRSCMRMAC3_DEFAULT_NN_BF_NUM_FRAB_AXIS_VALS    0x02
#define HDRSCMRMAC3_DEFAULT_NN_BF_T2P_AXIS_VALS_0       0x00
#define HDRSCMRMAC3_DEFAULT_NN_BF_T2P_AXIS_VALS_1       0x48
#define HDRSCMRMAC3_DEFAULT_NN_BF_FRAB_AXIS_VALS_0      0x08
#define HDRSCMRMAC3_DEFAULT_NN_BF_FRAB_AXIS_VALS_1      0x0B
#define HDRSCMRMAC3_DEFAULT_NN_BF_FRAB_AXIS_VALS_2      0x07
#define HDRSCMRMAC3_DEFAULT_NN_BUCKET_FACTOR_0_0        0x28
#define HDRSCMRMAC3_DEFAULT_NN_BUCKET_FACTOR_0_1        0x10
#define HDRSCMRMAC3_DEFAULT_NN_BUCKET_FACTOR_0_2        0x10
#define HDRSCMRMAC3_DEFAULT_NN_BUCKET_FACTOR_1_Y        0x08

/* T2PInflowRange00 */
#define HDRSCMRMAC3_DEFAULT_00_T2P_INFLOW_MIN           0x10
#define HDRSCMRMAC3_DEFAULT_00_T2P_INFLOW_MAX           0x2B
/* T2PInflowRangeNN */
#define HDRSCMRMAC3_DEFAULT_NN_T2P_INFLOW_MIN           0x07
#define HDRSCMRMAC3_DEFAULT_NN_T2P_INFLOW_MAX           0x78

/* T2PTransitionFunction00 */
#define HDRSCMRMAC3_DEFAULT_00_GUGD_NUM_T2P_VALS        0x01
#define HDRSCMRMAC3_DEFAULT_00_GUGD_NUM_FRAB_VALS       0x00
#define HDRSCMRMAC3_DEFAULT_00_T2P_AXIS_0               0x10
#define HDRSCMRMAC3_DEFAULT_00_T2P_AXIS_1               0x2B
#define HDRSCMRMAC3_DEFAULT_00_FRAB_AXIS_0              0x00
#define HDRSCMRMAC3_DEFAULT_UP_0_0                      0xF4
#define HDRSCMRMAC3_DEFAULT_UP_1_0                      0xBC
#define HDRSCMRMAC3_DEFAULT_DOWN_X_Y                    0xD8

#define HDRSCMRMAC3_DEFAULT_NN_GUGD_NUM_T2P_VALS        0x03
#define HDRSCMRMAC3_DEFAULT_NN_GUGD_NUM_FRAB_VALS       0x03
#define HDRSCMRMAC3_DEFAULT_NN_T2P_AXIS_0               0x00
#define HDRSCMRMAC3_DEFAULT_NN_T2P_AXIS_1               0x22
#define HDRSCMRMAC3_DEFAULT_NN_T2P_AXIS_2               0x47
#define HDRSCMRMAC3_DEFAULT_NN_T2P_AXIS_3               0x5A
#define HDRSCMRMAC3_DEFAULT_NN_FRAB_AXIS_0              0x08
#define HDRSCMRMAC3_DEFAULT_NN_FRAB_AXIS_1              0x0B
#define HDRSCMRMAC3_DEFAULT_NN_FRAB_AXIS_2              0x0E
#define HDRSCMRMAC3_DEFAULT_NN_FRAB_AXIS_3              0x07
#define HDRSCMRMAC3_DEFAULT_NN_UP_0_0                   0x1D
#define HDRSCMRMAC3_DEFAULT_NN_UP_0_1                   0x0A
#define HDRSCMRMAC3_DEFAULT_NN_UP_0_2                   0xFE
#define HDRSCMRMAC3_DEFAULT_NN_UP_0_3                   0xFE
#define HDRSCMRMAC3_DEFAULT_NN_UP_1_0                   0xFB
#define HDRSCMRMAC3_DEFAULT_NN_UP_1_1                   0xE8
#define HDRSCMRMAC3_DEFAULT_NN_UP_1_2                   0xDC
#define HDRSCMRMAC3_DEFAULT_NN_UP_1_3                   0xDC
#define HDRSCMRMAC3_DEFAULT_NN_UP_2_0                   0xFB
#define HDRSCMRMAC3_DEFAULT_NN_UP_2_1                   0xE8
#define HDRSCMRMAC3_DEFAULT_NN_UP_2_2                   0xDC
#define HDRSCMRMAC3_DEFAULT_NN_UP_2_3                   0xDC
#define HDRSCMRMAC3_DEFAULT_NN_UP_3_0                   0x18
#define HDRSCMRMAC3_DEFAULT_NN_UP_3_1                   0x05
#define HDRSCMRMAC3_DEFAULT_NN_UP_3_2                   0xF9
#define HDRSCMRMAC3_DEFAULT_NN_UP_3_3                   0xF9
#define HDRSCMRMAC3_DEFAULT_NN_DOWN_0_0                 0xE9
#define HDRSCMRMAC3_DEFAULT_NN_DOWN_0_1                 0xE2
#define HDRSCMRMAC3_DEFAULT_NN_DOWN_0_2                 0xD6
#define HDRSCMRMAC3_DEFAULT_NN_DOWN_0_3                 0xD6
#define HDRSCMRMAC3_DEFAULT_NN_DOWN_1_0                 0xE9
#define HDRSCMRMAC3_DEFAULT_NN_DOWN_1_1                 0xE2
#define HDRSCMRMAC3_DEFAULT_NN_DOWN_1_2                 0xD6
#define HDRSCMRMAC3_DEFAULT_NN_DOWN_1_3                 0xD6
#define HDRSCMRMAC3_DEFAULT_NN_DOWN_2_0                 0x0E
#define HDRSCMRMAC3_DEFAULT_NN_DOWN_2_1                 0x07
#define HDRSCMRMAC3_DEFAULT_NN_DOWN_2_2                 0xFB
#define HDRSCMRMAC3_DEFAULT_NN_DOWN_2_3                 0xFB
#define HDRSCMRMAC3_DEFAULT_NN_DOWN_3_0                 0x2D
#define HDRSCMRMAC3_DEFAULT_NN_DOWN_3_1                 0x26
#define HDRSCMRMAC3_DEFAULT_NN_DOWN_3_2                 0x1A
#define HDRSCMRMAC3_DEFAULT_NN_DOWN_3_3                 0x1A

 /* EJECT */
/*===========================================================================
 
                  DEFINITIONS AND DECLARATIONS FOR MODULE

  This section contains local definitions for constants, macros, types,
  variables and other items needed by this module.

===========================================================================*/

#define HDRSCMRMAC3_SIMPLE_ATTRIB_MSB_MASK              0xF000
#define HDRSCMRMAC3_SIMPLE_ATTRIB_MSB                   0xF000

#define HDRSCMRMAC3_NN_ATTRIB_3RD_NIBBLE_MASK           0x0F00
#define HDRSCMRMAC3_NON_NN_SIMPLE_ATTRIB_MASKED         0x0F00
#define HDRSCMRMAC3_NON_NN_COMPLEX_ATTRIB_MASKED        0x0000
#define HDRSCMRMAC3_POWER_PARAMS_INDEX_MASK             0x00FF
#define HDRSCMRMAC3_POWER_PARAMS_INDEX_ORIGIN           0x0005

#define HDRSCMRMAC3_SIMPLE_ATTRIB_INDEX_MASK            0x000F
#define HDRSCMRMAC3_SIMPLE_ATTRIB_MAX_INDEX             15
#define HDRSCMRMAC3_SIMPLE_ATTRIB_FIRST_HOLE            4
#define HDRSCMRMAC3_SIMPLE_ATTRIB_SECOND_HOLE           8
#define HDRSCMRMAC3_SIMPLE_ATTRIB_INDEX_ORIGIN          2

#define HDRSCMRMAC3_NN_SIMPLE_ATTRIB_INDEX_MASK         0x0F00
#define HDRSCMRMAC3_NN_SIMPLE_ATTRIB_INDEX_LSB          8
#define HDRSCMRMAC3_NN_SIMPLE_ATTRIB_INDEX_ORIGIN       9
#define HDRSCMRMAC3_NN_BASE_MASK                        0xFF00
#define HDRSCMRMAC3_NN_FLOW_ID_MASK                     0x00FF

#define HDRSCMRMAC3_NUM_GAUPABLE_NON_NN_ATTRIBS         21
#define HDRSCMRMAC3_NUM_GAUPABLE_NN_ATTRIBS             10


#define HDRSCMRMAC3_GET_MAC_FLOW_ID(attrib_id)          \
            ((attrib_id) & HDRSCMRMAC3_NN_FLOW_ID_MASK)

#define HDRSCMRMAC3_GET_NN_BASE(attrib_id)              \
            ((attrib_id) & HDRSCMRMAC3_NN_BASE_MASK)

/* Removed the redundant last condition to avoid lint warning. attrib_id 
   is uint16 and will always be less than or equal to 0xFFFF, which is 
   the last NON_NN_SIMPLE_ATTRIB. */
#define HDRSCMRMAC3_IS_ATTRIB_NON_NN_SIMPLE(attrib_id)                    \
            (((attrib_id) != HDRSCMRMAC3_NON_NN_SIMPLE_ATTRIB_RSVD1) &&   \
             ((attrib_id) != HDRSCMRMAC3_NON_NN_SIMPLE_ATTRIB_RSVD2) &&   \
             ((attrib_id) >= HDRSCMRMAC3_FIRST_NON_NN_SIMPLE_ATTRIB) /* &&   \
             ((attrib_id) <= HDRSCMRMAC3_LAST_NON_NN_SIMPLE_ATTRIB)*/ ) 

/* Check if the attribute specified by given attribute ID is a non-NN 
   complex attribute. Attrib ID is unsigned and since the first non-NN 
   complex attribute ID is 0x0000, it is sufficient to check with the 
   upper bound (last) */
#define HDRSCMRMAC3_IS_ATTRIB_NON_NN_COMPLEX(attrib_id)                   \
              ((attrib_id) <= HDRSCMRMAC3_LAST_COMPLEX_ATTRIB)

#define HDRSCMRMAC3_IS_NN_BASE_SIMPLE(nn_base)                            \
             (((nn_base) >= HDRSCMRMAC3_FIRST_SIMPLE_NN_ATTRIB_BASE) &&   \
              ((nn_base) <= HDRSCMRMAC3_LAST_SIMPLE_NN_ATTRIB_BASE))

#define HDRSCMRMAC3_IS_NN_BASE_COMPLEX(nn_base)                           \
             (((nn_base) == HDRSCMRMAC3_ASSOCIATED_FLOWS_NN_BASE) ||      \
              ((nn_base) == HDRSCMRMAC3_BUCKET_FACTOR_NN_BASE) ||         \
              ((nn_base) == HDRSCMRMAC3_T2P_INFLOW_RANGE_NN_BASE) ||      \
              ((nn_base) == HDRSCMRMAC3_T2P_TRANSITION_FUNC_NN_BASE))

/* Align attribute offset to the next octet boundary */
#define HDRSCMRMAC3_ALIGN_TO_NEXT_OCTET(attrib_offset)                 \
              ((attrib_offset) = ((((attrib_offset) + 7) / 8) * 8))

/* The type hdrscmrmac3_simple_attrib_valid_range_type defines a min and a 
 * max field to specify the Valid range of values that a simple attribute
 * could take. */
typedef struct
{
  uint8 min;    /* Minimum allowed value for the simple attribute */
  uint8 max;    /* Maximum allowed value for the simple attribute */

} hdrscmrmac3_simple_attrib_valid_range_type;

/* Complex attribute parsing routine type */
typedef hdrerrno_enum_type (*hdrscmrmac3_complex_attrib_handler_type)
(
  dsm_item_type                               *item_ptr,
  hdrscm_proposed_attrib_type   const *const   p_aip,
  hdrscm_accepted_attrib_type         *const   a_aip
);


/* Number of PowerParameters Attributes. AttribIDs (0x0005 - 0x0010) */
#define HDRSCMRMAC3_NUM_POWER_PARAMS_ATTRIBS  12


/*------------------------------------------------------------------------
                    FUNCTION FORWARD DECLARATIONS
 -----------------------------------------------------------------------*/

/* AuxiliaryPilotChannelParameters Attribute */
static hdrerrno_enum_type hdrscmrmac3_process_aux_pilot_chan_params
(
  dsm_item_type                               *item_ptr,
  hdrscm_proposed_attrib_type   const *const   p_aip,
  hdrscm_accepted_attrib_type         *const   a_aip
);

/* CommonPowerParameters Attribute */
static hdrerrno_enum_type hdrscmrmac3_process_common_power_params
(
  dsm_item_type                               *item_ptr,
  hdrscm_proposed_attrib_type   const *const   p_aip,
  hdrscm_accepted_attrib_type         *const   a_aip
);

/* MaxNumSubPackets Attribute */
static hdrerrno_enum_type hdrscmrmac3_process_max_num_sub_pkts
(
  dsm_item_type                               *item_ptr,
  hdrscm_proposed_attrib_type   const *const   p_aip,
  hdrscm_accepted_attrib_type         *const   a_aip
);

/* PermittedPayload Attribute */
static hdrerrno_enum_type hdrscmrmac3_process_permitted_payload
(
  dsm_item_type                               *item_ptr,
  hdrscm_proposed_attrib_type   const *const   p_aip,
  hdrscm_accepted_attrib_type         *const   a_aip
);

/* PilotStrength Attribute */
static hdrerrno_enum_type hdrscmrmac3_process_pilot_strength
(
  dsm_item_type                               *item_ptr,
  hdrscm_proposed_attrib_type   const *const   p_aip,
  hdrscm_accepted_attrib_type         *const   a_aip
);

/* PowerParams Attribute */
static hdrerrno_enum_type hdrscmrmac3_process_power_params
(
  dsm_item_type                               *item_ptr,
  hdrscm_proposed_attrib_type   const *const   p_aip,
  hdrscm_accepted_attrib_type         *const   a_aip
);

/* RequestParams Attribute */
static hdrerrno_enum_type hdrscmrmac3_process_request_params
(
  dsm_item_type                               *item_ptr,
  hdrscm_proposed_attrib_type   const *const   p_aip,
  hdrscm_accepted_attrib_type         *const   a_aip
);

/* RRIChannelPowerParameters Attribute */
static hdrerrno_enum_type hdrscmrmac3_process_rri_chan_power_params
(
  dsm_item_type                               *item_ptr,
  hdrscm_proposed_attrib_type   const *const   p_aip,
  hdrscm_accepted_attrib_type         *const   a_aip
);

/* TxT2PMax Attribute */
static hdrerrno_enum_type hdrscmrmac3_process_tx_t2p_max
(
  dsm_item_type                               *item_ptr,
  hdrscm_proposed_attrib_type   const *const   p_aip,
  hdrscm_accepted_attrib_type         *const   a_aip
);

/* MaxNumMACFlows Attribute */
static hdrerrno_enum_type hdrscmrmac3_process_max_mac_flows
(
  dsm_item_type                               *item_ptr,
  hdrscm_proposed_attrib_type   const *const   p_aip,
  hdrscm_accepted_attrib_type         *const   a_aip
);

/*------------------------------------------------------------------------
                            GLOBAL VARIABLES
 -----------------------------------------------------------------------*/

static struct
{
  hdrscmrmac3_aux_pilot_struct_type aux_pilot;
  hdrscmrmac3_common_pow_params_struct_type common_power_params;
  hdrscmrmac3_max_num_sub_pkts_struct_type max_num_sub_pkts;
  hdrscmrmac3_permitted_payload_type permitted_payload;
  hdrscmrmac3_pilot_strength_grid_struct_type pilot_strength;
  hdrscmrmac3_power_params_struct_type 
    power_params[HDRSCMRMACCMN_MAX_NUM_PAYLOADS][HDRSCMRMAC3_NUM_TX_MODES];
  hdrscmrmac3_req_params_struct_type req_params;
  hdrscmrmac3_rri_power_params_struct_type rri_chan_power_params
                                           [HDRSCMRMAC3_NUM_T2P_TRANS_POINTS];
  hdrscmrmac3_txt2p_max_grid_struct_type txt2p_max;
  hdrscmrmac3_max_mac_flows_struct_type max_mac_flows;

  /* Per-Flow attributes */
  hdrscmrmac3_t2p_frab_grid_struct_type 
    bucket_factor[HDRSCMRMAC3_MAX_NUM_MAC_FLOWS];
  hdrscmrmac3_gu_gd_struct_type 
    gu_gd[HDRSCMRMAC3_MAX_NUM_MAC_FLOWS];
  hdrscmrmac3_t2p_inflow_range_struct_type 
    t2p_inflow_range[HDRSCMRMAC3_MAX_NUM_MAC_FLOWS];
  hdrscmrmac3_associated_flows_struct_type 
    associated_flows[HDRSCMRMAC3_MAX_NUM_MAC_FLOWS];
 
  uint8 trans_id;
    /* Transaction ID used for AT-init Session negotiation */
  uint8 max_mac_flow_value_id;
 
} hdrscmrmac3;

/*------------------------------------------------------------------------
                              LOOKUP TABLES
 -----------------------------------------------------------------------*/

/* This table holds pointers to all Non-per-flow complex attribute parsing 
   routines for ease of access */
static hdrscmrmac3_complex_attrib_handler_type const 
  hdrscmrmac3_complex_attrib_handler[] =
{
  hdrscmrmac3_process_aux_pilot_chan_params,  /* 0x0000 */
  hdrscmrmac3_process_common_power_params,    /* 0x0001 */
  hdrscmrmac3_process_max_num_sub_pkts,       /* 0x0002 */
  hdrscmrmac3_process_permitted_payload,      /* 0x0003 */
  hdrscmrmac3_process_pilot_strength,         /* 0x0004 */
  hdrscmrmac3_process_power_params,           /* 0x0005 - 0x0010 */
  hdrscmrmac3_process_request_params,         /* 0x0011 */
  hdrscmrmac3_process_rri_chan_power_params,  /* 0x0012 */
  hdrscmrmac3_process_tx_t2p_max,             /* 0x0013 */
  hdrscmrmac3_process_max_mac_flows           /* 0x0014 */
};

/* This table defines the valid ranges for each of the simple attributes
   except the per-flow ones. This table could be indexed using the least
   significant Nibble. However, care must be taken to take care of the 
   holes. */
static hdrscmrmac3_simple_attrib_valid_range_type const
  hdrscmrmac3_valid_range[HDRSCMRMAC3_TOTAL_NUM_SIMPLE_ATTRIBS] =
{
  {0x00, 0x00},   /* 0.  0xFFF2 SupportGAUPMaxNumSubPkts; Accept default only */
  {0x00, 0x01},   /* 1.  0xFFF3 Rate1M8Supported */
  {0x00, 0x3F},   /* 2.  0xFFF5 PilotStrengthQRABThresholdDRCUnlock */
  {0x00, 0x3F},   /* 3.  0xFFF6 PilotStrengthQRABThresholdDRCLock */
  {0x00, 0x04},   /* 4.  0xFFF7 T2PNoTxFilterTC */
  {0x00, 0x01},   /* 5.  0xFFF9 QRABFilterTC */
  {0x00, 0x02},   /* 6.  0xFFFA PilotStrengthFilterTC */
  {0x00, 0x03},   /* 7.  0xFFFB PayloadThresh */
  {0x00, 0x05},   /* 8.  0xFFFC MergeThreshold */
  {0x00, 0x04},   /* 9.  0xFFFD FRABLow */
  {0x00, 0x03},   /* 10. 0xFFFE FRABFilterTC */
  {0x00, 0x01}    /* 11. 0xFFFF ARQMode */
};

/* The following table defines the valid range for each of the per-flow simple
   attributes. This table could be indexed using the second Most-significant
   Nibble. */
static hdrscmrmac3_simple_attrib_valid_range_type const
  hdrscmrmac3_nn_valid_range[HDRSCMRMAC3_TOTAL_NUM_NN_SIMPLE_ATTRIBS] =
{
  {0x00, 0x04},   /* 0xF9NN T2PFilterTCNN */
  {0x00, 0x03},   /* 0xFANN BurstDurationFactorNN */
  {0x00, 0x01},   /* 0xFBNN QRABSelect */
  {0x00, 0x01},   /* 0xFCNN TransmissionModeNN */
  {0x00, 0x06},   /* 0xFDNN MergeThresholdNN */
  {0x00, 0xFF}    /* 0xFENN BucketLevelMaxNN */
};


/* Attribute IDs of Non-Per-Flow GAUPable Attributes */
static uint16 const 
  hdrscmrmac3_gaup_non_nn_attrib_ids[HDRSCMRMAC3_NUM_GAUPABLE_NON_NN_ATTRIBS] = 
{
  /* Simple Attributes */
  0xFFF5, /* 0.  PilotStrengthQRABThresholdDRCUnLock */
  0xFFF6, /* 1.  PilotStrengthQRABThresholdDRCLock */
  0xFFFB, /* 2.  PayloadThresh */
  0xFFFC, /* 3.  MergeThreshold */
  0xFFFF, /* 4.  ARQMode */

  /* Complex Attributes */
  0x0000, /* 5.  AuxiliaryPilotChannelParameters */
  0x0002, /* 6.  MaxNumSubPackets */
  0x0003, /* 7.  PermittedPayload */
  0x0005, /* 8.  PowerParameters128 */
  0x0006, /* 9.  PowerParameters256 */
  0x0007, /* 10. PowerParameters512 */
  0x0008, /* 11. PowerParameters768 */
  0x0009, /* 12. PowerParameters1024 */
  0x000A, /* 13. PowerParameters1536 */
  0x000B, /* 14. PowerParameters2048 */
  0x000C, /* 15. PowerParameters3072 */
  0x000D, /* 16. PowerParameters4096 */
  0x000E, /* 17. PowerParameters6144 */
  0x000F, /* 18. PowerParameters8192 */
  0x0010, /* 19. PowerParameters12288 */
  0x0011  /* 20. RequestParameters */
};

static uint16 const 
  hdrscmrmac3_gaup_nn_attrib_ids
    [HDRSCMRMAC3_NUM_GAUPABLE_NN_ATTRIBS] = 
{
  /* Per-Flow Simple Attributes */
  0xF900, /* 21 T2PFilter00 */
  0xFA00, /* 22 BurstDurationFactor00 */
  0xFB00, /* 23 QRABSelect00 */
  0xFC00, /* 24 TransmissionMode00 */ 
  0xFD00, /* 25 MergeThreshold00 */
  0xFE00, /* 26 BucketLevelMax00 */

  /* Per-Flow Complex Attributes */
  0x0300, /* 27 BucketFactor00 */
  0x1600, /* 28 T2PTransitionFunction00 */
  0x1500, /* 29 T2PInflowRange00 */
  0x0100  /* 30 AssociatedFlows00 */
};

 /* EJECT */

/*===========================================================================

FUNCTION HDRSCMRMAC3_IS_AUX_PILOT_CHAN_PARAMS_VALID                     LOCAL

DESCRIPTION
  Check if the AuxillaryPilotChannelParameters is valid 

  0x0000 - AuxillaryPilotChannelParameters.

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  E_SUCCESS       - If the AuxillaryPilotChannelParameters Attribute is valid.
  E_OUT_OF_RANGE  - otherwise. 

SIDE EFFECTS
  None.

===========================================================================*/
static hdrerrno_enum_type hdrscmrmac3_is_aux_pilot_chan_params_valid(void)
{
  uint8 gain = hdrscmrmac3.aux_pilot.gain_db;
    /* AuxillaryPilotChannelGain */

  uint8 min_payload = hdrscmrmac3.aux_pilot.min_payload;
    /* AuxillaryPilotChannelPayload */

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */ 

  return ( (gain <= HDRSCMRMAC3_MAX_AUX_PILOT_CHAN_GAIN) &&
           (min_payload <= HDRSCMRMAC3_MAX_AUX_PILOT_CHAN_PAYLOAD)
         )? E_SUCCESS: E_OUT_OF_RANGE;

} /* hdrscmrmac3_is_aux_pilot_chan_params_valid */


 /* EJECT */
/*===========================================================================

FUNCTION HDRSCMRMAC3_PROCESS_AUX_PILOT_CHAN_PARAMS                    LOCAL

DESCRIPTION
  This is a generic interface to process the AuxiliaryPilotChannelParameters
  Attribute.

    0x0000 - AuxiliaryPilotChannelParameters Attribute 

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
static hdrerrno_enum_type hdrscmrmac3_process_aux_pilot_chan_params
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

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */ 

  /* Initialize the attribute offset to start of first Value ID */
  attrib_offset = p_aip->attrib_len_size + a_aip->attrib_id_size;

  /* While there are enough bits left in the Attribute Record for one more
   * set of AuxiliaryPilotChannelParameters complex Attribute Value */
  while ( (attrib_offset + p_aip->value_id_size + 
           HDRSCMRMAC3_AUX_PILOT_CHAN_PARAMS_SIZE) <= p_aip->attrib_len )
  {
    /*----------------------------------------------------------------------
     * There is at least one more attribute value in this attribute Record
     *--------------------------------------------------------------------*/

    /* Initialize the error code to success since we know that we have at least
     * one attrib value in the Attrib Record */
    err_no = E_SUCCESS;

    /* Unpack the Value ID into a uint8 */
    err_no = HDRBIT_UNPACK8( item_ptr,
                             &(value_id),
                             (p_aip->attrib_offset + attrib_offset),
                             p_aip->value_id_size );

    /* If error unpacking value ID, break */
    if (err_no != E_SUCCESS)
    {
      ERR("Error unpacking Value ID of AuxiliaryPilotChannelParameters Attrib",
            0,0,0);
      break;
    }

    /* Since Value Id was parsed sucessfully, Update the attribute offset to 
     * start of AuxiliaryPilotChannelGain field */ 
    attrib_offset += p_aip->value_id_size;

    err_no = HDRBIT_UNPACK8( item_ptr,
                             &(hdrscmrmac3.aux_pilot.gain_db),
                             (p_aip->attrib_offset + attrib_offset),
                             HDRSCMRMAC3_AUX_PILOT_CHAN_GAIN_SIZE );
                            
    /* If error unpacking AuxiliaryPilotChannelGain, break */
    if (err_no != E_SUCCESS)
    {
      ERR("Error unpacking AuxiliaryPilotChannelGain",
            0,0,0);
      break;
    }

    /* Since AuxiliaryPilotChannelGain field was parsed sucessfully, Update the 
       attribute offset to start of AuxillayPilotChannelMinPayload field */ 
    attrib_offset += HDRSCMRMAC3_AUX_PILOT_CHAN_GAIN_SIZE;

    err_no = HDRBIT_UNPACK8( item_ptr,
                             &(hdrscmrmac3.aux_pilot.min_payload),
                             (p_aip->attrib_offset + attrib_offset),
                             HDRSCMRMAC3_AUX_PILOT_CHAN_MIN_PAYLOAD_SIZE );

    /* If error unpacking AuxiliaryPilotChannelMinPayload, break */
    if (err_no != E_SUCCESS)
    {
      ERR("Error unpacking AuxiliaryPilotChannelMinPayload",
            0,0,0);
      break;
    }

    /* Parsing done! :-) Is the parsed attribute valid? */
    if ( (err_no = 
          hdrscmrmac3_is_aux_pilot_chan_params_valid()) != E_OUT_OF_RANGE )
    {
      /* Attrib valid! Yea! :-) print F3 mesg */
      HDR_MSG_PROT_2( MSG_LEGACY_HIGH, "AuxPilotChannelParams %x %x",
                      hdrscmrmac3.aux_pilot.gain_db,
                      hdrscmrmac3.aux_pilot.min_payload );

      /* Update the accepted attribute information */
      a_aip->is_accepted = TRUE;
      a_aip->value_id = value_id;
      a_aip->complex_val_ptr = (void*) &(hdrscmrmac3.aux_pilot);
      a_aip->complex_val_size = sizeof(hdrscmrmac3_aux_pilot_struct_type);
      break;
    }

    /* Not much luck :-( Let's try next set of values if there is one. 
       Shift the offset to start of next value ID. */
    attrib_offset += HDRSCMRMAC3_AUX_PILOT_CHAN_MIN_PAYLOAD_SIZE;

  } /* while */

  return err_no;

} /* hdrscmrmac3_process_aux_pilot_chan_params */


 /* EJECT */
/*===========================================================================

FUNCTION HDRSCMRMAC3_IS_COMMON_POWER_PARAMS_VALID                     LOCAL

DESCRIPTION
  Check if the CommonPowerParameters attribute is valid 

  0x0001 - CommonPowerParameters Attribute 

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  E_SUCCESS       - If the attribute is valid.
  E_OUT_OF_RANGE  - otherwise. 

SIDE EFFECTS
  None.

===========================================================================*/
static hdrerrno_enum_type hdrscmrmac3_is_common_power_params_valid(void)
{

  uint8 a_s = hdrscmrmac3.common_power_params.alloc_stagger;
    /* Allocation Stagger */

  uint8 rpcstep = hdrscmrmac3.common_power_params.rpc_step_db;
    /* RPC Step */

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */ 

  /* Any 8-bit value is valid for TxT2PMin. So checking for validity of 
     AllocationStagger and RPCStep alone! */
  return ( (a_s <= HDRSCMRMAC3_MAX_ALLOCATION_STAGGER) &&
           (rpcstep <= HDRSCMRMAC3_MAX_RPC_STEP) 
         )? E_SUCCESS: E_OUT_OF_RANGE;

} /* hdrscmrmac3_is_common_power_params_valid */


 /* EJECT */
/*===========================================================================

FUNCTION HDRSCMRMAC3_PROCESS_COMMON_POWER_PARAMS                    LOCAL

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
  TRUE  - If a valid attribute was parsed successfully.
  FALSE - otherwise. 

SIDE EFFECTS
  None.

===========================================================================*/
static hdrerrno_enum_type hdrscmrmac3_process_common_power_params
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

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */ 

  /* Initialize the attribute offset to start of first Value ID */
  attrib_offset = p_aip->attrib_len_size + a_aip->attrib_id_size;

  /* While there are enough bits left in the Attribute Record for one more
   * set of  CommonPowerParameters complex Attribute Value */
  while ( (attrib_offset + p_aip->value_id_size + 
           HDRSCMRMAC3_COMM_POW_PARAMS_SIZE ) <= p_aip->attrib_len )
  {
    /*----------------------------------------------------------------------
     * There is at least one more attribute value in this attribute Record
     *--------------------------------------------------------------------*/

    /* Initialize the error code to success since we know that we have at least
     * one attrib value in the Attrib Record */
    err_no = E_SUCCESS;

    /* Unpack the Value ID into a uint8 */
    err_no = HDRBIT_UNPACK8( item_ptr,
                             &(value_id),
                             (p_aip->attrib_offset + attrib_offset),
                             p_aip->value_id_size );

    /* If error unpacking value ID, break */
    if (err_no != E_SUCCESS)
    {
      ERR("Error unpacking Value ID of CommonPowerParameters Attrib",
            0,0,0);
      break;
    }

    /* Since Value Id was parsed sucessfully, Update the attribute offset to 
     * start of  field */ 
    attrib_offset += p_aip->value_id_size;

    /* Unpack Allocation Stagger */
    err_no = HDRBIT_UNPACK8( 
              item_ptr,
              &(hdrscmrmac3.common_power_params.alloc_stagger), 
              (p_aip->attrib_offset + attrib_offset), 
              HDRSCMRMAC3_ALLOCATION_STAGGER_SIZE );

    /* If error unpacking AllocationStagger, break */
    if (err_no != E_SUCCESS)
    {
      ERR("Error unpacking AllocationStagger of  CommonPowerParameters Attrib",
            0,0,0);
      break;
    }

    /* Shift the offset to start of TxT2PMin Field */
    attrib_offset += HDRSCMRMAC3_ALLOCATION_STAGGER_SIZE;

    /* Unpack TxT2PMin */
    err_no = HDRBIT_UNPACK8( 
              item_ptr,
              &(hdrscmrmac3.common_power_params.tx_t2p_min_db), 
              (p_aip->attrib_offset + attrib_offset), 
              HDRSCMRMAC3_TX_T2P_MIN_SIZE );

    /* If error unpacking TxT2PMin, break */
    if (err_no != E_SUCCESS)
    {
      ERR("Error unpacking TxT2PMin of  CommonPowerParameters Attrib",
            0,0,0);
      break;
    }

    /* Shift the offset to start of RPCStep field */
    attrib_offset += HDRSCMRMAC3_TX_T2P_MIN_SIZE;

    /* Unpack RPCStep */
    err_no = HDRBIT_UNPACK8( 
              item_ptr,
              &(hdrscmrmac3.common_power_params.rpc_step_db), 
              (p_aip->attrib_offset + attrib_offset), 
              HDRSCMRMAC3_RPC_STEP_SIZE );

    /* If error unpacking TxT2PMin, break */
    if (err_no != E_SUCCESS)
    {
      ERR("Error unpacking RPCStep of  CommonPowerParameters Attrib",
            0,0,0);
      break;
    }

    /* Parsing done! :-) Is the parsed attribute valid? */
    if ( (err_no = 
          hdrscmrmac3_is_common_power_params_valid()) != E_OUT_OF_RANGE)
    {
      /* Attrib valid! Yea! :-) print F3 mesg */
      HDR_MSG_PROT_3( MSG_LEGACY_HIGH, "CommonPowerParameters %x %x %x",
                      hdrscmrmac3.common_power_params.alloc_stagger,
                      hdrscmrmac3.common_power_params.tx_t2p_min_db,
                      hdrscmrmac3.common_power_params.rpc_step_db );

      /* Update the accepted attribute information */
      a_aip->is_accepted = TRUE;
      a_aip->value_id = value_id;
      a_aip->complex_val_ptr = (void*) &(hdrscmrmac3.common_power_params);
      a_aip->complex_val_size = sizeof(hdrscmrmac3_common_pow_params_struct_type);
      break;
    }

    /* Not much luck :-( Let's try next set of values if there is one. 
       Shift the offset to start of next value ID. */
    attrib_offset += (HDRSCMRMAC3_RPC_STEP_SIZE + 
                      HDRSCMRMAC3_RSVD_COM_POW_PARAM_SIZE);

  } /* while */

  return err_no;

} /* hdrscmrmac3_process_common_power_params */


/*===========================================================================

FUNCTION HDRSCMRMAC3_IS_MAX_NUM_SUB_PKTS_VALID                     LOCAL

DESCRIPTION
  Check if the following attribute is valid 

  0x0002 - MaxNumSubPackets 

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  E_SUCCESS      - If the Attribute is valid.
  E_OUT_OF_RANGE - otherwise. 

SIDE EFFECTS
  None.

===========================================================================*/
static hdrerrno_enum_type hdrscmrmac3_is_max_num_sub_pkts_valid (void)
{

  uint8 i0 = hdrscmrmac3.max_num_sub_pkts.interlace[0];
    /* Max Number of subpackets for interlace 0 */

  uint8 i1 = hdrscmrmac3.max_num_sub_pkts.interlace[1];
    /* Max Number of subpackets for interlace 1 */

  uint8 i2 = hdrscmrmac3.max_num_sub_pkts.interlace[2];
    /* Max Numeber of subpackets for interlace 2 */

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */   

  /* Accept default only (0x3) for Phase-1 */
  return ( (i0 == HDRSCMRMAC3_DEFAULT_MAX_NUM_SUB_PACKETS)  && 
           (i1 == HDRSCMRMAC3_DEFAULT_MAX_NUM_SUB_PACKETS)  &&
           (i2 == HDRSCMRMAC3_DEFAULT_MAX_NUM_SUB_PACKETS)  
         )? E_SUCCESS: E_OUT_OF_RANGE;

} /* hdrscmrmac3_is_max_num_sub_pkts_valid */


 /* EJECT */
/*===========================================================================

FUNCTION HDRSCMRMAC3_PROCESS_MAX_NUM_SUB_PKTS                    LOCAL

DESCRIPTION
  This is a generic interface to process the MaxNumSubPackets
  Attribute.

  0x0002 - MaxNumSubPackets Attribute 

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
static hdrerrno_enum_type hdrscmrmac3_process_max_num_sub_pkts
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

  uint8 inter = 0;
    /* Interlace number */

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */ 

  /* Initialize the attribute offset to start of first Value ID */
  attrib_offset = p_aip->attrib_len_size + a_aip->attrib_id_size;

  /* While there are enough bits left in the Attribute Record for one more
     set of  MaxNumSubPackets complex Attribute Value */
  while ( (attrib_offset + p_aip->value_id_size + 
           HDRSCMRMAC3_MAX_NUM_SUB_PKTS_SIZE ) <= p_aip->attrib_len )
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
      ERR("Error unpacking Value ID of  MaxNumSubPackets Attrib",
            0,0,0);
      break;
    }

    /* Since Value Id was parsed sucessfully, Update the attribute offset to 
     * start of  field */ 
    attrib_offset += p_aip->value_id_size ;

    for ( inter=0; (inter < HDRSCMRMAC3_MAX_NUM_INTERLACES) && (!err_no); 
          ++inter)
    {
      /* Unpack MaxNumSubPackets0 */
      err_no = HDRBIT_UNPACK8( item_ptr,
                               &(hdrscmrmac3.max_num_sub_pkts.interlace[inter]),
                               (p_aip->attrib_offset + attrib_offset),
                               HDRSCMRMAC3_MAX_NUM_SUB_PKTS_N_SIZE );

      /* Shift the offset to next field */
      attrib_offset += HDRSCMRMAC3_MAX_NUM_SUB_PKTS_N_SIZE;
    }

    /* If error unpacking MaxNumSubPackets, break */
    if (err_no != E_SUCCESS)
    {
      ERR("Error unpacking MaxNumSubPackets for interlace %d",
          (inter-1),0,0);
      break;
    }

    /* Parsing done! :-) Is the parsed attribute valid? */
    if ( (err_no = 
          hdrscmrmac3_is_max_num_sub_pkts_valid()) != E_OUT_OF_RANGE )
    {
      /* Attrib valid! Yea! :-) print F3 mesg */
      HDR_MSG_PROT_3( MSG_LEGACY_HIGH, "MaxNumSubPackets %x %x %x",
                      hdrscmrmac3.max_num_sub_pkts.interlace[0],
                      hdrscmrmac3.max_num_sub_pkts.interlace[1],
                      hdrscmrmac3.max_num_sub_pkts.interlace[2] );

      /* Update the accepted attribute information */
      a_aip->is_accepted = TRUE;
      a_aip->value_id = value_id;
      a_aip->complex_val_ptr = (void*) &(hdrscmrmac3.max_num_sub_pkts);
      a_aip->complex_val_size = sizeof(hdrscmrmac3_max_num_sub_pkts_struct_type);
      break;
    }

    /* Not much luck :-( Let's try next set of values if there is one. 
       Shift the offset to start of next value ID. */
    attrib_offset += HDRSCMRMAC3_RSVD_MAX_NUM_SUB_PKTS_SIZE;

  } /* while */

  return err_no;

} /* hdrscmrmac3_process_max_num_sub_pkts */


 /* EJECT */
/*===========================================================================

FUNCTION HDRSCMRMAC3_IS_PERMITTED_PAYLOAD_VALID                     LOCAL

DESCRIPTION
  Check if the following attribute is valid 

  0x0003 - PermittedPayload 

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  E_SUCCESS       - If the Attribute is valid.
  E_OUT_OF_RANGE  - otherwise. 

SIDE EFFECTS
  None.

===========================================================================*/
static hdrerrno_enum_type hdrscmrmac3_is_permitted_payload_valid(void)
{

  uint8 pp;
    /* Permitted payload */

  hdrscmrmaccmn_payload_size_enum_type p_ind;
    /* payload index */

  uint8 subframe_ind;
    /* Subframe history index */

  boolean is_valid = TRUE;  
    /* Value is valid */
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */ 

  /* For each possible payload size of pkts sent in previous subframes */
  for (p_ind = HDRSCMRMACCMN_FIRST_PAYLOAD_SIZE; 
        (p_ind < HDRSCMRMACCMN_MAX_NUM_PAYLOADS) && (is_valid); ++p_ind)
  {
    /* For each subframe history indices, n-1, n-2, & n-3 */
    for ( subframe_ind=0; 
          (subframe_ind < HDRSCMRMAC3_MAX_NUM_SUBFRAME_HISTORY)&&(is_valid); 
          ++subframe_ind )
    {
      /* Store to local variable for convenience */
      pp = hdrscmrmac3.permitted_payload[p_ind][subframe_ind];

      /* Is permittedPayload for payload index 'p_ind' and subframe history 
         index subframe_ind valid? */
      is_valid = (pp <= HDRSCMRMACCMN_LAST_PAYLOAD_SIZE);
    }

  }

  return (is_valid)? E_SUCCESS: E_OUT_OF_RANGE;

} /* hdrscmrmac3_is_permitted_payload_valid */


 /* EJECT */
/*===========================================================================

FUNCTION HDRSCMRMAC3_PROCESS_PERMITTED_PAYLOAD                    LOCAL

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
  TRUE  - If a valid attribute was parsed successfully.
  FALSE - otherwise. 

SIDE EFFECTS
  None.

===========================================================================*/
static hdrerrno_enum_type hdrscmrmac3_process_permitted_payload
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

  hdrscmrmaccmn_payload_size_enum_type p_ind;
    /* Payload Index */

  uint8 sf_hist = 0;
    /* Subframe history Index */

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */ 

  /* Initialize the attribute offset to start of first Value ID */
  attrib_offset = p_aip->attrib_len_size + a_aip->attrib_id_size;

  /* While there are enough bits left in the Attribute Record for one more
     set of  PermittedPayload complex Attribute Value */
  while ( (attrib_offset + p_aip->value_id_size + 
           HDRSCMRMAC3_PERMITTED_PAYLOAD_SIZE ) <= p_aip->attrib_len )
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
      ERR("Error unpacking Value ID of  PermittedPayload Attrib",
            0,0,0);
      break;
    }

    /* Since Value Id was parsed sucessfully, shift the attribute offset to 
     * start of PermittedPayload0_1 field */ 
    attrib_offset += p_aip->value_id_size ;


    /* For each possible payload size of pkts sent in previous subframes */
    for ( p_ind=HDRSCMRMACCMN_FIRST_PAYLOAD_SIZE; 
          (p_ind < HDRSCMRMACCMN_MAX_NUM_PAYLOADS) && (!err_no);
          ++p_ind )
    {
      /* For each subframe history indices, n-1, n-2, & n-3 */
      for ( sf_hist=0; (sf_hist<HDRSCMRMAC3_MAX_NUM_SUBFRAME_HISTORY) && 
            (err_no == E_SUCCESS); ++sf_hist)
      {
        /* Unpack permittedpayload(p_ind, sf_hist) */
        err_no = HDRBIT_UNPACK8( 
                  item_ptr, 
                  &(hdrscmrmac3.permitted_payload[p_ind][sf_hist]), 
                  (p_aip->attrib_offset + attrib_offset),
                  HDRSCMRMAC3_PERMITTED_PAYLOAD_X_Y_SIZE );

        /* Shift the attrib offset to next field */
        attrib_offset += HDRSCMRMAC3_PERMITTED_PAYLOAD_X_Y_SIZE;
      }
    }

    /* If error unpacking PermittedPayload attribute, break */
    if (err_no != E_SUCCESS)
    {
      ERR("Error unpacking PermittedPayload attribute for p: %d s: %d",
          (p_ind-1),(sf_hist-1),0);
      break;
    }

    /* Parsing done! :-) Is the parsed attribute valid? */
    if ( (err_no = 
          hdrscmrmac3_is_permitted_payload_valid()) != E_OUT_OF_RANGE)
    {
      /* Attrib valid! Yea! :-) print F3 mesg */
      for ( p_ind=HDRSCMRMACCMN_FIRST_PAYLOAD_SIZE; 
            (p_ind < HDRSCMRMACCMN_MAX_NUM_PAYLOADS); ++p_ind )
      {
        HDR_MSG_PROT_4(MSG_LEGACY_LOW,
                       "PermittedPayload%d_ %x, %x, %x",
                       p_ind,
                       hdrscmrmac3.permitted_payload[p_ind][0],
                       hdrscmrmac3.permitted_payload[p_ind][1],
                       hdrscmrmac3.permitted_payload[p_ind][2] );
      }

      /* Update the accepted attribute information */
      a_aip->is_accepted = TRUE;
      a_aip->value_id = value_id;
      a_aip->complex_val_ptr = (void*) &hdrscmrmac3.permitted_payload[0][0];
      a_aip->complex_val_size = sizeof(hdrscmrmac3_permitted_payload_type);
      break;
    }

    /* Not much luck :-( Let's try next set of values if there is one. 
       Shift the offset to start of next value ID. */
    attrib_offset += HDRSCMRMAC3_RSVD_PERMITTED_PAYLOAD_SIZE;

  } /* while */

  return err_no;

} /* hdrscmrmac3_process_permitted_payload */



 /* EJECT */
/*===========================================================================

FUNCTION HDRSCMRMAC3_DUMP_PILOT_STRENGTH                     LOCAL

DESCRIPTION
  Dump the following attribute in f3 mesg for debugging.

  0x0004 PilotStrength Attribute

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
static void hdrscmrmac3_dump_pilot_strength (void)
{

  uint8 ps_axis_point = 0;
    /* Array index */

  uint8 n_ps_axis_vals = hdrscmrmac3.pilot_strength.num_ps_axis_values;
    /* for Convenience */
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */   

  HDR_MSG_PROT_1( MSG_LEGACY_HIGH,
                  "NumPilotStrengthAxisValues: %x",
                  n_ps_axis_vals);

  for (ps_axis_point=0; ps_axis_point < n_ps_axis_vals; ++ps_axis_point)
  {
    HDR_MSG_PROT_3( 
      MSG_LEGACY_HIGH, "Ind: %x PS %x PSPS %x", 
      ps_axis_point,
      hdrscmrmac3.pilot_strength.ps_axis[ps_axis_point], 
      hdrscmrmac3.pilot_strength.ps_ps_axis[ps_axis_point] );
  }

}


 /* EJECT */
/*===========================================================================

FUNCTION HDRSCMRMAC3_IS_PILOT_STRENGTH_VALID                     LOCAL

DESCRIPTION
  Check if the following attribute is valid 

  0x0004 PilotStrength Attribute

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  E_SUCCESS      - If the Attribute is valid.
  E_OUT_OF_RANGE - otherwise. 

SIDE EFFECTS
  None.

===========================================================================*/
static hdrerrno_enum_type hdrscmrmac3_is_pilot_strength_valid (void)
{

  uint8 ps_axis_val;
    /* Local var PilotStrengthAxis values for convenience */

  uint8 n_ps_axis_vals = hdrscmrmac3.pilot_strength.num_ps_axis_values;
    /* Local var NumPilotStrengthAxisValues for convenience */

  uint8 ps_ind;
    /* Pilot Strength Axis index */

  boolean is_valid = FALSE;
    /* Is the attrib Valid? */

  boolean has_dup = FALSE;
    /* Does the axis have duplicate points */
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */   

  /*------------------------------------------------------------------------
     Any 4 bit value is valid for PilotStrengthPilotStrengthAxis field. So 
     Checking validity of NumPilotStrengthAxisValues and PilotStrengthAxis 
     fields alone.
   -----------------------------------------------------------------------*/

  /* NumPilotStrengthAxisValues cannot be zero */
  is_valid = 
    ((n_ps_axis_vals > 0) && (n_ps_axis_vals <= HDRSCMRMAC3_MAX_PS_POINTS));

  /* Inspect PilotStrengthAxis values */
  for (ps_ind=0; ((ps_ind < n_ps_axis_vals) && (is_valid)); ++ps_ind)
  {
    ps_axis_val = hdrscmrmac3.pilot_strength.ps_axis[ps_ind];

    is_valid = (ps_axis_val <= HDRSCMRMAC3_MAX_PS_AXIS);
  }

  if (is_valid && ((n_ps_axis_vals > 0) && (n_ps_axis_vals <= HDRSCMRMAC3_MAX_PS_POINTS)))
  {
    /* The pilot strength axis has to monotonically increase. At present, 
       software allows for non-sorted order. However, it does not allow 
       duplicate values in the axis. Checking for duplicates... */
    has_dup = hdrscmrmaccmn_axis_has_dup_points(
                  hdrscmrmac3.pilot_strength.ps_axis, 
                  n_ps_axis_vals);

    is_valid = (has_dup == FALSE);
  }

  if (!is_valid)
  {
    HDR_MSG_PROT(MSG_LEGACY_ERROR, "PilotStrength(0x0004) Invalid. Rejected");
    hdrscmrmac3_dump_pilot_strength();
  }

  return (is_valid)? E_SUCCESS: E_OUT_OF_RANGE;

} /* hdrscmrmac3_is_pilot_strength_valid */


 /* EJECT */
/*===========================================================================

FUNCTION HDRSCMRMAC3_PROCESS_PILOT_STRENGTH                    LOCAL

DESCRIPTION This is a generic interface to process the PilotStrength Attribute.

  0x0004 - PilotStrength Attribute

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
static hdrerrno_enum_type hdrscmrmac3_process_pilot_strength
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

  uint8 n_ps_axis_vals;
    /* Local var NumPilotStrengthAxisValues for convenience */

  uint8 ps_axis_point;
    /* PilotStrength axis point used as array index */

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */ 

  /* Initialize the attribute offset to start of first Value ID */
  attrib_offset = p_aip->attrib_len_size + a_aip->attrib_id_size;

  /* While there are enough bits left in the Attribute Record for one more
     set of  complex Attribute Value */
  while ( (attrib_offset + p_aip->value_id_size + 
           HDRSCMRMAC3_PS_ATTRIB_MIN_SIZE ) <= p_aip->attrib_len )
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
      ERR("Error unpacking Value ID of  Attrib",
            0,0,0);
      break;
    }

    /* Since Value Id was parsed sucessfully, Update the attribute offset to 
     * start of  field */ 
    attrib_offset += p_aip->value_id_size ;

    /* Unpack NumPilotStrengthAxisValues */
    err_no = HDRBIT_UNPACK8( item_ptr,
                             &(hdrscmrmac3.pilot_strength.num_ps_axis_values),
                             (p_aip->attrib_offset + attrib_offset),
                             HDRSCMRMAC3_PS_NUM_PS_AXIS_VALS_SIZE );

    /* If error unpacking NumPilotStrengthAxisValues, break */
    if (err_no != E_SUCCESS)
    {
      ERR("Error unpacking NumPilotStrengthAxisValues of PilotStrength Attrib",
          0,0,0);
      break;
    }

    /* Store into a local var that is easier to type! */
    n_ps_axis_vals = hdrscmrmac3.pilot_strength.num_ps_axis_values;

    /* Check if NumPilotStrengthAxisValues is within permissible range */
    if ((n_ps_axis_vals < HDRSCMRMAC3_MIN_PS_NUM_PS_AXIS_VALUES) || 
        (n_ps_axis_vals > HDRSCMRMAC3_MAX_PS_POINTS))
    { 
	  err_no = E_OUT_OF_RANGE;
      ERR("Invalid NumPilotStrengthAxisValues; Bailing out!", 0, 0, 0);
      break;
    }

    /* Valid NumPilotStrengthAxisValues! :-) Yeaaaaa
       Shift the offset to next field */
    attrib_offset += HDRSCMRMAC3_PS_NUM_PS_AXIS_VALS_SIZE;

    /* Loop around and parse n_ps_axis_vals instances of PSAxis and PSPSAxis 
       fields */
    for (ps_axis_point=0; ((ps_axis_point < n_ps_axis_vals) && (!err_no)); 
         ++ps_axis_point)
    {
      /* Unpack PilotStrengthAxis */
      err_no = HDRBIT_UNPACK8( 
                item_ptr, 
                &(hdrscmrmac3.pilot_strength.ps_axis[ps_axis_point]), 
                (p_aip->attrib_offset + attrib_offset),
                HDRSCMRMAC3_PS_AXIS_SIZE );

      /* If PilotStrengthAxis was successfully unpacked... */
      if (err_no == E_SUCCESS)
      {
        /* PSAxis successfully unpacked! Yeaaaa
           Shift the offset to start of PSPSAxis */
        attrib_offset += HDRSCMRMAC3_PS_AXIS_SIZE;

        /* Unpack PilotStrengthPilotStrengthAxis */
        err_no = HDRBIT_UNPACK8( 
                  item_ptr, 
                  &(hdrscmrmac3.pilot_strength.ps_ps_axis[ps_axis_point]), 
                  (p_aip->attrib_offset + attrib_offset),
                  HDRSCMRMAC3_PS_PS_AXIS_SIZE );

        /* Shift the offset to start of next PSAxis */
        attrib_offset += HDRSCMRMAC3_PS_PS_AXIS_SIZE;
      }

    } /* for */

    if (err_no != E_SUCCESS)
    {
      ERR("Error unpacking PSAxis/PSPSAxis at ind %d", (ps_axis_point-1),0,0);
      break;
    }

    /* Parsing done! :-) Is the parsed attribute valid? */
    if ( (err_no =
          hdrscmrmac3_is_pilot_strength_valid()) != E_OUT_OF_RANGE )
    {
      /* Attrib valid! Yea! :-) print F3 mesg */
      hdrscmrmac3_dump_pilot_strength();

      /* Update the accepted attribute information */
      a_aip->is_accepted = TRUE;
      a_aip->value_id = value_id;
      a_aip->complex_val_ptr = (void*) &(hdrscmrmac3.pilot_strength);
      a_aip->complex_val_size = 
        sizeof(hdrscmrmac3_pilot_strength_grid_struct_type);
      break;
    }

    /* Not much luck :-( Let's try next set of values if there is one. 
       Shift the offset to start of next value ID. */
    attrib_offset += HDRSCMRMAC3_RSVD_PS_SIZE;

  } /* while */

  return err_no;

} /* hdrscmrmac3_process_pilot_strength */


 /* EJECT */
/*===========================================================================

FUNCTION HDRSCMRMAC3_IS_POWER_PARAMS_VALID                     LOCAL

DESCRIPTION
  Check if the following attribute are valid 

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
  pl_ind - Payload Index.

RETURN VALUE
  E_SUCCESS      - If the Attribute is valid.
  E_OUT_OF_RANGE - otherwise. 

SIDE EFFECTS
  None.

===========================================================================*/
static hdrerrno_enum_type hdrscmrmac3_is_power_params_valid
(
  hdrscmrmaccmn_payload_size_enum_type pl_ind
)
{
  uint8 ll_pre_trans;
    /* Lo-Lat Pre-transition */

  uint8 ll_post_trans;
    /* Lo-Lat Post transition */

  uint8 hc_pre_trans;
    /* High-Cap Pre-transition */

  uint8 hc_post_trans;
    /* High-Cap Post-transition */

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */   

  /*------------------------------------------------------------------------
    Any 2 bit value is valid for HiCap/LoLat T2PTransition and Termination
    Target. So checking the validity of Pre/Post transition only.
   -----------------------------------------------------------------------*/

  /* Store them in a local variable for ease of access */
  ll_pre_trans = 
    hdrscmrmac3.power_params[pl_ind][HDRSCMRMAC3_TX_MODE_LO_LAT].pre_transition;
  ll_post_trans = 
    hdrscmrmac3.power_params[pl_ind][HDRSCMRMAC3_TX_MODE_LO_LAT].post_transition; 
  hc_pre_trans = 
    hdrscmrmac3.power_params[pl_ind][HDRSCMRMAC3_TX_MODE_HI_CAP].pre_transition;
  hc_post_trans = 
    hdrscmrmac3.power_params[pl_ind][HDRSCMRMAC3_TX_MODE_HI_CAP].post_transition;

  return ( (ll_pre_trans <= HDRSCMRMAC3_MAX_PP_PRE_TRANSITION)    &&
           (ll_post_trans <= HDRSCMRMAC3_MAX_PP_POST_TRANSITION)  &&
           (hc_pre_trans <= HDRSCMRMAC3_MAX_PP_PRE_TRANSITION)    &&
           (hc_post_trans <= HDRSCMRMAC3_MAX_PP_POST_TRANSITION)
         )? E_SUCCESS: E_OUT_OF_RANGE;

} /* hdrscmrmac3_is_power_params_valid */


 /* EJECT */
/*===========================================================================

FUNCTION HDRSCMRMAC3_PROCESS_POWER_PARAMS                    LOCAL

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
  TRUE  - If a valid attribute was parsed successfully.
  FALSE - otherwise. 

SIDE EFFECTS
  None.

===========================================================================*/
static hdrerrno_enum_type hdrscmrmac3_process_power_params
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

  hdrscmrmaccmn_payload_size_enum_type pl_ind;
    /* Payload Index corresponding to this power parameter */

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */ 

  /* First power parameter is stored at index 1 */
  pl_ind = (hdrscmrmaccmn_payload_size_enum_type) 
    (a_aip->attrib_id - (uint16) HDRSCMRMAC3_FIRST_POWER_PARAMS_ATTRIB + 1);
  
  /* Initialize the attribute offset to start of first Value ID */
  attrib_offset = p_aip->attrib_len_size + a_aip->attrib_id_size;

  /* While there are enough bits left in the Attribute Record for one more
     set of  PowerParameters complex Attribute Value */
  while ( (attrib_offset + p_aip->value_id_size + 
           HDRSCMRMAC3_POWER_PARAMS_SIZE ) <= p_aip->attrib_len )
  {
    /*----------------------------------------------------------------------
       There is at least one more attribute value in this attribute Record
     *--------------------------------------------------------------------*/

    /* Initialize the error code to success since we know that we have at least
       one attrib value set in the Attrib Record */
    err_no = E_SUCCESS;

    /*----------------------------------------------------------------------
                                  VALUE ID
     *--------------------------------------------------------------------*/
    /* Unpack the Value ID into a uint8 */
    err_no = HDRBIT_UNPACK8( item_ptr,
                             &(value_id),
                             (p_aip->attrib_offset + attrib_offset),
                             p_aip->value_id_size );

    /* If error unpacking value ID, break */
    if (err_no != E_SUCCESS)
    {
      ERR("Error unpacking Value ID of  Attrib",
            0,0,0);
      break;
    }

    /*----------------------------------------------------------------------
                              LoLatT2PTransition
     *--------------------------------------------------------------------*/

    /* Since Value Id was parsed successfully, Shift the attribute offset to 
     * start of  LoLatT2PTransitionXXX field */ 
    attrib_offset += p_aip->value_id_size ;

    /* Unpack LoLatT2PTransitionXXXX field */
    err_no = HDRBIT_UNPACK8( 
              item_ptr, 
              &(hdrscmrmac3.power_params
                [pl_ind][HDRSCMRMAC3_TX_MODE_LO_LAT].t2p_transition), 
              (p_aip->attrib_offset + attrib_offset),
              HDRSCMRMAC3_PP_T2P_TRANSITION_SIZE );

    /* If error unpacking LoLatT2PTransitionXXXX,  break */
    if (err_no != E_SUCCESS)
    {
      ERR("Error unpacking LoLatT2PTransitionXXXX of PowerParameters attrib",
          0,0,0);
      break;
    }

    /*----------------------------------------------------------------------
                            LoLatTerminationTarget
     *--------------------------------------------------------------------*/

    /* Shift the offset to LoLatTerminationTargetXXXX field */
    attrib_offset += HDRSCMRMAC3_PP_T2P_TRANSITION_SIZE;

    /* Unpack LoLatTerminationTargetXXXX field */
    err_no = HDRBIT_UNPACK8( 
              item_ptr, 
              &(hdrscmrmac3.power_params
                [pl_ind][HDRSCMRMAC3_TX_MODE_LO_LAT].termination_target), 
              (p_aip->attrib_offset + attrib_offset),
              HDRSCMRMAC3_PP_TERMINATION_TARGET_SIZE );

    /* If error unpacking LoLatTerminationTargetXXXX,  break */
    if (err_no != E_SUCCESS)
    {
      ERR("Error unpacking LoLatTerminationTargetXXXX of PowerParameters attrib",
          0,0,0);
      break;
    }

    /*----------------------------------------------------------------------
                              HiCapT2PTransition
     *--------------------------------------------------------------------*/

    /* Shift the offset to HiCapT2PTransition field */
    attrib_offset += HDRSCMRMAC3_PP_TERMINATION_TARGET_SIZE;

    /* Unpack HiCapT2PTransitionXXXX field */
    err_no = HDRBIT_UNPACK8( 
              item_ptr, 
              &(hdrscmrmac3.power_params
                [pl_ind][HDRSCMRMAC3_TX_MODE_HI_CAP].t2p_transition), 
              (p_aip->attrib_offset + attrib_offset),
              HDRSCMRMAC3_PP_T2P_TRANSITION_SIZE );

    /* If error unpacking HiCapT2PTransitionXXXX,  break */
    if (err_no != E_SUCCESS)
    {
      ERR("Error unpacking HiCapT2PTransitionXXXX of PowerParameters attrib",
          0,0,0);
      break;
    }

    /*----------------------------------------------------------------------
                            HiCapTerminationTarget
     *--------------------------------------------------------------------*/

    /* Shift the offset to HiCapTerminationTargetXXXX field */
    attrib_offset += HDRSCMRMAC3_PP_T2P_TRANSITION_SIZE;

    /* Unpack HiCapTerminationTargetXXXX field */
    err_no = HDRBIT_UNPACK8( 
              item_ptr,
              &(hdrscmrmac3.power_params
                [pl_ind][HDRSCMRMAC3_TX_MODE_HI_CAP].termination_target),
              (p_aip->attrib_offset + attrib_offset),
              HDRSCMRMAC3_PP_TERMINATION_TARGET_SIZE );

    /* If error unpacking HiCapTerminationTargetXXXX,  break */
    if (err_no != E_SUCCESS)
    {
      ERR("Error unpacking HiCapTerminationTargetXXXX of PowerParameters attrib",
          0,0,0);
      break;
    }

    /*----------------------------------------------------------------------
                           T2PLoLatPreTransition
     *--------------------------------------------------------------------*/

    /* Shift the offset to T2PLoLatPreTransitionXXXX field */
    attrib_offset += HDRSCMRMAC3_PP_TERMINATION_TARGET_SIZE;

    /* Unpack T2PLoLatPreTransitionXXXX field */
    err_no = HDRBIT_UNPACK8( 
              item_ptr,
              &(hdrscmrmac3.power_params
                [pl_ind][HDRSCMRMAC3_TX_MODE_LO_LAT].pre_transition), 
              (p_aip->attrib_offset + attrib_offset),
              HDRSCMRMAC3_PP_T2P_PRE_TRANSITION_SIZE );

    /* If error unpacking T2PLoLatPreTransitionXXXX,  break */
    if (err_no != E_SUCCESS)
    {
      ERR("Error unpacking T2PLoLatPreTransitionXXXX  of PowerParameters attrib",
          0,0,0);
      break;
    }

    /*----------------------------------------------------------------------
                          T2PLoLatPostTransition
     *--------------------------------------------------------------------*/

    /* Shift the offset to T2PLoLatPostTransitionXXXX field */
    attrib_offset += HDRSCMRMAC3_PP_T2P_PRE_TRANSITION_SIZE;

    /* Unpack T2PLoLatPostTransitionXXXX field */
    err_no = HDRBIT_UNPACK8( 
              item_ptr,
              &(hdrscmrmac3.power_params
                [pl_ind][HDRSCMRMAC3_TX_MODE_LO_LAT].post_transition),
              (p_aip->attrib_offset + attrib_offset),
              HDRSCMRMAC3_PP_T2P_POST_TRANSITION_SIZE );

    /* If error unpacking T2PLoLatPostTransitionXXXX,  break */
    if (err_no != E_SUCCESS)
    {
      ERR("Error unpacking T2PLoLatPostTransitionXXXX  of PowerParameters attrib",
          0,0,0);
      break;
    }

    /*----------------------------------------------------------------------
                          T2PHiCapPreTransition
     *--------------------------------------------------------------------*/

    /* Shift the offset to T2PHiCapPreTransitionXXXX field */
    attrib_offset += HDRSCMRMAC3_PP_T2P_POST_TRANSITION_SIZE;

    /* Unpack T2PHiCapPreTransitionXXXX field */
    err_no = HDRBIT_UNPACK8( 
              item_ptr,
              &(hdrscmrmac3.power_params
                [pl_ind][HDRSCMRMAC3_TX_MODE_HI_CAP].pre_transition),
              (p_aip->attrib_offset + attrib_offset),
              HDRSCMRMAC3_PP_T2P_PRE_TRANSITION_SIZE );

    /* If error unpacking T2PHiCapPreTransitionXXXX,  break */
    if (err_no != E_SUCCESS)
    {
      ERR("Error unpacking T2PHiCapPreTransitionXXXX  of PowerParameters attrib",
          0,0,0);
      break;
    }

    /*----------------------------------------------------------------------
                          T2PHiCapPostTransition
     *--------------------------------------------------------------------*/

    /* Shift the offset to T2PHiCapPostTransitionXXXX field */
    attrib_offset += HDRSCMRMAC3_PP_T2P_PRE_TRANSITION_SIZE;

    /* Unpack T2PHiCapPostTransitionXXXX field */
    err_no = HDRBIT_UNPACK8( 
              item_ptr,
              &(hdrscmrmac3.power_params
                [pl_ind][HDRSCMRMAC3_TX_MODE_HI_CAP].post_transition),
              (p_aip->attrib_offset + attrib_offset),
              HDRSCMRMAC3_PP_T2P_POST_TRANSITION_SIZE );

    /* If error unpacking T2PHiCapPostTransitionXXXX,  break */
    if (err_no != E_SUCCESS)
    {
      ERR("Error unpacking T2PHiCapPostTransitionXXXX  of PowerParameters attrib",
          0,0,0);
      break;
    }

    /* Parsing done! :-) Is the parsed attribute valid? */
    if ( (err_no =
          hdrscmrmac3_is_power_params_valid(pl_ind)) != E_OUT_OF_RANGE )
    {
      /* Attrib valid! Yea! :-) print F3 mesg */
      HDR_MSG_PROT_5( 
        MSG_LEGACY_LOW, "PowerParams: %x %x %x %x %x", 
        a_aip->attrib_id, 
        hdrscmrmac3.power_params[pl_ind][HDRSCMRMAC3_TX_MODE_LO_LAT].t2p_transition, 
        hdrscmrmac3.power_params[pl_ind][HDRSCMRMAC3_TX_MODE_LO_LAT].termination_target, 
        hdrscmrmac3.power_params[pl_ind][HDRSCMRMAC3_TX_MODE_HI_CAP].t2p_transition, 
        hdrscmrmac3.power_params[pl_ind][HDRSCMRMAC3_TX_MODE_HI_CAP].termination_target);

      HDR_MSG_PROT_5( 
        MSG_LEGACY_LOW, "PowerParams: %x contd. %x, %x, %x, %x",
        a_aip->attrib_id,
        hdrscmrmac3.power_params[pl_ind][HDRSCMRMAC3_TX_MODE_LO_LAT].pre_transition, 
        hdrscmrmac3.power_params[pl_ind][HDRSCMRMAC3_TX_MODE_LO_LAT].post_transition, 
        hdrscmrmac3.power_params[pl_ind][HDRSCMRMAC3_TX_MODE_HI_CAP].pre_transition, 
        hdrscmrmac3.power_params[pl_ind][HDRSCMRMAC3_TX_MODE_HI_CAP].post_transition );

      /* Update the accepted attribute information */
      a_aip->is_accepted = TRUE;
      a_aip->value_id = value_id;
      a_aip->complex_val_ptr = (void*) &(hdrscmrmac3.power_params[pl_ind][0]);
      a_aip->complex_val_size = 
        ( sizeof(hdrscmrmac3_power_params_struct_type) * 
          (uint32) HDRSCMRMAC3_NUM_TX_MODES );
      break;
    }

    /* Not much luck :-( Let's try next set of values if there is one. 
       Shift the offset to start of next value ID. */
    attrib_offset += HDRSCMRMAC3_PP_T2P_POST_TRANSITION_SIZE;

  } /* while */

  return err_no;

} /* hdrscmrmac3_process_power_params */


 /* EJECT */
/*===========================================================================

FUNCTION HDRSCMRMAC3_IS_REQUEST_PARAMS_VALID                     LOCAL

DESCRIPTION
  Check if the following attribute is valid 

  0x0011 - RequestParameters Attribute

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  E_SUCCESS      - If the Attribute is valid.
  E_OUT_OF_RANGE - otherwise. 

SIDE EFFECTS
  None.

===========================================================================*/
static hdrerrno_enum_type hdrscmrmac3_is_request_params_valid(void)
{

  uint8 req_ratio = hdrscmrmac3.req_params.req_ratio;
    /* ReqRatio. Convenience holder */

  uint8 max_req_interval = hdrscmrmac3.req_params.max_req_interval;
    /* MaxReqInteval Convenience holder */

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */   

  return ( (req_ratio <= HDRSCMRMAC3_RP_MAX_REQ_RATIO)   &&
           (max_req_interval <= HDRSCMRMAC3_RP_MAX_MAX_REQ_INTERVAL)
         )? E_SUCCESS: E_OUT_OF_RANGE;

} /* hdrscmrmac3_is_request_params_valid */


 /* EJECT */
/*===========================================================================

FUNCTION HDRSCMRMAC3_PROCESS_REQUEST_PARAMS                    LOCAL

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
  TRUE  - If a valid attribute was parsed successfully.
  FALSE - otherwise. 

SIDE EFFECTS
  None.

===========================================================================*/
static hdrerrno_enum_type hdrscmrmac3_process_request_params
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

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */ 

  /* Initialize the attribute offset to start of first Value ID */
  attrib_offset = p_aip->attrib_len_size + a_aip->attrib_id_size;

  /* While there are enough bits left in the Attribute Record for one more
     set of RequestParameters complex Attribute Value */
  while ( (attrib_offset + p_aip->value_id_size + 
           HDRSCMRMAC3_REQUEST_PARAMS_SIZE ) <= p_aip->attrib_len )
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
      ERR("Error unpacking Value ID of  Attrib",
            0,0,0);
      break;
    }

    /*----------------------------------------------------------------------
                                  ReqRatio
     *--------------------------------------------------------------------*/

    /* Shift the attribute offset to start of  ReqRatio field */ 
    attrib_offset += p_aip->value_id_size ;

    /* Unpack ReqRatio */
    err_no = HDRBIT_UNPACK8( item_ptr,
                             &(hdrscmrmac3.req_params.req_ratio),
                             (p_aip->attrib_offset + attrib_offset),
                             HDRSCMRMAC3_RP_REQ_RATIO_SIZE );

    /* If error unpacking ReqRatio, break */
    if (err_no != E_SUCCESS)
    {
      ERR("Error unpacking ReqRatio field of RequestParameters",
          0,0,0);
      break;
    }

    /*----------------------------------------------------------------------
                                  MaxReqInterval
     *--------------------------------------------------------------------*/

    /* Shift the offset to start of MaxReqInterval field */
    attrib_offset += HDRSCMRMAC3_RP_REQ_RATIO_SIZE;

    /* Unpack MaxReqInterval */
    err_no = HDRBIT_UNPACK8( item_ptr,
                             &(hdrscmrmac3.req_params.max_req_interval),
                             (p_aip->attrib_offset + attrib_offset),
                             HDRSCMRMAC3_RP_MAX_REQ_INTERVAL_SIZE );

    /* If error unpacking max_req_interval, break */
    if (err_no != E_SUCCESS)
    {
      ERR("Error unpacking max_req_interval field of RequestParameters",
          0,0,0);
      break;
    }

    /* Parsing done! :-) Is the parsed attribute valid? */
    if ( (err_no =
          hdrscmrmac3_is_request_params_valid()) != E_OUT_OF_RANGE )
    {
      /* Attrib valid! Yea! :-) print F3 mesg */
      HDR_MSG_PROT_2( MSG_LEGACY_HIGH, " RequestParams: %x %x ",
                      hdrscmrmac3.req_params.req_ratio,
                      hdrscmrmac3.req_params.max_req_interval );

      /* Update the accepted attribute information */
      a_aip->is_accepted = TRUE;
      a_aip->value_id = value_id;
      a_aip->complex_val_ptr = (void*) &(hdrscmrmac3.req_params);
      a_aip->complex_val_size = sizeof(hdrscmrmac3_req_params_struct_type);
      break;
    }

    /* Not much luck :-( Let's try next set of values if there is one. 
       Shift the offset to start of next value ID. */
    attrib_offset += HDRSCMRMAC3_RP_MAX_REQ_INTERVAL_SIZE;

  } /* while */

  return err_no;

} /* hdrscmrmac3_process_request_params */


 /* EJECT */
/*===========================================================================

FUNCTION HDRSCMRMAC3_IS_RRI_CHAN_POWER_PARAMS_VALID                     LOCAL

DESCRIPTION
  Check if the following attribute is valid 

  0x0012 - RRIChannelPowerParameters Attribute

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  E_SUCCESS      - If the Attribute is valid.
  E_OUT_OF_RANGE - otherwise. 

SIDE EFFECTS
  None.

===========================================================================*/
static hdrerrno_enum_type hdrscmrmac3_is_rri_chan_power_params_valid (void)
{

  /* Any 2's complement 4-bit number is valid for all the 
     RRIChannelGainPre/PostTransitions */
  return  E_SUCCESS;

} /* hdrscmrmac3_is_rri_chan_power_params_valid */


 /* EJECT */
/*===========================================================================

FUNCTION HDRSCMRMAC3_PROCESS_RRI_CHAN_POWER_PARAMS                   LOCAL

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
  TRUE  - If a valid attribute was parsed successfully.
  FALSE - otherwise. 

SIDE EFFECTS
  None.

===========================================================================*/
static hdrerrno_enum_type hdrscmrmac3_process_rri_chan_power_params
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

  uint8 t2p_trans;
    /* T2P Transition point */

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */ 

  /* Initialize the attribute offset to start of first Value ID */
  attrib_offset = p_aip->attrib_len_size + a_aip->attrib_id_size;

  /* While there are enough bits left in the Attribute Record for one more
     set of  RRIChannelPowerParameters complex Attribute Value */
  while ( (attrib_offset + p_aip->value_id_size + 
           HDRSCMRMAC3_RRI_CHAN_POW_PARAMS_SIZE ) <= p_aip->attrib_len )
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
      ERR("Error unpacking Value ID of  Attrib",
            0,0,0);
      break;
    }

    /*----------------------------------------------------------------------
                       RRIChannelGainPre/PostTransition(0,1,2)
     *--------------------------------------------------------------------*/

    /* Shift the attribute offset to start of  RRIChannelGainPreTransition0 */ 
    attrib_offset += p_aip->value_id_size ;

    for (t2p_trans=0; (err_no == E_SUCCESS) && 
          (t2p_trans < (HDRSCMRMAC3_NUM_T2P_TRANS_POINTS-1)); ++t2p_trans)
    {
      /* Unpack RRIChannlGainPreTransition<t2p_trans> */
      err_no = HDRBIT_UNPACK8( 
        item_ptr, 
        &(hdrscmrmac3.rri_chan_power_params[t2p_trans].
          rri_chan_gain_pre_transition ), 
        (p_aip->attrib_offset + attrib_offset), 
        HDRSCMRMAC3_RRI_CHAN_GAIN_TRANS_SIZE );

      if (err_no == E_SUCCESS)
      {
        /* Shift offset to start of RRIChannelGainPostTransition<t2p_trans> */
        attrib_offset += HDRSCMRMAC3_RRI_CHAN_GAIN_TRANS_SIZE; 

        /* Unpack RRIChannelGainPreTransition<t2p_trans> */
        err_no = HDRBIT_UNPACK8( 
          item_ptr, 
          &(hdrscmrmac3.rri_chan_power_params[t2p_trans].
            rri_chan_gain_post_transition ), 
          (p_aip->attrib_offset + attrib_offset), 
          HDRSCMRMAC3_RRI_CHAN_GAIN_TRANS_SIZE );

        /* Shift offset to start of RRIChannelGainPreTransition<t2p_trans+1> */
        attrib_offset += HDRSCMRMAC3_RRI_CHAN_GAIN_TRANS_SIZE; 
      }
    }

    /* If error unpacking, break */
    if (err_no != E_SUCCESS)
    {
      ERR("Error unpacking RRIChannelGainPre/PostTransition%d",
          (t2p_trans-1),0,0);
      break;
    }

    /*----------------------------------------------------------------------
                       RRIChannelGainPreTransition3
     *--------------------------------------------------------------------*/

     /* Unpack RRIChannelGainPreTransition<t2p_trans> */
    err_no = HDRBIT_UNPACK8( 
      item_ptr, 
      &(hdrscmrmac3.rri_chan_power_params
        [HDRSCMRMAC3_NUM_T2P_TRANS_POINTS-1]. rri_chan_gain_pre_transition ),
      (p_aip->attrib_offset + attrib_offset), 
      HDRSCMRMAC3_RRI_CHAN_GAIN_TRANS_SIZE );

    /* If error unpacking, break */
    if (err_no != E_SUCCESS)
    {
      ERR("Error unpacking RRIChannelGainPreTransition3",
          0,0,0);
      break;
    }

    /* Parsing done! :-) Is the parsed attribute valid? */
    if ( (err_no =
          hdrscmrmac3_is_rri_chan_power_params_valid()) != E_OUT_OF_RANGE )
    {
      /* Attrib valid! Yea! :-) print F3 mesg */
      for (t2p_trans=0; t2p_trans < (HDRSCMRMAC3_NUM_T2P_TRANS_POINTS-1);
           ++t2p_trans)
      {
        HDR_MSG_PROT_3( MSG_LEGACY_LOW, 
                        "RRIChannelGainPre/PostTransition %x %x %x ", 
                        t2p_trans, 
                        hdrscmrmac3.rri_chan_power_params[t2p_trans].
                          rri_chan_gain_pre_transition, 
                        hdrscmrmac3.rri_chan_power_params[t2p_trans].
                          rri_chan_gain_post_transition );
      }

      HDR_MSG_PROT_1(
        MSG_LEGACY_HIGH,
        "RRIChannelGainPreTransition3 %x", 
        hdrscmrmac3.rri_chan_power_params 
          [HDRSCMRMAC3_NUM_T2P_TRANS_POINTS-1].rri_chan_gain_pre_transition);

      /* Update the accepted attribute information */
      a_aip->is_accepted = TRUE;
      a_aip->value_id = value_id;
      a_aip->complex_val_ptr = 
        (void*) &(hdrscmrmac3.rri_chan_power_params[0]);
      a_aip->complex_val_size = 
        (sizeof(hdrscmrmac3_rri_power_params_struct_type) * 
         HDRSCMRMAC3_NUM_T2P_TRANS_POINTS);
      break;
    }

    /* Shift offset to start of Reserved field  */
    attrib_offset += HDRSCMRMAC3_RRI_CHAN_GAIN_TRANS_SIZE; 

    /* Not much luck :-( Let's try next set of values if there is one. 
       Shift the offset to start of next value ID. */
    attrib_offset += HDRSCMRMAC3_RSVD_RRI_CHAN_POW_PARAMS_SIZE;

  } /* while */

  return err_no;

} /* hdrscmrmac3_process_rri_chan_power_params */


 /* EJECT */
/*===========================================================================

FUNCTION HDRSCMRMAC3_DUMP_TX_T2P_MAX                     LOCAL

DESCRIPTION
  Dump the following attribute in f3 mesg for debugging.

  0x0013 - TxT2PMax Attribute 

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
static void hdrscmrmac3_dump_tx_t2p_max (void)
{
  uint8 ps_pt = 0;
    /* Array index */

  uint8 num_ps_axis_vals = hdrscmrmac3.txt2p_max.num_ps_axis_values;
    /* for convenience */
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */   

  HDR_MSG_PROT_1( MSG_LEGACY_HIGH, "NumPilotStrengthAxisValues %x",
                  num_ps_axis_vals );

  for (ps_pt=0; ps_pt < num_ps_axis_vals; ++ps_pt)
  {

    HDR_MSG_PROT_3( 
      MSG_LEGACY_LOW,
      "Ind: %x, PSAxis: %x, TxT2PPSAxis: %x", 
      ps_pt, 
      hdrscmrmac3.txt2p_max.ps_axis[ps_pt], 
      hdrscmrmac3.txt2p_max.txt2p_max_ps_axis[ps_pt] );

  } /* for */

}


 /* EJECT */
/*===========================================================================

FUNCTION HDRSCMRMAC3_IS_TX_T2P_MAX_VALID                     LOCAL

DESCRIPTION
  Check if the following attribute is valid 

  0x0013 - TxT2PMax Attribute 

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  E_SUCCESS      - If the Attribute is valid.
  E_OUT_OF_RANGE - otherwise. 

SIDE EFFECTS
  None.

===========================================================================*/
static hdrerrno_enum_type hdrscmrmac3_is_tx_t2p_max_valid (void)
{

  boolean is_valid = FALSE;
    /* Is the attrib valid? */

  uint8 num_ps_axis_vals = hdrscmrmac3.txt2p_max.num_ps_axis_values;
  /* Convenience storage for NumPilotStrengthAxisValues */

  uint8 ps_axis;
  /* Convenience storage for PilotStrengthAxis */

  uint8 ps_pt;
    /* PilotStrength axis point */

  boolean has_dup = FALSE;
    /* Does the axis have duplicate points? */
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */   

  /*------------------------------------------------------------------------
    Any 7 bit value is valid for TxT2PmaxPilotStrengthAxis. . So checking the
    validity of NumPilotStrengthAxisValues and PilotStrengthAxis only.
   -----------------------------------------------------------------------*/

  /* Is NumPilotStrengthAxisValues field valid? */
  is_valid = (num_ps_axis_vals >= HDRSCMRMAC3_MIN_TX_T2P_NUM_PS_AXIS_VALS) &&
             (num_ps_axis_vals <= HDRSCMRMAC3_MAX_PS_POINTS);

  /* Are the PilotStrengthAxis fields valid? */
  for ( ps_pt=0; (is_valid) && (ps_pt < num_ps_axis_vals); ++ps_pt )
  {
    ps_axis = hdrscmrmac3.txt2p_max.ps_axis[ps_pt];

    is_valid = (ps_axis <= HDRSCMRMAC3_MAX_TX_T2P_PS_AXIS);
  }

  if (is_valid && (num_ps_axis_vals >= HDRSCMRMAC3_MIN_TX_T2P_NUM_PS_AXIS_VALS) &&
              (num_ps_axis_vals <= HDRSCMRMAC3_MAX_PS_POINTS))
  {
    /* The pilot strength axis has to monotonically increase. At present, 
       software allows for non-sorted order. However, it does not allow 
       duplicate values in the axis. Checking for duplicates... */
    has_dup = hdrscmrmaccmn_axis_has_dup_points( 
                hdrscmrmac3.txt2p_max.ps_axis, num_ps_axis_vals);

    is_valid = (has_dup == FALSE);
  }

  if (!is_valid)
  {
    HDR_MSG_PROT(MSG_LEGACY_ERROR, "Invalid TxT2PMax(0x0013). Rejected");
    hdrscmrmac3_dump_tx_t2p_max();
  }

  return (is_valid)? E_SUCCESS: E_OUT_OF_RANGE;

} /* hdrscmrmac3_is_tx_t2p_max_valid */


 /* EJECT */
/*===========================================================================

FUNCTION HDRSCMRMAC3_PROCESS_TX_T2P_MAX                               LOCAL

DESCRIPTION
  This is a generic interface to process the 
  Attribute.

  0x0013 - TxT2PMax Attribute

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
static hdrerrno_enum_type hdrscmrmac3_process_tx_t2p_max
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

  uint8 num_ps_axis_vals;
    /* Convenience storage for NumPilotStrengthAxisValues */

  uint8 ps_pt;
    /* PilotStrength Axis point for array index */

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */ 

  /* Initialize the attribute offset to start of first Value ID */
  attrib_offset = p_aip->attrib_len_size + a_aip->attrib_id_size;

  /* While there are enough bits left in the Attribute Record for one more
     set of TxT2PMax complex Attribute Value */
  while ( (attrib_offset + p_aip->value_id_size + 
           HDRSCMRMAC3_MIN_TX_T2P_MAX_SIZE ) <= p_aip->attrib_len )
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
      ERR("Error unpacking Value ID of  Attrib",
            0,0,0);
      break;
    }

    /*----------------------------------------------------------------------
                            NumPilotStrengthAxisValues
     *--------------------------------------------------------------------*/
    /* Shift the attribute offset to NumPilotStrengthAxisValues field */
    attrib_offset += p_aip->value_id_size ;

    /* Unpack NumPilotStrengthAxisValues */
    err_no = HDRBIT_UNPACK8( 
              item_ptr, 
              &(hdrscmrmac3.txt2p_max.num_ps_axis_values), 
              (p_aip->attrib_offset + attrib_offset), 
              HDRSCMRMAC3_TX_T2P_NUM_PS_AXIS_VALS_SIZE );

    /* If error unpacking NumPilotStrengthAxisValues, break */
    if (err_no != E_SUCCESS)
    {
      ERR("Error unpacking NumPilotStrengthAxisValues %d", 0,0,0);
      break;
    }

    /*----------------------------------------------------------------------
                PilotStrengthAxis and TxT2PmaxPilotStrengthAxis
     *--------------------------------------------------------------------*/

    num_ps_axis_vals = hdrscmrmac3.txt2p_max.num_ps_axis_values;

    /* Check for the validity of NumPilotStrengthAxisValues before proceeding */
    if (num_ps_axis_vals < HDRSCMRMAC3_MIN_TX_T2P_NUM_PS_AXIS_VALS)
    {
	  err_no = E_OUT_OF_RANGE;
      HDR_MSG_PROT_1(MSG_LEGACY_HIGH, "Invalid NumPilotStrengthAxisvalues %d",
                     num_ps_axis_vals);
      break;
    }

    /* Shift the offset to start of  PilotStrengthAxis field */
    attrib_offset += HDRSCMRMAC3_TX_T2P_NUM_PS_AXIS_VALS_SIZE;

    /* Loop around and unpack num_ps_axis_vals fields */
    for (ps_pt = 0; 
          (err_no == E_SUCCESS) && (ps_pt < num_ps_axis_vals); ++ps_pt)
    {
      /* Unpack PilotStrengthAxis  */
      err_no = HDRBIT_UNPACK8( 
                item_ptr, 
                &(hdrscmrmac3.txt2p_max.ps_axis[ps_pt]), 
                (p_aip->attrib_offset + attrib_offset), 
                HDRSCMRMAC3_TX_T2P_PS_AXIS_SIZE );

      /* If success unpacking PilotStrengthAxis field */
      if (err_no == E_SUCCESS)
       {
        /* Shift the offset to start of  TxT2PmaxPilotStrengthAxis field */
        attrib_offset += HDRSCMRMAC3_TX_T2P_PS_AXIS_SIZE;

        /* Unpack TxT2PmaxPilotStrengthAxis */
        err_no = HDRBIT_UNPACK8( 
                  item_ptr, 
                  &(hdrscmrmac3.txt2p_max.txt2p_max_ps_axis[ps_pt]), 
                  (p_aip->attrib_offset + attrib_offset), 
                  HDRSCMRMAC3_TX_T2P_MAX_PS_AXIS_SIZE );

        /* Shift the offset to start of  next PilotStrengthAxis field */
        attrib_offset += HDRSCMRMAC3_TX_T2P_MAX_PS_AXIS_SIZE;
      }

    } /* for */

    /* If error unpacking PSAxis/TXT2PPSAxis fields, break */
    if (err_no != E_SUCCESS)
    {
      ERR("Error unpacking PSAxis/TxT2PPSAxis at index %d", (ps_pt-1),0,0);
      break;
    }

    /* Parsing done! :-) Is the parsed attribute valid? */
    if ( (err_no =
          hdrscmrmac3_is_tx_t2p_max_valid()) != E_OUT_OF_RANGE )
    {
      /* Attrib valid! Yea! :-) print F3 mesg */
      hdrscmrmac3_dump_tx_t2p_max();

      /* Update the accepted attribute information */
      a_aip->is_accepted = TRUE;
      a_aip->value_id = value_id;
      a_aip->complex_val_ptr = (void*) &(hdrscmrmac3.txt2p_max);
      a_aip->complex_val_size = sizeof(hdrscmrmac3_txt2p_max_grid_struct_type);
      break;
    }

    /* Not much luck :-( Let's try next set of values if there is one. 
       Shift the offset to start of next value ID by rounding-up to 
       next octet. */
    HDRSCMRMAC3_ALIGN_TO_NEXT_OCTET(attrib_offset);

  } /* while */

  return err_no;

} /* hdrscmrmac3_process_tx_t2p_max */


 /* EJECT */
/*===========================================================================

FUNCTION HDRSCMRMAC3_IS_MAX_MAC_FLOWS_VALID                            LOCAL

DESCRIPTION
  Check if the following attribute is valid 

  0x0014 - MaxMACFlows Attribute 

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  E_SUCCESS      - If the Attribute is valid.
  E_OUT_OF_RANGE - otherwise. 

SIDE EFFECTS
  None.

===========================================================================*/
static hdrerrno_enum_type hdrscmrmac3_is_max_mac_flows_valid (void)
{
  uint8 max_num_mac_flows = 
    hdrscmrmac3.max_mac_flows.max_num_mac_flows;
    /* Convenience storage for MaxNumMACFlows field */

  uint8 max_num_active_mac_flows = 
    hdrscmrmac3.max_mac_flows.max_num_active_mac_flows;
    /* Convenience storage for MaxNumActiveMACFlows */
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */   

  return ( ( (max_num_mac_flows >= HDRSCMRMAC3_MIN_MAX_NUM_MAC_FLOWS) &&
             (max_num_mac_flows <= HDRSCMRMAC3_MAX_NUM_MAC_FLOWS) ) &&
           ( (max_num_active_mac_flows >= 
                HDRSCMRMAC3_MIN_MAX_NUM_ACTIVE_MAC_FLOWS) &&
             (max_num_active_mac_flows <= max_num_mac_flows) )
         )? E_SUCCESS: E_OUT_OF_RANGE;

} /* hdrscmrmac3_is_max_mac_flows_valid */


 /* EJECT */
/*===========================================================================

FUNCTION HDRSCMRMAC3_PROCESS_MAX_MAC_FLOWS                            LOCAL

DESCRIPTION
  This is a generic interface to process the MaxMACFlows Attribute.

  0x0014 - MaxMACFlows Attribute 

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
static hdrerrno_enum_type hdrscmrmac3_process_max_mac_flows
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

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */ 

  /* Initialize the attribute offset to start of first Value ID */
  attrib_offset = p_aip->attrib_len_size + a_aip->attrib_id_size;

  /* While there are enough bits left in the Attribute Record for one more
     set of  MaxMACFlows complex Attribute Value */
  while ( (attrib_offset + p_aip->value_id_size + 
           HDRSCMRMAC3_MAX_MAC_FLOWS_SIZE ) <= p_aip->attrib_len )
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
      ERR("Error unpacking Value ID of  Attrib",
            0,0,0);
      break;
    }

    /*----------------------------------------------------------------------
                                MaxNumMACFlows
     *--------------------------------------------------------------------*/

    /* Shift the attribute offset to start of MaxNumMACFlows field */ 
    attrib_offset += p_aip->value_id_size ;

    /* Unpack MaxNumMACFlows field */
    err_no = HDRBIT_UNPACK8( 
              item_ptr, 
              &(hdrscmrmac3.max_mac_flows.max_num_mac_flows), 
              (p_aip->attrib_offset + attrib_offset), 
              HDRSCMRMAC3_MAX_NUM_MAC_FLOWS_SIZE );

    /* If error unpacking MaxNumMACFlows, break */
    if (err_no != E_SUCCESS)
    {
      ERR("Error unpacking MaxNumMACFlows", 0,0,0);
      break;
    }

    /*----------------------------------------------------------------------
                             MaxNumActiveMACFlows
     *--------------------------------------------------------------------*/

    /* Shift the offset to start of  MaxNumActiveMACFlows field */
    attrib_offset += HDRSCMRMAC3_MAX_NUM_MAC_FLOWS_SIZE;

    /* Unpack MaxNumActiveMACFlows field */
    err_no = HDRBIT_UNPACK8( 
              item_ptr, 
              &(hdrscmrmac3.max_mac_flows.max_num_active_mac_flows), 
              (p_aip->attrib_offset + attrib_offset), 
              HDRSCMRMAC3_MAX_NUM_ACTIVE_MAC_FLOWS_SIZE );

    /* If error unpacking MaxNumMACActiveMACFlows, break */
    if (err_no != E_SUCCESS)
    {
      ERR("Error unpacking MAxNumActiveMACFlows", 0,0,0);
      break;
    }

    /* Parsing done! :-) Is the parsed attribute valid? */
    if ( (err_no =
          hdrscmrmac3_is_max_mac_flows_valid()) != E_OUT_OF_RANGE )
    {
      /* Attrib valid! Yea! :-) print F3 mesg */
      HDR_MSG_PROT_2( 
        MSG_LEGACY_HIGH, 
        "MaxMACFlows: %x %x", 
        hdrscmrmac3.max_mac_flows.max_num_mac_flows, 
        hdrscmrmac3.max_mac_flows.max_num_active_mac_flows );

      /* Update the accepted attribute information */
      a_aip->is_accepted = TRUE;
      a_aip->value_id = value_id;
      a_aip->complex_val_ptr = (void*) &(hdrscmrmac3.max_mac_flows);
      a_aip->complex_val_size = sizeof(hdrscmrmac3_max_mac_flows_struct_type);
      break;
    }

    /* Not much luck :-( Let's try next set of values if there is one. 
       Shift the offset to start of next value ID. */
    attrib_offset += HDRSCMRMAC3_MAX_NUM_ACTIVE_MAC_FLOWS_SIZE;

  } /* while */

  return err_no;

} /* hdrscmrmac3_process_max_mac_flows */


 /* EJECT */
/*===========================================================================

FUNCTION HDRSCMRMAC3_IS_ASSOCIATED_FLOWS_NN_VALID                     LOCAL

DESCRIPTION
  Check if the following attribute is valid 

  0x0100 - 0x01NN - AssociatedFlowsNN attribute 

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  E_SUCCESS      - If the Attribute is valid.
  E_OUT_OF_RANGE - otherwise. 

SIDE EFFECTS
  None.

===========================================================================*/
static hdrerrno_enum_type hdrscmrmac3_is_associated_flows_nn_valid(void)
{

  return E_SUCCESS;

} /* hdrscmrmac3_is_associated_flows_valid */


 /* EJECT */
/*===========================================================================

FUNCTION HDRSCMRMAC3_PROCESS_ASSOCIATED_FLOWS_NN                    LOCAL

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
  TRUE  - If a valid attribute was parsed successfully.
  FALSE - otherwise. 

SIDE EFFECTS
  None.

===========================================================================*/
static hdrerrno_enum_type hdrscmrmac3_process_associated_flows_nn
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

  uint8 flow;
    /* Flow index */

  uint8 flow_id;
    /* Flow ID */

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */ 

  /* Get the flow ID from the attribute ID */
  flow_id = a_aip->attrib_id & HDRSCMRMAC3_NN_FLOW_ID_MASK;

  /* Initialize the attribute offset to start of first Value ID */
  attrib_offset = p_aip->attrib_len_size + a_aip->attrib_id_size;

  /* While there are enough bits left in the Attribute Record for one more
     set of  AssociatedFlowsNN complex Attribute Value */
  while ( (attrib_offset + p_aip->value_id_size + 
           HDRSCMRMAC3_MIN_ASSOCIATED_FLOWS_NN_SIZE ) <= p_aip->attrib_len )
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
      ERR("Error unpacking Value ID of AssociatedFlowsNN Attrib", 0,0,0);
      break;
    }

    /*----------------------------------------------------------------------
                                  FlowCount
     *--------------------------------------------------------------------*/

    /* Shift the attribute offset to start of  FlowCount field */ 
    attrib_offset += p_aip->value_id_size ;

    /* Unpack FlowCount */
    err_no = HDRBIT_UNPACK8( item_ptr,
                             &(hdrscmrmac3.associated_flows[flow_id].flow_count),
                             (p_aip->attrib_offset + attrib_offset),
                             HDRSCMRMAC3_AF_FLOW_COUNT_SIZE );

    /* If error unpacking FlowCount, break */
    if (err_no != E_SUCCESS)
    {
      ERR("Error unpacking FlowCount of AssociatedFlowNN attrib", 0,0,0);
      break;
    }

    /*----------------------------------------------------------------------
                              Stream and SubStream
     *--------------------------------------------------------------------*/

    if (hdrscmrmac3.associated_flows[flow_id].flow_count > 
          HDRSCMRMAC3_MAX_APP_FLOWS)
    {
	  err_no = E_OUT_OF_RANGE;
      HDR_MSG_PROT_2(MSG_LEGACY_ERROR,"FlowCount Invalid %d, Max allowed: %d",
                     hdrscmrmac3.associated_flows[flow_id].flow_count,
                     HDRSCMRMAC3_MAX_APP_FLOWS);
      break;
    }

    /* Shift the offset to start of  Stream field */
    attrib_offset += HDRSCMRMAC3_AF_FLOW_COUNT_SIZE;

    /* Unpack stream and sub-stream for each flow */
    for (flow = 0; (err_no == E_SUCCESS) 
          && (flow < hdrscmrmac3.associated_flows[flow_id].flow_count); ++flow)
    {
      /* Unpack StreamID */
      err_no = HDRBIT_UNPACK16( 
        item_ptr, 
        &(hdrscmrmac3.associated_flows[flow_id].app_flows[flow].stream_id), 
        (p_aip->attrib_offset + attrib_offset), 
        HDRSCMRMAC3_AF_STREAM_SIZE );

      if (err_no == E_SUCCESS)
      {
        /* Shift the offset to start of SubStream field */
        attrib_offset += HDRSCMRMAC3_AF_STREAM_SIZE;

        /* Unpack SubStreamID */
        err_no = HDRBIT_UNPACK8( 
          item_ptr, 
          &(hdrscmrmac3.associated_flows[flow_id].app_flows[flow].sub_stream_id), 
          (p_aip->attrib_offset + attrib_offset), 
          HDRSCMRMAC3_AF_SUB_STREAM_SIZE );

        /* Shift the offset to start of Stream field of next flow */
        attrib_offset += HDRSCMRMAC3_AF_SUB_STREAM_SIZE;
      }

    } /* For each flow */

    if (err_no != E_SUCCESS)
    {
      ERR("Error unpacking Stream/SubStream for flow %d", (flow-1),0,0);
      break;
    }

    /* Parsing done! :-) Is the parsed attribute valid? */
    if ( (err_no =
          hdrscmrmac3_is_associated_flows_nn_valid()) != E_OUT_OF_RANGE )
    {
      /* Attrib valid! Yea! :-) print F3 mesg */
      HDR_MSG_PROT_1( MSG_LEGACY_HIGH, "AF FlowCount %x",
                      hdrscmrmac3.associated_flows[flow_id].flow_count );

      for (flow=0; flow<hdrscmrmac3.associated_flows[flow_id].flow_count; ++flow)
      {
        HDR_MSG_PROT_3( 
          MSG_LEGACY_HIGH, 
          "AF FlowId: %x Stream: %x SubStream: %x", 
          flow, 
          hdrscmrmac3.associated_flows[flow_id].app_flows[flow].stream_id, 
          hdrscmrmac3.associated_flows[flow_id].app_flows[flow].sub_stream_id );
      }

      /* Update the accepted attribute information */
      a_aip->is_accepted = TRUE;
      a_aip->value_id = value_id;
      a_aip->complex_val_ptr = (void*) &(hdrscmrmac3.associated_flows[flow_id]);
      a_aip->complex_val_size = sizeof(hdrscmrmac3_associated_flows_struct_type);
      break;
    }

    /* Not much luck :-( Let's try next set of values if there is one. 
       Shift the offset to start of next value ID by aligning it to 
       the next octet boundary. */
    HDRSCMRMAC3_ALIGN_TO_NEXT_OCTET(attrib_offset);

  } /* while */

  return err_no;

} /* hdrscmrmac3_process_associated_flows_nn */



 /* EJECT */
/*===========================================================================

FUNCTION HDRSCMRMAC3_DUMP_BUCKET_FACTOR_NN                     LOCAL

DESCRIPTION
  Dump the following attribute in f3 mesg for debugging.

  0x0300 - 0x03NN - BucketFactorNN Attribute 

DEPENDENCIES
  None.

PARAMETERS
  flow_id - MAC Flow ID of the per-flow attrib specified. 

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
static void hdrscmrmac3_dump_bucket_factor_nn 
(
  uint8 flow_id
)
{
  uint8 t2p_ind = 0;
    /* T2P axis index */

  uint8 frab_ind = 0;
    /* FRAB axis index */

  uint8 num_t2p_axis_vals = 
    hdrscmrmac3.bucket_factor[flow_id].t2p_axis.num_points;
    /* Number of T2P axis values */

  uint8 num_frab_axis_vals = 
    hdrscmrmac3.bucket_factor[flow_id].frab_axis.num_points;
    /* Number of T2P axis values */
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */ 

  HDR_MSG_PROT_3(MSG_LEGACY_HIGH, 
                 "BF(0x03NN %d): NumT2PAxisValues %x NumFRABAxisValues %x",
                 flow_id,
                 num_t2p_axis_vals,
                 num_frab_axis_vals);

  /* Print T2P Axis */
  for (t2p_ind=0; t2p_ind <= num_t2p_axis_vals; ++t2p_ind)
  {
    HDR_MSG_PROT_2(
      MSG_LEGACY_HIGH,
      "BF: T2PAxis[%x] = %x", 
      t2p_ind, 
      hdrscmrmac3.bucket_factor[flow_id].t2p_axis.values[t2p_ind]);
  }

  /* Print FRAB Axis */
  for (frab_ind=0; frab_ind <= num_frab_axis_vals; ++frab_ind)
  {
    HDR_MSG_PROT_2(
      MSG_LEGACY_LOW,
      "BF: FRABAxis[%x] = %x", 
      frab_ind, 
      hdrscmrmac3.bucket_factor[flow_id].frab_axis.values[frab_ind]);
  }

  /* Print BFT2PAxisFRABAxis */
  for (t2p_ind=0; t2p_ind <= num_t2p_axis_vals; ++t2p_ind)
  {
    for (frab_ind=0; frab_ind <= num_frab_axis_vals; ++frab_ind)
    {
      HDR_MSG_PROT_3(
        MSG_LEGACY_LOW,
        "BF: T2PAxisFRABAxis[%x][%x] = %x", 
        t2p_ind,
        frab_ind,
        hdrscmrmac3.bucket_factor[flow_id].grid[t2p_ind][frab_ind]);
    }
  }

} /* hdrscmrmac3_dump_bucket_factor_nn */

 /* EJECT */
/*===========================================================================

FUNCTION HDRSCMRMAC3_IS_BUCKET_FACTOR_NN_VALID                     LOCAL

DESCRIPTION
  Check if the following attribute is valid 

  0x0300 - 0x03NN - BucketFactorNN Attribute 

DEPENDENCIES
  None.

PARAMETERS
  flow_id - ID of MAC flow for which the bucket factor is specified.

RETURN VALUE
  E_SUCCESS      - If the Attribute is valid.
  E_OUT_OF_RANGE - otherwise. 

SIDE EFFECTS
  None.

===========================================================================*/
static hdrerrno_enum_type hdrscmrmac3_is_bucket_factor_nn_valid 
(
  uint8 flow_id
)
{
  hdrerrno_enum_type err_no = E_SUCCESS;
    /* Error code */

  hdrscmrmac3_t2p_frab_grid_struct_type *bf = 
    &hdrscmrmac3.bucket_factor[flow_id];
    /* Convenience pointer */
  uint8  t2_axis_num_points = 0;
  uint8  frab_axis_num_points = 0;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */ 

  /* The T2P axis has to monotonically increase. At present, software 
     allows for non-sorted order. However, it does not allow duplicate 
     values in the axis. Checking for duplicates... 
     The num_points is one less than actual number of points. Incremented
     to adjust. */

  t2_axis_num_points = bf->t2p_axis.num_points+1;
  frab_axis_num_points = bf->frab_axis.num_points+1;
  if(t2_axis_num_points > HDRSCMRMAC3_MAX_T2P_POINTS)
  {
      HDR_MSG_PROT_2 (MSG_LEGACY_ERROR, 
                   "t2_axis_num_points %d greater than max allowed. Limiting to max %d.",
                     t2_axis_num_points,HDRSCMRMAC3_MAX_T2P_POINTS );
      t2_axis_num_points = HDRSCMRMAC3_MAX_T2P_POINTS;
  }
  
  if(frab_axis_num_points > HDRSCMRMAC3_MAX_FRAB_POINTS)
  {
      HDR_MSG_PROT_2 (MSG_LEGACY_ERROR, 
                   "frab_axis_num_point %d greater than max allowed. Limiting to max %d.",
                     frab_axis_num_points, HDRSCMRMAC3_MAX_FRAB_POINTS);
      frab_axis_num_points = HDRSCMRMAC3_MAX_FRAB_POINTS;
  }
  if (hdrscmrmaccmn_axis_has_dup_points( bf->t2p_axis.values, 
                                       t2_axis_num_points) == TRUE)
  {
    err_no = E_OUT_OF_RANGE;
    HDR_MSG_PROT_1(MSG_LEGACY_ERROR, 
                   "Duplicate T2P axis points in BF(0x03NN %d). Rejected.",
                   flow_id);
    hdrscmrmac3_dump_bucket_factor_nn(flow_id);
  } 
  /* The FRAB axis has to monotonically increase. At present, software
     allows for non-sorted order. However, it does not allow duplicate
     values in the axis. Checking for duplicates... 
     The num_points is one less than actual number of points. Incremented
     to adjust. */
  else if (hdrscmrmaccmn_axis_has_dup_points( 
              bf->frab_axis.values, frab_axis_num_points) == TRUE)
  {
    err_no = E_OUT_OF_RANGE;
    HDR_MSG_PROT_1(MSG_LEGACY_ERROR, 
                   "Duplicate FRAB axis points in BF(0x03NN %d). Rejected.",
                   flow_id);
    hdrscmrmac3_dump_bucket_factor_nn(flow_id);
  }

  return err_no;

} /* hdrscmrmac3_is_bucket_factor_nn_valid */


 /* EJECT */
/*===========================================================================

FUNCTION HDRSCMRMAC3_PROCESS_BUCKET_FACTOR_NN                         LOCAL

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
  TRUE  - If a valid attribute was parsed successfully.
  FALSE - otherwise. 

SIDE EFFECTS
  None.

===========================================================================*/
static hdrerrno_enum_type hdrscmrmac3_process_bucket_factor_nn
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

  uint8 num_t2p_axis_vals;
    /* NumT2PAxisVals */

  uint8 num_frab_axis_vals;
    /* NumFRABAxisVals */

  uint8 t2p_ind;
    /* Array index on T2PAxis */

  uint8 frab_ind;
    /* Array index on FRABAxis */

  uint8 flow_id;
    /* Flow ID */

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */ 
  
  /* Get the flow ID from the attribute ID */
  flow_id = a_aip->attrib_id & HDRSCMRMAC3_NN_FLOW_ID_MASK;

  /* Initialize the attribute offset to start of first Value ID */
  attrib_offset = p_aip->attrib_len_size + a_aip->attrib_id_size;

  /* While there are enough bits left in the Attribute Record for one more
     set of  BucketFactorNN complex Attribute Value */
  while ( (attrib_offset + p_aip->value_id_size + 
           HDRSCMRMAC3_MIN_BUCKET_FACTOR_NN_SIZE) <= p_aip->attrib_len )
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
      ERR("Error unpacking Value ID of BucketFactorNN Attrib",
            0,0,0);
      break;
    }

    /*----------------------------------------------------------------------
                                NumT2PAxisValues 
     *--------------------------------------------------------------------*/

    /* Shift the attribute offset to start of  NumT2PAxisValues field */ 
    attrib_offset += p_aip->value_id_size;

    /* Unpack NumT2PAxisValues */
    err_no = HDRBIT_UNPACK8( 
              item_ptr, 
              &(hdrscmrmac3.bucket_factor[flow_id].t2p_axis.num_points), 
              (p_aip->attrib_offset + attrib_offset), 
              HDRSCMRMAC3_BF_NUM_T2P_AXIS_VALS_SIZE );

    /* If error unpacking NumT2PAxisValues, break */
    if (err_no != E_SUCCESS)
    {
      ERR("Error unpacking NumT2PAxisValues of BucketFactorNN", 0,0,0);
      break;
    }

    num_t2p_axis_vals = hdrscmrmac3.bucket_factor[flow_id].t2p_axis.num_points;

    /* NumT2PAxisValues is number of occurences of T2PAxis field minus 1. 
       Check if it exceeds the assumed max? */
    if ((num_t2p_axis_vals+1) > HDRSCMRMAC3_MAX_T2P_POINTS)
    {
	  err_no = E_OUT_OF_RANGE;
      HDR_MSG_PROT_2(MSG_LEGACY_ERROR,
                     "Invalid NumT2PAxisPoints %d MaxAllowed %d",
                     num_t2p_axis_vals,
                     HDRSCMRMAC3_MAX_T2P_POINTS);
      break;
    }

    /*----------------------------------------------------------------------
                                NumFRABAxisValues 
     *--------------------------------------------------------------------*/

    /* Shift the offset to start of  NumFRABAxisValues field */
    attrib_offset += HDRSCMRMAC3_BF_NUM_T2P_AXIS_VALS_SIZE;

    /* Unpack NumT2PAxisValues */
    err_no = HDRBIT_UNPACK8( 
              item_ptr, 
              &(hdrscmrmac3.bucket_factor[flow_id].frab_axis.num_points), 
              (p_aip->attrib_offset + attrib_offset), 
              HDRSCMRMAC3_BF_NUM_FRAB_AXIS_VALS_SIZE );

    /* If error unpacking NumFRABAxisValues, break */
    if (err_no != E_SUCCESS)
    {
      ERR("Error unpacking NumFRABAxisValues of BucketFactorNN", 0,0,0);
      break;
    }

    num_frab_axis_vals = hdrscmrmac3.bucket_factor[flow_id].frab_axis.num_points;

    /* NumFRABAxisValues is number of occurences of FRABAxis field minus 1. 
       Check if it exceeds the assumed max? */
    if ((num_frab_axis_vals+1) > HDRSCMRMAC3_MAX_FRAB_POINTS)
    {
	  err_no = E_OUT_OF_RANGE;
      HDR_MSG_PROT_2(MSG_LEGACY_ERROR,
                     "Invalid NumFRABAxisPoints %d MaxAllowed %d",
                     num_frab_axis_vals,
                     HDRSCMRMAC3_MAX_FRAB_POINTS);
      break;
    }

    /*----------------------------------------------------------------------
                                      T2PAxis
     *--------------------------------------------------------------------*/

    /* Shift the offset to start of  T2PAxis field */
    attrib_offset += HDRSCMRMAC3_BF_NUM_FRAB_AXIS_VALS_SIZE;

    for (t2p_ind=0; (err_no == E_SUCCESS) && (t2p_ind <= num_t2p_axis_vals);
          ++t2p_ind)
    {
      /* Unpack T2PAxis */
      err_no = HDRBIT_UNPACK8( 
                item_ptr, 
                &(hdrscmrmac3.bucket_factor[flow_id].t2p_axis.values[t2p_ind]), 
                (p_aip->attrib_offset + attrib_offset), 
                HDRSCMRMAC3_BF_T2P_AXIS_SIZE );

      /* Shift offset to start of next T2PAxis / first FRABAxis field */
      attrib_offset += HDRSCMRMAC3_BF_T2P_AXIS_SIZE;
    }

    /* If error unpacking T2PAxis, break */
    if (err_no != E_SUCCESS)
    {
      ERR("Error unpacking T2PAxis @ %d of BucketFactorNN", (t2p_ind-1),0,0);
      break;
    }

    /*----------------------------------------------------------------------
                                      FRABAxis
     *--------------------------------------------------------------------*/

    for (frab_ind=0; (err_no == E_SUCCESS) && (frab_ind <= num_frab_axis_vals);
          ++frab_ind)
    {
      /* Unpack FRABAxis */
      err_no = HDRBIT_UNPACK8( 
                item_ptr, 
                &(hdrscmrmac3.bucket_factor[flow_id].frab_axis.values[frab_ind]), 
                (p_aip->attrib_offset + attrib_offset), 
                HDRSCMRMAC3_BF_FRAB_AXIS_SIZE );

      /* Shift offset to start of next FRABAxis / first BFT2PFRABAxis field */
      attrib_offset += HDRSCMRMAC3_BF_FRAB_AXIS_SIZE;
    }

    /* If error unpacking T2PAxis, break */
    if (err_no != E_SUCCESS)
    {
      ERR("Error unpacking FRABAxis @ %d of BucketFactorNN", (frab_ind-1),0,0);
      break;
    }

    /*----------------------------------------------------------------------
                             BucketFactorT2PAxisFRABAxis
     *--------------------------------------------------------------------*/

    /* For each point in T2PAxis */
    for (t2p_ind=0; (err_no == E_SUCCESS) && 
          (t2p_ind <= num_t2p_axis_vals); ++t2p_ind)
    {
      /* For each point in FRABAxis */
      for (frab_ind=0; (err_no == E_SUCCESS) && 
            (frab_ind <= num_frab_axis_vals); ++frab_ind)
      {
        /* Unpack BucketFactorT2PAxisFRABAxis */
        err_no = HDRBIT_UNPACK8( 
                  item_ptr, 
                  &(hdrscmrmac3.bucket_factor[flow_id].grid[t2p_ind][frab_ind]), 
                  (p_aip->attrib_offset + attrib_offset), 
                  HDRSCMRMAC3_BF_T2P_AXIS_FRAB_AXIS_SIZE );

        /* Shift offset to start of next BFT2PFRABAxis field */
        attrib_offset += HDRSCMRMAC3_BF_T2P_AXIS_FRAB_AXIS_SIZE;
      }
    }

    /* If error unpacking BFT2PFRABAxis, break */
    if (err_no != E_SUCCESS)
    {
      ERR("Error unpacking BFT2PFRABAxis(%d,%d) of BucketFactorNN", 
          (t2p_ind-1),(frab_ind-1),0);
      break;
    }

    /* Parsing done! :-) Is the parsed attribute valid? */
    if ( (err_no =
          hdrscmrmac3_is_bucket_factor_nn_valid(flow_id)) != E_OUT_OF_RANGE )
    {
      /* Attrib valid! Yea! :-) print F3 mesg */
      hdrscmrmac3_dump_bucket_factor_nn(flow_id);
      
      /* Update the accepted attribute information */
      a_aip->is_accepted = TRUE;
      a_aip->value_id = value_id;
      a_aip->complex_val_ptr = (void*) &(hdrscmrmac3.bucket_factor[flow_id]);
      a_aip->complex_val_size = sizeof(hdrscmrmac3_t2p_frab_grid_struct_type);
      break;
    }

    /* Not much luck :-( Let's try next set of values if there is one. 
       Shift the offset to start of next value ID by aligning the offset
       to next octet boundary. */
    HDRSCMRMAC3_ALIGN_TO_NEXT_OCTET(attrib_offset);

  } /* while */

  return err_no;

} /* hdrscmrmac3_process_bucket_factor_nn */


 /* EJECT */
/*===========================================================================

FUNCTION HDRSCMRMAC3_IS_T2P_INFLOW_RANGE_VALID                     LOCAL

DESCRIPTION
  Check if the following attribute is valid 

  0x1500 ~ 0x15NN - T2PInflowrangeNN Attribute.

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  E_SUCCESS      - If the Attribute is valid.
  E_OUT_OF_RANGE - otherwise. 

SIDE EFFECTS
  None.

===========================================================================*/
static hdrerrno_enum_type hdrscmrmac3_is_t2p_inflow_range_valid (void)
{

  /* All 8-bit values are valid for T2PInflowMin and T2PInflowMax fields */
  return E_SUCCESS;

} /* hdrscmrmac3_is_t2p_inflow_range_valid */


 /* EJECT */
/*===========================================================================

FUNCTION HDRSCMRMAC3_PROCESS_T2P_INFLOW_RANGE_NN                    LOCAL

DESCRIPTION
  This is a generic interface to process the T2PInflowRangeNN Attribute.

  0x1500 - 0x15NN - T2PInflowRangeNN Attributes

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
static hdrerrno_enum_type hdrscmrmac3_process_t2p_inflow_range_nn
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
  flow_id = a_aip->attrib_id & HDRSCMRMAC3_NN_FLOW_ID_MASK;

  /* Initialize the attribute offset to start of first Value ID */
  attrib_offset = p_aip->attrib_len_size + a_aip->attrib_id_size;

  /* While there are enough bits left in the Attribute Record for one more
     set of  T2PInflowRange complex Attribute Value */
  while ( (attrib_offset + p_aip->value_id_size + 
           HDRSCMRMAC3_T2P_INFLOW_RANGE_SIZE ) <= p_aip->attrib_len )
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
      ERR("Error unpacking Value ID of  T2PInflowRange Attrib", 0,0,0);
      break;
    }

    /*----------------------------------------------------------------------
                                  T2PInflowMin
     *--------------------------------------------------------------------*/

    /* Shift the attribute offset to start of  T2PInflowMin field */ 
    attrib_offset += p_aip->value_id_size ;

    /* Unpack T2PInflowMin */
    err_no = HDRBIT_UNPACK8( item_ptr,
                             &(hdrscmrmac3.t2p_inflow_range[flow_id].min ),
                             (p_aip->attrib_offset + attrib_offset),
                             HDRSCMRMAC3_T2P_INFLOW_MIN_SIZE );


    /* If error unpacking T2PInflowMin, break */
    if (err_no != E_SUCCESS)
    {
      ERR("Error unpacking T2PInflowMin", 0,0,0);
      break;
    }

    /*----------------------------------------------------------------------
                                  T2PInflowMax
     *--------------------------------------------------------------------*/

    /* Shift the offset to start of T2PInflowMax field */
    attrib_offset += HDRSCMRMAC3_T2P_INFLOW_MIN_SIZE;

    /* Unpack T2PInflowMax */
    err_no = HDRBIT_UNPACK8( item_ptr,
                             &(hdrscmrmac3.t2p_inflow_range[flow_id].max ),
                             (p_aip->attrib_offset + attrib_offset),
                             HDRSCMRMAC3_T2P_INFLOW_MAX_SIZE );


    /* If error unpacking T2PInflowMax, break */
    if (err_no != E_SUCCESS)
    {
      ERR("Error unpacking T2PInflowMax", 0,0,0);
      break;
    }

    /* Parsing done! :-) Is the parsed attribute valid? */
    if ( (err_no =
          hdrscmrmac3_is_t2p_inflow_range_valid()) != E_OUT_OF_RANGE )
    {
      /* Attrib valid! Yea! :-) print F3 mesg */
      HDR_MSG_PROT_2( MSG_LEGACY_HIGH, "T2PInflowRange: Min: %x Max: %x",
                      hdrscmrmac3.t2p_inflow_range[flow_id].min,
                      hdrscmrmac3.t2p_inflow_range[flow_id].max );

      /* Update the accepted attribute information */
      a_aip->is_accepted = TRUE;
      a_aip->value_id = value_id;
      a_aip->complex_val_ptr = (void*) &(hdrscmrmac3.t2p_inflow_range[flow_id]);
      a_aip->complex_val_size = sizeof(hdrscmrmac3_t2p_inflow_range_struct_type);
      break;
    }

    /* Not much luck :-( Let's try next set of values if there is one. 
       Shift the offset to start of next value ID. */
    attrib_offset += HDRSCMRMAC3_T2P_INFLOW_MAX_SIZE;

  } /* while */

  return err_no;

} /* hdrscmrmac3_process_t2p_inflow_range_nn */

 /* EJECT */
/*===========================================================================

FUNCTION HDRSCMRMAC3_IS_T2P_TRANS_FUNC_NN_VALID                     LOCAL

DESCRIPTION
  Dump the following attribute in f3 mesg for debugging.

  0x1600 ~ 0x16NN - T2PTransitionFunctionNN attribute

DEPENDENCIES
  None.

PARAMETERS
  flow_id - MAC Flow ID of the per-flow attrib specified. 

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
static void hdrscmrmac3_dump_t2p_trans_func_nn 
(
  uint8 flow_id
)
{
  uint8 t2p_ind = 0;
    /* T2P axis index */

  uint8 frab_ind = 0;
    /* FRAB axis index */

  uint8 num_t2p_axis_vals = hdrscmrmac3.gu_gd[flow_id].t2p_axis.num_points;
    /* Number of T2P axis points */

  uint8 num_frab_axis_vals = hdrscmrmac3.gu_gd[flow_id].frab_axis.num_points;
    /* Number of FRAB axis points */

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */ 

  HDR_MSG_PROT_3(MSG_LEGACY_HIGH, 
                 "GUGD(0x16NN %d): NumT2PAxisValues %x NumFRABAxisValues %x",
                 flow_id,
                 num_t2p_axis_vals,
                 num_frab_axis_vals);

  /* Print T2P Axis */
  for (t2p_ind=0; t2p_ind <= num_t2p_axis_vals; ++t2p_ind)
  {
    HDR_MSG_PROT_2(
      MSG_LEGACY_LOW,
      "GUGD: T2PAxis[%x] = %x", 
      t2p_ind, 
      hdrscmrmac3.gu_gd[flow_id].t2p_axis.values[t2p_ind]);
  }

  /* Print FRAB Axis */
  for (frab_ind=0; frab_ind <= num_frab_axis_vals; ++frab_ind)
  {
    HDR_MSG_PROT_2(
      MSG_LEGACY_LOW,
      "GUGD: FRABAxis[%x] = %x", 
      frab_ind, 
      hdrscmrmac3.gu_gd[flow_id].frab_axis.values[frab_ind]);
  }

  /* Print GU/GD */
  for (t2p_ind=0; t2p_ind <= num_t2p_axis_vals; ++t2p_ind)
  {
    for (frab_ind=0; frab_ind <= num_frab_axis_vals; ++frab_ind)
    {
      HDR_MSG_PROT_4(
        MSG_LEGACY_LOW,
        "GUGD: For T2PAxisFRABAxis[%x][%x] Up = %x; Down = %x", 
        t2p_ind,
        frab_ind,
        hdrscmrmac3.gu_gd[flow_id].up[t2p_ind][frab_ind],
        hdrscmrmac3.gu_gd[flow_id].down[t2p_ind][frab_ind]);
    }
  }

} /* hdrscmrmac3_dump_t2p_trans_func_nn */


 /* EJECT */
/*===========================================================================

FUNCTION HDRSCMRMAC3_IS_T2P_TRANS_FUNC_NN_VALID                     LOCAL

DESCRIPTION
  Check if the following attribute is valid 

  0x1600 ~ 0x16NN - T2PTransitionFunctionNN attribute

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
static hdrerrno_enum_type hdrscmrmac3_is_t2p_trans_func_nn_valid 
(
  uint8 flow_id
)
{

  hdrerrno_enum_type err_no = E_SUCCESS;
    /* Error code */
  uint8  t2_axis_num_points = 0;
  uint8  frab_axis_num_points = 0;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */ 

  /* The T2P axis has to monotonically increase. At present, software 
     allows for non-sorted order. However, it does not allow duplicate 
     values in the axis. Checking for duplicates... 
     The num_points is one less than actual number of points. Incremented
     to adjust. */

  t2_axis_num_points = hdrscmrmac3.gu_gd[flow_id].t2p_axis.num_points+1;
  frab_axis_num_points = hdrscmrmac3.gu_gd[flow_id].frab_axis.num_points+1;
  if(t2_axis_num_points > HDRSCMRMAC3_MAX_T2P_POINTS)
  {
      HDR_MSG_PROT_2 (MSG_LEGACY_ERROR, 
                   "t2_axis_num_points %d greater than max allowed. Limiting to max %d.",
                     t2_axis_num_points, HDRSCMRMAC3_MAX_T2P_POINTS);
      t2_axis_num_points = HDRSCMRMAC3_MAX_T2P_POINTS;
  }
  
  if(frab_axis_num_points > HDRSCMRMAC3_MAX_FRAB_POINTS)
  {
      HDR_MSG_PROT_2 (MSG_LEGACY_ERROR, 
                   "frab_axis_num_point %d greater than max allowed. Limiting to max %d.",
                     frab_axis_num_points, HDRSCMRMAC3_MAX_FRAB_POINTS);
      frab_axis_num_points = HDRSCMRMAC3_MAX_FRAB_POINTS;
  }
  
  if (hdrscmrmaccmn_axis_has_dup_points( 
        hdrscmrmac3.gu_gd[flow_id].t2p_axis.values, 
        t2_axis_num_points) == TRUE)
  {
    err_no = E_OUT_OF_RANGE;
    HDR_MSG_PROT_1(MSG_LEGACY_ERROR, 
                   "Duplicate T2P axis points in GuGd(0x16NN %d). Rejected.",
                   flow_id);
    hdrscmrmac3_dump_t2p_trans_func_nn(flow_id);
  }
  /* The FRAB axis has to monotonically increase. At present, software
     allows for non-sorted order. However, it does not allow duplicate
     values in the axis. Checking for duplicates... 
     The num_points is one less than actual number of points. Incremented
     to adjust. */
  else if (hdrscmrmaccmn_axis_has_dup_points( 
            hdrscmrmac3.gu_gd[flow_id].frab_axis.values, 
            frab_axis_num_points) == TRUE)
  {
    err_no = E_OUT_OF_RANGE;
    HDR_MSG_PROT_1(MSG_LEGACY_ERROR, 
                   "Duplicate FRAB axis points in GuGd(0x16NN %d). Rejected.",
                   flow_id);
    hdrscmrmac3_dump_t2p_trans_func_nn(flow_id);
  }

  return err_no;

} /* hdrscmrmac3_is_t2p_trans_func_nn_valid */


 /* EJECT */
/*===========================================================================

FUNCTION HDRSCMRMAC3_PROCESS_T2P_TRANS_FUNC_NN                    LOCAL

DESCRIPTION
  This is a generic interface to process the T2PTransitionFunctionNN
  Attribute.

  0x1600 - 0x16NN - T2PTransitionFunctionNN Attributes

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
static hdrerrno_enum_type hdrscmrmac3_process_t2p_trans_func_nn
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

  uint8 num_t2p_axis_vals;
    /* NumT2PAxisVals */

  uint8 num_frab_axis_vals;
    /* NumFRABAxisVals */

  uint8 t2p_ind;
    /* Array index on T2PAxis */

  uint8 frab_ind;
    /* Array index on FRABAxis */

  uint8 flow_id;
    /* Flow ID */

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */ 

  /* Get the flow ID from the attribute ID */
  flow_id = a_aip->attrib_id & HDRSCMRMAC3_NN_FLOW_ID_MASK;

  /* Initialize the attribute offset to start of first Value ID */
  attrib_offset = p_aip->attrib_len_size + a_aip->attrib_id_size;

  /* While there are enough bits left in the Attribute Record for one more
     set of T2PTransitionFunctionNN complex Attribute Value */
  while ( (attrib_offset + p_aip->value_id_size + 
           HDRSCMRMAC3_MIN_GUGD_SIZE) <= p_aip->attrib_len )
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
      ERR("Error unpacking Value ID of T2PTransitionFunctionNN Attrib",
            0,0,0);
      break;
    }

    /*----------------------------------------------------------------------
                                NumT2PAxisValues 
     *--------------------------------------------------------------------*/

    /* Shift the attribute offset to start of  NumT2PAxisValues field */ 
    attrib_offset += p_aip->value_id_size;

    /* Unpack NumT2PAxisValues */
    err_no = HDRBIT_UNPACK8( 
              item_ptr, 
              &(hdrscmrmac3.gu_gd[flow_id].t2p_axis.num_points), 
              (p_aip->attrib_offset + attrib_offset), 
              HDRSCMRMAC3_GUGD_NUM_T2P_AXIS_VALS_SIZE );

    /* If error unpacking NumT2PAxisValues, break */
    if (err_no != E_SUCCESS)
    {
      ERR("Error unpacking NumT2PAxisValues of T2PTransitionFunctionNN", 0,0,0);
      break;
    }

    num_t2p_axis_vals = hdrscmrmac3.gu_gd[flow_id].t2p_axis.num_points;

    /* NumT2PAxisValues is number of occurences of T2PAxis field minus 1. 
       Check if it exceeds the assumed max? */
    if ((num_t2p_axis_vals+1) > HDRSCMRMAC3_MAX_T2P_POINTS)
    {
	  err_no = E_OUT_OF_RANGE;
      HDR_MSG_PROT_2(MSG_LEGACY_ERROR,
                     "GUGD: Invalid NumT2PAxisPoints %d MaxAllowed %d",
                     num_t2p_axis_vals,
                     HDRSCMRMAC3_MAX_T2P_POINTS);
      break;
    }

    /*----------------------------------------------------------------------
                                NumFRABAxisValues 
     *--------------------------------------------------------------------*/

    /* Shift the offset to start of  NumFRABAxisValues field */
    attrib_offset += HDRSCMRMAC3_GUGD_NUM_T2P_AXIS_VALS_SIZE;

    /* Unpack NumFRABAxisValues */
    err_no = HDRBIT_UNPACK8( 
              item_ptr, 
              &(hdrscmrmac3.gu_gd[flow_id].frab_axis.num_points), 
              (p_aip->attrib_offset + attrib_offset), 
              HDRSCMRMAC3_GUGD_NUM_FRAB_AXIS_VALS_SIZE );

    /* If error unpacking NumFRABAxisValues, break */
    if (err_no != E_SUCCESS)
    {
      ERR("Error unpacking NumFRABAxisValues of T2PTransitionFunctionNN", 
          0,0,0);
      break;
    }

    num_frab_axis_vals = hdrscmrmac3.gu_gd[flow_id].frab_axis.num_points;

    /* NumFRABAxisValues is number of occurences of FRABAxis field minus 1. 
       Check if it exceeds the assumed max? */
    if ((num_frab_axis_vals+1) > HDRSCMRMAC3_MAX_FRAB_POINTS)
    {
	  err_no = E_OUT_OF_RANGE;
      HDR_MSG_PROT_2(MSG_LEGACY_ERROR,
                     "GUGD: Invalid NumFRABAxisPoints %d MaxAllowed %d",
                     num_frab_axis_vals,
                     HDRSCMRMAC3_MAX_FRAB_POINTS);
      break;
    }

    /*----------------------------------------------------------------------
                                      T2PAxis
     *--------------------------------------------------------------------*/

    /* Shift the offset to start of  T2PAxis field */
    attrib_offset += HDRSCMRMAC3_GUGD_NUM_FRAB_AXIS_VALS_SIZE;

    for (t2p_ind=0; (err_no == E_SUCCESS) && (t2p_ind <= num_t2p_axis_vals);
          ++t2p_ind)
    {
      /* Unpack T2PAxis */
      err_no = HDRBIT_UNPACK8( 
                item_ptr, 
                &(hdrscmrmac3.gu_gd[flow_id].t2p_axis.values[t2p_ind]), 
                (p_aip->attrib_offset + attrib_offset), 
                HDRSCMRMAC3_GUGD_T2P_AXIS_SIZE );

      /* Shift offset to start of next T2PAxis / first FRABAxis field */
      attrib_offset += HDRSCMRMAC3_GUGD_T2P_AXIS_SIZE;
    }

    /* If error unpacking T2PAxis, break */
    if (err_no != E_SUCCESS)
    {
      ERR("Error unpacking T2PAxis @ %d of T2PTransitionFunctionNN", 
          (t2p_ind-1),0,0);
      break;
    }

    /*----------------------------------------------------------------------
                                      FRABAxis
     *--------------------------------------------------------------------*/

    for (frab_ind=0; (err_no == E_SUCCESS) && (frab_ind <= num_frab_axis_vals);
          ++frab_ind)
    {
      /* Unpack FRABAxis */
      err_no = HDRBIT_UNPACK8( 
                item_ptr, 
                &(hdrscmrmac3.gu_gd[flow_id].frab_axis.values[frab_ind]), 
                (p_aip->attrib_offset + attrib_offset), 
                HDRSCMRMAC3_GUGD_FRAB_AXIS_SIZE );

      /* Shift offset to start of next FRABAxis / first T2PUpT2PFRABAxis 
         field */
      attrib_offset += HDRSCMRMAC3_GUGD_FRAB_AXIS_SIZE;
    }

    /* If error unpacking FRABAxis, break */
    if (err_no != E_SUCCESS)
    {
      ERR("Error unpacking FRABAxis @ %d of T2PTransitionFunctionNN", 
          (frab_ind-1),0,0);
      break;
    }

    /*----------------------------------------------------------------------
                              T2PUpT2PAxisFRABAxis 
     *--------------------------------------------------------------------*/

    /* For each point in T2PAxis */
    for (t2p_ind=0; (err_no == E_SUCCESS) && 
          (t2p_ind <= num_t2p_axis_vals); ++t2p_ind)
    {
      /* For each point in FRABAxis */
      for (frab_ind=0; (err_no == E_SUCCESS) && 
            (frab_ind <= num_frab_axis_vals); ++frab_ind)
      {
        /* Unpack  T2PUpT2PAxisFRABAxis */
        err_no = HDRBIT_UNPACK8( 
                  item_ptr, 
                  &(hdrscmrmac3.gu_gd[flow_id].up[t2p_ind][frab_ind]),
                  (p_aip->attrib_offset + attrib_offset), 
                  HDRSCMRMAC3_GUGD_T2P_UP_T2P_FRAB_AXIS_SIZE );

        if (err_no == E_SUCCESS)
        {
          /* Shift offset to start of next T2PDownT2PAxisFRABAxis field */
          attrib_offset += HDRSCMRMAC3_GUGD_T2P_UP_T2P_FRAB_AXIS_SIZE;

          /* Unpack  T2PDownT2PAxisFRABAxis */
          err_no = HDRBIT_UNPACK8( 
                    item_ptr, 
                    &(hdrscmrmac3.gu_gd[flow_id].down[t2p_ind][frab_ind]),
                    (p_aip->attrib_offset + attrib_offset), 
                    HDRSCMRMAC3_GUGD_T2P_DOWN_T2P_FRAB_AXIS_SIZE );

          /* Shift offset to start of next T2PUpT2PAxisFRABAxis field */
          attrib_offset += HDRSCMRMAC3_GUGD_T2P_DOWN_T2P_FRAB_AXIS_SIZE;
        }
      }
    }

    /* If error unpacking T2PUp/DownT2PAxisFRABAxis, break */
    if (err_no != E_SUCCESS)
    {
      ERR("Error unpacking T2PUp/DownT2PAxisFRABAxis(%d,%d)", 
          (t2p_ind-1),(frab_ind-1),0);
      break;
    }

    /* Parsing done! :-) Is the parsed attribute valid? */
    if ( (err_no =
          hdrscmrmac3_is_t2p_trans_func_nn_valid(flow_id)) != E_OUT_OF_RANGE )
    {
      /* Attrib valid! Yea! :-) print F3 mesg */
      hdrscmrmac3_dump_t2p_trans_func_nn(flow_id);
      
      /* Update the accepted attribute information */
      a_aip->is_accepted = TRUE;
      a_aip->value_id = value_id;
      a_aip->complex_val_ptr = (void*) &(hdrscmrmac3.gu_gd[flow_id]);
      a_aip->complex_val_size = sizeof(hdrscmrmac3_gu_gd_struct_type);
      break;
    }

    /* Not much luck :-( Let's try next set of values if there is one. 
       Shift the offset to start of next value ID by aligning the offset
       to next octet boundary. */
    HDRSCMRMAC3_ALIGN_TO_NEXT_OCTET(attrib_offset);

  } /* while */

  return err_no;

} /* hdrscmrmac3_process_t2p_trans_func_nn */

 /* EJECT */
/*===========================================================================

FUNCTION HDRSCMRMAC3_GET_INDEX_FROM_ATTRIB_ID                         LOCAL

DESCRIPTION
  This function is used to map the attribute ID of a Non-Per-Flow Simple 
  attribute to an index.
  
DEPENDENCIES
  The function assumes that the index is a Non-Per-Flow Simple attribute

PARAMETERS
  attrib_id - Attribute ID of the simple attribute.
  pindex    - Address where the computed index is stored.

RETURN VALUE
  E_SUCCESS       - If the attrib_value is valid.
  E_DATA_INVALID  - If the attribute specified by attrib_id is not valid.

SIDE EFFECTS
  None.
  
===========================================================================*/
static hdrerrno_enum_type hdrscmrmac3_get_index_from_attrib_id
(
  uint16 attrib_id,
  uint16 *pindex
)
{
  hdrerrno_enum_type err_no = E_SUCCESS;
    /* Error Code */

  uint16 index;
    /* Simple attribute Index */

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */ 

  /* Compute the index of non-per-flow simple attribute by masking off all but 
     the least significant Nibble */
  index = (attrib_id & HDRSCMRMAC3_SIMPLE_ATTRIB_INDEX_MASK);

  /* Sanity Check for invalid attrib IDs */
  if ((index == HDRSCMRMAC3_SIMPLE_ATTRIB_FIRST_HOLE) || 
      (index == HDRSCMRMAC3_SIMPLE_ATTRIB_SECOND_HOLE) ||
      (index > HDRSCMRMAC3_SIMPLE_ATTRIB_MAX_INDEX) )
  {
    err_no = E_DATA_INVALID;
  }
  else
  {
    /* Index valid! There are holes in index got from attrib_id. Account
       for holes by shifting the origin by appropriate offset for various
       ranges */
    index -= ((index < HDRSCMRMAC3_SIMPLE_ATTRIB_FIRST_HOLE)? 
                HDRSCMRMAC3_SIMPLE_ATTRIB_INDEX_ORIGIN: 
                (index < HDRSCMRMAC3_SIMPLE_ATTRIB_SECOND_HOLE)? 
                (HDRSCMRMAC3_SIMPLE_ATTRIB_INDEX_ORIGIN+1): 
                (HDRSCMRMAC3_SIMPLE_ATTRIB_INDEX_ORIGIN+2));

    /* Store the computed index in the outparam */
    *pindex = index;
  }

  return err_no;

} /* hdrscmrmac3_get_index_from_attrib_id */


 /* EJECT */
/*===========================================================================

FUNCTION HDRSCMRMAC3_GET_SIMPLE_ATTRIB_OFFSET_AND_SIZE

DESCRIPTION
  This function computes the byte offset of the simple attribute specified by 
  the attrib_id in the struct hdrscmrmac3_config_struct_type and stores it in 
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
hdrerrno_enum_type hdrscmrmac3_get_simple_attrib_offset_and_size
(
  uint16 attrib_id, 
  uint16 *attrib_offset,
  uint16 *attrib_size
)
{
  hdrerrno_enum_type err_no = E_SUCCESS;
    /* Error Code */

  uint16 index = 0;
    /* Simple attribute Index */

  uint8 flow_id = 0;
    /* MAC flow index */

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */ 

  /* Check if the simple attribute is a non-per-flow simple attribute? */
  if ((attrib_id & HDRSCMRMAC3_NN_ATTRIB_3RD_NIBBLE_MASK) ==
        HDRSCMRMAC3_NON_NN_SIMPLE_ATTRIB_MASKED )
  {

    err_no = hdrscmrmac3_get_index_from_attrib_id(attrib_id, &index);

  } /* Non-Per-Flow */

  /* Per-Flow Simple Attribute */
  else
  {
    /* Account for twelve non-per-flow simple attributes defined ahead */
    index += HDRSCMRMAC3_TOTAL_NUM_SIMPLE_ATTRIBS;

    flow_id = attrib_id & HDRSCMRMAC3_NN_FLOW_ID_MASK;

    /* There are <flow_id> per-flow attribute structs defined ahead of 
       the current attribute. */
    index += (flow_id * sizeof(hdrscmrmac3_flow_config_struct_type));

    /* The third Nibble of NN simple attributes are consecutive starting from
       9 thru E. This is used as an index by masking of all but the third 
       Nibble and shift right by 8 bits. Since the index starts with zero, 
       the origin is shifted by subtracting 9 from it. */
    index += (((attrib_id & HDRSCMRMAC3_NN_SIMPLE_ATTRIB_INDEX_MASK) >> 
                HDRSCMRMAC3_NN_SIMPLE_ATTRIB_INDEX_LSB) - 
                HDRSCMRMAC3_NN_SIMPLE_ATTRIB_INDEX_ORIGIN);

  } /* Per-Flow */

  if (err_no == E_SUCCESS)
  {
    /* Each simple attribute is of size uint8 */
    *attrib_offset = (index * sizeof(uint8));

    *attrib_size = sizeof(uint8);
  }

  return err_no;

} /* hdrscmrmac3_get_simple_attrib_offset_and_size */


 /* EJECT */
/*===========================================================================

FUNCTION HDRSCMRMAC3_GET_COMPLEX_ATTRIB_OFFSET_AND_SIZE               LOCAL

DESCRIPTION
  This function computes the byte offset of the complex attribute specified by
  the attrib_id in the struct hdrscmrmac3_config_struct_type and stores it in 
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
static hdrerrno_enum_type hdrscmrmac3_get_complex_attrib_offset_and_size
(
  uint16 attrib_id, 
  uint16 *attrib_offset,
  uint16 *attrib_size
)
{
  hdrerrno_enum_type err_no = E_SUCCESS;
    /* Error code */

  uint8 flow_id;
    /* Flow id NN got from 0xXXNN per-flow attribute ID */

  uint8 power_param_ind;
    /* PowerParameters Attribute Index */

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */ 

  /* Does the attrib_id belongs to one of the twelve PowerParameters Attrib? */
  if ((attrib_id >= (uint16) HDRSCMRMAC3_FIRST_POWER_PARAMS_ATTRIB) &&
      (attrib_id <= (uint16) HDRSCMRMAC3_LAST_POWER_PARAMS_ATTRIB))
  {
    /* Mask all but the Least significant Nibble of attribute ID to get the 
       index into the PowerParameters array */
    power_param_ind = (attrib_id & HDRSCMRMAC3_POWER_PARAMS_INDEX_MASK);

    /* Shift the Origin to 1 */
    power_param_ind -= (HDRSCMRMAC3_POWER_PARAMS_INDEX_ORIGIN-1);

    *attrib_offset = 
      FPOS(hdrscmrmac3_config_struct_type, power_params[power_param_ind]);

    *attrib_size = ( sizeof(hdrscmrmac3_power_params_struct_type) * 
                     (uint16) HDRSCMRMAC3_NUM_TX_MODES );
    
  } /* PowerParameters attribute */

  /* Is it a per-flow complex attrib? */
  else if ((attrib_id & HDRSCMRMAC3_NN_ATTRIB_3RD_NIBBLE_MASK) == 
            HDRSCMRMAC3_NON_NN_COMPLEX_ATTRIB_MASKED)
  {
    switch (attrib_id)
    {
      /* 0x0000 - AuxillaryPilotChannelParameters */
      case HDRSCMRMAC3_AUX_PILOT_CHAN_PARAMS_ATTRIB:

        *attrib_offset = FPOS(hdrscmrmac3_config_struct_type, aux_pilot_info);
        *attrib_size = sizeof(hdrscmrmac3_aux_pilot_struct_type);
        break;

      /* 0x0001 - CommonPowerParameters */
      case HDRSCMRMAC3_COMMON_PARAMS_ATTRIB:
        *attrib_offset = FPOS(hdrscmrmac3_config_struct_type, common_pow_params);
        *attrib_size = sizeof(hdrscmrmac3_common_pow_params_struct_type);
        break;

      /* 0x0002 - MaxNumSubPackets */
      case HDRSCMRMAC3_MAX_NUM_SUB_PACKETS_ATTRIB:
        *attrib_offset = FPOS(hdrscmrmac3_config_struct_type, max_num_sub_pkts);
        *attrib_size = sizeof(hdrscmrmac3_max_num_sub_pkts_struct_type);
        break;

      /* 0x0003 - PermittedPayload */
      case HDRSCMRMAC3_PERMITTED_PAYLOAD_ATTRIB:
        *attrib_offset = FPOS(hdrscmrmac3_config_struct_type, permitted_payload);
        *attrib_size = sizeof(hdrscmrmac3_permitted_payload_type);
        break;

      /* 0x0004 - PilotStrength */
      case HDRSCMRMAC3_PILOT_STRENGTH_ATTRIB:
        *attrib_offset = FPOS(hdrscmrmac3_config_struct_type, pilot_strength);
        *attrib_size = sizeof(hdrscmrmac3_pilot_strength_grid_struct_type);
        break;

      /* 0x0011 - RequestParameters */
      case HDRSCMRMAC3_REQUEST_PARAMS_ATTRIB:
        *attrib_offset = FPOS(hdrscmrmac3_config_struct_type, req_params);
        *attrib_size = sizeof(hdrscmrmac3_req_params_struct_type);
        break;

      /* 0x0012 - RRIChannelPowerParameters */
      case HDRSCMRMAC3_RRI_CHAN_POWER_PARAMS_ATTRIB:
        *attrib_offset = 
          FPOS(hdrscmrmac3_config_struct_type, rri_chan_power_params);
        *attrib_size = (sizeof(hdrscmrmac3_rri_power_params_struct_type) * 
                        HDRSCMRMAC3_NUM_T2P_TRANS_POINTS);
        break;

      /* 0x0013 - TxT2PMax */
      case HDRSCMRMAC3_TX_T2P_MAX_ATTRIB:
        *attrib_offset = FPOS(hdrscmrmac3_config_struct_type, txt2p_max);
        *attrib_size = sizeof(hdrscmrmac3_txt2p_max_grid_struct_type);
        break;

      /* 0x0014 - MaxMACFlows */
      case HDRSCMRMAC3_MAX_MAC_FLOWS_ATTRIB:
        *attrib_offset = FPOS(hdrscmrmac3_config_struct_type, max_mac_flows);
        *attrib_size = sizeof(hdrscmrmac3_max_mac_flows_struct_type);
        break;

      /* Unrecognized Non-NN complex attribute ID */
      default:
        HDR_MSG_PROT_1(MSG_LEGACY_ERROR,
                       "Unrecognized Non-NN complex attrib ID: %x",
                       attrib_id);  
        err_no = E_DATA_INVALID;

    } /* switch */

  } /* Non-NN complex attribute */

  /* Yes. It is complex per-flow NN attribute */
  else
  {
    /* Get the flow ID 'NN' from the attrib ID 0xXXNN */
    flow_id = (attrib_id & HDRSCMRMAC3_NN_FLOW_ID_MASK);

    if (flow_id < HDRSCMRMAC3_MAX_NUM_MAC_FLOWS)
    {
      switch (attrib_id & HDRSCMRMAC3_NN_BASE_MASK)
      {
        /* 0x0100~0x01NN AssociatedFlowsNN Attribute */
        case HDRSCMRMAC3_ASSOCIATED_FLOWS_NN_BASE:
          *attrib_offset = 
            FPOS(hdrscmrmac3_config_struct_type, flows[flow_id].flow_mapping); 
          *attrib_size = sizeof(hdrscmrmac3_associated_flows_struct_type);
          break;

        /* 0x0300~0x03NN - BucketFactorNN Attribute */
        case HDRSCMRMAC3_BUCKET_FACTOR_NN_BASE:
          *attrib_offset = 
            FPOS(hdrscmrmac3_config_struct_type, flows[flow_id].bucket_factor);
          *attrib_size = sizeof(hdrscmrmac3_t2p_frab_grid_struct_type);
          break;

        /* 0x01500~0x15NN - T2PInflowRangeNN Attribute */
        case HDRSCMRMAC3_T2P_INFLOW_RANGE_NN_BASE:
          *attrib_offset = 
            FPOS(hdrscmrmac3_config_struct_type, flows[flow_id].t2p_inflow_range);
          *attrib_size = sizeof(hdrscmrmac3_t2p_inflow_range_struct_type);
          break;

        /* 0x1600~0x16NN - T2PTranistionFunctionNN Attribute */
        case HDRSCMRMAC3_T2P_TRANSITION_FUNC_NN_BASE:
          *attrib_offset =
            FPOS(hdrscmrmac3_config_struct_type, flows[flow_id].gu_gd);
          *attrib_size = sizeof(hdrscmrmac3_gu_gd_struct_type);
          break;

        /* Unrecognized Per-flow complex attribute ID */
        default:
          HDR_MSG_PROT_1(MSG_LEGACY_ERROR,
                         "Unrecognized NN complex attrib ID: %x",
                         attrib_id);  
          err_no = E_DATA_INVALID;

      } /* switch */

    }
    else
    {
      /* NN greater than what AT supports */
      HDR_MSG_PROT_2(MSG_LEGACY_ERROR, 
                     "Attrib Id 0x%x for MAC flow %d not supported", 
                     attrib_id,
                     flow_id);

      err_no = E_DATA_INVALID;
    }

  } /* NN-Complex attribute */

  return err_no;

} /* hdrscmrmac3_get_complex_attrib_offset_and_size */


 /* EJECT */
/*============================================================================

                        EXTERNAL FUNCTION DEFINITIONS

=============================================================================*/

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
  E_SUCCESS      - If a valid complex attribute was parsed successfully.
  E_NO_DATA      - If the config request message does not have enough data.
  E_OUT_OF_RANGE - If the config request message does not have any valid data.
  E_DATA_INVALID - If the given attrib_id is not valid.

SIDE EFFECTS
  None.

===========================================================================*/
hdrerrno_enum_type hdrscmrmac3_process_complex_attrib
(
  dsm_item_type                               *item_ptr,
  hdrscm_proposed_attrib_type   const *const   p_aip,
  hdrscm_accepted_attrib_type         *const   a_aip
)
{
  hdrerrno_enum_type err_no = E_SUCCESS;
    /* Error code */

  uint8 m_flow_id = 0;
    /* MAC flow ID */

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */ 

  /*---------------------------------------------------------------------------
    Each one of the complex attribute has one handler routine registered in 
    hdrscmrmac3_complex_attrib_handler registration table. The 
    hdrscmrmac3_complex_attrib_id_enum_type lists all the complex attributes 
    arranged in the order of their attribute IDs which is used to index into 
    the function pointer table. However the enum does not translate directly
    into a linear index in the table since all of the twelve power parameters 
    attributes has a single handler registered in the table. So the index in 
    the registration table needs to be shifted for attributes occuring after
    the power parameters attribute in the table.
   --------------------------------------------------------------------------*/

  /* If the attrib_id occurs before the first power params attrib in the enum, 
     just use the attrib_id as index */
  if (a_aip->attrib_id < (uint16) HDRSCMRMAC3_FIRST_POWER_PARAMS_ATTRIB )
  {
    err_no = hdrscmrmac3_complex_attrib_handler[a_aip->attrib_id]
      (item_ptr, p_aip, a_aip);
  }
  /* If the attrib is one of the twelve powerparams attrib, use the first power
     params attrib_id as the index */
  else if (a_aip->attrib_id >= (uint16) HDRSCMRMAC3_FIRST_POWER_PARAMS_ATTRIB && 
           a_aip->attrib_id <= (uint16) HDRSCMRMAC3_LAST_POWER_PARAMS_ATTRIB )
  {
    err_no = hdrscmrmac3_complex_attrib_handler
              [HDRSCMRMAC3_FIRST_POWER_PARAMS_ATTRIB] (item_ptr, p_aip, a_aip);
  }
  /* If the attrib_id occurs after the last power params attrib, shift the 
     index by subtracting the number of powerparams attrib from attrib_id */
  else if ((a_aip->attrib_id > (uint16) HDRSCMRMAC3_LAST_POWER_PARAMS_ATTRIB) && 
           (a_aip->attrib_id <= (uint16) HDRSCMRMAC3_LAST_COMPLEX_ATTRIB))
  {
    err_no = hdrscmrmac3_complex_attrib_handler
              [a_aip->attrib_id-(HDRSCMRMAC3_NUM_POWER_PARAMS_ATTRIBS-1)]
                (item_ptr, p_aip, a_aip);
  }
  else
  {
    m_flow_id = (a_aip->attrib_id & HDRSCMRMAC3_NN_FLOW_ID_MASK);

    if ( m_flow_id  < HDRSCMRMAC3_MAX_NUM_MAC_FLOWS )
    {
      /* If we reach here... the attribute must either be a per-flow NN 
         complex attribute or an invalid attribute ID. Mask off the LSB to 
         find out the class of NN flow Attribute */
      switch (a_aip->attrib_id & HDRSCMRMAC3_NN_BASE_MASK)
      {
        /* BucketFactorNN Attribute */
        case HDRSCMRMAC3_BUCKET_FACTOR_NN_BASE:
          err_no = hdrscmrmac3_process_bucket_factor_nn(item_ptr, p_aip , a_aip);
          break;

        /* T2PInflowRangeNN Attribute */
        case HDRSCMRMAC3_T2P_INFLOW_RANGE_NN_BASE:
          err_no = hdrscmrmac3_process_t2p_inflow_range_nn(item_ptr, p_aip, a_aip);
          break;

        /* T2PTransitionFunctionNN Attribute */
        case HDRSCMRMAC3_T2P_TRANSITION_FUNC_NN_BASE:
          err_no = hdrscmrmac3_process_t2p_trans_func_nn(item_ptr, p_aip, a_aip);
          break;

        /* AssociatedFlowsNN Attribute */
        case HDRSCMRMAC3_ASSOCIATED_FLOWS_NN_BASE:
          err_no = hdrscmrmac3_process_associated_flows_nn(item_ptr, p_aip, a_aip);
          break;

        default:
          HDR_MSG_PROT_1(MSG_LEGACY_HIGH,
                         "Invalid Attribute ID: %x", 
                         a_aip->attrib_id);

          /* Given Attrib_id is invalid */
          err_no = E_DATA_INVALID;
      } /* switch */

    }
    else
    {
      /* NN greater than what AT supports */
      HDR_MSG_PROT_2(MSG_LEGACY_ERROR, 
                     "Attrib Id 0x%x for MAC flow %d not supported", 
                     a_aip->attrib_id,
                     m_flow_id);

      err_no = E_DATA_INVALID;
    }

  } /* else */

  return err_no;

} /* hdrscmrmac3_process_complex_attrib */


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
boolean hdrscmrmac3_is_attrib_simple
(
  uint16 const attrib_id
)
{

  return ((attrib_id & HDRSCMRMAC3_SIMPLE_ATTRIB_MSB_MASK) == 
            HDRSCMRMAC3_SIMPLE_ATTRIB_MSB )? TRUE : FALSE;

} /* hdrscmrmac3_is_attrib_simple */


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
  E_OUT_OF_RANGE     - If the attrib_value is not valid.
  E_DATA_INVALID     - If the attribute specified by attrib_id is not simple.

SIDE EFFECTS
  None.
  
===========================================================================*/
hdrerrno_enum_type hdrscmrmac3_is_simple_attrib_valid
(
  uint16 const attrib_id,
  uint32 const attrib_value 
)
{
  hdrerrno_enum_type err_no = E_OUT_OF_RANGE;
    /* Error Code */

  uint16 index;
    /* Index into the valid_range table */

  uint8 m_flow_id = 0;
    /* MAC flow ID */

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */ 

  /* Check if the attribute is simple */
  if (hdrscmrmac3_is_attrib_simple(attrib_id) == FALSE)
  {
    /* Not simple */
    err_no = E_DATA_INVALID;
  }
  /* Attribute is simple */
  else
  {
    /* Check if the simple attribute is a non-per-flow simple attribute? */
    if ((attrib_id & HDRSCMRMAC3_NN_ATTRIB_3RD_NIBBLE_MASK) ==
          HDRSCMRMAC3_NON_NN_SIMPLE_ATTRIB_MASKED )
    {

      /* Get the simple attribute index from the attribute ID */
      if ( (err_no = hdrscmrmac3_get_index_from_attrib_id(attrib_id, &index))
            == E_SUCCESS )
      {
        /* Check the validity of the simple attribute */
        err_no = ( (attrib_value >= hdrscmrmac3_valid_range[index].min) &&
                   (attrib_value <= hdrscmrmac3_valid_range[index].max) )?
                   E_SUCCESS: E_OUT_OF_RANGE;
      }

    } /* Non-Per-Flow */

    /* Per-Flow Simple Attribute */
    else
    {
      /* Get the MAC flow ID */
      m_flow_id = (attrib_id & HDRSCMRMAC3_NN_FLOW_ID_MASK);

      /* Check if the NN is within valid range */
      if (m_flow_id < HDRSCMRMAC3_MAX_NUM_MAC_FLOWS)
      {
        /* Compute the index by masking off the attrib_id and shifting */
        index = ( (attrib_id & HDRSCMRMAC3_NN_SIMPLE_ATTRIB_INDEX_MASK) >>
                    HDRSCMRMAC3_NN_SIMPLE_ATTRIB_INDEX_LSB);

        /* Shift the origin to zero */
        index -= HDRSCMRMAC3_NN_SIMPLE_ATTRIB_INDEX_ORIGIN;

        /* The valid range for MergeThreshold Flow 0 is different from other flows
           Check that as a special case */
        if (attrib_id == (uint16) HDRSCMRMAC3_MERGE_THRESHOLD_NN_BASE)
        {
          /* Check the validity of NN simple attribute */
          err_no = ((attrib_value <= HDRSCMRMAC3_INFINITE_MERGE_THRESHOLD_00)? 
                     E_SUCCESS: E_OUT_OF_RANGE);
        }
        else if (attrib_id == (uint16) HDRSCMRMAC3_BUCKET_LEVEL_MAX_NN_BASE)
        {
          /* BucketLevelMax for Flow 0 has to be greater than zero. */
          err_no = (((attrib_value > hdrscmrmac3_nn_valid_range[index].min) &&
                     (attrib_value <= hdrscmrmac3_nn_valid_range[index].max) 
                    )? E_SUCCESS: E_OUT_OF_RANGE);
        }
        else
        {
          /* Check the validity of NN simple attribute */
          err_no = ( (attrib_value >= hdrscmrmac3_nn_valid_range[index].min) &&
                     (attrib_value <= hdrscmrmac3_nn_valid_range[index].max) )?
                      E_SUCCESS: E_OUT_OF_RANGE;
        }

      }
      else
      {
        /* NN greater than what AT supports */
        HDR_MSG_PROT_2(MSG_LEGACY_ERROR, 
                       "Attrib Id 0x%x for MAC flow %d not supported", 
                       attrib_id,
                       m_flow_id);

        err_no = E_DATA_INVALID;
      }
    } /* Per-Flow */

  } /* Attribute is simple */

  if (err_no != E_SUCCESS)
  {
    HDR_MSG_PROT_2(MSG_LEGACY_ERROR, 
                   "Simple attrib %x proposed with invalid value %d", 
                   attrib_id,
                   attrib_value);
  }

  return err_no;

} /* hdrscmrmac3_is_simple_attrib_valid */


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
hdrerrno_enum_type hdrscmrmac3_get_attrib_offset_and_size
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
  if (hdrscmrmac3_is_attrib_simple(attrib_id))
  {
    /* Get the offset and size of the simple attribute. */
    err_no = hdrscmrmac3_get_simple_attrib_offset_and_size(attrib_id,
                                                           attrib_offset,
                                                           attrib_size);
  }
  else
  {
    /* Get the offset and size of the complex attribute */
    err_no = hdrscmrmac3_get_complex_attrib_offset_and_size(attrib_id,
                                                            attrib_offset,
                                                            attrib_size);
  }

  return err_no;

} /* hdrscmrmac3_get_attrib_offset_and_size */


/* EJECT */
/*===========================================================================

FUNCTION HDRSCMRMAC3_GET_ATTRIB_VERNO_INDEX

DESCRIPTION
  Returns the index of the given attribute in the version number array.

DEPENDENCIES
  None

PARAMETERS
  attrib_id     - Attribute Id.
  pindex        - Address where the index is stored.
  
RETURN VALUE
  E_SUCCESS - If the given attribute ID is valid.
  

SIDE EFFECTS
  None

===========================================================================*/
hdrerrno_enum_type hdrscmrmac3_get_attrib_verno_index
(
  uint16 attrib_id,
  uint16 *pindex
)
{
  hdrerrno_enum_type err_no = E_SUCCESS;
    /* Error Code */

  uint16 index = 0;
    /* Version number array index */

  uint8 flow_id;
    /* Flow ID */

  boolean is_simple = FALSE;
    /* Is attribute simple */

  uint8 num_nn_attribs_ahead = 0;
    /* Num of per-flow attributes present in front of the current NN attrib */

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */ 

  /* Check if attribute is simple? */
  is_simple = hdrscmrmac3_is_attrib_simple(attrib_id);

  /* Check if the attribute is per-flow NN attribute? */
  if ( ((attrib_id & HDRSCMRMAC3_NN_ATTRIB_3RD_NIBBLE_MASK) == 
         HDRSCMRMAC3_NON_NN_SIMPLE_ATTRIB_MASKED) ||
       ((attrib_id & HDRSCMRMAC3_NN_ATTRIB_3RD_NIBBLE_MASK) == 
          HDRSCMRMAC3_NON_NN_COMPLEX_ATTRIB_MASKED) )
  {
    /* No. Attribute is Not Per-Flow!. Is it simple though? */
    if (is_simple)
    {
      /* Non-Per-Flow simple attribute. Get the index... */
      err_no = hdrscmrmac3_get_index_from_attrib_id(attrib_id, &index);
    }
    /* Complex Non-per-flow attribute */
    else
    {
      /* Complex Non-per-flow's attribute ID translated directly into an 
         index in an array of non-per-flow complex attributes.  However account
         for twelve non-per-flow simple attributes defined ahead */
      index = attrib_id + HDRSCMRMAC3_TOTAL_NUM_SIMPLE_ATTRIBS;
    }
  }
  /* Per-Flow attributes */
  else
  {

    /*-------------------------------------------------------------------------
      The per-flow attribute version numbers are arranged as shown in the fig.
      below when the number of MAC flows is five (for example).

        Attrib1      Attrib2     Attrib3           AttribN
      --------------------------------------------------------
      | + + + + + | + + + + + | + + + + + | .... | + + + + + |
      --------------------------------------------------------
      |                       |     |
      Step1                   |     Step3 for Attrib3, flow 3
                              Step2 for Attrib3

      Each per-flow attribute has the version numbers for all the flows grouped
      together as a block followed by next attribute. The version number index
      of a per-flow attribute is found in three steps.
      1. Offset into start of per-flow attribs blocks by adding non-per flow
         attribute count.
      2. Offset into start of current attribute flow group's version number 
         block by adding num_nn_attribs_ahead block sizes.
      3. Offset into start of current attribute's version number index by 
         adding flow id to result of 2.
     ------------------------------------------------------------------------*/

    /* STEP 1: Account for twelve non-per-flow simple attributes and 21 
       non-per-flow complex attributes defined ahead */
    index += (HDRSCMRMAC3_TOTAL_NUM_SIMPLE_ATTRIBS +
              HDRSCMRMAC3_TOTAL_NUM_COMPLEX_ATTRIBS);

    /* Get the Flow ID from the attribute ID */
    flow_id = (attrib_id & HDRSCMRMAC3_NN_FLOW_ID_MASK);

    /* Is the attribute simple? */
    if (is_simple)
    {
      /* STEP2: Compute num_nn_attribs_ahead of current attribute by masking 
         off all but the second Most significant Nibble of the attrib_id and 
         shifting it 8 bits. Shift the origin to zero by subtracting the 
         current origin. */
      num_nn_attribs_ahead = 
        (((attrib_id & HDRSCMRMAC3_NN_SIMPLE_ATTRIB_INDEX_MASK) >>
            HDRSCMRMAC3_NN_SIMPLE_ATTRIB_INDEX_LSB) - 
          HDRSCMRMAC3_NN_SIMPLE_ATTRIB_INDEX_ORIGIN);

      /* There are num_nn_attribs_ahead blocks of NN version numbers before
         the current attribute and each block size is given by 
         HDRSCMRMAC3_MAX_NUM_MAC_FLOWS */
      index += (HDRSCMRMAC3_MAX_NUM_MAC_FLOWS * num_nn_attribs_ahead);

    }
    /* Complex NN Attribute */
    else
    {
      /* STEP 1: Simple NN attributes' version number blocks are placed ahead
         of complex attributes */
      index += (HDRSCMRMAC3_TOTAL_NUM_NN_SIMPLE_ATTRIBS * 
                HDRSCMRMAC3_MAX_NUM_MAC_FLOWS);

      /* STEP 2: Switch on the NN Complex Attribute Base and offset to 
         start of current attribute's version number block */
      switch (attrib_id & HDRSCMRMAC3_NN_BASE_MASK)
      {
        /* BucketfactorNN follows right after... */
        case HDRSCMRMAC3_BUCKET_FACTOR_NN_BASE:
          break;

        /* T2PTransitionFunctionNN Attribute */
        case HDRSCMRMAC3_T2P_TRANSITION_FUNC_NN_BASE:
          index += HDRSCMRMAC3_MAX_NUM_MAC_FLOWS;
          break;

        /* T2PInflowRangeNN Attributes */
        case HDRSCMRMAC3_T2P_INFLOW_RANGE_NN_BASE:
          index += (2 * HDRSCMRMAC3_MAX_NUM_MAC_FLOWS);
          break;

        /* AssociatedFlowNN Attribute */
        case HDRSCMRMAC3_ASSOCIATED_FLOWS_NN_BASE:
          index += (3 * HDRSCMRMAC3_MAX_NUM_MAC_FLOWS);
          break;

        /* Unrecognized Per-flow complex attribute ID */
        default:
          HDR_MSG_PROT_1(MSG_LEGACY_ERROR,
                         "Unrecognized NN complex attrib ID: %x",
                         attrib_id);
          err_no = E_DATA_INVALID;

      } /* switch */

    } /* Complex NN Attribute */

    /* STEP 3: Now the index points to the start of version number block for
       current attribute. Shift it by flow_id to point to the version number 
       of current flow */
    index += flow_id;

  } /* NN Attributes */

  /* Return the index */
  *pindex = index;

  return err_no;

} /* hdrscmrmac3_get_attrib_verno_index */


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
void hdrscmrmac3_set_config_to_default
(
  hdrscmrmac3_config_struct_type *p_config
)
{

  hdrscmrmaccmn_payload_size_enum_type pl_ind;
    /* Payload index */

  uint8 sf_hist;
    /* Subframe history index */

  uint8 flow;
    /* Flow index */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/*---------------------------------------------------------------------------
  Setting Non-per-flow simple configuration attributes to its default values. 
  The configuration attributes that has zero default values are not explicitly
  set. They are ARQMode, QRABFilterTC, Rate1M8Supported, and
  SupportGAUPMaxNumSubPackets.
----------------------------------------------------------------------------*/
  p_config->frab_filter_tc = HDRSCMRMAC3_DEFAULT_FRAB_FILTER_TC;
  p_config->frab_low = HDRSCMRMAC3_DEFAULT_FRAB_LOW;
  p_config->merge_thresh = HDRSCMRMAC3_DEFAULT_MERGE_THRESHOLD;
  p_config->payload_thresh = HDRSCMRMAC3_DEFAULT_PAYLOAD_THRESHOLD;
  p_config->ps_filter_tc = HDRSCMRMAC3_DEFAULT_PILOT_STRENGTH_FILTER_TC;
  p_config->t2p_no_tx_filter_tc = HDRSCMRMAC3_DEFAULT_T2P_NO_TX_FILTER_TC;
  p_config->ps_qrab_thresh_drc_lock = 
    HDRSCMRMAC3_DEFAULT_PS_QRAB_THRESH_DRC_LOCK;
  p_config->ps_qrab_thresh_drc_unlock = 
    HDRSCMRMAC3_DEFAULT_PS_QRAB_THRESH_DRC_UNLOCK;
  p_config->rate_1m8_supported = TRUE;


/*---------------------------------------------------------------------------
  Setting Per-flow simple configuration attributes to its default values. The 
  configuration attributes that has zero default values are not explicitly
  set. They are BucketLevelMaxNN, MergeThreshold00, TransmissionModeNN,
  QRABSelectNN, and BurstDurationFactorNN.
----------------------------------------------------------------------------*/
  p_config->flows[0].bucket_level_max = 
    HDRSCMRMAC3_DEFAULT_00_BUCKET_LEVEL_MAX;
  p_config->flows[1].bucket_level_max = 
    HDRSCMRMAC3_DEFAULT_01_BUCKET_LEVEL_MAX;

  p_config->flows[0].t2p_filt_tc = HDRSCMRMAC3_DEFAULT_NN_T2P_FILTER_TC;

  for (flow = 0x01; flow < HDRSCMRMAC3_MAX_NUM_MAC_FLOWS; ++flow)
  {
    p_config->flows[flow].merge_thresh = 
      HDRSCMRMAC3_DEFAULT_NN_MERGE_THRESHOLD;
    p_config->flows[flow].t2p_filt_tc = 
      HDRSCMRMAC3_DEFAULT_NN_T2P_FILTER_TC;
  } 

/*---------------------------------------------------------------------------
  Setting Non-Per-Flow Complex attributes to default values.
----------------------------------------------------------------------------*/
  /* Setting AuxiliaryPilotChannelParameters to default values */
  p_config->aux_pilot_info.gain_db = HDRSCMRMAC3_DEFAULT_AUX_PILOT_CHAN_GAIN;
  p_config->aux_pilot_info.min_payload = 
    HDRSCMRMAC3_DEFAULT_AUX_PILOT_CHAN_MIN_PAYLOAD;

  /* Setting CommonPowerParameters Attribute to default value. Default value of
     AllocationStagger is '0000'. So not explicitly set */
  p_config->common_pow_params.tx_t2p_min_db = HDRSCMRMAC3_DEFAULT_CPP_TX_T2P_MIN;
  p_config->common_pow_params.rpc_step_db = HDRSCMRMAC3_DEFAULT_CPP_RPC_STEP;

  /* Setting MaxNumSubPackets Attribute to default values */
  p_config->max_num_sub_pkts.interlace[0] = 
    HDRSCMRMAC3_DEFAULT_MAX_NUM_SUB_PACKETS;
  p_config->max_num_sub_pkts.interlace[1] = 
    HDRSCMRMAC3_DEFAULT_MAX_NUM_SUB_PACKETS;
  p_config->max_num_sub_pkts.interlace[2] = 
    HDRSCMRMAC3_DEFAULT_MAX_NUM_SUB_PACKETS;

  /* Setting PermittedPayload (0, 128, 256, 512, 768) to default values */
  for (pl_ind = HDRSCMRMACCMN_FIRST_PAYLOAD_SIZE; 
        pl_ind < HDRSCMRMACCMN_1024_BITS; ++pl_ind)
  {
    for (sf_hist = 0; sf_hist < HDRSCMRMAC3_MAX_NUM_SUBFRAME_HISTORY; ++sf_hist)
    {
      p_config->permitted_payload[pl_ind][sf_hist] = 
        HDRSCMRMAC3_DEFAULT_PERMITTED_PAYLOAD_0_4;
    }
  }

  /* Setting PermittedPayload (1024, 1536) Attribute to default values */
  for (pl_ind = HDRSCMRMACCMN_1024_BITS; 
        pl_ind < HDRSCMRMACCMN_2048_BITS; ++pl_ind)
  {
    for (sf_hist = 0; sf_hist < HDRSCMRMAC3_MAX_NUM_SUBFRAME_HISTORY; ++sf_hist)
    {
      p_config->permitted_payload[pl_ind][sf_hist] = 
        HDRSCMRMAC3_DEFAULT_PERMITTED_PAYLOAD_5_6;
    }
  }

  /* Setting PermittedPayload (2048, 3072) Attribute to default values */
  for (pl_ind = HDRSCMRMACCMN_2048_BITS; 
        pl_ind < HDRSCMRMACCMN_4096_BITS; ++pl_ind)
  {
    for (sf_hist = 0; sf_hist < HDRSCMRMAC3_MAX_NUM_SUBFRAME_HISTORY; ++sf_hist)
    {
      p_config->permitted_payload[pl_ind][sf_hist] = 
        HDRSCMRMAC3_DEFAULT_PERMITTED_PAYLOAD_7_8;
    }
  }

  /* Setting PermittedPayload (4096, 6144) Attribute to default values */
  for (pl_ind = HDRSCMRMACCMN_4096_BITS; 
        pl_ind < HDRSCMRMACCMN_8192_BITS; ++pl_ind)
  {
    for (sf_hist = 0; sf_hist < HDRSCMRMAC3_MAX_NUM_SUBFRAME_HISTORY; ++sf_hist)
    {
      p_config->permitted_payload[pl_ind][sf_hist] = 
        HDRSCMRMAC3_DEFAULT_PERMITTED_PAYLOAD_9_A;
    }
  }

  /* Setting PermittedPayload (8192, 12288) Attribute to default values */
  for (pl_ind = HDRSCMRMACCMN_8192_BITS; 
        pl_ind < HDRSCMRMACCMN_MAX_NUM_PAYLOADS; ++pl_ind)
  {
    for (sf_hist = 0; sf_hist < HDRSCMRMAC3_MAX_NUM_SUBFRAME_HISTORY; ++sf_hist)
    {
      p_config->permitted_payload[pl_ind][sf_hist] = 
        HDRSCMRMAC3_DEFAULT_PERMITTED_PAYLOAD_B_C;
    }
  }

  /* Setting PilotStrength attribute to default values. ps_axis[1] and ps_ps_axis
     have default values of zero. So not explicitly set! */
  p_config->pilot_strength.num_ps_axis_values = 
    HDRSCMRMAC3_DEFAULT_PS_NUM_PS_AXIS_VALS;
  p_config->pilot_strength.ps_axis[0] = HDRSCMRMAC3_DEFAULT_PS_PS_AXIS_0;


  /* Set LoLat/HiCap T2PTransition and TerminationTarget fields for 
     PowerParameters128-12288 Attributes */
  for (pl_ind = HDRSCMRMACCMN_FIRST_PAYLOAD_SIZE; 
        pl_ind < HDRSCMRMACCMN_MAX_NUM_PAYLOADS; ++pl_ind)
  {
    p_config->power_params[pl_ind][HDRSCMRMAC3_TX_MODE_LO_LAT].t2p_transition = 
      HDRSCMRMAC3_DEFAULT_PP_LL_T2P_TRANS;
    p_config->power_params
      [pl_ind][HDRSCMRMAC3_TX_MODE_LO_LAT].termination_target = 
        HDRSCMRMAC3_DEFAULT_PP_LL_TERM_TARGET;
    p_config->power_params
      [pl_ind][HDRSCMRMAC3_TX_MODE_HI_CAP].t2p_transition =
        HDRSCMRMAC3_DEFAULT_PP_HC_T2P_TRANS;
    p_config->power_params
      [pl_ind][HDRSCMRMAC3_TX_MODE_HI_CAP].termination_target =
        HDRSCMRMAC3_DEFAULT_PP_HC_TERM_TARGET;
  }

  /* Set LoLat/HiCap Pre/Post Transition fields of PowerParameters128 */
  p_config->power_params
    [HDRSCMRMACCMN_128_BITS][HDRSCMRMAC3_TX_MODE_LO_LAT].pre_transition = 
      HDRSCMRMAC3_DEFAULT_PP_LL_PRE_TRANS_128;
  p_config->power_params
    [HDRSCMRMACCMN_128_BITS][HDRSCMRMAC3_TX_MODE_LO_LAT].post_transition =
      HDRSCMRMAC3_DEFAULT_PP_LL_POST_TRANS_128;
  p_config->power_params
    [HDRSCMRMACCMN_128_BITS][HDRSCMRMAC3_TX_MODE_HI_CAP].pre_transition =
      HDRSCMRMAC3_DEFAULT_PP_HC_TRANS_128;
  p_config->power_params
    [HDRSCMRMACCMN_128_BITS][HDRSCMRMAC3_TX_MODE_HI_CAP].post_transition =
      HDRSCMRMAC3_DEFAULT_PP_HC_TRANS_128;

  /* Set LoLat/HiCap Pre/Post Transition fields of PowerParameters256 */
  p_config->power_params
    [HDRSCMRMACCMN_256_BITS][HDRSCMRMAC3_TX_MODE_LO_LAT].pre_transition = 
      HDRSCMRMAC3_DEFAULT_PP_LL_PRE_TRANS_256;
  p_config->power_params
    [HDRSCMRMACCMN_256_BITS][HDRSCMRMAC3_TX_MODE_LO_LAT].post_transition =
      HDRSCMRMAC3_DEFAULT_PP_LL_POST_TRANS_256;
  p_config->power_params
    [HDRSCMRMACCMN_256_BITS][HDRSCMRMAC3_TX_MODE_HI_CAP].pre_transition =
      HDRSCMRMAC3_DEFAULT_PP_HC_TRANS_256;
  p_config->power_params
    [HDRSCMRMACCMN_256_BITS][HDRSCMRMAC3_TX_MODE_HI_CAP].post_transition =
      HDRSCMRMAC3_DEFAULT_PP_HC_TRANS_256;

  /* Set LoLat/HiCap Pre/Post Transition fields of PowerParameters512 */
  p_config->power_params
    [HDRSCMRMACCMN_512_BITS][HDRSCMRMAC3_TX_MODE_LO_LAT].pre_transition = 
      HDRSCMRMAC3_DEFAULT_PP_LL_PRE_TRANS_512;
  p_config->power_params
    [HDRSCMRMACCMN_512_BITS][HDRSCMRMAC3_TX_MODE_LO_LAT].post_transition =
      HDRSCMRMAC3_DEFAULT_PP_LL_POST_TRANS_512;
  p_config->power_params
    [HDRSCMRMACCMN_512_BITS][HDRSCMRMAC3_TX_MODE_HI_CAP].pre_transition =
      HDRSCMRMAC3_DEFAULT_PP_HC_TRANS_512;
  p_config->power_params
    [HDRSCMRMACCMN_512_BITS][HDRSCMRMAC3_TX_MODE_HI_CAP].post_transition =
      HDRSCMRMAC3_DEFAULT_PP_HC_TRANS_512;

  /* Set LoLat/HiCap Pre/Post Transition fields of PowerParameters768 */
  p_config->power_params
    [HDRSCMRMACCMN_768_BITS][HDRSCMRMAC3_TX_MODE_LO_LAT].pre_transition = 
      HDRSCMRMAC3_DEFAULT_PP_LL_PRE_TRANS_768;
  p_config->power_params
    [HDRSCMRMACCMN_768_BITS][HDRSCMRMAC3_TX_MODE_LO_LAT].post_transition =
      HDRSCMRMAC3_DEFAULT_PP_LL_POST_TRANS_768;
  p_config->power_params
    [HDRSCMRMACCMN_768_BITS][HDRSCMRMAC3_TX_MODE_HI_CAP].pre_transition =
      HDRSCMRMAC3_DEFAULT_PP_HC_TRANS_768;
  p_config->power_params
    [HDRSCMRMACCMN_768_BITS][HDRSCMRMAC3_TX_MODE_HI_CAP].post_transition =
      HDRSCMRMAC3_DEFAULT_PP_HC_TRANS_768;

  /* Set LoLat/HiCap Pre/Post Transition fields of PowerParameters1024 */
  p_config->power_params
    [HDRSCMRMACCMN_1024_BITS][HDRSCMRMAC3_TX_MODE_LO_LAT].pre_transition = 
      HDRSCMRMAC3_DEFAULT_PP_LL_PRE_TRANS_1024;
  p_config->power_params
    [HDRSCMRMACCMN_1024_BITS][HDRSCMRMAC3_TX_MODE_LO_LAT].post_transition =
      HDRSCMRMAC3_DEFAULT_PP_LL_POST_TRANS_1024;
  p_config->power_params
    [HDRSCMRMACCMN_1024_BITS][HDRSCMRMAC3_TX_MODE_HI_CAP].pre_transition =
      HDRSCMRMAC3_DEFAULT_PP_HC_TRANS_1024;
  p_config->power_params
    [HDRSCMRMACCMN_1024_BITS][HDRSCMRMAC3_TX_MODE_HI_CAP].post_transition =
      HDRSCMRMAC3_DEFAULT_PP_HC_TRANS_1024;

  /* Set LoLat/HiCap Pre/Post Transition fields of PowerParameters1536 */
  p_config->power_params
    [HDRSCMRMACCMN_1536_BITS][HDRSCMRMAC3_TX_MODE_LO_LAT].pre_transition = 
      HDRSCMRMAC3_DEFAULT_PP_LL_PRE_TRANS_1536;
  p_config->power_params
    [HDRSCMRMACCMN_1536_BITS][HDRSCMRMAC3_TX_MODE_LO_LAT].post_transition =
      HDRSCMRMAC3_DEFAULT_PP_LL_POST_TRANS_1536;
  p_config->power_params
    [HDRSCMRMACCMN_1536_BITS][HDRSCMRMAC3_TX_MODE_HI_CAP].pre_transition =
      HDRSCMRMAC3_DEFAULT_PP_HC_TRANS_1536;
  p_config->power_params
    [HDRSCMRMACCMN_1536_BITS][HDRSCMRMAC3_TX_MODE_HI_CAP].post_transition =
      HDRSCMRMAC3_DEFAULT_PP_HC_TRANS_1536;

  /* Set LoLat/HiCap Pre/Post Transition fields of PowerParameters2048 */
  p_config->power_params
    [HDRSCMRMACCMN_2048_BITS][HDRSCMRMAC3_TX_MODE_LO_LAT].pre_transition = 
      HDRSCMRMAC3_DEFAULT_PP_LL_PRE_TRANS_2048;
  p_config->power_params
    [HDRSCMRMACCMN_2048_BITS][HDRSCMRMAC3_TX_MODE_LO_LAT].post_transition =
      HDRSCMRMAC3_DEFAULT_PP_LL_POST_TRANS_2048;
  p_config->power_params
    [HDRSCMRMACCMN_2048_BITS][HDRSCMRMAC3_TX_MODE_HI_CAP].pre_transition =
      HDRSCMRMAC3_DEFAULT_PP_HC_TRANS_2048;
  p_config->power_params
    [HDRSCMRMACCMN_2048_BITS][HDRSCMRMAC3_TX_MODE_HI_CAP].post_transition =
      HDRSCMRMAC3_DEFAULT_PP_HC_TRANS_2048;

  /* Set LoLat/HiCap Pre/Post Transition fields of PowerParameters3072 */
  p_config->power_params
    [HDRSCMRMACCMN_3072_BITS][HDRSCMRMAC3_TX_MODE_LO_LAT].pre_transition = 
      HDRSCMRMAC3_DEFAULT_PP_LL_PRE_TRANS_3072;
  p_config->power_params
    [HDRSCMRMACCMN_3072_BITS][HDRSCMRMAC3_TX_MODE_LO_LAT].post_transition =
      HDRSCMRMAC3_DEFAULT_PP_LL_POST_TRANS_3072;
  p_config->power_params
    [HDRSCMRMACCMN_3072_BITS][HDRSCMRMAC3_TX_MODE_HI_CAP].pre_transition =
      HDRSCMRMAC3_DEFAULT_PP_HC_TRANS_3072;
  p_config->power_params
    [HDRSCMRMACCMN_3072_BITS][HDRSCMRMAC3_TX_MODE_HI_CAP].post_transition =
      HDRSCMRMAC3_DEFAULT_PP_HC_TRANS_3072;

  /* Set LoLat/HiCap Pre/Post Transition fields of PowerParameters4096 */
  p_config->power_params
    [HDRSCMRMACCMN_4096_BITS][HDRSCMRMAC3_TX_MODE_LO_LAT].pre_transition = 
      HDRSCMRMAC3_DEFAULT_PP_LL_PRE_TRANS_4096;
  p_config->power_params
    [HDRSCMRMACCMN_4096_BITS][HDRSCMRMAC3_TX_MODE_LO_LAT].post_transition =
      HDRSCMRMAC3_DEFAULT_PP_LL_POST_TRANS_4096;
  p_config->power_params
    [HDRSCMRMACCMN_4096_BITS][HDRSCMRMAC3_TX_MODE_HI_CAP].pre_transition =
      HDRSCMRMAC3_DEFAULT_PP_HC_TRANS_4096;
  p_config->power_params
    [HDRSCMRMACCMN_4096_BITS][HDRSCMRMAC3_TX_MODE_HI_CAP].post_transition =
      HDRSCMRMAC3_DEFAULT_PP_HC_TRANS_4096;

  /* Set LoLat/HiCap Pre/Post Transition fields of PowerParameters6144 */
  p_config->power_params
    [HDRSCMRMACCMN_6144_BITS][HDRSCMRMAC3_TX_MODE_LO_LAT].pre_transition = 
      HDRSCMRMAC3_DEFAULT_PP_LL_PRE_TRANS_6144;
  p_config->power_params
    [HDRSCMRMACCMN_6144_BITS][HDRSCMRMAC3_TX_MODE_LO_LAT].post_transition =
    HDRSCMRMAC3_DEFAULT_PP_LL_POST_TRANS_6144;
  p_config->power_params
    [HDRSCMRMACCMN_6144_BITS][HDRSCMRMAC3_TX_MODE_HI_CAP].pre_transition =
      HDRSCMRMAC3_DEFAULT_PP_HC_TRANS_6144;
  p_config->power_params
    [HDRSCMRMACCMN_6144_BITS][HDRSCMRMAC3_TX_MODE_HI_CAP].post_transition =
      HDRSCMRMAC3_DEFAULT_PP_HC_TRANS_6144;

  /* Set LoLat/HiCap Pre/Post Transition fields of PowerParameters8192 */
  p_config->power_params
    [HDRSCMRMACCMN_8192_BITS][HDRSCMRMAC3_TX_MODE_LO_LAT].pre_transition = 
      HDRSCMRMAC3_DEFAULT_PP_LL_PRE_TRANS_8192;
  p_config->power_params
    [HDRSCMRMACCMN_8192_BITS][HDRSCMRMAC3_TX_MODE_LO_LAT].post_transition =
      HDRSCMRMAC3_DEFAULT_PP_LL_POST_TRANS_8192;
  p_config->power_params
    [HDRSCMRMACCMN_8192_BITS][HDRSCMRMAC3_TX_MODE_HI_CAP].pre_transition =
      HDRSCMRMAC3_DEFAULT_PP_HC_TRANS_8192;
  p_config->power_params
    [HDRSCMRMACCMN_8192_BITS][HDRSCMRMAC3_TX_MODE_HI_CAP].post_transition =
      HDRSCMRMAC3_DEFAULT_PP_HC_TRANS_8192;

  /* Set LoLat/HiCap Pre/Post Transition fields of PowerParameters12288 */
  p_config->power_params
    [HDRSCMRMACCMN_12288_BITS][HDRSCMRMAC3_TX_MODE_LO_LAT].pre_transition = 
      HDRSCMRMAC3_DEFAULT_PP_LL_PRE_TRANS_12288;
  p_config->power_params
    [HDRSCMRMACCMN_12288_BITS][HDRSCMRMAC3_TX_MODE_LO_LAT].post_transition =
      HDRSCMRMAC3_DEFAULT_PP_LL_POST_TRANS_12288;
  p_config->power_params
    [HDRSCMRMACCMN_12288_BITS][HDRSCMRMAC3_TX_MODE_HI_CAP].pre_transition =
      HDRSCMRMAC3_DEFAULT_PP_HC_TRANS_12288;
  p_config->power_params
    [HDRSCMRMACCMN_12288_BITS][HDRSCMRMAC3_TX_MODE_HI_CAP].post_transition =
      HDRSCMRMAC3_DEFAULT_PP_HC_TRANS_12288;

  /* RequestParameters Default values are zero. So not explicitly set */

  /* RRIChannelPowerParameters Attribute. PreTransition0 and PreTransition1 has
     zero default values and hence not explicitly set. */
  p_config->rri_chan_power_params[0].rri_chan_gain_post_transition =
    HDRSCMRMAC3_DEFAULT_RRI_CHAN_GAIN_TRANS;
  p_config->rri_chan_power_params[1].rri_chan_gain_post_transition =
    HDRSCMRMAC3_DEFAULT_RRI_CHAN_GAIN_TRANS;
  p_config->rri_chan_power_params[2].rri_chan_gain_pre_transition =
    HDRSCMRMAC3_DEFAULT_RRI_CHAN_GAIN_TRANS;
  p_config->rri_chan_power_params[2].rri_chan_gain_post_transition =
    HDRSCMRMAC3_DEFAULT_RRI_CHAN_GAIN_TRANS;
  p_config->rri_chan_power_params[3].rri_chan_gain_pre_transition =
    HDRSCMRMAC3_DEFAULT_RRI_CHAN_GAIN_TRANS;

  /* TxT2PMax Attribute */
  p_config->txt2p_max.num_ps_axis_values = 
    HDRSCMRMAC3_DEFAULT_TXT2P_NUM_PS_AXIS_VALS;
  p_config->txt2p_max.ps_axis[0] = HDRSCMRMAC3_DEFAULT_PS_AXIS_0;
  p_config->txt2p_max.ps_axis[1] = HDRSCMRMAC3_DEFAULT_PS_AXIS_1;
  p_config->txt2p_max.txt2p_max_ps_axis[0] = HDRSCMRMAC3_DEFAULT_TXT2P_PS_AXIS_0;
  p_config->txt2p_max.txt2p_max_ps_axis[1] = HDRSCMRMAC3_DEFAULT_TXT2P_PS_AXIS_1; 

  /* MaxMACFlows */
  p_config->max_mac_flows.max_num_mac_flows = HDRSCMRMAC3_DEFAULT_MAX_MAC_FLOWS;
  p_config->max_mac_flows.max_num_active_mac_flows = 
    HDRSCMRMAC3_DEFAULT_MAX_MAC_FLOWS;

/*---------------------------------------------------------------------------
  Setting Per-Flow Complex attributes to default values.
----------------------------------------------------------------------------*/

  /* BucketFactor00 */
  p_config->flows[0].bucket_factor.t2p_axis.num_points =
    HDRSCMRMAC3_DEFAULT_00_BF_NUM_T2P_AXIS_VALS;
  p_config->flows[0].bucket_factor.frab_axis.num_points = 
    HDRSCMRMAC3_DEFAULT_00_BF_NUM_FRAB_AXIS_VALS;
  p_config->flows[0].bucket_factor.t2p_axis.values[0] =
    HDRSCMRMAC3_DEFAULT_00_BF_T2P_AXIS_VALS_0;
  p_config->flows[0].bucket_factor.t2p_axis.values[1] = 
    HDRSCMRMAC3_DEFAULT_00_BF_T2P_AXIS_VALS_1;
  p_config->flows[0].bucket_factor.grid[0][0] = 
    HDRSCMRMAC3_DEFAULT_00_BUCKET_FACTOR_0_0;
  p_config->flows[0].bucket_factor.grid[1][0] = 
    HDRSCMRMAC3_DEFAULT_00_BUCKET_FACTOR_1_0;

  /* T2PInflowRange00 */
  p_config->flows[0].t2p_inflow_range.min = 
    HDRSCMRMAC3_DEFAULT_00_T2P_INFLOW_MIN;
  p_config->flows[0].t2p_inflow_range.max = 
    HDRSCMRMAC3_DEFAULT_00_T2P_INFLOW_MAX;

  /* TransitionFunction00 */
  p_config->flows[0].gu_gd.t2p_axis.num_points = 
    HDRSCMRMAC3_DEFAULT_00_GUGD_NUM_T2P_VALS;
  p_config->flows[0].gu_gd.frab_axis.num_points =
    HDRSCMRMAC3_DEFAULT_00_GUGD_NUM_FRAB_VALS;
  p_config->flows[0].gu_gd.t2p_axis.values[0] =
    HDRSCMRMAC3_DEFAULT_00_T2P_AXIS_0;
  p_config->flows[0].gu_gd.t2p_axis.values[1] =
    HDRSCMRMAC3_DEFAULT_00_T2P_AXIS_1;
  p_config->flows[0].gu_gd.frab_axis.values[0] =
    HDRSCMRMAC3_DEFAULT_00_FRAB_AXIS_0;
  p_config->flows[0].gu_gd.up[0][0] = HDRSCMRMAC3_DEFAULT_UP_0_0;
  p_config->flows[0].gu_gd.up[1][0] = HDRSCMRMAC3_DEFAULT_UP_1_0;
  p_config->flows[0].gu_gd.down[0][0] = HDRSCMRMAC3_DEFAULT_DOWN_X_Y;
  p_config->flows[0].gu_gd.down[1][0] = HDRSCMRMAC3_DEFAULT_DOWN_X_Y;

  for (flow = 1; flow < HDRSCMRMAC3_MAX_NUM_MAC_FLOWS; ++flow)
  {
    /* Set AssociatedFlows only for Flow 1 */
    if (flow == 1)
    {
      /* AssociatedFlowsNN SubStream has zero default value and hence not set 
         explicitly */
      p_config->flows[flow].flow_mapping.flow_count = 
        HDRSCMRMAC3_DEFAULT_NN_ASSO_FLOWS_FLOW_COUNT;
      /* Flow count is 1. So just set the 0th higher layer flow's stream ID */
      p_config->flows[flow].flow_mapping.app_flows[0].stream_id = 
        HDRSCMRMAC3_DEFAULT_NN_ASSO_FLOWS_STREAM;
    }

    /* BucketFactorNN */
    p_config->flows[flow].bucket_factor.t2p_axis.num_points = 
      HDRSCMRMAC3_DEFAULT_NN_BF_NUM_T2P_AXIS_VALS;
    p_config->flows[flow].bucket_factor.frab_axis.num_points =
      HDRSCMRMAC3_DEFAULT_NN_BF_NUM_FRAB_AXIS_VALS;
    p_config->flows[flow].bucket_factor.t2p_axis.values[0] =
      HDRSCMRMAC3_DEFAULT_NN_BF_T2P_AXIS_VALS_0;
    p_config->flows[flow].bucket_factor.t2p_axis.values[1] =
      HDRSCMRMAC3_DEFAULT_NN_BF_T2P_AXIS_VALS_1;
    p_config->flows[flow].bucket_factor.frab_axis.values[0] =
      HDRSCMRMAC3_DEFAULT_NN_BF_FRAB_AXIS_VALS_0;
    p_config->flows[flow].bucket_factor.frab_axis.values[1] =
      HDRSCMRMAC3_DEFAULT_NN_BF_FRAB_AXIS_VALS_1;
    p_config->flows[flow].bucket_factor.frab_axis.values[2] =
      HDRSCMRMAC3_DEFAULT_NN_BF_FRAB_AXIS_VALS_2;
    p_config->flows[flow].bucket_factor.grid[0][0] = 
      HDRSCMRMAC3_DEFAULT_NN_BUCKET_FACTOR_0_0;
    p_config->flows[flow].bucket_factor.grid[0][1] = 
      HDRSCMRMAC3_DEFAULT_NN_BUCKET_FACTOR_0_1;
    p_config->flows[flow].bucket_factor.grid[0][2] = 
      HDRSCMRMAC3_DEFAULT_NN_BUCKET_FACTOR_0_2;
    p_config->flows[flow].bucket_factor.grid[1][0] = 
      HDRSCMRMAC3_DEFAULT_NN_BUCKET_FACTOR_1_Y;
    p_config->flows[flow].bucket_factor.grid[1][1] = 
      HDRSCMRMAC3_DEFAULT_NN_BUCKET_FACTOR_1_Y;
    p_config->flows[flow].bucket_factor.grid[1][2] = 
      HDRSCMRMAC3_DEFAULT_NN_BUCKET_FACTOR_1_Y;

    /* T2PInflowRangeNN */
    p_config->flows[flow].t2p_inflow_range.min =
      HDRSCMRMAC3_DEFAULT_NN_T2P_INFLOW_MIN;
    p_config->flows[flow].t2p_inflow_range.max =
      HDRSCMRMAC3_DEFAULT_NN_T2P_INFLOW_MAX;

    /* T2PTransitionFunctionNN */
    p_config->flows[flow].gu_gd.t2p_axis.num_points = 
      HDRSCMRMAC3_DEFAULT_NN_GUGD_NUM_T2P_VALS;
    p_config->flows[flow].gu_gd.frab_axis.num_points = 
      HDRSCMRMAC3_DEFAULT_NN_GUGD_NUM_FRAB_VALS;
    p_config->flows[flow].gu_gd.t2p_axis.values[0] = 
      HDRSCMRMAC3_DEFAULT_NN_T2P_AXIS_0;
    p_config->flows[flow].gu_gd.t2p_axis.values[1] = 
      HDRSCMRMAC3_DEFAULT_NN_T2P_AXIS_1;
    p_config->flows[flow].gu_gd.t2p_axis.values[2] = 
      HDRSCMRMAC3_DEFAULT_NN_T2P_AXIS_2;
    p_config->flows[flow].gu_gd.t2p_axis.values[3] = 
      HDRSCMRMAC3_DEFAULT_NN_T2P_AXIS_3;
    p_config->flows[flow].gu_gd.frab_axis.values[0] = 
      HDRSCMRMAC3_DEFAULT_NN_FRAB_AXIS_0;
    p_config->flows[flow].gu_gd.frab_axis.values[1] = 
      HDRSCMRMAC3_DEFAULT_NN_FRAB_AXIS_1;
    p_config->flows[flow].gu_gd.frab_axis.values[2] = 
      HDRSCMRMAC3_DEFAULT_NN_FRAB_AXIS_2;
    p_config->flows[flow].gu_gd.frab_axis.values[3] = 
      HDRSCMRMAC3_DEFAULT_NN_FRAB_AXIS_3;
    p_config->flows[flow].gu_gd.up[0][0] = HDRSCMRMAC3_DEFAULT_NN_UP_0_0;
    p_config->flows[flow].gu_gd.up[0][1] = HDRSCMRMAC3_DEFAULT_NN_UP_0_1;
    p_config->flows[flow].gu_gd.up[0][2] = HDRSCMRMAC3_DEFAULT_NN_UP_0_2;  
    p_config->flows[flow].gu_gd.up[0][3] = HDRSCMRMAC3_DEFAULT_NN_UP_0_3;
    p_config->flows[flow].gu_gd.up[1][0] = HDRSCMRMAC3_DEFAULT_NN_UP_1_0;
    p_config->flows[flow].gu_gd.up[1][1] = HDRSCMRMAC3_DEFAULT_NN_UP_1_1;
    p_config->flows[flow].gu_gd.up[1][2] = HDRSCMRMAC3_DEFAULT_NN_UP_1_2;
    p_config->flows[flow].gu_gd.up[1][3] = HDRSCMRMAC3_DEFAULT_NN_UP_1_3;
    p_config->flows[flow].gu_gd.up[2][0] = HDRSCMRMAC3_DEFAULT_NN_UP_2_0;
    p_config->flows[flow].gu_gd.up[2][1] = HDRSCMRMAC3_DEFAULT_NN_UP_2_1;
    p_config->flows[flow].gu_gd.up[2][2] = HDRSCMRMAC3_DEFAULT_NN_UP_2_2;
    p_config->flows[flow].gu_gd.up[2][3] = HDRSCMRMAC3_DEFAULT_NN_UP_2_3;
    p_config->flows[flow].gu_gd.up[3][0] = HDRSCMRMAC3_DEFAULT_NN_UP_3_0;
    p_config->flows[flow].gu_gd.up[3][1] = HDRSCMRMAC3_DEFAULT_NN_UP_3_1;
    p_config->flows[flow].gu_gd.up[3][2] = HDRSCMRMAC3_DEFAULT_NN_UP_3_2;
    p_config->flows[flow].gu_gd.up[3][3] = HDRSCMRMAC3_DEFAULT_NN_UP_3_3;
    p_config->flows[flow].gu_gd.down[0][0] = HDRSCMRMAC3_DEFAULT_NN_DOWN_0_0;
    p_config->flows[flow].gu_gd.down[0][1] = HDRSCMRMAC3_DEFAULT_NN_DOWN_0_1;
    p_config->flows[flow].gu_gd.down[0][2] = HDRSCMRMAC3_DEFAULT_NN_DOWN_0_2;
    p_config->flows[flow].gu_gd.down[0][3] = HDRSCMRMAC3_DEFAULT_NN_DOWN_0_3;
    p_config->flows[flow].gu_gd.down[1][0] = HDRSCMRMAC3_DEFAULT_NN_DOWN_1_0;
    p_config->flows[flow].gu_gd.down[1][1] = HDRSCMRMAC3_DEFAULT_NN_DOWN_1_1;
    p_config->flows[flow].gu_gd.down[1][2] = HDRSCMRMAC3_DEFAULT_NN_DOWN_1_2;
    p_config->flows[flow].gu_gd.down[1][3] = HDRSCMRMAC3_DEFAULT_NN_DOWN_1_3;
    p_config->flows[flow].gu_gd.down[2][0] = HDRSCMRMAC3_DEFAULT_NN_DOWN_2_0;
    p_config->flows[flow].gu_gd.down[2][1] = HDRSCMRMAC3_DEFAULT_NN_DOWN_2_1;
    p_config->flows[flow].gu_gd.down[2][2] = HDRSCMRMAC3_DEFAULT_NN_DOWN_2_2;
    p_config->flows[flow].gu_gd.down[2][3] = HDRSCMRMAC3_DEFAULT_NN_DOWN_2_3;
    p_config->flows[flow].gu_gd.down[3][0] = HDRSCMRMAC3_DEFAULT_NN_DOWN_3_0;
    p_config->flows[flow].gu_gd.down[3][1] = HDRSCMRMAC3_DEFAULT_NN_DOWN_3_1;
    p_config->flows[flow].gu_gd.down[3][2] = HDRSCMRMAC3_DEFAULT_NN_DOWN_3_2;
    p_config->flows[flow].gu_gd.down[3][3] = HDRSCMRMAC3_DEFAULT_NN_DOWN_3_3;
  } 

} /* hdrscmrmac3_set_config_to_default */


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
boolean hdrscmrmac3_prop_max_mac_flows( void )
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
                           hdrscmrmac3.trans_id, FALSE)) != E_SUCCESS)
  {
    /* No free timer was available. */
    status = FALSE;
  }

  if (status)
  {
    HDR_MSG_PROT(MSG_LEGACY_HIGH, "AT-init Proposal of MaxMACFlows(0x0014)");

    hdrscmrmac3.max_mac_flows.max_num_mac_flows = HDRSCMRMAC3_MAX_NUM_MAC_FLOWS;
    hdrscmrmac3.max_mac_flows.max_num_active_mac_flows = 
      HDRSCMRMAC3_MAX_NUM_MAC_FLOWS;

    /* Get a new DSM buffer */
    send_msg_ptr = hdrutil_new_msg_buffer();

    /* Pack AttribUpdateRequest message's Message ID */
    HDRBIT_PACK8(send_msg_ptr, HDRSCM_CONFIG_REQ_MSG,
                 attrib_offset, HDRSCM_MESSAGE_ID_SIZE );

    /* Shift the offset to start of Transaction ID */
    attrib_offset += HDRSCM_MESSAGE_ID_SIZE;

    /* Pack the message's Transaction ID */
    HDRBIT_PACK8(send_msg_ptr, hdrscmrmac3.trans_id, attrib_offset, 
                 HDRSCM_TRANS_ID_SIZE);

    /* Shift the offset to start of Attribute Record's Length */
    attrib_offset += HDRSCM_TRANS_ID_SIZE;

    HDRBIT_PACK8(send_msg_ptr, HDRSCMRMAC3_MAX_MAC_FLOWS_BYTE_LENGTH, 
                 attrib_offset, HDRSCM_LENGTH_SIZE);

    /* Shift the offset to start of AttributeID field */
    attrib_offset += HDRSCM_LENGTH_SIZE;

    HDRBIT_PACK16(send_msg_ptr, HDRSCMRMAC3_MAX_MAC_FLOWS_ATTRIB, attrib_offset,
                  HDRSCMRMAC3_ATTRIB_ID_SIZE);

    /* Shift the offset to start of Value ID field */
    attrib_offset += HDRSCMRMAC3_ATTRIB_ID_SIZE;

    /* Pack Value ID */
    hdrscmrmac3.max_mac_flow_value_id++;
    HDRBIT_PACK8(send_msg_ptr, hdrscmrmac3.max_mac_flow_value_id, attrib_offset, 
                 HDRSCMRMAC3_VALUE_ID_SIZE);
    /* Shift the offset to start of MaxNumMACFlows field  */
    attrib_offset += HDRSCMRMAC3_VALUE_ID_SIZE;

    /* Pack MaxNumMACFlows */
    HDRBIT_PACK8(send_msg_ptr, HDRSCMRMAC3_MAX_NUM_MAC_FLOWS, attrib_offset, 
                 HDRSCMRMAC3_MAX_NUM_MAC_FLOWS_SIZE);

    /* Shift the offset to start of MaxNumActiveMACFlows field */
    attrib_offset += HDRSCMRMAC3_MAX_NUM_MAC_FLOWS_SIZE;

    /* Pack MaxNumActiveMACflows */
    HDRBIT_PACK8(send_msg_ptr, HDRSCMRMAC3_MAX_NUM_MAC_FLOWS, attrib_offset, 
                 HDRSCMRMAC3_MAX_NUM_ACTIVE_MAC_FLOWS_SIZE);

    attrib_offset += HDRSCMRMAC3_MAX_NUM_ACTIVE_MAC_FLOWS_SIZE;

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
                   HDRSCMRMAC3_MAX_NUM_MAC_FLOWS,
                   HDRSCMRMAC3_MAX_NUM_MAC_FLOWS);

    /* Increment the transaction ID for next transaction */ 
    ++hdrscmrmac3.trans_id;
  }

  return status;

} /* hdrscmrmac3_prop_max_mac_flows */


/* EJECT */
/*===========================================================================

FUNCTION HDRSCMRMAC3_GET_PROP_COMPLEX_ATTRIB_VALUE

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
)
{

  hdrerrno_enum_type err_no = E_FAILURE;
    /* Error Code */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Make sure Attribute Information pointer is not NULL */
  ASSERT(a_aip != NULL);

  if (trans_id == (uint8)(hdrscmrmac3.trans_id-1))
  {
    switch(a_aip->attrib_id)
    {
      case HDRSCMRMAC3_MAX_MAC_FLOWS_ATTRIB:
      a_aip->value_id = hdrscmrmac3.max_mac_flow_value_id;
      a_aip->complex_val_ptr = (void*) &(hdrscmrmac3.max_mac_flows);
      a_aip->complex_val_size = sizeof(hdrscmrmac3.max_mac_flows);
      a_aip->is_accepted = TRUE;
      err_no = E_SUCCESS;
      break;
    
      default:
      /* unsupported attrib_id */
      HDR_MSG_PROT_1(MSG_LEGACY_ERROR, 
          "RMAC3: Attrib id %d not supported", a_aip->attrib_id);
      break;
    }
  }
  else
  {
    HDR_MSG_PROT_2(MSG_LEGACY_ERROR, 
        "RMAC3: trans_id %d mismatched proposed trans_id %d",
         trans_id, (uint8)(hdrscmrmac3.trans_id-1));
  }
  return err_no;
}


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
)
{

  hdrind_ind_data_union_type ind_data;

  /* Allocate out of the heap because it is possible for this indication to
  be queued from a different task context. Otherwise, the payload may be
  corrupted which could cause a null pointer de-reference. */
  static hdrscp_scp_fail_ind_type session_fail_data;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* fill the indication data */
  session_fail_data.type = HDRHAI_RTC_MAC_PROTOCOL;
  session_fail_data.subtype = HDRSCP_SUB3_RMAC;
  session_fail_data.attribute_id_length = HDRSCMRMAC3_MAX_MAC_FLOWS_BYTE_LENGTH;
  session_fail_data.attribute_id = HDRSCMRMAC3_MAX_MAC_FLOWS_ATTRIB;
  session_fail_data.reason = close_reason;

  ind_data.scp_fail_ptr = (void *)&session_fail_data;

  hdrind_give_ind(HDRIND_RMAC_PROT_NEG_FAILED,(void *)&ind_data);

} /* hdrscmrmac3_fail_curr_config_neg */



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
)
{
  uint8 gaup_ind = 0;
    /* Loop index */

  hdrerrno_enum_type err_no = E_NOT_ALLOWED;
    /* Error code */

  uint16 nn_attrib_base;
    /* Base of per-flow attribute */

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Check if the attribute is a non-per-flow attribute? */
  if ( ((attrib_id & HDRSCMRMAC3_NN_ATTRIB_3RD_NIBBLE_MASK) ==
         HDRSCMRMAC3_NON_NN_SIMPLE_ATTRIB_MASKED) ||
       ((attrib_id & HDRSCMRMAC3_NN_ATTRIB_3RD_NIBBLE_MASK) == 
         HDRSCMRMAC3_NON_NN_COMPLEX_ATTRIB_MASKED) )

  {
    /* Loop around the non-NN GAUPable attribute ID LUT to check if the given 
     attrib_id is one of them */
    for (gaup_ind = 0; (gaup_ind < HDRSCMRMAC3_NUM_GAUPABLE_NON_NN_ATTRIBS) &&
                     (err_no != E_SUCCESS); ++gaup_ind)
    {
      if (attrib_id == hdrscmrmac3_gaup_non_nn_attrib_ids[gaup_ind])
      {
        /* attrib_id is GAUPable */
        err_no = E_SUCCESS;
      }
    }
  }
  else
  /* Attribute is per-flow */
  {
    /* Find the NN-attribute base */
    nn_attrib_base = (attrib_id & HDRSCMRMAC3_NN_BASE_MASK);

    /* Loop around the NN GAUPable attribute ID LUT to check if the given 
       attrib_id is one of them */
    for (gaup_ind = 0; (gaup_ind < HDRSCMRMAC3_NUM_GAUPABLE_NN_ATTRIBS) &&
                       (err_no != E_SUCCESS); ++gaup_ind)
    {
      if (nn_attrib_base == hdrscmrmac3_gaup_nn_attrib_ids[gaup_ind])
      {
        /* attrib_id is GAUPable */
        err_no = E_SUCCESS;
      }
    }
  }

  return err_no;

} /* hdrscmrmac3_is_attrib_gaupable */


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
)
{
  boolean known = FALSE;

  uint8 mac_flow_id;
    /* MAC flow ID */

  uint16 nn_base;
    /* NN Base */
/*- - - - - - - - - - - - -- - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Check if the attribute is a non-NN simple attribute */
  known = HDRSCMRMAC3_IS_ATTRIB_NON_NN_SIMPLE(attrib_id);

  /* If the attribute is not known, it could either be a Non-NN complex, 
     an NN attribute or an Unknown attribute */
  if (!known)
  {
    /* Check if this is a non-NN complex attribute */
    known = HDRSCMRMAC3_IS_ATTRIB_NON_NN_COMPLEX(attrib_id);
  }

  /* If the attribute is still not known */
  if (!known)
  {
    /* At this stage, it could either be an NN attrib or an unknown attrib */

    /* Check if the MAC flow ID is in range */
    mac_flow_id = HDRSCMRMAC3_GET_MAC_FLOW_ID(attrib_id);

    if (mac_flow_id < HDRSCMRMAC3_MAX_NUM_MAC_FLOWS)
    {
      /* Attrib ID has an NN with-in supported range */

      nn_base = HDRSCMRMAC3_GET_NN_BASE(attrib_id);

      /* Check if the NN base is with-in supported range for either
         simple or complex NN attributes */
      known = (HDRSCMRMAC3_IS_NN_BASE_SIMPLE(nn_base) ||
               HDRSCMRMAC3_IS_NN_BASE_COMPLEX(nn_base));
    }
  }

  return known;

} /* hdrscmrmac3_attrib_is_supported */
