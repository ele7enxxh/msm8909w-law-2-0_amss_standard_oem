/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

      H D R   S E S S I O N   C O N F I G U R A T I O N   M A N A G E R

          S U B T Y P E - 0  A N D  S U B T Y P E - 1  R E V E R S E  

                    C H A N N E L  M A C  P R O T O C O L

       
GENERAL DESCRIPTION
  This module provides supporting API for SCM module to process
  ConfigurationRequest messages and AttributeUpdateRequest messages directed
  to Subtype-0 and Subtype-1 Reverse Traffic channel MAC protocol. 
    
EXTERNALIZED FUNCTIONS
  hdrscmrmac0_process_complex_attrib 
    Processes all FMAC complex attributes 
    
  hdrscmrmac0_is_attrib_simple 
    Returns true if an attribute is simple

  hdrscmrmac0_is_simple_attrib_valid
    Verifies the validity of the simple attribute

  hdrscmrmac0_get_attrib_offset_and_size
    Returns attribute size and offset into the config structure.

  hdrscmrmac0_get_attrib_verno_index
    Returns the index of an attribute in the version number array. 
   
INITIALIZATION AND SEQUENCING REQUIREMENTS

  
  Copyright (c) 2005 - 2013 by Qualcomm Technologies, Inc.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                           EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/hdr/cp/src/hdrscmrmac0.c#1 $ $DateTime: 2016/12/13 07:58:05 $ $Author: mplcsds1 $
   
when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/05/13   cnx     Replace NV reading errors with MSG_LEGACY_HIGH.
06/21/10   cnx     Fixed compiler warnings.
09/28/09   etv     Fixed Klocwork errors.
09/21/07   wsh     Fixed RMAC1 rejecting AN GAUPing RateParameter 
08/22/07   etv     Fixed high lint warnings.
01/03/07   etv     Added support to handle unknown attributes.
07/11/06   etv     Fixed bug by passing dsm_item_typ** instead of dsm_item_typ*
04/18/06   etv     Enhanced SCM to support indefinite num of attribs in CReq.
04/07/06   hal     Changed hdrutil_new_buffer() to use hdrutil_new_msg_buffer()
12/13/05   pba     Modified SCMDB calls to use updated API's
11/27/05   etv     Updated for scm_reserve_timer API changes.
11/21/05   etv     Added header tags for perforce to fill in
11/02/05   etv     Changed SCM design to propogate subtype info.
10/26/05   dna     Integration updates to GAUP support for RMAC1 rate params
10/20/05   etv     Changed union of complex attribs to separate instance.
08/22/05   etv     Created module.

===========================================================================*/

/*===========================================================================

                          INCLUDE FILES FOR MODULE
                     
===========================================================================*/

#include "dsm.h"
#include "comdef.h"
#include "err.h"
#include "nv.h"

#include "hdrdebug.h"
#include "hdrdsm.h"
#include "hdrscp.h"
#include "hdrhai.h"
#include "hdrtrace.h"
#include "hdrbit.h"
#include "hdrutil.h"
#include "hdrhmp.h"

#include "hdrscmrmac0.h"
#include "hdrscm.h"
#include "hdrscmdb.h"


/*===========================================================================

             DEFINITIONS  FROM  THE  HDR  AIR  INTERFACE  SPEC

===========================================================================*/

/*---------------------------------------------------------------------------
  Sizes of the complex attributes and its fields in bits 
----------------------------------------------------------------------------*/
#define HDRSCMRMAC0_ATTRIB_UPDATE_REQ_MSG_ID          0x52
#define HDRSCMRMAC0_ATTRIB_UPDATE_REQ_MSG_PRIO        40
#define HDRSCMRMAC0_MESSAGE_ID_SIZE                   8
#define HDRSCMRMAC0_TRANS_ID_SIZE                     8

#define HDRSCMRMAC0_AR_LENGTH_SIZE                    8
#define HDRSCMRMAC0_AR_ATTRIB_ID_SIZE                 8
#define HDRSCMRMAC0_AR_VALUE_ID_SIZE                  8

/* PowerParameters Attribute */
#define HDRSCMRMAC0_PP_DATA_OFFSET_RATE_SIZE          4
#define HDRSCMRMAC0_PP_RPC_STEP_SIZE                  2
#define HDRSCMRMAC0_PP_RSVD_SIZE                      6
#define HDRSCMRMAC0_POWER_PARAMS_SIZE                 32

/* RateParameters Attribute */
#define HDRSCMRMAC0_RP_TRANSITIONS_SIZE               8
#define HDRSCMRMAC0_RATE_PARAMS_SIZE                  64
#define HDRSCMRMAC0_RATE_PARAMS_ATTRIB_ID             0x01
#define HDRSCMRMAC0_RATE_PARAMS_ATTRIB_SIZE           10

/*---------------------------------------------------------------------------
  Valid Range definitions for each of the complex attribute fields
----------------------------------------------------------------------------*/
#define HDRSCMRMAC0_PP_RPC_STEP_HALF_DB               0x00
#define HDRSCMRMAC0_PP_RPC_STEP_ONE_DB                0x01


/*--------------------------------------------------------------------------
  Default Values for each of the ST-0/1 Configuration attributes.
----------------------------------------------------------------------------*/
/* Power Parameter default configuration values. */
#define HDRSCMRMAC0_DEFAULT_DATA_OFFSET_NOM     0  /* 0.50 dB increments */
#define HDRSCMRMAC0_DEFAULT_DATA_OFFSET_9K6     0  /* 0.25 dB increments */
#define HDRSCMRMAC0_DEFAULT_DATA_OFFSET_19K2    0  /* 0.25 dB increments */
#define HDRSCMRMAC0_DEFAULT_DATA_OFFSET_38K4    0  /* 0.25 dB increments */
#define HDRSCMRMAC0_DEFAULT_DATA_OFFSET_76K8    0  /* 0.25 dB increments */
#define HDRSCMRMAC0_DEFAULT_DATA_OFFSET_153K6   0  /* 0.25 dB increments */
#define HDRSCMRMAC0_DEFAULT_RPC_STEP            1  /* 0.50 dB inc. + 0.50 dB */

