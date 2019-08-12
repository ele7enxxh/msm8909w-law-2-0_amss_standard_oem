/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

      H D R   S E S S I O N   C O N F I G U R A T I O N   M A N A G E R

       R E V E R S E  C H A N N E L  F L U I D  M A C  P R O T O C O L

       
GENERAL DESCRIPTION
  This module containing processing functions to handle the attributs
  processing for Reverse Channel MAC.  This module currently supports
  the processing of the common attributes between the Subtype 3 and 
  Subtype 4 RTCMAC protocols.
    
EXTERNALIZED FUNCTIONS

  hdrscmrmaccmn_process_aux_pilot_chan_params
    Process the AuxiliaryPilotChannelParameters attribute.

  hdrscmrmaccmn_process_common_power_params
    Process the CommonPowerParameters attribute.

  hdrscmrmaccmn_process_permitted_payload
    Process the PermittedPayload attribute.

  hdrscmrmaccmn_process_power_params
    Process the PowerParameters128-PowerParameters12288 attributes.

  hdrscmrmaccmn_process_request_params
    Process the RequestParameters attribute.

  hdrscmrmaccmn_process_rri_chan_power_params
    Process the RRIChannelPowerParameters atteibute.

  hdrscmrmaccmn_process_associated_flows_nn
    Process the AssociatedFlowsNN attribute.

  hdrscmrmaccmn_process_bucket_factor_nn
    Process the BucketFactorNN attribute.

  hdrscmrmaccmn_process_t2p_inflow_range_nn__config_xx
    Process the T2PInflowRangeNN or T2PInflowRangeConfigXX attributes.

  hdrscmrmaccmn_process_t2p_trans_func_nn__config_xx
    Process the T2PTransitionFunctionNN or T2PTransitionFunctionConfigXX
    attributes.

  hdrscmrmaccmn_process_max_num_sub_pkts__cc
    Process the MaxNumSubPackets or MaxNumSubPacketsCC attributes.

  hdrscmrmaccmn_process_pilot_strength__config_xx
    Process the PilotStrength or PilotStrengthConfigXX attributes.

  hdrscmrmaccmn_process_tx_t2p_max__config_xx
    Process the TxT2PMax or TxT2PMaxConfigXX attributes.

INITIALIZATION AND SEQUENCING REQUIREMENTS

  None.

  
  Copyright (c) 2007-2008 by Qualcomm Technologies, Inc.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                           EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/hdr/cp/src/hdrscmrmaccmn.c#1 $ $DateTime: 2016/12/13 07:58:05 $ $Author: mplcsds1 $
   
when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/07/14   vko     Fixed klocwork issues
01/30/14   vko     Fixed improper handling of the Flow Count values by AT
09/19/13   ukl     Reduce Code Duplication HDR-CP
12/04/08   etv     Lint fixes.
12/04/08   pba     Lint and Compiler warning fixes
03/30/07   cc      Created module.

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

#include "hdrscm.h"
#include "hdrscmdb.h"
#include "hdrscmrmaccmn.h"

/*===========================================================================

             DEFINITIONS  FROM  THE  HDR  AIR  INTERFACE  SPEC

===========================================================================*/

/*---------------------------------------------------------------------------
          Sizes of the complex attributes and its fields in bits 
----------------------------------------------------------------------------*/
/* AuxiliaryPilotChannelParameters */
#define HDRSCMRMACCMN_AUX_PILOT_CHAN_GAIN_SIZE          4
#define HDRSCMRMACCMN_AUX_PILOT_CHAN_MIN_PAYLOAD_SIZE   4
#define HDRSCMRMACCMN_AUX_PILOT_CHAN_PARAMS_SIZE        8

/* CommonPowerParameters */
#define HDRSCMRMACCMN_ALLOCATION_STAGGER_SIZE           4
#define HDRSCMRMACCMN_TX_T2P_MIN_SIZE                   8
#define HDRSCMRMACCMN_RPC_STEP_SIZE                     2
#define HDRSCMRMACCMN_RSVD_COM_POW_PARAM_SIZE           2
#define HDRSCMRMACCMN_COMM_POW_PARAMS_SIZE              16

/* MaxNumSubPackets or MaxNumSubPacketsCC */
#define HDRSCMRMACCMN_MAX_NUM_SUB_PKTS_N_SIZE           2
#define HDRSCMRMACCMN_RSVD_MAX_NUM_SUB_PKTS_SIZE        2
#define HDRSCMRMACCMN_MAX_NUM_SUB_PKTS_SIZE             8

/* PermittedPayload */
#define HDRSCMRMACCMN_PERMITTED_PAYLOAD_X_Y_SIZE        4
#define HDRSCMRMACCMN_RSVD_PERMITTED_PAYLOAD_SIZE       4
#define HDRSCMRMACCMN_PERMITTED_PAYLOAD_SIZE            160

/* PilotStrength or PilotStrengthConfigXX */
#define HDRSCMRMACCMN_PS_NUM_PS_AXIS_VALS_SIZE          4
#define HDRSCMRMACCMN_PS_AXIS_SIZE                      6
#define HDRSCMRMACCMN_PS_PS_AXIS_SIZE                   4
#define HDRSCMRMACCMN_PS_ATTRIB_MIN_SIZE                16

/* PowerParameters */
#define HDRSCMRMACCMN_PP_T2P_TRANSITION_SIZE            2
#define HDRSCMRMACCMN_PP_TERMINATION_TARGET_SIZE        2
#define HDRSCMRMACCMN_PP_T2P_PRE_TRANSITION_SIZE        8
#define HDRSCMRMACCMN_PP_T2P_POST_TRANSITION_SIZE       8
#define HDRSCMRMACCMN_POWER_PARAMS_SIZE                 40

/* RequestParameters */
#define HDRSCMRMACCMN_RP_REQ_RATIO_SIZE                 4
#define HDRSCMRMACCMN_RP_MAX_REQ_INTERVAL_SIZE          4
#define HDRSCMRMACCMN_REQUEST_PARAMS_SIZE               8

/* RRIChannelPowerParameters */
#define HDRSCMRMACCMN_RRI_CHAN_GAIN_TRANS_SIZE          4
#define HDRSCMRMACCMN_RSVD_RRI_CHAN_POW_PARAMS_SIZE     4
#define HDRSCMRMACCMN_RRI_CHAN_POW_PARAMS_SIZE          32

/* TxT2PMax or TxT2PMaxConfigXX */
#define HDRSCMRMACCMN_TX_T2P_NUM_PS_AXIS_VALS_SIZE      3
#define HDRSCMRMACCMN_TX_T2P_PS_AXIS_SIZE               6
#define HDRSCMRMACCMN_TX_T2P_MAX_PS_AXIS_SIZE           7
#define HDRSCMRMACCMN_MIN_TX_T2P_MAX_SIZE               16

/* AssociatedFlowsNN */
#define HDRSCMRMACCMN_AF_FLOW_COUNT_SIZE                8
#define HDRSCMRMACCMN_AF_STREAM_SIZE                    9
#define HDRSCMRMACCMN_AF_SUB_STREAM_SIZE                8
#define HDRSCMRMACCMN_MIN_ASSOCIATED_FLOWS_NN_SIZE      8

/* BucketFactorNN */
#define HDRSCMRMACCMN_BF_NUM_T2P_AXIS_VALS_SIZE         4
#define HDRSCMRMACCMN_BF_NUM_FRAB_AXIS_VALS_SIZE        3
#define HDRSCMRMACCMN_BF_T2P_AXIS_SIZE                  8
#define HDRSCMRMACCMN_BF_FRAB_AXIS_SIZE                 4
#define HDRSCMRMACCMN_BF_T2P_AXIS_FRAB_AXIS_SIZE        8
#define HDRSCMRMACCMN_MIN_BUCKET_FACTOR_NN_SIZE         32

/* T2PInflowRange or T2PInflowRangeConfigXX */
#define HDRSCMRMACCMN_T2P_INFLOW_MIN_SIZE               8
#define HDRSCMRMACCMN_T2P_INFLOW_MAX_SIZE               8
#define HDRSCMRMACCMN_T2P_INFLOW_RANGE_SIZE             16

/* T2PTransitionFunction NN or T2PTransitionFunctionConfigXX */
#define HDRSCMRMACCMN_GUGD_NUM_T2P_AXIS_VALS_SIZE       4
#define HDRSCMRMACCMN_GUGD_NUM_FRAB_AXIS_VALS_SIZE      3
#define HDRSCMRMACCMN_GUGD_T2P_AXIS_SIZE                8
#define HDRSCMRMACCMN_GUGD_FRAB_AXIS_SIZE               4
#define HDRSCMRMACCMN_GUGD_T2P_UP_T2P_FRAB_AXIS_SIZE    8
#define HDRSCMRMACCMN_GUGD_T2P_DOWN_T2P_FRAB_AXIS_SIZE  8
#define HDRSCMRMACCMN_MIN_GUGD_SIZE                     40 

/*---------------------------------------------------------------------------
     Valid Range definitions for each of the complex attribute fields
----------------------------------------------------------------------------*/
/* AuxiliaryPilotChannelParameters */
#define HDRSCMRMACCMN_MAX_AUX_PILOT_CHAN_GAIN           0x0F
#define HDRSCMRMACCMN_MAX_AUX_PILOT_CHAN_PAYLOAD        0x0B

/* CommonPowerParameters */
#define HDRSCMRMACCMN_MAX_ALLOCATION_STAGGER            0x0F
#define HDRSCMRMACCMN_MIN_RPC_STEP                      0x00
#define HDRSCMRMACCMN_MAX_RPC_STEP                      0x03

/* PilotStrength */
#define HDRSCMRMACCMN_MIN_PS_NUM_PS_AXIS_VALUES         0x01 
#define HDRSCMRMACCMN_MAX_PS_AXIS                       0x3F

/* PowerParameters */
#define HDRSCMRMACCMN_MAX_PP_PRE_TRANSITION             0x80
#define HDRSCMRMACCMN_MAX_PP_POST_TRANSITION            0x80

/* RequestParameters */
#define HDRSCMRMACCMN_RP_MAX_REQ_RATIO                  0x0F
#define HDRSCMRMACCMN_RP_MAX_MAX_REQ_INTERVAL           0x0F

/* TxT2PMax */
#define HDRSCMRMACCMN_MIN_TX_T2P_NUM_PS_AXIS_VALS       0x01
#define HDRSCMRMACCMN_MAX_TX_T2P_PS_AXIS                0x3F

/* MaxMACFlows */
#define HDRSCMRMACCMN_MIN_MAX_NUM_MAC_FLOWS             0x04
#define HDRSCMRMACCMN_MIN_MAX_NUM_ACTIVE_MAC_FLOWS      0x04

 /* EJECT */
/*===========================================================================
 
                  DEFINITIONS AND DECLARATIONS FOR MODULE

  This section contains local definitions for constants, macros, types,
  variables and other items needed by this module.

===========================================================================*/



/*------------------------------------------------------------------------
                    FUNCTION FORWARD DECLARATIONS
 -----------------------------------------------------------------------*/

/*------------------------------------------------------------------------
                            GLOBAL VARIABLES
 -----------------------------------------------------------------------*/


 /* EJECT */
/*===========================================================================

                         LOCAL FUNCTION DEFINITIONS
                    
===========================================================================*/

/*===========================================================================

FUNCTION HDRSCMRMACCMN_IS_AUX_PILOT_CHAN_PARAMS_VALID                 LOCAL

DESCRIPTION
  Check if the AuxillaryPilotChannelParameters is valid 

  0x0000 - AuxillaryPilotChannelParameters.

DEPENDENCIES
  None.

PARAMETERS
  aux_pilot_ptr   - Points to the AuxiliaryPilotChannelParameters structure.

RETURN VALUE
  E_SUCCESS       - If the AuxillaryPilotChannelParameters Attribute is valid.
  E_OUT_OF_RANGE  - otherwise. 

SIDE EFFECTS
  None.

===========================================================================*/
static hdrerrno_enum_type hdrscmrmaccmn_is_aux_pilot_chan_params_valid
(
  hdrscmrmaccmn_aux_pilot_struct_type *aux_pilot_ptr
)
{
  uint8 gain = aux_pilot_ptr->gain_db;
    /* AuxillaryPilotChannelGain */

  uint8 min_payload = aux_pilot_ptr->min_payload;
    /* AuxillaryPilotChannelPayload */

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */ 

  return ( (gain <= HDRSCMRMACCMN_MAX_AUX_PILOT_CHAN_GAIN) &&
           (min_payload <= HDRSCMRMACCMN_MAX_AUX_PILOT_CHAN_PAYLOAD)
         )? E_SUCCESS: E_OUT_OF_RANGE;

} /* hdrscmrmaccmn_is_aux_pilot_chan_params_valid */


 /* EJECT */