/* Rate Parameter default configuration values. */
#define HDRSCMRMAC0_DEFAULT_TRANSITION_12       0x30
#define HDRSCMRMAC0_DEFAULT_TRANSITION_23       0x10
#define HDRSCMRMAC0_DEFAULT_TRANSITION_34       0x08
#define HDRSCMRMAC0_DEFAULT_TRANSITION_45       0x08
#define HDRSCMRMAC0_DEFAULT_TRANSITION_21       0x10
#define HDRSCMRMAC0_DEFAULT_TRANSITION_32       0x10
#define HDRSCMRMAC0_DEFAULT_TRANSITION_43       0x20
#define HDRSCMRMAC0_DEFAULT_TRANSITION_54       0xFF

/* Rate Parameter Video Telephony Configuration Values */
#define HDRSCMRMAC0_RP_VT_TRANSITION_12         0xFF
#define HDRSCMRMAC0_RP_VT_TRANSITION_23         0xFF
#define HDRSCMRMAC0_RP_VT_TRANSITION_34         0xFF
#define HDRSCMRMAC0_RP_VT_TRANSITION_45         0x00
#define HDRSCMRMAC0_RP_VT_TRANSITION_21         0x00
#define HDRSCMRMAC0_RP_VT_TRANSITION_32         0x01
#define HDRSCMRMAC0_RP_VT_TRANSITION_43         0x08
#define HDRSCMRMAC0_RP_VT_TRANSITION_54         0xFF


 /* EJECT */
/*===========================================================================
 
                  DEFINITIONS AND DECLARATIONS FOR MODULE

  This section contains local definitions for constants, macros, types,
  variables and other items needed by this module.

===========================================================================*/

/* This struct encapsulates an InConfig AttribUpdateRequest Transaction. */
typedef struct
{
  uint8 trans_id;
    /* Transaction ID of this Attrib Update request */

  hdrscmrmac0_rate_params_struct_type *rp;
    /* Pointer to the InConfig instance of RateParams. SCMRMAC0 is not
       notified when a GAUP update is either accepted/Rejected. So this
       doesn't necessarily point to current instance of rate params. 
       Although it could after a while if the GAUP update is accepted. */

  hdrscmrmac0_rate_params_mode_enum_type rp_mode;
    /* Rate Params Mode of the current transaction */

} hdrscmrmac0_rp_inconfig_trans_struct_type;

/* A max of only outstanding AttribUpdateRequest transactions are allowed at
   any instance. */
#define HDRSCMRMAC0_NUM_CONCURRENT_GAUPS              2

/*------------------------------------------------------------------------
                            GLOBAL VARIABLES
 -----------------------------------------------------------------------*/

/* Be sure to add new variables after rp_normal. If you add them before 
   rp_normal, please make sure you make appropriate changes to initialization
   order */
static struct
{
  uint8 gaup_trans_id;
    /* Transaction ID of the next GAUP request msg to send*/

  hdrscmrmac0_rate_params_struct_type rp_normal;
    /* Normal Rate Parameters. When the Packet App requests SCMRMAC0 to propose 
       GAUP update for VT rate params, the existing configuration of rate params
       needs to be remembered to be able to revert back when the packet APP 
       requests to stop high proirity traffic mode. This variable is used 
       for that purpose. Initialized to default values */

  hdrscmrmac0_rate_params_struct_type rp_vt;
    /* Video Telephony Rate Parameters. Just before the first GAUP request, 
       the custom VT Rate params is loaded from NV. If the NV read fails, 
       it is set to default VT configuration */

  hdrscmrmac0_rp_inconfig_trans_struct_type in_config
                                              [HDRSCMRMAC0_NUM_CONCURRENT_GAUPS];
    /* Array of InConfig Transactions. There could be atmost 
       HDRSCMRMAC0_NUM_CONCURRENT_GAUPS outstanding transactions. This array is 
       used as a circular buffer and is indexed using 
       gaup_trans_id mod HDRSCMRMAC0_NUM_CONCURRENT_GAUPS */

  hdrscmrmac0_power_params_struct_type power_params;
    /* PowerParameters attribute */

  hdrscmrmac0_rate_params_struct_type rate_params;
    /* RateParameters attribute */


  rex_crit_sect_type rp_crit_sect; 
    /* Critical Section variable for RateParams GAUP mechanism */

} hdrscmrmac0;


 /* EJECT */
/*------------------------------------------------------------------------
                              LOOKUP TABLES
 -----------------------------------------------------------------------*/


/*===========================================================================

                         LOCAL FUNCTION DEFINITIONS
                    
===========================================================================*/

/*===========================================================================

FUNCTION HDRSCMRMAC0_IS_POWER_PARAMS_VALID                     LOCAL

DESCRIPTION
  Check if the PowerParameters attribute is valid 

  0x00 - PowerParameters attribute

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  E_SUCCESS       - If the PowerParameters Attribute is valid.
  E_OUT_OF_RANGE  - otherwise. 

SIDE EFFECTS
  None.

===========================================================================*/
static hdrerrno_enum_type hdrscmrmac0_is_power_params_valid(void)
{

  /* All valid 4-bit 2's complement number are valid for DataOffset<rate> 
     fields. Checking the validity of RPCStep only... */
  return ( (hdrscmrmac0.power_params.rpc_step == 
             HDRSCMRMAC0_PP_RPC_STEP_HALF_DB) ||
           (hdrscmrmac0.power_params.rpc_step == 
             HDRSCMRMAC0_PP_RPC_STEP_ONE_DB) 
         )? E_SUCCESS: E_OUT_OF_RANGE;

} /* hdrscmrmac0_is_power_params_valid */


 /* EJECT */
/*===========================================================================

FUNCTION HDRSCMRMAC0_PROCESS_POWER_PARAMETERS                         LOCAL

DESCRIPTION
  This is a generic interface to process the PowerParameters Attribute.

  0x00 - PowerParameters

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
static hdrerrno_enum_type hdrscmrmac0_process_power_parameters
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

  uint8 data_rate;
    /* Data Rate used as an index in the DataOffset Array */

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */ 

  /* Initialize the attribute offset to start of first Value ID */
  attrib_offset = p_aip->attrib_len_size + a_aip->attrib_id_size;

  /* While there are enough bits left in the Attribute Record for one more
   * set of AuxiliaryPilotChannelParameters complex Attribute Value */
  while ( (attrib_offset + p_aip->value_id_size + 
           HDRSCMRMAC0_POWER_PARAMS_SIZE) <= p_aip->attrib_len )
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
      ERR("Error unpacking Value ID of PowerParameters Attrib",
            0,0,0);
      break;
    }

    /*----------------------------------------------------------------------
                                DataOffset<Rate> Fields
     *--------------------------------------------------------------------*/

    /* Since Value Id was parsed sucessfully, Update the attribute offset to 
     * start of DataOffsetNom field */ 
    attrib_offset += p_aip->value_id_size;

    for (data_rate=0; (err_no == E_SUCCESS) && 
          (data_rate < HDRSCMRMAC0_NUM_DATA_OFFSET_RATES); ++data_rate)
    {
      /* Unpack DataOffset<rate> */
      err_no = HDRBIT_UNPACK8( 
                item_ptr, 
                &(hdrscmrmac0.power_params.data_offset_rate[data_rate]),
                (p_aip->attrib_offset + attrib_offset), 
                HDRSCMRMAC0_PP_DATA_OFFSET_RATE_SIZE );

      attrib_offset += HDRSCMRMAC0_PP_DATA_OFFSET_RATE_SIZE;
    }

    /* If error unpacking DataOffset<rate> field, break */
    if (err_no != E_SUCCESS)
    {
      ERR("Error unpacking DataOffset at %d", (data_rate-1),0,0);
      break;
    }

    /*----------------------------------------------------------------------
                                    RPCStep 
     *--------------------------------------------------------------------*/

    /* Unpack RPCStep */
    err_no = HDRBIT_UNPACK8(item_ptr,
                            &(hdrscmrmac0.power_params.rpc_step),
                            (p_aip->attrib_offset + attrib_offset),
                            HDRSCMRMAC0_PP_RPC_STEP_SIZE);

    /* If error unpacking RPCStep field, break */
    if (err_no != E_SUCCESS)
    {
      ERR("Error unpacking RPCStep", 0,0,0);
      break;
    }

    /* Parsing done! :-) Is the parsed attribute valid? */
    if ( (err_no = 
          hdrscmrmac0_is_power_params_valid()) != E_OUT_OF_RANGE )
    {
      /* Attrib valid! Yea! :-) print F3 mesg */
      HDR_MSG_PROT_7(MSG_LEGACY_HIGH,
                     "PowerParams -> DataOffset: %d %d %d %d %d %d RPCStep: %d",
                     hdrscmrmac0.power_params.data_offset_rate[0],
                     hdrscmrmac0.power_params.data_offset_rate[1],
                     hdrscmrmac0.power_params.data_offset_rate[2],
                     hdrscmrmac0.power_params.data_offset_rate[3],
                     hdrscmrmac0.power_params.data_offset_rate[4],
                     hdrscmrmac0.power_params.data_offset_rate[5],
                     hdrscmrmac0.power_params.rpc_step);

      /* Update the accepted attribute information */
      a_aip->is_accepted = TRUE;
      a_aip->value_id = value_id;
      a_aip->complex_val_ptr = (void*) &(hdrscmrmac0.power_params);
      a_aip->complex_val_size = sizeof(hdrscmrmac0_power_params_struct_type);
      break;
    }

    /* Not much luck :-( Let's try next set of values if there is one. 
       Shift the offset to start of next value ID. */
    attrib_offset += (HDRSCMRMAC0_PP_RPC_STEP_SIZE + HDRSCMRMAC0_PP_RSVD_SIZE); 

  } /* while */

  return err_no;

} /* hdrscmrmac0_process_power_parameters */

/*===========================================================================

FUNCTION HDRSCMRMAC0_IS_RATE_PARAMS_VALID                     LOCAL

DESCRIPTION
  Check if the RateParameters attribute is valid 

  0x01 - RateParameters attribute

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  E_SUCCESS       - If the PowerParameters Attribute is valid.
  E_OUT_OF_RANGE  - otherwise. 

SIDE EFFECTS
  None.

===========================================================================*/
static hdrerrno_enum_type hdrscmrmac0_is_rate_params_valid(void)
{

  /* All valid 8-bit numbers are valid for Transition fields. */
  return E_SUCCESS;

} /* hdrscmrmac0_is_rate_params_valid */

 /* EJECT */