/*===========================================================================

FUNCTION HDRSCMRMACCMN_IS_COMMON_POWER_PARAMS_VALID                  LOCAL

DESCRIPTION
  Check if the CommonPowerParameters attribute is valid 

  0x0001 - CommonPowerParameters Attribute 

DEPENDENCIES
  None.

PARAMETERS
  common_pow_params_ptr - Pointer to the CommonPowerParameters structure.

RETURN VALUE
  E_SUCCESS       - If the attribute is valid.
  E_OUT_OF_RANGE  - otherwise. 

SIDE EFFECTS
  None.

===========================================================================*/
static hdrerrno_enum_type hdrscmrmaccmn_is_common_power_params_valid
(
  hdrscmrmaccmn_common_pow_params_struct_type *common_pow_params_ptr
)
{

  uint8 a_s = common_pow_params_ptr->alloc_stagger;
    /* Allocation Stagger */

  uint8 rpcstep = common_pow_params_ptr->rpc_step_db;
    /* RPC Step */

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */ 

  /* Any 8-bit value is valid for TxT2PMin. So checking for validity of 
     AllocationStagger and RPCStep alone! */
  return ( (a_s <= HDRSCMRMACCMN_MAX_ALLOCATION_STAGGER) &&
           (rpcstep <= HDRSCMRMACCMN_MAX_RPC_STEP) 
         )? E_SUCCESS: E_OUT_OF_RANGE;

} /* hdrscmrmaccmn_is_common_power_params_valid */


 /* EJECT */
/*===========================================================================

FUNCTION HDRSCMRMACCMN_IS_PERMITTED_PAYLOAD_VALID                     LOCAL

DESCRIPTION
  Check if the following attribute is valid 

  0x0003 - PermittedPayload 

DEPENDENCIES
  None.

PARAMETERS
  permitted_payload - Pointer to the PermittedPayload structure.

RETURN VALUE
  E_SUCCESS       - If the Attribute is valid.
  E_OUT_OF_RANGE  - otherwise. 

SIDE EFFECTS
  None.

===========================================================================*/
static hdrerrno_enum_type hdrscmrmaccmn_is_permitted_payload_valid
(
  uint8 permitted_payload[][HDRSCMRMACCMN_MAX_NUM_SUBFRAME_HISTORY]
)
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
       (p_ind < HDRSCMRMACCMN_MAX_NUM_PAYLOADS) && (is_valid); 
       ++p_ind)
  {
    /* For each subframe history indices, n-1, n-2, & n-3 */
    for ( subframe_ind=0; 
          (subframe_ind < HDRSCMRMACCMN_MAX_NUM_SUBFRAME_HISTORY)&&(is_valid); 
          ++subframe_ind )
    {
      /* Store to local variable for convenience */
      pp = permitted_payload[p_ind][subframe_ind];

      /* Is permittedPayload for payload index 'p_ind' and subframe history 
         index subframe_ind valid? */
      is_valid = (pp <= (uint8) HDRSCMRMACCMN_LAST_PAYLOAD_SIZE);
    }
  }

  return (is_valid)? E_SUCCESS: E_OUT_OF_RANGE;

} /* hdrscmrmaccmn_is_permitted_payload_valid */


 /* EJECT */
/*===========================================================================

FUNCTION HDRSCMRMAC4_IS_POWER_PARAMS_VALID                              LOCAL

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
  power_params_ptr - Pointer to the PowerParameters structure associated with
                     a particular payload index.

RETURN VALUE
  E_SUCCESS      - If the Attribute is valid.
  E_OUT_OF_RANGE - otherwise. 

SIDE EFFECTS
  None.

===========================================================================*/
static hdrerrno_enum_type hdrscmrmaccmn_is_power_params_valid
(
  hdrscmrmaccmn_power_params_struct_type *power_params_ptr
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
    power_params_ptr[HDRSCMRMACCMN_TX_MODE_LO_LAT].pre_transition;
  ll_post_trans = 
    power_params_ptr[HDRSCMRMACCMN_TX_MODE_LO_LAT].post_transition; 
  hc_pre_trans = 
    power_params_ptr[HDRSCMRMACCMN_TX_MODE_HI_CAP].pre_transition;
  hc_post_trans = 
    power_params_ptr[HDRSCMRMACCMN_TX_MODE_HI_CAP].post_transition;

  return ( (ll_pre_trans <= HDRSCMRMACCMN_MAX_PP_PRE_TRANSITION)    &&
           (ll_post_trans <= HDRSCMRMACCMN_MAX_PP_POST_TRANSITION)  &&
           (hc_pre_trans <= HDRSCMRMACCMN_MAX_PP_PRE_TRANSITION)    &&
           (hc_post_trans <= HDRSCMRMACCMN_MAX_PP_POST_TRANSITION)
         )? E_SUCCESS: E_OUT_OF_RANGE;

} /* hdrscmrmaccmn_is_power_params_valid */


 /* EJECT */
/*===========================================================================

FUNCTION HDRSCMRMACCMN_IS_REQUEST_PARAMS_VALID                     LOCAL

DESCRIPTION
  Check if the following attribute is valid 

  0x0011 - RequestParameters Attribute

DEPENDENCIES
  None.

PARAMETERS
  req_params_ptr - Pointer to the RequestParameters structure.

RETURN VALUE
  E_SUCCESS      - If the Attribute is valid.
  E_OUT_OF_RANGE - otherwise. 

SIDE EFFECTS
  None.

===========================================================================*/
static hdrerrno_enum_type hdrscmrmaccmn_is_request_params_valid
(
  hdrscmrmaccmn_req_params_struct_type *req_params_ptr
)
{

  uint8 req_ratio = req_params_ptr->req_ratio;
    /* ReqRatio. Convenience holder */

  uint8 max_req_interval = req_params_ptr->max_req_interval;
    /* MaxReqInteval Convenience holder */

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */   

  return ( (req_ratio <= HDRSCMRMACCMN_RP_MAX_REQ_RATIO)   &&
           (max_req_interval <= HDRSCMRMACCMN_RP_MAX_MAX_REQ_INTERVAL)
         )? E_SUCCESS: E_OUT_OF_RANGE;

} /* hdrscmrmaccmn_is_request_params_valid */


 /* EJECT */
/*===========================================================================

FUNCTION HDRSCMRMACCMN_IS_RRI_CHAN_POWER_PARAMS_VALID                LOCAL

DESCRIPTION
  Check if the following attribute is valid 

  0x0012 - RRIChannelPowerParameters Attribute

DEPENDENCIES
  None.

PARAMETERS
  rri_power_params_ptr - Pointer to the RRIChannelPowerParameters structure.

RETURN VALUE
  E_SUCCESS      - If the Attribute is valid.
  E_OUT_OF_RANGE - otherwise. 

SIDE EFFECTS
  None.

===========================================================================*/
static hdrerrno_enum_type hdrscmrmaccmn_is_rri_chan_power_params_valid
(
  hdrscmrmaccmn_rri_power_params_struct_type *rri_power_params_ptr
)
{

  SYS_ARG_NOT_USED(rri_power_params_ptr);

  /* Any 2's complement 4-bit number is valid for all the 
     RRIChannelGainPre/PostTransitions */
  return  E_SUCCESS;

} /* hdrscmrmaccmn_is_rri_chan_power_params_valid */


 /* EJECT */
/*===========================================================================

FUNCTION HDRSCMRMACCMN_IS_ASSOCIATED_FLOWS_NN_VALID                   LOCAL

DESCRIPTION
  Check if the following attribute is valid 

  0x0100 - 0x01NN - AssociatedFlowsNN attribute 

DEPENDENCIES
  None.

PARAMETERS
  associated_flows_ptr - Pointer to the AssociatedFlowsNN Structure.

RETURN VALUE
  E_SUCCESS      - If the Attribute is valid.
  E_OUT_OF_RANGE - otherwise. 

SIDE EFFECTS
  None.

===========================================================================*/
static hdrerrno_enum_type hdrscmrmaccmn_is_associated_flows_nn_valid
(
  hdrscmrmaccmn_associated_flows_struct_type *associated_flows_ptr
)
{
  SYS_ARG_NOT_USED(associated_flows_ptr);

  return E_SUCCESS;

} /* hdrscmrmaccmn_is_associated_flows_nn_valid */


 /* EJECT */
/*===========================================================================

FUNCTION HDRSCMRMACCMN_DUMP_BUCKET_FACTOR_NN                     LOCAL

DESCRIPTION
  Dump the following attribute in f3 mesg for debugging.

  0x0300 - 0x03NN - BucketFactorNN Attribute 

DEPENDENCIES
  None.

PARAMETERS
  bucket_factor_ptr - Pointer to the BucketFactorNN structure.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
static void hdrscmrmaccmn_dump_bucket_factor_nn
(
  hdrscmrmaccmn_t2p_frab_grid_struct_type *bucket_factor_ptr
)
{
  uint8 t2p_ind = 0;
    /* T2P axis index */

  uint8 frab_ind = 0;
    /* FRAB axis index */

  uint8 num_t2p_axis_vals = 
    bucket_factor_ptr->t2p_axis.num_points;
    /* Number of T2P axis values */

  uint8 num_frab_axis_vals = 
    bucket_factor_ptr->frab_axis.num_points;
    /* Number of T2P axis values */

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */ 

  HDR_MSG_PROT_2(MSG_LEGACY_HIGH, 
                 "BF(0x03NN): NumT2PAxisValues %x NumFRABAxisValues %x",
                 num_t2p_axis_vals,
                 num_frab_axis_vals);

  /* Print T2P Axis */
  for (t2p_ind=0; t2p_ind <= num_t2p_axis_vals; ++t2p_ind)
  {
    HDR_MSG_PROT_2(
      MSG_LEGACY_HIGH,
      "BF: T2PAxis[%x] = %x", 
      t2p_ind, 
      bucket_factor_ptr->t2p_axis.values[t2p_ind]);
  }

  /* Print FRAB Axis */
  for (frab_ind=0; frab_ind <= num_frab_axis_vals; ++frab_ind)
  {
    HDR_MSG_PROT_2(
      MSG_LEGACY_HIGH,
      "BF: FRABAxis[%x] = %x", 
      frab_ind, 
      bucket_factor_ptr->frab_axis.values[frab_ind]);
  }

  /* Print BFT2PAxisFRABAxis */
  for (t2p_ind=0; t2p_ind <= num_t2p_axis_vals; ++t2p_ind)
  {
    for (frab_ind=0; frab_ind <= num_frab_axis_vals; ++frab_ind)
    {
      HDR_MSG_PROT_3(
        MSG_LEGACY_HIGH,
        "BF: T2PAxisFRABAxis[%x][%x] = %x", 
        t2p_ind,
        frab_ind,
        bucket_factor_ptr->grid[t2p_ind][frab_ind]);
    }
  }

} /* hdrscmrmaccmn_dump_bucket_factor_nn */


 /* EJECT */
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
)
{
  uint8 key_ind = 0;
    /* Key Index */

  uint8 chk_ind = 0;
    /* Duplicate check index */

  boolean has_dup = FALSE;
    /* Does the Axis have duplicate points? */
    
  uint8 key = 0;
    /* element to be compared with rest of the elements of array for DUP 
       check */
  
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */ 

  /* For each point in the axis */
  for (key_ind=0; (!has_dup) && (key_ind < num_points); ++key_ind)
  {
    /* Let current point be the key */
    key = axis[key_ind];

    /* Look for key in rest of the array */
    for (chk_ind = key_ind+1; (!has_dup) && (chk_ind < num_points); ++chk_ind)
    {
      /* If the key is repeated, abort search and return */
      if (key == axis[chk_ind])
      {
        has_dup = TRUE;
      }
    }
  }

  return has_dup;

} /* hdrscmrmaccmn_axis_has_dup_points */


 /* EJECT */
/*===========================================================================

FUNCTION HDRSCMRMAC4_IS_BUCKET_FACTOR_NN_VALID                     LOCAL

DESCRIPTION
  Check if the following attribute is valid 

  0x0300 - 0x03NN - BucketFactorNN Attribute 

DEPENDENCIES
  None.

PARAMETERS
  bucket_factor_ptr - Pointer to the BucketFactorNN structure.

RETURN VALUE
  E_SUCCESS      - If the Attribute is valid.
  E_OUT_OF_RANGE - otherwise. 

SIDE EFFECTS
  None.

===========================================================================*/
static hdrerrno_enum_type hdrscmrmaccmn_is_bucket_factor_nn_valid
(
  hdrscmrmaccmn_t2p_frab_grid_struct_type *bucket_factor_ptr
)
{
  hdrerrno_enum_type err_no = E_SUCCESS;
    /* Error code */

  hdrscmrmaccmn_t2p_frab_grid_struct_type *bf = bucket_factor_ptr;
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
  if(t2_axis_num_points > HDRSCMRMACCMN_MAX_T2P_POINTS)
  {
      HDR_MSG_PROT_2 (MSG_LEGACY_ERROR, 
                   "t2_axis_num_points %d greater than max allowed. Limiting to max %d.",
                    t2_axis_num_points, HDRSCMRMACCMN_MAX_T2P_POINTS);
      t2_axis_num_points = HDRSCMRMACCMN_MAX_T2P_POINTS;
  }
  
  if(frab_axis_num_points > HDRSCMRMACCMN_MAX_FRAB_POINTS)
  {
      HDR_MSG_PROT_2 (MSG_LEGACY_ERROR, 
                   "frab_axis_num_point %d greater than max allowed. Limiting to max %d.",
                    frab_axis_num_points, HDRSCMRMACCMN_MAX_FRAB_POINTS);
      frab_axis_num_points = HDRSCMRMACCMN_MAX_FRAB_POINTS;
  }  
  
  if (hdrscmrmaccmn_axis_has_dup_points( bf->t2p_axis.values, 
                                          t2_axis_num_points) == TRUE)
  {
    err_no = E_OUT_OF_RANGE;
    HDR_MSG_PROT(MSG_LEGACY_ERROR, 
                 "Duplicate T2P axis points in BF(0x03NN). Rejected.");
    hdrscmrmaccmn_dump_bucket_factor_nn(bf);
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
    HDR_MSG_PROT(MSG_LEGACY_ERROR, 
                 "Duplicate FRAB axis points in BF(0x03NN). Rejected.");
    hdrscmrmaccmn_dump_bucket_factor_nn(bf);
  }

  return err_no;

} /* hdrscmrmaccmn_is_bucket_factor_nn_valid */


 /* EJECT */
/*===========================================================================

FUNCTION HDRSCMRMACCMN_IS_T2P_INFLOW_RANGE_VALID                    LOCAL

DESCRIPTION
  Check if the following attribute is valid 

  Subtype 3
    0x15NN  - T2PInflowRangeNN

  Subtype 4
    0x15XX  - T2PInflowRangeConfigXX

DEPENDENCIES
  None.

PARAMETERS
  t2p_inflow_range_ptr - Pointer to the T2PInflowRange structure.

RETURN VALUE
  E_SUCCESS      - If the Attribute is valid.
  E_OUT_OF_RANGE - otherwise. 

SIDE EFFECTS
  None.

===========================================================================*/
static hdrerrno_enum_type hdrscmrmaccmn_is_t2p_inflow_range_valid
(
  hdrscmrmaccmn_t2p_inflow_range_struct_type *t2p_inflow_range_ptr
)
{
  SYS_ARG_NOT_USED(t2p_inflow_range_ptr);

  /* All 8-bit values are valid for T2PInflowMin and T2PInflowMax fields */
  return E_SUCCESS;

} /* hdrscmrmaccmn_is_t2p_inflow_range_valid */


 /* EJECT */
/*===========================================================================

FUNCTION HDRSCMRMACCMN_DUMP_T2P_TRANS_FUNC                            LOCAL

DESCRIPTION
  Dump the following attribute in f3 mesg for debugging.

  Subtype 3
    0x16NN  - T2PTransitionFunctionNN

  Subtype 4
    0x16XX  - T2PTransitionFunctionConfigXX

DEPENDENCIES
  None.

PARAMETERS
  gu_gd_ptr - Pointer to the T2PTransitionFunction structure.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
static void hdrscmrmaccmn_dump_t2p_trans_func
(
  hdrscmrmaccmn_gu_gd_struct_type *gu_gd_ptr
)
{
  uint8 t2p_ind = 0;
    /* T2P axis index */

  uint8 frab_ind = 0;
    /* FRAB axis index */

  uint8 num_t2p_axis_vals = gu_gd_ptr->t2p_axis.num_points;
    /* Number of T2P axis points */

  uint8 num_frab_axis_vals = gu_gd_ptr->frab_axis.num_points;
    /* Number of FRAB axis points */

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */ 

  HDR_MSG_PROT_2(MSG_LEGACY_HIGH, 
                 "GUGD(0x16XX): NumT2PAxisValues %x NumFRABAxisValues %x",
                 num_t2p_axis_vals,
                 num_frab_axis_vals);

  /* Print T2P Axis */
  for (t2p_ind=0; t2p_ind <= num_t2p_axis_vals; ++t2p_ind)
  {
    HDR_MSG_PROT_2(
      MSG_LEGACY_HIGH,
      "GUGD: T2PAxis[%x] = %x", 
      t2p_ind, 
      gu_gd_ptr->t2p_axis.values[t2p_ind]);
  }

  /* Print FRAB Axis */
  for (frab_ind=0; frab_ind <= num_frab_axis_vals; ++frab_ind)
  {
    HDR_MSG_PROT_2(
      MSG_LEGACY_HIGH,
      "GUGD: FRABAxis[%x] = %x", 
      frab_ind, 
      gu_gd_ptr->frab_axis.values[frab_ind]);
  }

  /* Print GU/GD */
  for (t2p_ind=0; t2p_ind <= num_t2p_axis_vals; ++t2p_ind)
  {
    for (frab_ind=0; frab_ind <= num_frab_axis_vals; ++frab_ind)
    {
      HDR_MSG_PROT_4(
        MSG_LEGACY_HIGH,
        "GUGD: For T2PAxisFRABAxis[%x][%x] Up = %x; Down = %x", 
        t2p_ind,
        frab_ind,
        gu_gd_ptr->up[t2p_ind][frab_ind],
        gu_gd_ptr->down[t2p_ind][frab_ind]);
    }
  }

} /* hdrscmrmaccmn_dump_t2p_trans_func */


 /* EJECT */
/*===========================================================================

FUNCTION HDRSCMRMACCMN_IS_T2P_TRANS_FUNC_VALID                 LOCAL

DESCRIPTION
  Check if the following attribute is valid 

  Subtype 3
    0x16NN  - T2PTransitionFunctionNN

  Subtype 4
    0x16XX  - T2PTransitionFunctionConfigXX

DEPENDENCIES
  None.

PARAMETERS
  gu_gd_ptr - Pointer to the T2PTransitionFunction structure.

RETURN VALUE
  E_SUCCESS      - If the Attribute is valid.
  E_OUT_OF_RANGE - otherwise. 

SIDE EFFECTS
  None.

===========================================================================*/
static hdrerrno_enum_type hdrscmrmaccmn_is_t2p_trans_func_valid
(
  hdrscmrmaccmn_gu_gd_struct_type *gu_gd_ptr
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
  t2_axis_num_points = gu_gd_ptr->t2p_axis.num_points+1;
  frab_axis_num_points = gu_gd_ptr->frab_axis.num_points+1;
  if(t2_axis_num_points > HDRSCMRMACCMN_MAX_T2P_POINTS)
  {
      HDR_MSG_PROT_2 (MSG_LEGACY_ERROR, 
                   "t2_axis_num_points %d greater than max allowed. Limiting to max %d.",
                     t2_axis_num_points,HDRSCMRMACCMN_MAX_T2P_POINTS);
      t2_axis_num_points = HDRSCMRMACCMN_MAX_T2P_POINTS;
  }
  
  if(frab_axis_num_points > HDRSCMRMACCMN_MAX_FRAB_POINTS)
  {
      HDR_MSG_PROT_2 (MSG_LEGACY_ERROR, 
                   "frab_axis_num_points %d greater than max allowed. Limiting to max %d.",
                     frab_axis_num_points, HDRSCMRMACCMN_MAX_FRAB_POINTS);
      frab_axis_num_points = HDRSCMRMACCMN_MAX_FRAB_POINTS;
  }
  
  if (hdrscmrmaccmn_axis_has_dup_points( 
        gu_gd_ptr->t2p_axis.values, 
        t2_axis_num_points) == TRUE)
  {
    err_no = E_OUT_OF_RANGE;
    HDR_MSG_PROT(MSG_LEGACY_ERROR, 
                 "Duplicate T2P axis points in GuGd(0x16XX). Rejected.");
    hdrscmrmaccmn_dump_t2p_trans_func(gu_gd_ptr);
  }
  /* The FRAB axis has to monotonically increase. At present, software
     allows for non-sorted order. However, it does not allow duplicate
     values in the axis. Checking for duplicates... 
     The num_points is one less than actual number of points. Incremented
     to adjust. */
  else if (hdrscmrmaccmn_axis_has_dup_points( 
            gu_gd_ptr->frab_axis.values, 
            frab_axis_num_points) == TRUE)
  {
    err_no = E_OUT_OF_RANGE;
    HDR_MSG_PROT(MSG_LEGACY_ERROR, 
                 "Duplicate FRAB axis points in GuGd(0x16XX). Rejected.");
    hdrscmrmaccmn_dump_t2p_trans_func(gu_gd_ptr);
  }

  return err_no;

} /* hdrscmrmaccmn_is_t2p_trans_func_valid */


 /* EJECT */
/*===========================================================================

FUNCTION HDRSCMRMACCMN_IS_MAX_NUM_SUB_PKTS_VALID                     LOCAL

DESCRIPTION
  Check if the following attribute is valid 

  Subtype 3
    0x0002  - MaxNumSubpackets

  Subtype 4
    0x1ACC  - MaxNumSubPacketsCC

DEPENDENCIES
  None.

PARAMETERS
  max_num_sub_pkts_ptr - Pointer to the MaxNumSubPackets structure.

RETURN VALUE
  E_SUCCESS      - If the Attribute is valid.
  E_OUT_OF_RANGE - otherwise. 

SIDE EFFECTS
  None.

===========================================================================*/
static hdrerrno_enum_type hdrscmrmaccmn_is_max_num_sub_pkts_valid
(
  hdrscmrmaccmn_max_num_sub_pkts_struct_type *max_num_sub_pkts_ptr
)
{

  uint8 i0 = max_num_sub_pkts_ptr->interlace[0];
    /* Max Number of subpackets for interlace 0 */

  uint8 i1 = max_num_sub_pkts_ptr->interlace[1];
    /* Max Number of subpackets for interlace 1 */

  uint8 i2 = max_num_sub_pkts_ptr->interlace[2];
    /* Max Numeber of subpackets for interlace 2 */

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */   

  /* Accept default only (0x3) for Phase-1 */
  return ( (i0 == HDRSCMRMACCMN_DEFAULT_MAX_NUM_SUB_PACKETS)  && 
           (i1 == HDRSCMRMACCMN_DEFAULT_MAX_NUM_SUB_PACKETS)  &&
           (i2 == HDRSCMRMACCMN_DEFAULT_MAX_NUM_SUB_PACKETS)  
         )? E_SUCCESS: E_OUT_OF_RANGE;

} /* hdrscmrmaccmn_is_max_num_sub_pkts_valid */


 /* EJECT */