/*===========================================================================

FUNCTION HDRSCMRMAC0_PROCESS_RATE_PARAMETERS                         LOCAL

DESCRIPTION
  This is a generic interface to process the RateParameters Attribute.

  0x01 - RateParameters Attribute 

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
static hdrerrno_enum_type hdrscmrmac0_process_rate_parameters
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

  uint8 trans_ind;
    /* Transitions index */

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */ 

  /* Initialize the attribute offset to start of first Value ID */
  attrib_offset = p_aip->attrib_len_size + a_aip->attrib_id_size;

  /* While there are enough bits left in the Attribute Record for one more
   * set of Rate Parameters complex Attribute Value */
  while ( (attrib_offset + p_aip->value_id_size + 
           HDRSCMRMAC0_RATE_PARAMS_SIZE) <= p_aip->attrib_len )
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
      ERR("Error unpacking Value ID of RateParameters Attrib", 0,0,0);
      break;
    }

    /*----------------------------------------------------------------------
                                  Transitions<x>_<y>
     *--------------------------------------------------------------------*/
    /* Since Value Id was parsed sucessfully, Update the attribute offset to 
     * start of Transition009k6_019k2 field */ 
    attrib_offset += p_aip->value_id_size;

    for (trans_ind=0; (err_no == E_SUCCESS) && 
          (trans_ind < HDRSCMRMAC0_NUM_RATE_PARAM_TRANS); ++trans_ind)
    {
      err_no = HDRBIT_UNPACK8(
                item_ptr, 
                &(hdrscmrmac0.rate_params.transition[trans_ind]), 
                (p_aip->attrib_offset + attrib_offset), 
                HDRSCMRMAC0_RP_TRANSITIONS_SIZE );

      /* Shift the offset to next transition field */
      attrib_offset += HDRSCMRMAC0_RP_TRANSITIONS_SIZE;
    }

    /* If error unpacking transitions, break */
    if (err_no != E_SUCCESS)
    {
      ERR("Error unpacking Transition %d", (trans_ind-1),0,0);
      break;
    }

    /* Parsing done! :-) Is the parsed attribute valid? */
    if ( (err_no = 
          hdrscmrmac0_is_rate_params_valid()) != E_OUT_OF_RANGE )
    {
      /* Attrib valid! Yea! :-) print F3 mesg */
      HDR_MSG_PROT_8(MSG_LEGACY_HIGH, 
                     "RateParameters -> Transitions: %d %d %d %d %d %d %d %d",
                     hdrscmrmac0.rate_params.transition[0],
                     hdrscmrmac0.rate_params.transition[1],
                     hdrscmrmac0.rate_params.transition[2],
                     hdrscmrmac0.rate_params.transition[3],
                     hdrscmrmac0.rate_params.transition[4],
                     hdrscmrmac0.rate_params.transition[5],
                     hdrscmrmac0.rate_params.transition[6],
                     hdrscmrmac0.rate_params.transition[7]);

      /* Update the accepted attribute information */
      a_aip->is_accepted = TRUE;
      a_aip->value_id = value_id;
      a_aip->complex_val_ptr = (void*) &(hdrscmrmac0.rate_params);
      a_aip->complex_val_size = sizeof(hdrscmrmac0_rate_params_struct_type);
      break;
    }

    /* Not much luck :-( Let's try next set of values if there is one. Fall thru
       to next iteration. */

  } /* while */

  return err_no;

} /* hdrscmrmac0_process_rate_parameters */

 /* EJECT */
/*===========================================================================

FUNCTION HDRSCMRMAC0_READ_VT_RP_FROM_NV                                LOCAL

DESCRIPTION
  This function is used to read custom VT Rate parameters from NV.

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void hdrscmrmac0_read_vt_rp_from_nv (void)
{

  nv_hdrrmac_rate_params_type   nv_rate_params; 
    /* NV Rate Params type */

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */ 

  /* Initialization */
  memset(&nv_rate_params, 0, sizeof(nv_hdrrmac_rate_params_type));

  HDRTRACE_INIT_NV( HDRHAI_RTC_MAC_PROTOCOL );

  /*-------------------------------------------------------------------------
                Read RMAC custom VT Rate Parameters from NV 
  -------------------------------------------------------------------------*/
  if( hdrutil_read_nv_item( NV_HDRRMAC_RATE_PARAMS_I, 
                        (nv_item_type*)&nv_rate_params ) != NV_DONE_S )
  {
    HDR_MSG_PROT( MSG_LEGACY_HIGH, 
                  "VT Rate Parameters NV read failed...using default" );

    /* Set VT rate params to default VT configuration */
    hdrscmrmac0.rp_vt.transition[0] = HDRSCMRMAC0_RP_VT_TRANSITION_12;
    hdrscmrmac0.rp_vt.transition[1] = HDRSCMRMAC0_RP_VT_TRANSITION_23;
    hdrscmrmac0.rp_vt.transition[2] = HDRSCMRMAC0_RP_VT_TRANSITION_34;
    hdrscmrmac0.rp_vt.transition[3] = HDRSCMRMAC0_RP_VT_TRANSITION_45;
    hdrscmrmac0.rp_vt.transition[4] = HDRSCMRMAC0_RP_VT_TRANSITION_21;
    hdrscmrmac0.rp_vt.transition[5] = HDRSCMRMAC0_RP_VT_TRANSITION_32;
    hdrscmrmac0.rp_vt.transition[6] = HDRSCMRMAC0_RP_VT_TRANSITION_43;
    hdrscmrmac0.rp_vt.transition[7] = HDRSCMRMAC0_RP_VT_TRANSITION_54;
  }
  else
  {
    /* Copy data into struct individually since NV items are PACKED */
    hdrscmrmac0.rp_vt.transition[0] = nv_rate_params.transition_009k6_019k2;
    hdrscmrmac0.rp_vt.transition[1] = nv_rate_params.transition_019k2_038k4;
    hdrscmrmac0.rp_vt.transition[2] = nv_rate_params.transition_038k4_076k8;
    hdrscmrmac0.rp_vt.transition[3] = nv_rate_params.transition_076k8_153k6;
    hdrscmrmac0.rp_vt.transition[4] = nv_rate_params.transition_019k2_009k6;
    hdrscmrmac0.rp_vt.transition[5] = nv_rate_params.transition_038k4_019k2;
    hdrscmrmac0.rp_vt.transition[6] = nv_rate_params.transition_076k8_038k4;
    hdrscmrmac0.rp_vt.transition[7] = nv_rate_params.transition_153k6_076k8;
  }

} /* hdrscmrmac0_read_vt_rp_from_nv */


 /* EJECT */
/*============================================================================

                        EXTERNAL FUNCTION DEFINITIONS

=============================================================================*/

/*===========================================================================

FUNCTION HDRSCMRMAC0_PROCESS_COMPLEX_ATTRIB

DESCRIPTION
  This is a generic interface to process the configuration request messages of
  all complex attributes of Subtype-0 and Subtype-1 Reverse channel MAC 
  protocol.

DEPENDENCIES
  None.

PARAMETERS
  item_ptr - pointer to the DSM item holding the message to be parsed.
  p_aip    - proposed attribute information pointer.
  a_aip    - accepted attribute information pointer.

RETURN VALUE
  E_SUCCESS      - If a valid complex attribute was parsed successfully.
  E_NO_DATA      - If the config request message does not have enough data.
  E_OUT_OF_RANGE - If the config request message does not have any valid data.
  E_DATA_INVALID - If the given attrib_id is not valid.

SIDE EFFECTS
  None.

===========================================================================*/
hdrerrno_enum_type hdrscmrmac0_process_complex_attrib
(
  dsm_item_type                               *item_ptr,
  hdrscm_proposed_attrib_type   const *const   p_aip,
  hdrscm_accepted_attrib_type         *const   a_aip
)
{

  hdrerrno_enum_type err_no = E_DATA_INVALID;
    /* Error Code */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  switch (p_aip->attrib_id)
  {
    /* PowerParameters Attribute */
    case HDRSCMRMAC0_ATTRIB_POWER_PARAMS:
      err_no = hdrscmrmac0_process_power_parameters(item_ptr,
                                                    p_aip,
                                                    a_aip);
      break;

    /* RateParameters Attribute */
    case HDRSCMRMAC0_ATTRIB_RATE_PARAMS:
      err_no = hdrscmrmac0_process_rate_parameters(item_ptr,
                                                   p_aip,
                                                   a_aip);
      break;

    /* Unrecognized attribute ID */
    default:
      HDR_MSG_PROT_1(MSG_LEGACY_ERROR, 
                     "Unrecognized attribute ID %d",
                     p_aip->attrib_id);
  }

  return err_no;
  
} /* hdrscmrmac0_process_complex_attrib */


 /* EJECT */
/*===========================================================================

FUNCTION HDRSCMRMAC0_IS_ATTRIB_SIMPLE

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
boolean hdrscmrmac0_is_attrib_simple
(
  uint16 const attrib_id
)
{

  /* There are no simple attributes in subtype-0 and subtype-1 RMAC */
  return FALSE;

} /* hdrscmrmac0_is_attrib_simple */


 /* EJECT */
/*===========================================================================

FUNCTION HDRSCMRMAC0_IS_SIMPLE_ATTRIB_VALID

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
hdrerrno_enum_type hdrscmrmac0_is_simple_attrib_valid
(
  uint16 const attrib_id,
  uint32 const attrib_value 
)
{

  /* There are no simple attributes in ST-0 and ST-1 RMAC */
  return E_DATA_INVALID;

} /* hdrscmrmac0_is_simple_attrib_valid */


 /* EJECT */
/*===========================================================================

FUNCTION HDRSCMRMAC0_GET_ATTRIB_OFFSET_AND_SIZE

DESCRIPTION
  This function computes the byte offset of the attribute specified by the 
  attrib_id in the struct hdrscmrmac0_config_struct_type and stores it in 
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
hdrerrno_enum_type hdrscmrmac0_get_attrib_offset_and_size
(
  uint16 attrib_id, 
  uint16 *attrib_offset,
  uint16 *attrib_size
)
{

  hdrerrno_enum_type err_no = E_SUCCESS;
    /* Error Code */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  switch (attrib_id)
  {
    /* PowerParameters */
    case HDRSCMRMAC0_ATTRIB_POWER_PARAMS:

      *attrib_offset = FPOS(hdrscmrmac0_config_struct_type, power_params);
      *attrib_size = sizeof(hdrscmrmac0_power_params_struct_type);
      break;

    /* RateParameters */
    case HDRSCMRMAC0_ATTRIB_RATE_PARAMS:

      *attrib_offset = FPOS(hdrscmrmac0_config_struct_type, rate_params);
      *attrib_size = sizeof(hdrscmrmac0_rate_params_struct_type);
      break;

    /* Unrecognized attribute ID */
    default:
      HDR_MSG_PROT_1(MSG_LEGACY_ERROR, 
                     "Unrecognized attribute ID %d",
                     attrib_id);

      err_no = E_DATA_INVALID;
  }

  return err_no;

} /* hdrscmrmac0_get_attrib_offset_and_size */


 /* EJECT */
/*===========================================================================

FUNCTION HDRSCMRMAC0_GET_ATTRIB_VERNO_INDEX

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
hdrerrno_enum_type hdrscmrmac0_get_attrib_verno_index
(
  uint16 attrib_id,
  uint16 *pindex
)
{

  hdrerrno_enum_type err_no = E_SUCCESS;
    /* Error Code */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If the attribute ID is either one of the two ST-0/1 RMAC attributes... */
  if ( (attrib_id == HDRSCMRMAC0_ATTRIB_POWER_PARAMS) ||
       (attrib_id == HDRSCMRMAC0_ATTRIB_RATE_PARAMS) )
  {
    /* The attribute ID maps to a linear index */
    *pindex = attrib_id;
  }
  /* Unrecognized attribute ID */
  else
  {
    err_no = E_DATA_INVALID;
  }

  return err_no;

} /* hdrscmrmac0_get_attrib_verno_index */


 /* EJECT */