/*===========================================================================

FUNCTION HDRSCMRMACCMN_DUMP_PILOT_STRENGTH                          LOCAL

DESCRIPTION
  Dump the following attribute in f3 mesg for debugging.

  Subtype 3
    0x0004  - PilotStrength

  Subtype 4
    0x1BXX  - PilotStrengthConfigXX

DEPENDENCIES
  None.

PARAMETERS
  pilot_strength_ptr - Pointer to the PilotStrength structure.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
static void hdrscmrmaccmn_dump_pilot_strength
(
  hdrscmrmaccmn_pilot_strength_grid_struct_type *pilot_strength_ptr
)
{

  uint8 ps_axis_point = 0;
    /* Array index */

  uint8 n_ps_axis_vals = pilot_strength_ptr->num_ps_axis_values;
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
      pilot_strength_ptr->ps_axis[ps_axis_point], 
      pilot_strength_ptr->ps_ps_axis[ps_axis_point] );
  }

} /* hdrscmrmaccmn_dump_pilot_strength */


 /* EJECT */
/*===========================================================================

FUNCTION HDRSCMRMACCMN_IS_PILOT_STRENGTH_VALID                        LOCAL

DESCRIPTION
  Check if the following attribute is valid 

  Subtype 3
    0x0004  - PilotStrength

  Subtype 4
    0x1BXX  - PilotStrengthConfigXX

DEPENDENCIES
  None.

PARAMETERS
  pilot_strength_ptr - Pointer to the PilotStrength structure.

RETURN VALUE
  E_SUCCESS      - If the Attribute is valid.
  E_OUT_OF_RANGE - otherwise. 

SIDE EFFECTS
  None.

===========================================================================*/
static hdrerrno_enum_type hdrscmrmaccmn_is_pilot_strength_valid
(
  hdrscmrmaccmn_pilot_strength_grid_struct_type *pilot_strength_ptr
)
{

  uint8 ps_axis_val;
    /* Local var PilotStrengthAxis values for convenience */

  uint8 n_ps_axis_vals = pilot_strength_ptr->num_ps_axis_values;
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
    ((n_ps_axis_vals > 0) && (n_ps_axis_vals <= HDRSCMRMACCMN_MAX_PS_POINTS));

  /* Inspect PilotStrengthAxis values */
  for (ps_ind=0; ((ps_ind < n_ps_axis_vals) && (is_valid)); ++ps_ind)
  {
    ps_axis_val = pilot_strength_ptr->ps_axis[ps_ind];

    is_valid = (ps_axis_val <= HDRSCMRMACCMN_MAX_PS_AXIS);
  }

  if ((is_valid)&&((n_ps_axis_vals > 0) && (n_ps_axis_vals <= HDRSCMRMACCMN_MAX_PS_POINTS)))
  {
    /* The pilot strength axis has to monotonically increase. At present, 
       software allows for non-sorted order. However, it does not allow 
       duplicate values in the axis. Checking for duplicates... */
    has_dup = hdrscmrmaccmn_axis_has_dup_points(
                  pilot_strength_ptr->ps_axis, 
                  n_ps_axis_vals);

    is_valid = (has_dup == FALSE);
  }

  if (!is_valid)
  {
    HDR_MSG_PROT(MSG_LEGACY_ERROR, "PilotStrength Invalid. Rejected");
    hdrscmrmaccmn_dump_pilot_strength(pilot_strength_ptr);
  }

  return (is_valid)? E_SUCCESS: E_OUT_OF_RANGE;

} /* hdrscmrmaccmn_is_pilot_strength_valid */


 /* EJECT */
/*===========================================================================

FUNCTION HDRSCMRMACCMN_DUMP_TX_T2P_MAX                              LOCAL

DESCRIPTION
  Dump the following attribute in f3 mesg for debugging.

  Subtype 3
    0x0013  - TxT2PMax

  Subtype 4
    0x1CXX  - TxT2PMaxConfigXX

DEPENDENCIES
  None.

PARAMETERS
  config_id - ID of Configuration for which the TxT2Pmax is specified.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
static void hdrscmrmaccmn_dump_tx_t2p_max
(
  hdrscmrmaccmn_txt2p_max_grid_struct_type *txt2p_max_ptr
)
{
  uint8 ps_pt = 0;
    /* Array index */

  uint8 num_ps_axis_vals = txt2p_max_ptr->num_ps_axis_values;
    /* for convenience */

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */   

  HDR_MSG_PROT_1( MSG_LEGACY_HIGH, "NumPilotStrengthAxisValues %x",
                  num_ps_axis_vals );

  for (ps_pt=0; ps_pt < num_ps_axis_vals; ++ps_pt)
  {

    HDR_MSG_PROT_3( 
      MSG_LEGACY_HIGH,
      "Ind: %x, PSAxis: %x, TxT2PPSAxis: %x", 
      ps_pt, 
      txt2p_max_ptr->ps_axis[ps_pt], 
      txt2p_max_ptr->txt2p_max_ps_axis[ps_pt] );

  } /* for */

} /* hdrscmrmaccmn_dump_tx_t2p_max */


 /* EJECT */
/*===========================================================================

FUNCTION HDRSCMRMACCMN_IS_TX_T2P_MAX_VALID                           LOCAL

DESCRIPTION
  Check if the following attribute is valid 

  Subtype 3
    0x0013  - TxT2PMax

  Subtype 4
    0x1CXX  - TxT2PMaxConfigXX

DEPENDENCIES
  None.

PARAMETERS
  txt2p_max_ptr - Pointer to the TxT2PMax structure.

RETURN VALUE
  E_SUCCESS      - If the Attribute is valid.
  E_OUT_OF_RANGE - otherwise. 

SIDE EFFECTS
  None.

===========================================================================*/
static hdrerrno_enum_type hdrscmrmaccmn_is_tx_t2p_max_valid 
(
  hdrscmrmaccmn_txt2p_max_grid_struct_type *txt2p_max_ptr
)
{

  boolean is_valid = FALSE;
    /* Is the attrib valid? */

  uint8 num_ps_axis_vals = txt2p_max_ptr->num_ps_axis_values;
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
  is_valid = ((num_ps_axis_vals >= HDRSCMRMACCMN_MIN_TX_T2P_NUM_PS_AXIS_VALS) &&
               (num_ps_axis_vals <= HDRSCMRMACCMN_MAX_PS_POINTS));

  /* Are the PilotStrengthAxis fields valid? */
  for ( ps_pt=0; (is_valid) && (ps_pt < num_ps_axis_vals); ++ps_pt )
  {
    ps_axis = txt2p_max_ptr->ps_axis[ps_pt];

    is_valid = (ps_axis <= HDRSCMRMACCMN_MAX_TX_T2P_PS_AXIS);
  }

 if (is_valid && ((num_ps_axis_vals >= HDRSCMRMACCMN_MIN_TX_T2P_NUM_PS_AXIS_VALS) &&
               (num_ps_axis_vals <= HDRSCMRMACCMN_MAX_PS_POINTS)))
  {
    /* The pilot strength axis has to monotonically increase. At present, 
       software allows for non-sorted order. However, it does not allow 
       duplicate values in the axis. Checking for duplicates... */
    has_dup = hdrscmrmaccmn_axis_has_dup_points( 
                txt2p_max_ptr->ps_axis, num_ps_axis_vals);

    is_valid = (has_dup == FALSE);
  }

  if (!is_valid)
  {
    HDR_MSG_PROT(MSG_LEGACY_ERROR, "Invalid TxT2PMax. Rejected");
    hdrscmrmaccmn_dump_tx_t2p_max(txt2p_max_ptr);
  }

  return (is_valid)? E_SUCCESS: E_OUT_OF_RANGE;

} /* hdrscmrmaccmn_is_tx_t2p_max_valid */


 /* EJECT */