/*===========================================================================

FUNCTION HDRSCMRMAC0_SET_CONFIG_TO_DEFAULT

DESCRIPTION
  Set the ST-0/1 RMAC configuration attributes to its default values.

DEPENDENCIES
  None

PARAMETERS
  p_config - Pointer to hdrscmrmac0_config_struct_type
  
RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
void hdrscmrmac0_set_config_to_default
(
  hdrscmrmac0_config_struct_type *p_config
)
{
  p_config->power_params.data_offset_rate[0] = 
    HDRSCMRMAC0_DEFAULT_DATA_OFFSET_NOM;
  p_config->power_params.data_offset_rate[1] =
    HDRSCMRMAC0_DEFAULT_DATA_OFFSET_9K6;
  p_config->power_params.data_offset_rate[2] =
    HDRSCMRMAC0_DEFAULT_DATA_OFFSET_19K2;
  p_config->power_params.data_offset_rate[3] =
    HDRSCMRMAC0_DEFAULT_DATA_OFFSET_38K4;
  p_config->power_params.data_offset_rate[4] =
    HDRSCMRMAC0_DEFAULT_DATA_OFFSET_76K8;
  p_config->power_params.data_offset_rate[5] =
    HDRSCMRMAC0_DEFAULT_DATA_OFFSET_153K6;

  p_config->power_params.rpc_step = HDRSCMRMAC0_DEFAULT_RPC_STEP;

  p_config->rate_params.transition[0] = HDRSCMRMAC0_DEFAULT_TRANSITION_12;
  p_config->rate_params.transition[1] = HDRSCMRMAC0_DEFAULT_TRANSITION_23;
  p_config->rate_params.transition[2] = HDRSCMRMAC0_DEFAULT_TRANSITION_34;
  p_config->rate_params.transition[3] = HDRSCMRMAC0_DEFAULT_TRANSITION_45;
  p_config->rate_params.transition[4] = HDRSCMRMAC0_DEFAULT_TRANSITION_21;
  p_config->rate_params.transition[5] = HDRSCMRMAC0_DEFAULT_TRANSITION_32;
  p_config->rate_params.transition[6] = HDRSCMRMAC0_DEFAULT_TRANSITION_43;
  p_config->rate_params.transition[7] = HDRSCMRMAC0_DEFAULT_TRANSITION_54;

} /* hdrscmrmac0_set_config_to_default */


 /* EJECT */
/*===========================================================================

FUNCTION HDRSCMRMAC0_PROP_GAUP_RATE_PARAMS

DESCRIPTION
  This routine is used by ST-1 RMAC to send GAUP update request for Rate 
  Parameters attribute.

DEPENDENCIES
  Since SCMRMAC0 is not notified whether the GAUP update is accepted or not, it
  doesn't know the current Mode of RateParameters. It is the responsibility of
  the PacketApp to check for the following scenarios. 

  The 'IV' is an invalid scenario, 'U' is a useless scenario and 'V's are valid 
  scenarios.

                      Requested RP Mode
                     
                         | Normal |   VT  |
                  -----------------------------
       Current    Normal |   U    |   V   |
         RP       -----------------------------
        Mode        VT   |   V    |   IV  |


  The PacketApp should also make sure that this function is not called while it 
  is waiting on two AttribUpdateRequests already. The effect of calling this 
  function again could be an Error Fatal owing to insufficient Response timers.

ASSUMPTIONS
  When a VT mode is requested, the current mode could only be Normal. If this
  assumption is broken, the current configuration saved in rp_normal would be 
  lost and the configuration would stay VT until session close!

PARAMETERS
  rp_mode - Rate Parameters Mode. Could only be one of the following two values
            HDRSCMRMAC0_RATE_PARAMS_VT_MODE
            HDRSCMRMAC0_RATE_PARAMS_NORMAL_MODE
  p_trans_id - Address where the transaction ID is stored to be returned back.
  
RETURN VALUE
  E_SUCCESS - If a GAUP Update was sent successfully.
  E_NOT_AVAILABLE - If we didn't have a free Rsp timer to send this GAUP update.

SIDE EFFECTS
  Error Fatal if there aren't enough response timers to proceed.

===========================================================================*/
hdrerrno_enum_type hdrscmrmac0_prop_gaup_rate_params
(
  hdrscmrmac0_rate_params_mode_enum_type rp_mode,
  uint8 *p_trans_id
)
{

  dsm_item_type * send_msg_ptr = NULL;
    /* Pointer to gaup req. message to send */

  uint8 attrib_offset = 0;
    /* Attribute field offset from start of DSM item */

  uint8 timer_id;
    /* ID of the reserved timer */

  uint8 value_id = 0;
    /* Value Id of complex attrib Rate Param */

  uint8 trans_ind = 0;
    /* Rate Param Transition Index */

  uint8 gaup_ind = 0;
    /* Index into Inconfig GAUP transaction Array */

  static boolean read_nv = TRUE;
    /* Read custom VT Rate parameters from NV only once during powerup */

  hdrerrno_enum_type err_no = E_SUCCESS;
    /* Error Code */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Reserve a Response timer for this GAUP update */
  if ( (err_no = hdrscm_reserve_rsp_timer(HDRHAI_RTC_MAC_PROTOCOL, &timer_id, 
                           hdrscmrmac0.gaup_trans_id, TRUE)) != E_SUCCESS)
  {
    /* No free timer was available. Forward the error code to Apps! */
    return err_no;
  }

  /* If this is the first call to this function, */
  if (read_nv)
  {
    /* Initialize transction ID */
    hdrscmrmac0.gaup_trans_id = 0;

    /* Initialize the Normal Rate Parameters */
    hdrscmrmac0.rp_normal.transition[0] = HDRSCMRMAC0_DEFAULT_TRANSITION_12;
    hdrscmrmac0.rp_normal.transition[1] = HDRSCMRMAC0_DEFAULT_TRANSITION_23;
    hdrscmrmac0.rp_normal.transition[2] = HDRSCMRMAC0_DEFAULT_TRANSITION_34;
    hdrscmrmac0.rp_normal.transition[3] = HDRSCMRMAC0_DEFAULT_TRANSITION_45;
    hdrscmrmac0.rp_normal.transition[4] = HDRSCMRMAC0_DEFAULT_TRANSITION_21;
    hdrscmrmac0.rp_normal.transition[5] = HDRSCMRMAC0_DEFAULT_TRANSITION_32;
    hdrscmrmac0.rp_normal.transition[6] = HDRSCMRMAC0_DEFAULT_TRANSITION_43;
    hdrscmrmac0.rp_normal.transition[7] = HDRSCMRMAC0_DEFAULT_TRANSITION_54;

    /* Initialize the critical section variable first time thru this func */
    rex_init_crit_sect(&hdrscmrmac0.rp_crit_sect);

    /* Turn off the flag to make sure we don't read NV or Initialize critical 
       section for subsequent calls */
    read_nv = FALSE;

  } /* read_nv */

  /* Entering critical section... */
  rex_enter_crit_sect(&hdrscmrmac0.rp_crit_sect);

  /* Index of current Transaction in the in_config array */
  gaup_ind = hdrscmrmac0.gaup_trans_id % HDRSCMRMAC0_NUM_CONCURRENT_GAUPS;

  /* Save the transaction ID chosen for this transaction */
  hdrscmrmac0.in_config[gaup_ind].trans_id = hdrscmrmac0.gaup_trans_id;

  /* VT Mode */
  if (rp_mode == HDRSCMRMAC0_RATE_PARAMS_VT_MODE)
  {
    /* Save the current instance of Rate params in order to be able to revert
       back to current configuration from VT Mode. It has been assumed that 
       if the requested RP Mode is VT, the current mode could only be Normal. 
       If this assumption is broken, be warned that the current configuration
       saved in rp_normal would be lost and the configuration would stay VT 
       until session close! */
    (void) hdrscmdb_get_inuse_attrib(HDRHAI_RTC_MAC_PROTOCOL, 
                                     HDRSCMRMAC0_RATE_PARAMS_ATTRIB_ID, 
                                     &hdrscmrmac0.rp_normal, 
                                     HDRSCP_SUB1_RMAC);

    /* The inconfig instance is Video telephony */
    hdrscmrmac0.in_config[gaup_ind].rp = &hdrscmrmac0.rp_vt;

    /* RP Mode in config is Video telephony */
    hdrscmrmac0.in_config[gaup_ind].rp_mode = HDRSCMRMAC0_RATE_PARAMS_VT_MODE;
  }
  else
  /* Normal Mode */
  {
    /* When Normal mode is requested, revert back to our saved configuration */
    hdrscmrmac0.in_config[gaup_ind].rp = &hdrscmrmac0.rp_normal;

    /* Rate Parameters Mode in Config is Normal */
    hdrscmrmac0.in_config[gaup_ind].rp_mode = 
      HDRSCMRMAC0_RATE_PARAMS_NORMAL_MODE;
  }

  /*------------------------------------------------------------------------
    Now that we have chosen the rate parameters attribute value to GAUP 
    based on the given RP mode, proceed with packing the AttribUpdateReq msg.
   ------------------------------------------------------------------------*/

  /* Select the transaction ID for current transaction */
  *p_trans_id = hdrscmrmac0.gaup_trans_id;

  /* Increment the transaction ID for next transaction */
  hdrscmrmac0.gaup_trans_id++;

  /* Leave critical section... */
  rex_leave_crit_sect(&hdrscmrmac0.rp_crit_sect);

  /* Get a new DSM buffer */
  send_msg_ptr = hdrutil_new_msg_buffer();

  /* Pack AttribUpdateRequest message's Message ID */
  HDRBIT_PACK8(send_msg_ptr, HDRSCMRMAC0_ATTRIB_UPDATE_REQ_MSG_ID,
               attrib_offset, HDRSCMRMAC0_MESSAGE_ID_SIZE );

  /* Shift the offset to start of Transaction ID */
  attrib_offset += HDRSCMRMAC0_MESSAGE_ID_SIZE;

  /* Pack the message's Transaction ID */
  HDRBIT_PACK8(send_msg_ptr, *(p_trans_id), attrib_offset, 
               HDRSCMRMAC0_TRANS_ID_SIZE);

  /* Shift the offset to start of Attribute Record's Length */
  attrib_offset += HDRSCMRMAC0_TRANS_ID_SIZE;

  /* Pack the Length field of the attribute Record */
  HDRBIT_PACK8(send_msg_ptr, HDRSCMRMAC0_RATE_PARAMS_ATTRIB_SIZE,
               attrib_offset, HDRSCMRMAC0_AR_LENGTH_SIZE);

  /* Shift the offset to start of Attribute ID field */
  attrib_offset += HDRSCMRMAC0_AR_LENGTH_SIZE;

  /* Pack the Attribute ID of the rate parameters */
  HDRBIT_PACK8(send_msg_ptr, HDRSCMRMAC0_RATE_PARAMS_ATTRIB_ID, attrib_offset,
               HDRSCMRMAC0_AR_ATTRIB_ID_SIZE);

  /* Shift the offset to start of Value ID */
  attrib_offset += HDRSCMRMAC0_AR_ATTRIB_ID_SIZE;

  /* Pack the value ID */
  HDRBIT_PACK8(send_msg_ptr, value_id, attrib_offset, 
               HDRSCMRMAC0_AR_VALUE_ID_SIZE);

  /* Shift the offset to start of first transition */
  attrib_offset += HDRSCMRMAC0_AR_VALUE_ID_SIZE;

  /* Loop around and Pack all eight transitions */
  for (trans_ind = 0; trans_ind < HDRSCMRMAC0_NUM_RATE_PARAM_TRANS; ++trans_ind)
  {
    /* Pack Transition<x_y> */
    HDRBIT_PACK8(send_msg_ptr, 
                 hdrscmrmac0.in_config[gaup_ind].rp->transition[trans_ind],
                 attrib_offset, 
                 HDRSCMRMAC0_RP_TRANSITIONS_SIZE);

    /* Shift the offset the next transition */
    attrib_offset += HDRSCMRMAC0_RP_TRANSITIONS_SIZE;
  }

  /* Send AttribUpdateRequest msg on RTC. All config messages are reliable. */
  hdrhmp_send_msg(HDRHAI_RTC_MAC_PROTOCOL,
                  HDRHAI_RTC,
                  TRUE,
                  HDRSCMRMAC0_ATTRIB_UPDATE_REQ_MSG_PRIO,
                  HDRHAI_IN_USE,
                  send_msg_ptr,
                  hdrscm_tx_outcome_cb,
                  (void*) ((dword) timer_id) );

  return err_no;

} /* hdrscmrmac0_prop_gaup_rate_params */


 /* EJECT */