/*============================================================================

                        EXTERNAL FUNCTION DEFINITIONS

=============================================================================*/

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
           HDRSCMRMACCMN_AUX_PILOT_CHAN_PARAMS_SIZE) <= p_aip->attrib_len )
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
                             &(aux_pilot_ptr->gain_db),
                             (p_aip->attrib_offset + attrib_offset),
                             HDRSCMRMACCMN_AUX_PILOT_CHAN_GAIN_SIZE );
                            
    /* If error unpacking AuxiliaryPilotChannelGain, break */
    if (err_no != E_SUCCESS)
    {
      ERR("Error unpacking AuxiliaryPilotChannelGain",
            0,0,0);
      break;
    }

    /* Since AuxiliaryPilotChannelGain field was parsed sucessfully, Update the 
       attribute offset to start of AuxillayPilotChannelMinPayload field */ 
    attrib_offset += HDRSCMRMACCMN_AUX_PILOT_CHAN_GAIN_SIZE;

    err_no = HDRBIT_UNPACK8( item_ptr,
                             &(aux_pilot_ptr->min_payload),
                             (p_aip->attrib_offset + attrib_offset),
                             HDRSCMRMACCMN_AUX_PILOT_CHAN_MIN_PAYLOAD_SIZE );

    /* If error unpacking AuxiliaryPilotChannelMinPayload, break */
    if (err_no != E_SUCCESS)
    {
      ERR("Error unpacking AuxiliaryPilotChannelMinPayload",
            0,0,0);
      break;
    }

    /* Parsing done! :-) Is the parsed attribute valid? */
    if ( (err_no = 
          hdrscmrmaccmn_is_aux_pilot_chan_params_valid(aux_pilot_ptr)) != E_OUT_OF_RANGE )
    {
      /* Attrib valid! Yea! :-) print F3 mesg */
      HDR_MSG_PROT_2( MSG_LEGACY_HIGH, "AuxPilotChannelParams %x %x",
                      aux_pilot_ptr->gain_db,
                      aux_pilot_ptr->min_payload );

      /* Update the accepted attribute information */
      a_aip->is_accepted = TRUE;
      a_aip->value_id = value_id;
      a_aip->complex_val_ptr = (void*) aux_pilot_ptr;
      a_aip->complex_val_size = sizeof(hdrscmrmaccmn_aux_pilot_struct_type);
      break;
    }

    /* Not much luck :-( Let's try next set of values if there is one. 
       Shift the offset to start of next value ID. */
    attrib_offset += HDRSCMRMACCMN_AUX_PILOT_CHAN_MIN_PAYLOAD_SIZE;

  } /* while */

  return err_no;

} /* hdrscmrmaccmn_process_aux_pilot_chan_params */


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
           HDRSCMRMACCMN_COMM_POW_PARAMS_SIZE ) <= p_aip->attrib_len )
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
              &(common_pow_params_ptr->alloc_stagger), 
              (p_aip->attrib_offset + attrib_offset), 
              HDRSCMRMACCMN_ALLOCATION_STAGGER_SIZE );

    /* If error unpacking AllocationStagger, break */
    if (err_no != E_SUCCESS)
    {
      ERR("Error unpacking AllocationStagger of CommonPowerParameters Attrib",
            0,0,0);
      break;
    }

    /* Shift the offset to start of TxT2PMin Field */
    attrib_offset += HDRSCMRMACCMN_ALLOCATION_STAGGER_SIZE;

    /* Unpack TxT2PMin */
    err_no = HDRBIT_UNPACK8( 
              item_ptr,
              &(common_pow_params_ptr->tx_t2p_min_db), 
              (p_aip->attrib_offset + attrib_offset), 
              HDRSCMRMACCMN_TX_T2P_MIN_SIZE );

    /* If error unpacking TxT2PMin, break */
    if (err_no != E_SUCCESS)
    {
      ERR("Error unpacking TxT2PMin of CommonPowerParameters Attrib",
            0,0,0);
      break;
    }

    /* Shift the offset to start of RPCStep field */
    attrib_offset += HDRSCMRMACCMN_TX_T2P_MIN_SIZE;

    /* Unpack RPCStep */
    err_no = HDRBIT_UNPACK8( 
              item_ptr,
              &(common_pow_params_ptr->rpc_step_db), 
              (p_aip->attrib_offset + attrib_offset), 
              HDRSCMRMACCMN_RPC_STEP_SIZE );

    /* If error unpacking TxT2PMin, break */
    if (err_no != E_SUCCESS)
    {
      ERR("Error unpacking RPCStep of CommonPowerParameters Attrib",
            0,0,0);
      break;
    }

    /* Parsing done! :-) Is the parsed attribute valid? */
    if ( (err_no = 
          hdrscmrmaccmn_is_common_power_params_valid(common_pow_params_ptr)) != E_OUT_OF_RANGE)
    {
      /* Attrib valid! Yea! :-) print F3 mesg */
      HDR_MSG_PROT_3( MSG_LEGACY_HIGH, "CommonPowerParameters %x %x %x",
                      common_pow_params_ptr->alloc_stagger,
                      common_pow_params_ptr->tx_t2p_min_db,
                      common_pow_params_ptr->rpc_step_db );

      /* Update the accepted attribute information */
      a_aip->is_accepted = TRUE;
      a_aip->value_id = value_id;
      a_aip->complex_val_ptr = (void*) common_pow_params_ptr;
      a_aip->complex_val_size = sizeof(hdrscmrmaccmn_common_pow_params_struct_type);
      break;
    }

    /* Not much luck :-( Let's try next set of values if there is one. 
       Shift the offset to start of next value ID. */
    attrib_offset += (HDRSCMRMACCMN_RPC_STEP_SIZE + 
                      HDRSCMRMACCMN_RSVD_COM_POW_PARAM_SIZE);

  } /* while */

  return err_no;

} /* hdrscmrmaccmn_process_common_power_params */


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
           HDRSCMRMACCMN_PERMITTED_PAYLOAD_SIZE ) <= p_aip->attrib_len )
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
      for ( sf_hist=0; (sf_hist<HDRSCMRMACCMN_MAX_NUM_SUBFRAME_HISTORY) && 
            (err_no == E_SUCCESS); ++sf_hist)
      {
        /* Unpack permittedpayload(p_ind, sf_hist) */
        err_no = HDRBIT_UNPACK8( 
                  item_ptr,
                  &permitted_payload[p_ind][sf_hist],
                  (p_aip->attrib_offset + attrib_offset),
                  HDRSCMRMACCMN_PERMITTED_PAYLOAD_X_Y_SIZE );

        /* Shift the attrib offset to next field */
        attrib_offset += HDRSCMRMACCMN_PERMITTED_PAYLOAD_X_Y_SIZE;
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
          hdrscmrmaccmn_is_permitted_payload_valid(permitted_payload)) != E_OUT_OF_RANGE)
    {
      /* Attrib valid! Yea! :-) print F3 mesg */
      for ( p_ind=HDRSCMRMACCMN_FIRST_PAYLOAD_SIZE; 
            (p_ind < HDRSCMRMACCMN_MAX_NUM_PAYLOADS); ++p_ind )
      {
        HDR_MSG_PROT_4(MSG_LEGACY_HIGH,
                       "PermittedPayload%d_ %x, %x, %x",
                       p_ind,
                       permitted_payload[p_ind][0],
                       permitted_payload[p_ind][1],
                       permitted_payload[p_ind][2]);
      }

      /* Update the accepted attribute information */
      a_aip->is_accepted = TRUE;
      a_aip->value_id = value_id;
      a_aip->complex_val_ptr = (void*) permitted_payload;
      a_aip->complex_val_size = sizeof(hdrscmrmaccmn_permitted_payload_type);
      break;
    }

    /* Not much luck :-( Let's try next set of values if there is one. 
       Shift the offset to start of next value ID. */
    attrib_offset += HDRSCMRMACCMN_RSVD_PERMITTED_PAYLOAD_SIZE;

  } /* while */

  return err_no;

} /* hdrscmrmaccmn_process_permitted_payload */


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
     set of  PowerParameters complex Attribute Value */
  while ( (attrib_offset + p_aip->value_id_size + 
           HDRSCMRMACCMN_POWER_PARAMS_SIZE ) <= p_aip->attrib_len )
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
      ERR("Error unpacking Value ID of PowerParameters Attrib",
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
              &(power_params_ptr
                [HDRSCMRMACCMN_TX_MODE_LO_LAT].t2p_transition), 
              (p_aip->attrib_offset + attrib_offset),
              HDRSCMRMACCMN_PP_T2P_TRANSITION_SIZE );

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
    attrib_offset += HDRSCMRMACCMN_PP_T2P_TRANSITION_SIZE;

    /* Unpack LoLatTerminationTargetXXXX field */
    err_no = HDRBIT_UNPACK8( 
              item_ptr, 
              &(power_params_ptr
                [HDRSCMRMACCMN_TX_MODE_LO_LAT].termination_target), 
              (p_aip->attrib_offset + attrib_offset),
              HDRSCMRMACCMN_PP_TERMINATION_TARGET_SIZE );

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
    attrib_offset += HDRSCMRMACCMN_PP_TERMINATION_TARGET_SIZE;

    /* Unpack HiCapT2PTransitionXXXX field */
    err_no = HDRBIT_UNPACK8( 
              item_ptr, 
              &(power_params_ptr
                [HDRSCMRMACCMN_TX_MODE_HI_CAP].t2p_transition), 
              (p_aip->attrib_offset + attrib_offset),
              HDRSCMRMACCMN_PP_T2P_TRANSITION_SIZE );

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
    attrib_offset += HDRSCMRMACCMN_PP_T2P_TRANSITION_SIZE;

    /* Unpack HiCapTerminationTargetXXXX field */
    err_no = HDRBIT_UNPACK8( 
              item_ptr,
              &(power_params_ptr
                [HDRSCMRMACCMN_TX_MODE_HI_CAP].termination_target),
              (p_aip->attrib_offset + attrib_offset),
              HDRSCMRMACCMN_PP_TERMINATION_TARGET_SIZE );

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
    attrib_offset += HDRSCMRMACCMN_PP_TERMINATION_TARGET_SIZE;

    /* Unpack T2PLoLatPreTransitionXXXX field */
    err_no = HDRBIT_UNPACK8( 
              item_ptr,
              &(power_params_ptr
                [HDRSCMRMACCMN_TX_MODE_LO_LAT].pre_transition), 
              (p_aip->attrib_offset + attrib_offset),
              HDRSCMRMACCMN_PP_T2P_PRE_TRANSITION_SIZE );

    /* If error unpacking T2PLoLatPreTransitionXXXX, break */
    if (err_no != E_SUCCESS)
    {
      ERR("Error unpacking T2PLoLatPreTransitionXXXX of PowerParameters attrib",
          0,0,0);
      break;
    }

    /*----------------------------------------------------------------------
                          T2PLoLatPostTransition
     *--------------------------------------------------------------------*/

    /* Shift the offset to T2PLoLatPostTransitionXXXX field */
    attrib_offset += HDRSCMRMACCMN_PP_T2P_PRE_TRANSITION_SIZE;

    /* Unpack T2PLoLatPostTransitionXXXX field */
    err_no = HDRBIT_UNPACK8( 
              item_ptr,
              &(power_params_ptr
                [HDRSCMRMACCMN_TX_MODE_LO_LAT].post_transition),
              (p_aip->attrib_offset + attrib_offset),
              HDRSCMRMACCMN_PP_T2P_POST_TRANSITION_SIZE );

    /* If error unpacking T2PLoLatPostTransitionXXXX, break */
    if (err_no != E_SUCCESS)
    {
      ERR("Error unpacking T2PLoLatPostTransitionXXXX of PowerParameters attrib",
          0,0,0);
      break;
    }

    /*----------------------------------------------------------------------
                          T2PHiCapPreTransition
     *--------------------------------------------------------------------*/

    /* Shift the offset to T2PHiCapPreTransitionXXXX field */
    attrib_offset += HDRSCMRMACCMN_PP_T2P_POST_TRANSITION_SIZE;

    /* Unpack T2PHiCapPreTransitionXXXX field */
    err_no = HDRBIT_UNPACK8( 
              item_ptr,
              &(power_params_ptr
                [HDRSCMRMACCMN_TX_MODE_HI_CAP].pre_transition),
              (p_aip->attrib_offset + attrib_offset),
              HDRSCMRMACCMN_PP_T2P_PRE_TRANSITION_SIZE );

    /* If error unpacking T2PHiCapPreTransitionXXXX, break */
    if (err_no != E_SUCCESS)
    {
      ERR("Error unpacking T2PHiCapPreTransitionXXXX of PowerParameters attrib",
          0,0,0);
      break;
    }

    /*----------------------------------------------------------------------
                          T2PHiCapPostTransition
     *--------------------------------------------------------------------*/

    /* Shift the offset to T2PHiCapPostTransitionXXXX field */
    attrib_offset += HDRSCMRMACCMN_PP_T2P_PRE_TRANSITION_SIZE;

    /* Unpack T2PHiCapPostTransitionXXXX field */
    err_no = HDRBIT_UNPACK8( 
              item_ptr,
              &(power_params_ptr
                [HDRSCMRMACCMN_TX_MODE_HI_CAP].post_transition),
              (p_aip->attrib_offset + attrib_offset),
              HDRSCMRMACCMN_PP_T2P_POST_TRANSITION_SIZE );

    /* If error unpacking T2PHiCapPostTransitionXXXX, break */
    if (err_no != E_SUCCESS)
    {
      ERR("Error unpacking T2PHiCapPostTransitionXXXX of PowerParameters attrib",
          0,0,0);
      break;
    }

    /* Parsing done! :-) Is the parsed attribute valid? */
    if ( (err_no =
          hdrscmrmaccmn_is_power_params_valid(power_params_ptr)) != E_OUT_OF_RANGE )
    {
      /* Attrib valid! Yea! :-) print F3 mesg */
      HDR_MSG_PROT_5( 
        MSG_LEGACY_HIGH, "PowerParams: %x %x %x %x %x", 
        a_aip->attrib_id, 
        power_params_ptr[HDRSCMRMACCMN_TX_MODE_LO_LAT].t2p_transition, 
        power_params_ptr[HDRSCMRMACCMN_TX_MODE_LO_LAT].termination_target, 
        power_params_ptr[HDRSCMRMACCMN_TX_MODE_HI_CAP].t2p_transition, 
        power_params_ptr[HDRSCMRMACCMN_TX_MODE_HI_CAP].termination_target);

      HDR_MSG_PROT_5( 
        MSG_LEGACY_HIGH, "PowerParams: %x contd. %x, %x, %x, %x",
        a_aip->attrib_id,
        power_params_ptr[HDRSCMRMACCMN_TX_MODE_LO_LAT].pre_transition, 
        power_params_ptr[HDRSCMRMACCMN_TX_MODE_LO_LAT].post_transition, 
        power_params_ptr[HDRSCMRMACCMN_TX_MODE_HI_CAP].pre_transition, 
        power_params_ptr[HDRSCMRMACCMN_TX_MODE_HI_CAP].post_transition );

      /* Update the accepted attribute information */
      a_aip->is_accepted = TRUE;
      a_aip->value_id = value_id;
      a_aip->complex_val_ptr = (void*) power_params_ptr;
      a_aip->complex_val_size = 
        ( sizeof(hdrscmrmaccmn_power_params_struct_type) * 
          (uint16) HDRSCMRMACCMN_NUM_TX_MODES );
      break;
    }

    /* Not much luck :-( Let's try next set of values if there is one. 
       Shift the offset to start of next value ID. */
    attrib_offset += HDRSCMRMACCMN_PP_T2P_POST_TRANSITION_SIZE;

  } /* while */

  return err_no;

} /* hdrscmrmaccmn_process_power_params */


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
           HDRSCMRMACCMN_REQUEST_PARAMS_SIZE ) <= p_aip->attrib_len )
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
                             &(req_params_ptr->req_ratio),
                             (p_aip->attrib_offset + attrib_offset),
                             HDRSCMRMACCMN_RP_REQ_RATIO_SIZE );

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
    attrib_offset += HDRSCMRMACCMN_RP_REQ_RATIO_SIZE;

    /* Unpack MaxReqInterval */
    err_no = HDRBIT_UNPACK8( item_ptr,
                             &(req_params_ptr->max_req_interval),
                             (p_aip->attrib_offset + attrib_offset),
                             HDRSCMRMACCMN_RP_MAX_REQ_INTERVAL_SIZE );

    /* If error unpacking max_req_interval, break */
    if (err_no != E_SUCCESS)
    {
      ERR("Error unpacking max_req_interval field of RequestParameters",
          0,0,0);
      break;
    }

    /* Parsing done! :-) Is the parsed attribute valid? */
    if ( (err_no =
          hdrscmrmaccmn_is_request_params_valid(req_params_ptr)) != E_OUT_OF_RANGE )
    {
      /* Attrib valid! Yea! :-) print F3 mesg */
      HDR_MSG_PROT_2( MSG_LEGACY_HIGH, 
                      "RequestParams: %x %x",
                      req_params_ptr->req_ratio,
                      req_params_ptr->max_req_interval );

      /* Update the accepted attribute information */
      a_aip->is_accepted = TRUE;
      a_aip->value_id = value_id;
      a_aip->complex_val_ptr = (void*) req_params_ptr;
      a_aip->complex_val_size = sizeof(hdrscmrmaccmn_req_params_struct_type);
      break;
    }

    /* Not much luck :-( Let's try next set of values if there is one. 
       Shift the offset to start of next value ID. */
    attrib_offset += HDRSCMRMACCMN_RP_MAX_REQ_INTERVAL_SIZE;

  } /* while */

  return err_no;

} /* hdrscmrmaccmn_process_request_params */


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
           HDRSCMRMACCMN_RRI_CHAN_POW_PARAMS_SIZE ) <= p_aip->attrib_len )
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
          (t2p_trans < (HDRSCMRMACCMN_NUM_T2P_TRANS_POINTS-1)); ++t2p_trans)
    {
      /* Unpack RRIChannlGainPreTransition<t2p_trans> */
      err_no = HDRBIT_UNPACK8( 
        item_ptr, 
        &(rri_power_params_ptr[t2p_trans].rri_chan_gain_pre_transition ), 
        (p_aip->attrib_offset + attrib_offset), 
        HDRSCMRMACCMN_RRI_CHAN_GAIN_TRANS_SIZE );

      if (err_no == E_SUCCESS)
      {
        /* Shift offset to start of RRIChannelGainPostTransition<t2p_trans> */
        attrib_offset += HDRSCMRMACCMN_RRI_CHAN_GAIN_TRANS_SIZE; 

        /* Unpack RRIChannelGainPreTransition<t2p_trans> */
        err_no = HDRBIT_UNPACK8( 
          item_ptr, 
          &(rri_power_params_ptr[t2p_trans].rri_chan_gain_post_transition ), 
          (p_aip->attrib_offset + attrib_offset), 
          HDRSCMRMACCMN_RRI_CHAN_GAIN_TRANS_SIZE );

        /* Shift offset to start of RRIChannelGainPreTransition<t2p_trans+1> */
        attrib_offset += HDRSCMRMACCMN_RRI_CHAN_GAIN_TRANS_SIZE; 
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
      &(rri_power_params_ptr[HDRSCMRMACCMN_NUM_T2P_TRANS_POINTS-1].
        rri_chan_gain_pre_transition ),
      (p_aip->attrib_offset + attrib_offset), 
      HDRSCMRMACCMN_RRI_CHAN_GAIN_TRANS_SIZE );

    /* If error unpacking, break */
    if (err_no != E_SUCCESS)
    {
      ERR("Error unpacking RRIChannelGainPreTransition3",
          0,0,0);
      break;
    }

    /* Parsing done! :-) Is the parsed attribute valid? */
    if ( (err_no =
          hdrscmrmaccmn_is_rri_chan_power_params_valid(rri_power_params_ptr)) != E_OUT_OF_RANGE )
    {
      /* Attrib valid! Yea! :-) print F3 mesg */
      for (t2p_trans=0; t2p_trans < (HDRSCMRMACCMN_NUM_T2P_TRANS_POINTS-1);
           ++t2p_trans)
      {
        HDR_MSG_PROT_3( MSG_LEGACY_HIGH, 
                        "RRIChannelGainPre/PostTransition %x %x %x ", 
                        t2p_trans, 
                        rri_power_params_ptr[t2p_trans].rri_chan_gain_pre_transition, 
                        rri_power_params_ptr[t2p_trans].rri_chan_gain_post_transition );
      }

      HDR_MSG_PROT_1(
        MSG_LEGACY_HIGH,
        "RRIChannelGainPreTransition3 %x", 
        rri_power_params_ptr[HDRSCMRMACCMN_NUM_T2P_TRANS_POINTS-1].
        rri_chan_gain_pre_transition);

      /* Update the accepted attribute information */
      a_aip->is_accepted = TRUE;
      a_aip->value_id = value_id;
      a_aip->complex_val_ptr = (void*) rri_power_params_ptr;
      a_aip->complex_val_size = 
        (sizeof(hdrscmrmaccmn_rri_power_params_struct_type) * 
         HDRSCMRMACCMN_NUM_T2P_TRANS_POINTS);
      break;
    }

    /* Shift offset to start of Reserved field  */
    attrib_offset += HDRSCMRMACCMN_RRI_CHAN_GAIN_TRANS_SIZE; 

    /* Not much luck :-( Let's try next set of values if there is one. 
       Shift the offset to start of next value ID. */
    attrib_offset += HDRSCMRMACCMN_RSVD_RRI_CHAN_POW_PARAMS_SIZE;

  } /* while */

  return err_no;

} /* hdrscmrmaccmn_process_rri_chan_power_params */


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

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */ 

  /* Initialize the attribute offset to start of first Value ID */
  attrib_offset = p_aip->attrib_len_size + a_aip->attrib_id_size;

  /* While there are enough bits left in the Attribute Record for one more
     set of  AssociatedFlowsNN complex Attribute Value */
  while ( (attrib_offset + p_aip->value_id_size + 
           HDRSCMRMACCMN_MIN_ASSOCIATED_FLOWS_NN_SIZE ) <= p_aip->attrib_len )
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
                             &(associated_flows_ptr->flow_count),
                             (p_aip->attrib_offset + attrib_offset),
                             HDRSCMRMACCMN_AF_FLOW_COUNT_SIZE );

    /* If error unpacking FlowCount, break */
    if (err_no != E_SUCCESS)
    {
      ERR("Error unpacking FlowCount of AssociatedFlowNN attrib", 0,0,0);
      break;
    }

    /*----------------------------------------------------------------------
                              Stream and SubStream
     *--------------------------------------------------------------------*/

    if (associated_flows_ptr->flow_count > HDRSCMRMACCMN_MAX_APP_FLOWS)
    {
	  err_no = E_OUT_OF_RANGE;
      HDR_MSG_PROT_2(MSG_LEGACY_ERROR,"FlowCount Invalid %d, Max allowed: %d",
                     associated_flows_ptr->flow_count,
                     HDRSCMRMACCMN_MAX_APP_FLOWS);
      break;
    }

    /* Shift the offset to start of  Stream field */
    attrib_offset += HDRSCMRMACCMN_AF_FLOW_COUNT_SIZE;

    /* Unpack stream and sub-stream for each flow */
    for (flow = 0; (err_no == E_SUCCESS) 
          && (flow < associated_flows_ptr->flow_count); ++flow)
    {
      /* Unpack StreamID */
      err_no = HDRBIT_UNPACK16( 
        item_ptr, 
        &(associated_flows_ptr->app_flows[flow].stream_id), 
        (p_aip->attrib_offset + attrib_offset), 
        HDRSCMRMACCMN_AF_STREAM_SIZE );

      if (err_no == E_SUCCESS)
      {
        /* Shift the offset to start of SubStream field */
        attrib_offset += HDRSCMRMACCMN_AF_STREAM_SIZE;

        /* Unpack SubStreamID */
        err_no = HDRBIT_UNPACK8( 
          item_ptr, 
          &(associated_flows_ptr->app_flows[flow].sub_stream_id), 
          (p_aip->attrib_offset + attrib_offset), 
          HDRSCMRMACCMN_AF_SUB_STREAM_SIZE );

        /* Shift the offset to start of Stream field of next flow */
        attrib_offset += HDRSCMRMACCMN_AF_SUB_STREAM_SIZE;
      }

    } /* For each flow */

    if (err_no != E_SUCCESS)
    {
      ERR("Error unpacking Stream/SubStream for flow %d", (flow-1),0,0);
      break;
    }

    /* Parsing done! :-) Is the parsed attribute valid? */
    if ( (err_no =
          hdrscmrmaccmn_is_associated_flows_nn_valid(associated_flows_ptr)) != E_OUT_OF_RANGE )
    {
      /* Attrib valid! Yea! :-) print F3 mesg */
      HDR_MSG_PROT_1( MSG_LEGACY_HIGH,
                      "AF FlowCount %x",
                      associated_flows_ptr->flow_count );

      for (flow=0; flow<associated_flows_ptr->flow_count; ++flow)
      {
        HDR_MSG_PROT_3( 
          MSG_LEGACY_HIGH, 
          "AF FlowId: %x Stream: %x SubStream: %x", 
          flow, 
          associated_flows_ptr->app_flows[flow].stream_id, 
          associated_flows_ptr->app_flows[flow].sub_stream_id );
      }

      /* Update the accepted attribute information */
      a_aip->is_accepted = TRUE;
      a_aip->value_id = value_id;
      a_aip->complex_val_ptr = (void*) associated_flows_ptr;
      a_aip->complex_val_size = sizeof(hdrscmrmaccmn_associated_flows_struct_type);
      break;
    }

    /* Not much luck :-( Let's try next set of values if there is one. 
       Shift the offset to start of next value ID by aligning it to 
       the next octet boundary. */
    HDRSCMRMACCMN_ALIGN_TO_NEXT_OCTET(attrib_offset);

  } /* while */

  return err_no;

} /* hdrscmrmaccmn_process_associated_flows_nn */


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

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */ 
  
  /* Initialize the attribute offset to start of first Value ID */
  attrib_offset = p_aip->attrib_len_size + a_aip->attrib_id_size;

  /* While there are enough bits left in the Attribute Record for one more
     set of  BucketFactorNN complex Attribute Value */
  while ( (attrib_offset + p_aip->value_id_size + 
           HDRSCMRMACCMN_MIN_BUCKET_FACTOR_NN_SIZE) <= p_aip->attrib_len )
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
              &(bucket_factor_ptr->t2p_axis.num_points), 
              (p_aip->attrib_offset + attrib_offset), 
              HDRSCMRMACCMN_BF_NUM_T2P_AXIS_VALS_SIZE );

    /* If error unpacking NumT2PAxisValues, break */
    if (err_no != E_SUCCESS)
    {
      ERR("Error unpacking NumT2PAxisValues of BucketFactorNN", 0,0,0);
      break;
    }

    num_t2p_axis_vals = (bucket_factor_ptr->t2p_axis.num_points+1);

    /* NumT2PAxisValues is number of occurences of T2PAxis field minus 1. 
       Check if it exceeds the assumed max? */
    if (num_t2p_axis_vals > HDRSCMRMACCMN_MAX_T2P_POINTS)
    {
	  err_no = E_OUT_OF_RANGE;
      HDR_MSG_PROT_2(MSG_LEGACY_ERROR,
                     "Invalid NumT2PAxisPoints %d MaxAllowed %d",
                     num_t2p_axis_vals,
                     HDRSCMRMACCMN_MAX_T2P_POINTS);
      break;
    }

    /*----------------------------------------------------------------------
                                NumFRABAxisValues 
     *--------------------------------------------------------------------*/

    /* Shift the offset to start of  NumFRABAxisValues field */
    attrib_offset += HDRSCMRMACCMN_BF_NUM_T2P_AXIS_VALS_SIZE;

    /* Unpack NumT2PAxisValues */
    err_no = HDRBIT_UNPACK8( 
              item_ptr, 
              &(bucket_factor_ptr->frab_axis.num_points), 
              (p_aip->attrib_offset + attrib_offset), 
              HDRSCMRMACCMN_BF_NUM_FRAB_AXIS_VALS_SIZE );

    /* If error unpacking NumFRABAxisValues, break */
    if (err_no != E_SUCCESS)
    {
      ERR("Error unpacking NumFRABAxisValues of BucketFactorNN", 0,0,0);
      break;
    }

    num_frab_axis_vals = (bucket_factor_ptr->frab_axis.num_points+1);

    /* NumFRABAxisValues is number of occurences of FRABAxis field minus 1. 
       Check if it exceeds the assumed max? */
    if (num_frab_axis_vals > HDRSCMRMACCMN_MAX_FRAB_POINTS)
    {
	  err_no = E_OUT_OF_RANGE;
      HDR_MSG_PROT_2(MSG_LEGACY_ERROR,
                     "Invalid NumFRABAxisPoints %d MaxAllowed %d",
                     num_frab_axis_vals,
                     HDRSCMRMACCMN_MAX_FRAB_POINTS);
      break;
    }

    /*----------------------------------------------------------------------
                                      T2PAxis
     *--------------------------------------------------------------------*/

    /* Shift the offset to start of  T2PAxis field */
    attrib_offset += HDRSCMRMACCMN_BF_NUM_FRAB_AXIS_VALS_SIZE;

    for (t2p_ind=0; (err_no == E_SUCCESS) && (t2p_ind < num_t2p_axis_vals);
          ++t2p_ind)
    {
      /* Unpack T2PAxis */
      err_no = HDRBIT_UNPACK8( 
                item_ptr, 
                &(bucket_factor_ptr->t2p_axis.values[t2p_ind]), 
                (p_aip->attrib_offset + attrib_offset), 
                HDRSCMRMACCMN_BF_T2P_AXIS_SIZE );

      /* Shift offset to start of next T2PAxis / first FRABAxis field */
      attrib_offset += HDRSCMRMACCMN_BF_T2P_AXIS_SIZE;
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

    for (frab_ind=0; (err_no == E_SUCCESS) && (frab_ind < num_frab_axis_vals);
          ++frab_ind)
    {
      /* Unpack FRABAxis */
      err_no = HDRBIT_UNPACK8( 
                item_ptr, 
                &(bucket_factor_ptr->frab_axis.values[frab_ind]), 
                (p_aip->attrib_offset + attrib_offset), 
                HDRSCMRMACCMN_BF_FRAB_AXIS_SIZE );

      /* Shift offset to start of next FRABAxis / first BFT2PFRABAxis field */
      attrib_offset += HDRSCMRMACCMN_BF_FRAB_AXIS_SIZE;
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
          (t2p_ind < num_t2p_axis_vals); ++t2p_ind)
    {
      /* For each point in FRABAxis */
      for (frab_ind=0; (err_no == E_SUCCESS) && 
            (frab_ind < num_frab_axis_vals); ++frab_ind)
      {
        /* Unpack BucketFactorT2PAxisFRABAxis */
        err_no = HDRBIT_UNPACK8( 
                  item_ptr, 
                  &(bucket_factor_ptr->grid[t2p_ind][frab_ind]), 
                  (p_aip->attrib_offset + attrib_offset), 
                  HDRSCMRMACCMN_BF_T2P_AXIS_FRAB_AXIS_SIZE );

        /* Shift offset to start of next BFT2PFRABAxis field */
        attrib_offset += HDRSCMRMACCMN_BF_T2P_AXIS_FRAB_AXIS_SIZE;
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
          hdrscmrmaccmn_is_bucket_factor_nn_valid(bucket_factor_ptr)) != E_OUT_OF_RANGE )
    {
      /* Attrib valid! Yea! :-) print F3 mesg */
      hdrscmrmaccmn_dump_bucket_factor_nn(bucket_factor_ptr);
      
      /* Update the accepted attribute information */
      a_aip->is_accepted = TRUE;
      a_aip->value_id = value_id;
      a_aip->complex_val_ptr = (void*) bucket_factor_ptr;
      a_aip->complex_val_size = sizeof(hdrscmrmaccmn_t2p_frab_grid_struct_type);
      break;
    }

    /* Not much luck :-( Let's try next set of values if there is one. 
       Shift the offset to start of next value ID by aligning the offset
       to next octet boundary. */
    HDRSCMRMACCMN_ALIGN_TO_NEXT_OCTET(attrib_offset);

  } /* while */

  return err_no;

} /* hdrscmrmaccmn_process_bucket_factor_nn */


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
     set of  T2PInflowRangeConfigXX complex Attribute Value */
  while ( (attrib_offset + p_aip->value_id_size + 
           HDRSCMRMACCMN_T2P_INFLOW_RANGE_SIZE ) <= p_aip->attrib_len )
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
      ERR("Error unpacking Value ID of T2PInflowRange Attrib", 0,0,0);
      break;
    }

    /*----------------------------------------------------------------------
                                  T2PInflowMin
     *--------------------------------------------------------------------*/

    /* Shift the attribute offset to start of  T2PInflowMin field */ 
    attrib_offset += p_aip->value_id_size ;

    /* Unpack T2PInflowMin */
    err_no = HDRBIT_UNPACK8( item_ptr,
                             &(t2p_inflow_range_ptr->min),
                             (p_aip->attrib_offset + attrib_offset),
                             HDRSCMRMACCMN_T2P_INFLOW_MIN_SIZE );


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
    attrib_offset += HDRSCMRMACCMN_T2P_INFLOW_MIN_SIZE;

    /* Unpack T2PInflowMax */
    err_no = HDRBIT_UNPACK8( item_ptr,
                             &(t2p_inflow_range_ptr->max ),
                             (p_aip->attrib_offset + attrib_offset),
                             HDRSCMRMACCMN_T2P_INFLOW_MAX_SIZE );


    /* If error unpacking T2PInflowMax, break */
    if (err_no != E_SUCCESS)
    {
      ERR("Error unpacking T2PInflowMax", 0,0,0);
      break;
    }

    /* Parsing done! :-) Is the parsed attribute valid? */
    if ( (err_no =
          hdrscmrmaccmn_is_t2p_inflow_range_valid(t2p_inflow_range_ptr)) != E_OUT_OF_RANGE )
    {
      /* Attrib valid! Yea! :-) print F3 mesg */
      HDR_MSG_PROT_2( MSG_LEGACY_HIGH, "T2PInflowRange: Min: %x Max: %x",
                      t2p_inflow_range_ptr->min,
                      t2p_inflow_range_ptr->max );

      /* Update the accepted attribute information */
      a_aip->is_accepted = TRUE;
      a_aip->value_id = value_id;
      a_aip->complex_val_ptr = (void*) t2p_inflow_range_ptr;
      a_aip->complex_val_size = sizeof(hdrscmrmaccmn_t2p_inflow_range_struct_type);
      break;
    }

    /* Not much luck :-( Let's try next set of values if there is one. 
       Shift the offset to start of next value ID. */
    attrib_offset += HDRSCMRMACCMN_T2P_INFLOW_MAX_SIZE;

  } /* while */

  return err_no;

} /* hdrscmrmaccmn_process_t2p_inflow_range_nn__config_xx */


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

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */ 

  /* Initialize the attribute offset to start of first Value ID */
  attrib_offset = p_aip->attrib_len_size + a_aip->attrib_id_size;

  /* While there are enough bits left in the Attribute Record for one more
     set of T2PTransitionFunctionConfigXX complex Attribute Value */
  while ( (attrib_offset + p_aip->value_id_size + 
           HDRSCMRMACCMN_MIN_GUGD_SIZE) <= p_aip->attrib_len )
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
      ERR("Error unpacking Value ID of T2PTransitionFunction Attrib",
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
              &(gu_gd_ptr->t2p_axis.num_points),
              (p_aip->attrib_offset + attrib_offset), 
              HDRSCMRMACCMN_GUGD_NUM_T2P_AXIS_VALS_SIZE );

    /* If error unpacking NumT2PAxisValues, break */
    if (err_no != E_SUCCESS)
    {
      ERR("Error unpacking NumT2PAxisValues of T2PTransitionFunction", 0,0,0);
      break;
    }

    num_t2p_axis_vals = (gu_gd_ptr->t2p_axis.num_points+1);

    /* NumT2PAxisValues is number of occurences of T2PAxis field minus 1. 
       Check if it exceeds the assumed max? */
    if (num_t2p_axis_vals > HDRSCMRMACCMN_MAX_T2P_POINTS)
    {
	  err_no = E_OUT_OF_RANGE;
      HDR_MSG_PROT_2(MSG_LEGACY_ERROR,
                     "GUGD: Invalid NumT2PAxisPoints %d MaxAllowed %d",
                     num_t2p_axis_vals,
                     HDRSCMRMACCMN_MAX_T2P_POINTS);
      break;
    }

    /*----------------------------------------------------------------------
                                NumFRABAxisValues 
     *--------------------------------------------------------------------*/

    /* Shift the offset to start of  NumFRABAxisValues field */
    attrib_offset += HDRSCMRMACCMN_GUGD_NUM_T2P_AXIS_VALS_SIZE;

    /* Unpack NumFRABAxisValues */
    err_no = HDRBIT_UNPACK8( 
              item_ptr, 
              &(gu_gd_ptr->frab_axis.num_points),
              (p_aip->attrib_offset + attrib_offset), 
              HDRSCMRMACCMN_GUGD_NUM_FRAB_AXIS_VALS_SIZE );

    /* If error unpacking NumFRABAxisValues, break */
    if (err_no != E_SUCCESS)
    {
      ERR("Error unpacking NumFRABAxisValues of T2PTransitionFunction", 
          0,0,0);
      break;
    }

    num_frab_axis_vals = (gu_gd_ptr->frab_axis.num_points+1);

    /* NumFRABAxisValues is number of occurences of FRABAxis field minus 1. 
       Check if it exceeds the assumed max? */
    if (num_frab_axis_vals > HDRSCMRMACCMN_MAX_FRAB_POINTS)
    {
	  err_no = E_OUT_OF_RANGE;
      HDR_MSG_PROT_2(MSG_LEGACY_ERROR,
                     "GUGD: Invalid NumFRABAxisPoints %d MaxAllowed %d",
                     num_frab_axis_vals,
                     HDRSCMRMACCMN_MAX_FRAB_POINTS);
      break;
    }

    /*----------------------------------------------------------------------
                                      T2PAxis
     *--------------------------------------------------------------------*/

    /* Shift the offset to start of  T2PAxis field */
    attrib_offset += HDRSCMRMACCMN_GUGD_NUM_FRAB_AXIS_VALS_SIZE;

    for (t2p_ind=0; (err_no == E_SUCCESS) && (t2p_ind < num_t2p_axis_vals);
          ++t2p_ind)
    {
      /* Unpack T2PAxis */
      err_no = HDRBIT_UNPACK8( 
                item_ptr, 
                &(gu_gd_ptr->t2p_axis.values[t2p_ind]),
                (p_aip->attrib_offset + attrib_offset), 
                HDRSCMRMACCMN_GUGD_T2P_AXIS_SIZE );

      /* Shift offset to start of next T2PAxis / first FRABAxis field */
      attrib_offset += HDRSCMRMACCMN_GUGD_T2P_AXIS_SIZE;
    }

    /* If error unpacking T2PAxis, break */
    if (err_no != E_SUCCESS)
    {
      ERR("Error unpacking T2PAxis @ %d of T2PTransitionFunction", 
          (t2p_ind-1),0,0);
      break;
    }

    /*----------------------------------------------------------------------
                                      FRABAxis
     *--------------------------------------------------------------------*/

    for (frab_ind=0; (err_no == E_SUCCESS) && (frab_ind < num_frab_axis_vals);
          ++frab_ind)
    {
      /* Unpack FRABAxis */
      err_no = HDRBIT_UNPACK8( 
                item_ptr, 
                &(gu_gd_ptr->frab_axis.values[frab_ind]),
                (p_aip->attrib_offset + attrib_offset), 
                HDRSCMRMACCMN_GUGD_FRAB_AXIS_SIZE );

      /* Shift offset to start of next FRABAxis / first T2PUpT2PFRABAxis 
         field */
      attrib_offset += HDRSCMRMACCMN_GUGD_FRAB_AXIS_SIZE;
    }

    /* If error unpacking FRABAxis, break */
    if (err_no != E_SUCCESS)
    {
      ERR("Error unpacking FRABAxis @ %d of T2PTransitionFunction", 
          (frab_ind-1),0,0);
      break;
    }

    /*----------------------------------------------------------------------
                              T2PUpT2PAxisFRABAxis 
     *--------------------------------------------------------------------*/

    /* For each point in T2PAxis */
    for (t2p_ind=0; (err_no == E_SUCCESS) && 
          (t2p_ind < num_t2p_axis_vals); ++t2p_ind)
    {
      /* For each point in FRABAxis */
      for (frab_ind=0; (err_no == E_SUCCESS) && 
            (frab_ind < num_frab_axis_vals); ++frab_ind)
      {
        /* Unpack  T2PUpT2PAxisFRABAxis */
        err_no = HDRBIT_UNPACK8( 
                  item_ptr, 
                  &(gu_gd_ptr->up[t2p_ind][frab_ind]),
                  (p_aip->attrib_offset + attrib_offset), 
                  HDRSCMRMACCMN_GUGD_T2P_UP_T2P_FRAB_AXIS_SIZE );

        if (err_no == E_SUCCESS)
        {
          /* Shift offset to start of next T2PDownT2PAxisFRABAxis field */
          attrib_offset += HDRSCMRMACCMN_GUGD_T2P_UP_T2P_FRAB_AXIS_SIZE;

          /* Unpack  T2PDownT2PAxisFRABAxis */
          err_no = HDRBIT_UNPACK8( 
                    item_ptr, 
                    &(gu_gd_ptr->down[t2p_ind][frab_ind]),
                    (p_aip->attrib_offset + attrib_offset), 
                    HDRSCMRMACCMN_GUGD_T2P_DOWN_T2P_FRAB_AXIS_SIZE );

          /* Shift offset to start of next T2PUpT2PAxisFRABAxis field */
          attrib_offset += HDRSCMRMACCMN_GUGD_T2P_DOWN_T2P_FRAB_AXIS_SIZE;
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
          hdrscmrmaccmn_is_t2p_trans_func_valid(gu_gd_ptr)) != E_OUT_OF_RANGE )
    {
      /* Attrib valid! Yea! :-) print F3 mesg */
      hdrscmrmaccmn_dump_t2p_trans_func(gu_gd_ptr);
      
      /* Update the accepted attribute information */
      a_aip->is_accepted = TRUE;
      a_aip->value_id = value_id;
      a_aip->complex_val_ptr = (void*) gu_gd_ptr;
      a_aip->complex_val_size = sizeof(hdrscmrmaccmn_gu_gd_struct_type);
      break;
    }

    /* Not much luck :-( Let's try next set of values if there is one. 
       Shift the offset to start of next value ID by aligning the offset
       to next octet boundary. */
    HDRSCMRMACCMN_ALIGN_TO_NEXT_OCTET(attrib_offset);

  } /* while */

  return err_no;

} /* hdrscmrmaccmn_process_t2p_trans_func_nn__config_xx */


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
           HDRSCMRMACCMN_MAX_NUM_SUB_PKTS_SIZE ) <= p_aip->attrib_len )
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

    for ( inter=0; (inter < HDRSCMRMACCMN_MAX_NUM_INTERLACES) && (!err_no); 
          ++inter)
    {
      /* Unpack MaxNumSubPackets0 */
      err_no = HDRBIT_UNPACK8( item_ptr,
                               &(max_num_sub_pkts_ptr->interlace[inter]),
                               (p_aip->attrib_offset + attrib_offset),
                               HDRSCMRMACCMN_MAX_NUM_SUB_PKTS_N_SIZE );

      /* Shift the offset to next field */
      attrib_offset += HDRSCMRMACCMN_MAX_NUM_SUB_PKTS_N_SIZE;
    }

    /* If error unpacking MaxNumSubPackets, break */
    if (err_no != E_SUCCESS)
    {
      ERR("Error unpacking MaxNumSubPackets for interlace 2",
          0,0,0);
      break;
    }

    /* Parsing done! :-) Is the parsed attribute valid? */
    if ( (err_no = 
          hdrscmrmaccmn_is_max_num_sub_pkts_valid(max_num_sub_pkts_ptr)) != E_OUT_OF_RANGE )
    {
      /* Attrib valid! Yea! :-) print F3 mesg */
      HDR_MSG_PROT_3( MSG_LEGACY_HIGH, "MaxNumSubPackets %x %x %x",
                      max_num_sub_pkts_ptr->interlace[0],
                      max_num_sub_pkts_ptr->interlace[1],
                      max_num_sub_pkts_ptr->interlace[2] );

      /* Update the accepted attribute information */
      a_aip->is_accepted = TRUE;
      a_aip->value_id = value_id;
      a_aip->complex_val_ptr = (void*) max_num_sub_pkts_ptr;
      a_aip->complex_val_size = sizeof(hdrscmrmaccmn_max_num_sub_pkts_struct_type);
      break;
    }

    /* Not much luck :-( Let's try next set of values if there is one. 
       Shift the offset to start of next value ID. */
    attrib_offset += HDRSCMRMACCMN_RSVD_MAX_NUM_SUB_PKTS_SIZE;

  } /* while */

  return err_no;

} /* hdrscmrmaccmn_process_max_num_sub_pkts__cc */


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
           HDRSCMRMACCMN_PS_ATTRIB_MIN_SIZE ) <= p_aip->attrib_len )
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
      ERR("Error unpacking Value ID of PilotStrgnth Attrib",
            0,0,0);
      break;
    }

    /* Since Value Id was parsed sucessfully, Update the attribute offset to 
     * start of  field */ 
    attrib_offset += p_aip->value_id_size ;

    /* Unpack NumPilotStrengthAxisValues */
    err_no = HDRBIT_UNPACK8( item_ptr,
                             &(pilot_strength_ptr->num_ps_axis_values),
                             (p_aip->attrib_offset + attrib_offset),
                             HDRSCMRMACCMN_PS_NUM_PS_AXIS_VALS_SIZE );

    /* If error unpacking NumPilotStrengthAxisValues, break */
    if (err_no != E_SUCCESS)
    {
      ERR("Error unpacking NumPilotStrengthAxisValues of PilotStrength Attrib",
          0,0,0);
      break;
    }

    /* Store into a local var that is easier to type! */
    n_ps_axis_vals = pilot_strength_ptr->num_ps_axis_values;

    /* Check if NumPilotStrengthAxisValues is within permissible range */
    if ((n_ps_axis_vals < HDRSCMRMACCMN_MIN_PS_NUM_PS_AXIS_VALUES) || 
        (n_ps_axis_vals > HDRSCMRMACCMN_MAX_PS_POINTS))
    { 
	  err_no = E_OUT_OF_RANGE;
      ERR("Invalid NumPilotStrengthAxisValues; Bailing out!", 0, 0, 0);
      break;
    }

    /* Valid NumPilotStrengthAxisValues! :-) Yeaaaaa
       Shift the offset to next field */
    attrib_offset += HDRSCMRMACCMN_PS_NUM_PS_AXIS_VALS_SIZE;

    /* Loop around and parse n_ps_axis_vals instances of PSAxis and PSPSAxis 
       fields */
    for (ps_axis_point=0; ((ps_axis_point < n_ps_axis_vals) && (!err_no)); 
         ++ps_axis_point)
    {
      /* Unpack PilotStrengthAxis */
      err_no = HDRBIT_UNPACK8( 
                item_ptr, 
                &(pilot_strength_ptr->ps_axis[ps_axis_point]), 
                (p_aip->attrib_offset + attrib_offset),
                HDRSCMRMACCMN_PS_AXIS_SIZE );

      /* If PilotStrengthAxis was successfully unpacked... */
      if (err_no == E_SUCCESS)
      {
        /* PSAxis successfully unpacked! Yeaaaa
           Shift the offset to start of PSPSAxis */
        attrib_offset += HDRSCMRMACCMN_PS_AXIS_SIZE;

        /* Unpack PilotStrengthPilotStrengthAxis */
        err_no = HDRBIT_UNPACK8( 
                  item_ptr, 
                  &(pilot_strength_ptr->ps_ps_axis[ps_axis_point]), 
                  (p_aip->attrib_offset + attrib_offset),
                  HDRSCMRMACCMN_PS_PS_AXIS_SIZE );

        /* Shift the offset to start of next PSAxis */
        attrib_offset += HDRSCMRMACCMN_PS_PS_AXIS_SIZE;
      }

    } /* for */

    if (err_no != E_SUCCESS)
    {
      ERR("Error unpacking PSAxis/PSPSAxis at ind %d", (ps_axis_point-1),0,0);
      break;
    }

    /* Parsing done! :-) Is the parsed attribute valid? */
    if ( (err_no =
          hdrscmrmaccmn_is_pilot_strength_valid(pilot_strength_ptr)) != E_OUT_OF_RANGE )
    {
      /* Attrib valid! Yea! :-) print F3 mesg */
      hdrscmrmaccmn_dump_pilot_strength(pilot_strength_ptr);

      /* Update the accepted attribute information */
      a_aip->is_accepted = TRUE;
      a_aip->value_id = value_id;
      a_aip->complex_val_ptr = (void*) pilot_strength_ptr;
      a_aip->complex_val_size = 
        sizeof(hdrscmrmaccmn_pilot_strength_grid_struct_type);
      break;
    }

    /* Not much luck :-( Let's try next set of values if there is one. 
       Shift the offset to start of next value ID by rounding-up to 
       next octet. */
    HDRSCMRMACCMN_ALIGN_TO_NEXT_OCTET(attrib_offset);

  } /* while */

  return err_no;

} /* hdrscmrmaccmn_process_pilot_strength__config_xx */


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
           HDRSCMRMACCMN_MIN_TX_T2P_MAX_SIZE ) <= p_aip->attrib_len )
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
      ERR("Error unpacking Value ID of TxT2PMax Attrib",
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
              &(txt2p_max_ptr->num_ps_axis_values), 
              (p_aip->attrib_offset + attrib_offset), 
              HDRSCMRMACCMN_TX_T2P_NUM_PS_AXIS_VALS_SIZE );

    /* If error unpacking NumPilotStrengthAxisValues, break */
    if (err_no != E_SUCCESS)
    {
      ERR("Error unpacking NumPilotStrengthAxisValues %d", 0,0,0);
      break;
    }

    /*----------------------------------------------------------------------
                PilotStrengthAxis and TxT2PmaxPilotStrengthAxis
     *--------------------------------------------------------------------*/

    num_ps_axis_vals = txt2p_max_ptr->num_ps_axis_values;

    /* Check for the validity of NumPilotStrengthAxisValues before proceeding */
    if (num_ps_axis_vals < HDRSCMRMACCMN_MIN_TX_T2P_NUM_PS_AXIS_VALS)
    {
	  err_no = E_OUT_OF_RANGE;
      HDR_MSG_PROT_1(MSG_LEGACY_HIGH, "Invalid NumPilotStrengthAxisvalues %d",
                     num_ps_axis_vals);
      break;
    }

    /* Shift the offset to start of  PilotStrengthAxis field */
    attrib_offset += HDRSCMRMACCMN_TX_T2P_NUM_PS_AXIS_VALS_SIZE;

    /* Loop around and unpack num_ps_axis_vals fields */
    for (ps_pt = 0; 
          (err_no == E_SUCCESS) && (ps_pt < num_ps_axis_vals); ++ps_pt)
    {
      /* Unpack PilotStrengthAxis  */
      err_no = HDRBIT_UNPACK8( 
                item_ptr, 
                &(txt2p_max_ptr->ps_axis[ps_pt]), 
                (p_aip->attrib_offset + attrib_offset), 
                HDRSCMRMACCMN_TX_T2P_PS_AXIS_SIZE );

      /* If success unpacking PilotStrengthAxis field */
      if (err_no == E_SUCCESS)
       {
        /* Shift the offset to start of  TxT2PmaxPilotStrengthAxis field */
        attrib_offset += HDRSCMRMACCMN_TX_T2P_PS_AXIS_SIZE;

        /* Unpack TxT2PmaxPilotStrengthAxis */
        err_no = HDRBIT_UNPACK8( 
                  item_ptr, 
                  &(txt2p_max_ptr->txt2p_max_ps_axis[ps_pt]), 
                  (p_aip->attrib_offset + attrib_offset), 
                  HDRSCMRMACCMN_TX_T2P_MAX_PS_AXIS_SIZE );

        /* Shift the offset to start of  next PilotStrengthAxis field */
        attrib_offset += HDRSCMRMACCMN_TX_T2P_MAX_PS_AXIS_SIZE;
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
          hdrscmrmaccmn_is_tx_t2p_max_valid(txt2p_max_ptr)) != E_OUT_OF_RANGE )
    {
      /* Attrib valid! Yea! :-) print F3 mesg */
      hdrscmrmaccmn_dump_tx_t2p_max(txt2p_max_ptr);

      /* Update the accepted attribute information */
      a_aip->is_accepted = TRUE;
      a_aip->value_id = value_id;
      a_aip->complex_val_ptr = (void*) txt2p_max_ptr;
      a_aip->complex_val_size = sizeof(hdrscmrmaccmn_txt2p_max_grid_struct_type);
      break;
    }

    /* Not much luck :-( Let's try next set of values if there is one. 
       Shift the offset to start of next value ID by rounding-up to 
       next octet. */
    HDRSCMRMACCMN_ALIGN_TO_NEXT_OCTET(attrib_offset);

  } /* while */

  return err_no;

} /* hdrscmrmaccmn_process_tx_t2p_max__config_xx */