/*===========================================================================

FUNCTION HDRSCMRMAC0_GET_GAUPED_COMPLEX_ATTRIB_VALUE

DESCRIPTION
  This function is used by SCM to get the complex attribute value proposed by
  AT as GAUP update.

DEPENDENCIES
  None.

PARAMETERS
  trans_id      - Transaction ID.
  a_aip         - Array of attribute information pointers where the info about
                  each of the attributes in the transaction specified by 
                  trans_id are stored.
  attrib_count  - Number of attributes GAUPed in this transaction.
 
RETURN VALUE
  E_SUCCESS       - If the given transaction ID is valid.
  E_NOT_AVAILABLE - If a valid transaction specified by 'trans_id' is not 
                    available.

SIDE EFFECTS
  None.

===========================================================================*/
hdrerrno_enum_type hdrscmrmac0_get_gauped_complex_attrib_value
(
  uint8 const trans_id,
  dsm_item_type **aai_dsm,
  uint8 *const attrib_count
)
{

  uint8 gaup_ind = (trans_id % HDRSCMRMAC0_NUM_CONCURRENT_GAUPS);
    /* Index into InConfig array */

  hdrerrno_enum_type err_no = E_NOT_AVAILABLE;
    /* Error Code. A valid transaction not available! */

  hdrscm_accepted_attrib_type gauped_attribs;
    /* Accepted Attribute information type for the GAUPed attribute */

  hdrscm_accepted_attrib_type *a_aip = &gauped_attribs;
    /* Pointer to the GAUPed attribute */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* RateParams is the only attribute that could be GAUPed by RMAC ST-1 AT. 
     Verifying if we have a valid transaction identified by 'trans_id' */
  if (hdrscmrmac0.in_config[gaup_ind].trans_id == trans_id)
  {
    /* Found a valid transaction; Set error code to success */
    err_no = E_SUCCESS;

    /* Populate the attribute information pointer array's first index with the
       InConfig instance of Rate Parameters attribute GAUPed in this 
       transaction */
    a_aip->complex_val_ptr = hdrscmrmac0.in_config[gaup_ind].rp;
    a_aip->attrib_id = HDRSCMRMAC0_RATE_PARAMS_ATTRIB_ID;
    a_aip->is_accepted = TRUE;
    a_aip->complex_val_size = sizeof(hdrscmrmac0_rate_params_struct_type);

    /* Since Rate params is the only GAUPable attribute, */
    *attrib_count = 1;

    (void) dsm_pushdown(aai_dsm, &gauped_attribs, 
                        sizeof(hdrscm_accepted_attrib_type), 
                        DSM_DS_SMALL_ITEM_POOL);
  }

  return err_no;

} /* hdrscmrmac0_get_gauped_complex_attrib_value */


/* EJECT */
/*===========================================================================

FUNCTION HDRSCMRMAC0_ATTRIB_IS_SUPPORTED

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
boolean hdrscmrmac0_attrib_is_supported
(
  uint16 const attrib_id
)
{
  boolean known = FALSE;

/*- - - - - - - - - - - - -- - - - - - - - - - - - - - - - - - - - - - - - -*/

  known = ((attrib_id == HDRSCMRMAC0_ATTRIB_POWER_PARAMS) ||
           (attrib_id == HDRSCMRMAC0_ATTRIB_RATE_PARAMS));

  return known;
          
} /* hdrscmrmac0_attrib_is_supported */

/*===========================================================================

FUNCTION HDRSCMRMAC0_IS_ATTRIB_GAUPABLE

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
hdrerrno_enum_type hdrscmrmac0_is_attrib_gaupable
(
  uint16 const attrib_id
)
{
  if (attrib_id == HDRSCMRMAC0_ATTRIB_RATE_PARAMS)
  {
    return E_SUCCESS;
  }
  else
  {
    return E_NOT_ALLOWED;
  }
} /* hdrscmrmac0_is_attrib_gaupable */

