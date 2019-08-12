/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

        H D R   S E S S I O N  C O N F I G U R A T I O N  M A N A G E R 

           A C C E S S   C H A N N E L   M A C   P R O T O C O L
                                    
                            H E A D E R   F I L E       

GENERAL DESCRIPTION
  This module contains the configuration interface of the HDR Access Channel
  (MAC) Protocol.

EXTERNALIZED FUNCTIONS

  hdrscmamac_is_attrib_simple 
    Returns true if an attribute is simple

  hdrscmamac_is_simple_attrib_valid
    Verifies the validity of the simple attribute

  hdrscmamac_get_attrib_offset_and_size
    Returns attribute size and offset into the config structure.

  hdrscmamac_get_attrib_verno_index
    Returns the index of an attribute in the version number array. 

INITIALIZATION AND SEQUENCING REQUIREMENTS


  Copyright (c) 2001-2007 by Qualcomm Technologies, Inc.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/
/*===========================================================================

                           EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/hdr/cp/src/hdrscmamac.c#1 $ $DateTime: 2016/12/13 07:58:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/19/13   ukl     Reduce Code Duplication HDR-CP
05/22/08   pba     Klockwork fixes.
08/28/07   kss     Added hdrscmamac_config_is_needed().
08/22/07   etv     Fixed high lint warnings.
06/26/07   kss     Change hdrscmamac_get_prop_attrib_value() to simply return 
                   the value_id (rather than check the value as well).
04/27/07   kss     Added PNRandomizationDelaySupported.
01/03/07   etv     Added support to handle unknown attributes.
07/27/06   etv     0xFF is invalid for TermAccessRateMax attribute.
06/08/06   etv     Send session close if AN GAUPs a non-GAUPable attribute.
04/07/06   hal     Changed hdrutil_new_buffer() to use hdrutil_new_msg_buffer()
01/31/06   etv     Removed ASSERT for graceful degradation.
12/13/05   pba     Modified SCMDB calls to use updated API's
11/27/05   etv     Created Module.

===========================================================================*/

/*===========================================================================

                          INCLUDE FILES FOR MODULE
                     
===========================================================================*/
#include "dsm.h"
#include "comdef.h"
#include "err.h"

#include "hdrdebug.h"
#include "hdrdsm.h"
#include "hdrscp.h"
#include "hdrhai.h"
#include "hdrtrace.h"
#include "hdrbit.h"
#include "hdrutil.h"
#include "hdrhmp.h"

#include "hdrscmcmac.h"
#include "hdrscmdb.h"
#include "hdrscm.h"
#include "hdrscmamac.h"

/*===========================================================================

             DEFINITIONS  FROM  THE  HDR  AIR  INTERFACE  SPEC

===========================================================================*/
/* Attribute ID of Initial Configuration atttribute */
#define HDRSCMAMAC_INITIAL_CONFIG_ATTRIB_ID                 0x00
/* Size of fields of Initial Configuration Attribute  */
#define HDRSCMAMAC_IC_PROBE_SEQ_MAX_SIZE                    4
#define HDRSCMAMAC_IC_PROBE_BACKOFF_SIZE                    4
#define HDRSCMAMAC_IC_PROBE_SEQ_BACKOFF_SIZE                4
#define HDRSCMAMAC_IC_RSVD_FIELD_SIZE                       4
#define HDRSCMAMAC_INITIAL_CONFIG_ATTRIB_SIZE               16

/* Attrib ID of Power Parameters attribute. Only for Default subtype */
#define HDRSCMAMAC_POWER_PARAMS_ATTRIB_ID                   0x01
/* Size of fields of Power Parameters attribute */
#define HDRSCMAMAC_PP_DATA_OFFSET_NOM_SIZE                  4
#define HDRSCMAMAC_PP_DATA_OFFSET_9K6_SIZE                  4
#define HDRSCMAMAC_POWER_PARAMS_ATTRIB_SIZE                 8

#define HDRSCMAMAC_SIMPLE_ATTRIB_SIZE                       2
#define HDRSCMAMAC_LENGTH_FIELD_SIZE                        8
#define HDRSCMAMAC_ATTRIB_ID_SIZE                           8
#define HDRSCMAMAC_VALUE_ID_SIZE                            8

#define HDRSCMAMAC_AGGRESSIVE_AP_OVERRIDE                   0

/*------------------------------------------------------------------
    Valid Ranges for fields of Complex Configuration Attributes
-------------------------------------------------------------------*/
/* Initial Configuration Attribute */
/* ProbeSequenceMax */
#define HDRSCMAMAC_MIN_IC_PROBE_SEQ_MAX                     0x01
#define HDRSCMAMAC_MAX_IC_PROBE_SEQ_MAX                     0x0F

/*------------------------------------------------------------------
              Default Values for Configuration Attributes
-------------------------------------------------------------------*/
#define HDRSCMAMAC_DEFAULT_APERSISTENCE_OVERRIDE            0xFF
#define HDRSCMAMAC_DEFAULT_TERMINAL_ACCESS_RATE_MAX         0x00
#define HDRSCMAMAC_DEFAULT_DATA_OFFSET_NOM                  0x00
#define HDRSCMAMAC_DEFAULT_DATA_OFFSET_9K6                  0x00
#define HDRSCMAMAC_DEFAULT_DATA_OFFSET_19K2                 0x00
#define HDRSCMAMAC_DEFAULT_DATA_OFFSET_38K4                 0x00
#define HDRSCMAMAC_DEFAULT_AT_CLASS_OVERRIDE                0xFF
#define HDRSCMAMAC_DEFAULT_PN_DELAY_SUPPORT                 0x00


/* Initial Configuration Attribute */
#define HDRSCMAMAC_DEFAULT_IC_PROBE_SEQ_MAX                 0x03
#define HDRSCMAMAC_DEFAULT_IC_PROBE_BACKOFF                 0x04
#define HDRSCMAMAC_DEFAULT_IC_PROBE_SEQ_BACKOFF             0x08

/* Proposed value for PN Randomization Delay Supported */
#define HDRSCMAMAC_PN_DELAY_SUPPORT_ENABLED                 0x01


/*===========================================================================
 
                  DEFINITIONS AND DECLARATIONS FOR MODULE

  This section contains local definitions for constants, macros, types,
  variables and other items needed by this module.

===========================================================================*/
/* Index of Initial configuration attribute's version number in Verno array */
#define HDRSCMAMAC_IC_VERNO_INDEX       8     

/* Index of PowerParameters attribute's version number in Verno array */
#define HDRSCMAMAC_PP_VERNO_INDEX       9

/* Get the attribute's index in LUTs from the attribute ID */
#define HDRSCMAMAC_SIMPLE_ATTRIB_ID_TO_IND(attrib_id) ((attrib_id & 0x0F) - 8)

/* Finds if the given attribute Id is of a simple attribute */
#define HDRSCMAMAC_IS_ATTRIB_SIMPLE(attrib_id)  \
  ((attrib_id >= HDRSCMAMAC_FIRST_SIMPLE_ATTRIB) && \
    (attrib_id <= HDRSCMAMAC_LAST_SIMPLE_ATTRIB))

/* The type hdrscmamac_simple_attrib_invalid_range_type defines a min and a 
   max field to specify the range of values that is invalid for a given 
   simple attribute */
typedef struct
{
  uint8 min;    /* Start of Invalid Range */
  uint8 max;    /* End of Invalid Range */

} hdrscmamac_simple_attrib_invalid_range_type;


/*------------------------------------------------------------------------
                            GLOBAL VARIABLES
 -----------------------------------------------------------------------*/

static struct
{
  hdrscmamac_initial_config_attrib_type initial_config;
    /* Local instance of initial Configuration attribute */

  hdrscmamac_power_params_attrib_type power_params;
    /* PowerParameters attribute */

  uint8 gaup_trans_id;
    /* Next Transaction ID for AttributeUpdateRequest sent out */
  
  uint8 ap_override;
    /* Current ApersistenceOverride */

  uint8 trans_id;
    /* Next Transaction ID for Request sent out */

} hdrscmamac;

/*------------------------------------------------------------------------
                              LOOKUP TABLES
 -----------------------------------------------------------------------*/

/* There are several discontinuous valid ranges for each of the attribute. So
   it is more convenients to check for the invalid range since the invalid
   range was continuous. */
static hdrscmamac_simple_attrib_invalid_range_type 
  hdrscmamac_invalid_range[HDRSCMAMAC_NUM_SIMPLE_ATTRIBS] = 
{
  {0x02, 0xFF},       /* 0. 0xF8 PNRandomizationDelaySupported */
  {0x03, 0xFF},       /* 1. 0xF9 AccessTerminalClassOverride */
  {0x07, 0xF8},       /* 2. 0xFA DataOffset38k4 */  
  {0x07, 0xF8},       /* 3. 0xFB DataOffset19k2 */
  {0x07, 0xF8},       /* 4. 0xFC DataOffset9k6 */
  {0x07, 0xF8},       /* 5. 0xFD DataOffsetNom */
  {0x02, 0xFF},       /* 6. 0xFE TerminalAccessRateMax */
  {0x3F, 0xFF}        /* 7. 0xFF APersistenceOverride */
};

/* GAUPable configuration attributes of AMAC */
LOCAL const hdrscmamac_attrib_id_enum_type 
  hdrscmamac_gaup_attrib_ids[HDRSCMAMAC_NUM_GAUPABLE_ATTRIBS] = 
{
  HDRSCMAMAC_AT_CLASS_OVERRIDE_ATTRIB,      /* Only AN GAUP supported */
  HDRSCMAMAC_TERM_ACCESS_RATE_MAX_ATTRIB,   /* Only AN GAUP supported */
  HDRSCMAMAC_APERSISTENCE_OVERRIDE_ATTRIB   /* Both AT and AN GAUP supported */
};


/*===========================================================================

                         LOCAL FUNCTION DEFINITIONS
                    
===========================================================================*/

 /* EJECT */
/*===========================================================================

FUNCTION HDRSCMAMAC_IS_POWER_PARAMS_VALID                     LOCAL

DESCRIPTION
  Check if the following attribute is valid 

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
static hdrerrno_enum_type hdrscmamac_is_power_params_valid (void)
{

  /* Any 4 bit number is valid for DataOffsetNom and DataOffset9k6 */
  return E_SUCCESS;

} /* hdrscmamac_is_power_params_valid */


 /* EJECT */
/*===========================================================================

FUNCTION HDRSCMAMAC_PROCESS_POWER_PARAMS                         LOCAL

DESCRIPTION
  This is a generic interface to process the PowerParameters Attribute.

  0x01 - PowerParameters Attribute.

  Note: This attribute is specific only to Default subtype 

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
static hdrerrno_enum_type hdrscmamac_process_power_params
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
     set of  T2PInflowRange complex Attribute Value */
  while ( (attrib_offset + p_aip->value_id_size + 
           HDRSCMAMAC_POWER_PARAMS_ATTRIB_SIZE ) <= p_aip->attrib_len )
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
      ERR("Error unpacking Value ID of PowerParameters attrib", 0,0,0);
      break;
    }

    /*----------------------------------------------------------------------
                                  DataOffsetNom
     *--------------------------------------------------------------------*/

    /* Shift the attribute offset to start of DataOffsetNom field */ 
    attrib_offset += p_aip->value_id_size ;

    /* Unpack DataOffsetNom */
    err_no = HDRBIT_UNPACK8( item_ptr,
                             &(hdrscmamac.power_params.data_offset_nom),
                             (p_aip->attrib_offset + attrib_offset),
                             HDRSCMAMAC_PP_DATA_OFFSET_NOM_SIZE );

    /* If error unpacking DataOffsetNom, break */
    if (err_no != E_SUCCESS)
    {
      ERR("Error unpacking DataoffsetNom of PowerParams attrib", 0,0,0);
      break;
    }

    /*----------------------------------------------------------------------
                                  DataOffset9k6
     *--------------------------------------------------------------------*/

    /* Shift the attribute offset to start of DataOffset9k6 field */ 
    attrib_offset += HDRSCMAMAC_PP_DATA_OFFSET_NOM_SIZE;

    /* Unpack DataOffset9k6 */
    err_no = HDRBIT_UNPACK8( item_ptr,
                             &(hdrscmamac.power_params.data_offset_9k6),
                             (p_aip->attrib_offset + attrib_offset),
                             HDRSCMAMAC_PP_DATA_OFFSET_9K6_SIZE );

    /* If error unpacking DataOffset9k6, break */
    if (err_no != E_SUCCESS)
    {
      ERR("Error unpacking Dataoffset9k6 of PowerParams attrib", 0,0,0);
      break;
    }

    /* Parsing done! :-) Is the parsed attribute valid? */
    if ( (err_no =
          hdrscmamac_is_power_params_valid()) != E_OUT_OF_RANGE )
    {
      /* Attrib valid! Yea! :-) print F3 mesg */
      HDR_MSG_PROT_2( MSG_LEGACY_HIGH, "PP: DONom: %d DO9k6 %d",
                      hdrscmamac.power_params.data_offset_nom,
                      hdrscmamac.power_params.data_offset_9k6);

      /* Update the accepted attribute information */
      a_aip->is_accepted = TRUE;
      a_aip->value_id = value_id;
      a_aip->complex_val_ptr = (void*) &(hdrscmamac.power_params);
      a_aip->complex_val_size = sizeof(hdrscmamac_power_params_attrib_type);
      break;
    }

    /* Not much luck :-( Let's try next set of values if there is one. 
       Shift the offset to start of next value ID. */
    attrib_offset += HDRSCMAMAC_PP_DATA_OFFSET_9K6_SIZE;

  } /* while */

  return err_no;

} /* hdrscmamac_process_power_params */


 /* EJECT */
/*===========================================================================

FUNCTION HDRSCMAMAC_IS_INITIAL_CONFIG_VALID                     LOCAL

DESCRIPTION
  Check if the following attribute is valid 

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
static hdrerrno_enum_type hdrscmamac_is_initial_config_valid (void)
{

  /* Any four bit number is valid for both ProbeBackoff and 
     ProbeSequenceBackoff fields */

  /* Any four bit number other than zero is valid for ProbeSequenceMax */
  return ((hdrscmamac.initial_config.probe_seq_max != 0)? 
          E_SUCCESS: E_OUT_OF_RANGE);

} /* hdrscmamac_is_initial_config_valid */


 /* EJECT */
/*===========================================================================

FUNCTION HDRSCMAMAC_PROCESS_INITIAL_CONFIG                         LOCAL

DESCRIPTION
  This is a generic interface to process the InitialConfiguration Attribute.

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
static hdrerrno_enum_type hdrscmamac_process_initial_config
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
     set of  T2PInflowRange complex Attribute Value */
  while ( (attrib_offset + p_aip->value_id_size + 
           HDRSCMAMAC_INITIAL_CONFIG_ATTRIB_SIZE ) <= p_aip->attrib_len )
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
      ERR("Error unpacking Value ID of InitialConfiguration attrib", 0,0,0);
      break;
    }


    /*----------------------------------------------------------------------
                                ProbeSequenceMax
     *--------------------------------------------------------------------*/

    /* Shift the attribute offset to start of ProbeSequenceMax field */ 
    attrib_offset += p_aip->value_id_size;

    /* Unpack ProbeSequenceMax */
    err_no = HDRBIT_UNPACK8( item_ptr,
                             &(hdrscmamac.initial_config.probe_seq_max ),
                             (p_aip->attrib_offset + attrib_offset),
                             HDRSCMAMAC_IC_PROBE_SEQ_MAX_SIZE );


    /* If error unpacking ProbeSequenceMax, break */
    if (err_no != E_SUCCESS)
    {
      ERR("Error unpacking ProbeSequenceMax of InitialConfig attrib", 0,0,0);
      break;
    }

    /*----------------------------------------------------------------------
                                    ProbeBackoff
     *--------------------------------------------------------------------*/

    /* Shift the attribute offset to start of ProbeBackoff field */ 
    attrib_offset += HDRSCMAMAC_IC_PROBE_SEQ_MAX_SIZE;

    /* Unpack ProbeBackoff */
    err_no = HDRBIT_UNPACK8( item_ptr,
                             &(hdrscmamac.initial_config.probe_backoff ),
                             (p_aip->attrib_offset + attrib_offset),
                             HDRSCMAMAC_IC_PROBE_BACKOFF_SIZE );

    /* If error unpacking ProbeBackoff, break */
    if (err_no != E_SUCCESS)
    {
      ERR("Error unpacking ProbeBackoff of InitialConfig attrib", 0,0,0);
      break;
    }

    /*----------------------------------------------------------------------
                                  ProbeSequenceBackoff
     *--------------------------------------------------------------------*/

    /* Shift the attribute offset to start of ProbeSequenceBackoff field */ 
    attrib_offset += HDRSCMAMAC_IC_PROBE_BACKOFF_SIZE;

    /* Unpack ProbeSequenceBackoff */
    err_no = HDRBIT_UNPACK8( item_ptr,
                             &(hdrscmamac.initial_config.probe_seq_backoff ),
                             (p_aip->attrib_offset + attrib_offset),
                             HDRSCMAMAC_IC_PROBE_SEQ_BACKOFF_SIZE );

    /* If error unpacking ProbeSequenceBackoff, break */
    if (err_no != E_SUCCESS)
    {
      ERR("Error unpacking ProbeSequenceBackoff of InitialConfig attrib", 0,0,0);
      break;
    }


    /* Parsing done! :-) Is the parsed attribute valid? */
    if ( (err_no =
          hdrscmamac_is_initial_config_valid()) != E_OUT_OF_RANGE )
    {
      /* Attrib valid! Yea! :-) print F3 mesg */
      HDR_MSG_PROT_3( MSG_LEGACY_HIGH, "IC: PSMax: %d PBackoff: %d PSBackoff %d",
                      hdrscmamac.initial_config.probe_seq_max,
                      hdrscmamac.initial_config.probe_backoff,
                      hdrscmamac.initial_config.probe_seq_backoff);

      /* Update the accepted attribute information */
      a_aip->is_accepted = TRUE;
      a_aip->value_id = value_id;
      a_aip->complex_val_ptr = (void*) &(hdrscmamac.initial_config);
      a_aip->complex_val_size = sizeof(hdrscmamac_initial_config_attrib_type);
      break;
    }

    /* Not much luck :-( Let's try next set of values if there is one. 
       Shift the offset to start of next value ID. */
    attrib_offset += (HDRSCMAMAC_IC_PROBE_SEQ_BACKOFF_SIZE + 
                      HDRSCMAMAC_IC_RSVD_FIELD_SIZE);

  } /* while */

  return err_no;

} /* hdrscmamac_process_initial_config */


 /* EJECT */
/*============================================================================

                        EXTERNAL FUNCTION DEFINITIONS

=============================================================================*/

 /* EJECT */
/*===========================================================================

FUNCTION HDRSCMAMAC_PROCESS_COMPLEX_ATTRIB

DESCRIPTION
  This is a generic interface to process all complex attributes given below.


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
hdrerrno_enum_type hdrscmamac_process_complex_attrib
(
  dsm_item_type                               *item_ptr,
  hdrscm_proposed_attrib_type   const *const   p_aip,
  hdrscm_accepted_attrib_type         *const   a_aip,
  uint16 subtype
)
{

  hdrerrno_enum_type err_no = E_SUCCESS;
    /* Error Code */

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */ 

  switch (p_aip->attrib_id)
  {
    /* Initial Configuration Attribute */
    case HDRSCMAMAC_INITIAL_CONFIG_ATTRIB_ID:

      err_no = hdrscmamac_process_initial_config(item_ptr, p_aip, a_aip);
      break;

    /* Power Params attribute */
    case HDRSCMAMAC_POWER_PARAMS_ATTRIB_ID:

      /* PowerParameters attribute is applicable only for default ACMAC. */
      if (subtype == HDRSCP_DEFAULT_ACMAC)
      {
        err_no = hdrscmamac_process_power_params(item_ptr, p_aip, a_aip);
      }
      else
      {
        err_no = E_DATA_INVALID;
      }

      break;

    default:

      HDR_MSG_PROT_1(MSG_LEGACY_ERROR, "Invalid attrib ID %d ignored", 
                     p_aip->attrib_id);
      err_no = E_DATA_INVALID;
  }

  return err_no;

} /* hdrscmamac_process_complex_attrib */


 /* EJECT */
/*===========================================================================

FUNCTION HDRSCMAMAC_IS_ATTRIB_SIMPLE

DESCRIPTION
  This function is used by SCM to query if an attribute is simple, given the
  attribute ID.
  
DEPENDENCIES
  None.

PARAMETERS
  attrib_id - Attribute ID 
  subtype   - Subtype negotiated for AMAC 

RETURN VALUE
  TRUE - if the attribute given by 'attrib_id' is simple.
  FALSE - otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
boolean hdrscmamac_is_attrib_simple
(
  uint16 const attrib_id,
  uint16 const subtype
)
{

   return HDRSCMAMAC_IS_ATTRIB_SIMPLE(attrib_id);

} /* hdrscmamac_is_attrib_simple */


 /* EJECT */
/*===========================================================================

FUNCTION HDRSCMAMAC_IS_SIMPLE_ATTRIB_VALID

DESCRIPTION
  This function is used by SCM to query if the given 'attrib_value' is a 
  valid value for the attribute specified by 'attrib_id'.
  
DEPENDENCIES
  None.

PARAMETERS
  attrib_id    - Attribute ID of the simple attribute.
  attrib_value - Value of the simple attribute. 
  subtype      - Subtype Negotiated

RETURN VALUE
  E_SUCCESS          - If the attrib_value is valid.
  E_OUT_OF_RANGE     - Otherwise.

SIDE EFFECTS
  None.
  
===========================================================================*/
hdrerrno_enum_type hdrscmamac_is_simple_attrib_valid
(
  uint16 const attrib_id,
  uint32 const attrib_value,
  uint16 const subtype
)
{
  uint8 attrib_ind;
    /* Index of simple attribute in look up table */

  hdrerrno_enum_type err_no = E_SUCCESS;

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */ 

  /* There are no simple attribs in Def. AMAC. Make sure we don't get here. */
  if (subtype == HDRSCP_DEFAULT_ACMAC)
  {
    HDR_MSG_PROT_1(MSG_LEGACY_ERROR, 
                   "Err: Got simple attrib 0x%x for Def AMAC", attrib_id);
    err_no = E_DATA_INVALID;
  }
  /* Make sure the attribute is simple */
  else if (HDRSCMAMAC_IS_ATTRIB_SIMPLE(attrib_id) == FALSE)
  {
    HDR_MSG_PROT_1(MSG_LEGACY_ERROR, 
                   "Err: 0x%x is not a simple attrib", attrib_id);
    err_no = E_DATA_INVALID;
  }
  else
  {
    attrib_ind = HDRSCMAMAC_SIMPLE_ATTRIB_ID_TO_IND(attrib_id);

    if (attrib_id == HDRSCMAMAC_TERM_ACCESS_RATE_MAX_ATTRIB)
    {
      err_no = ((attrib_value <= hdrscmamac_invalid_range[attrib_ind].min)? 
                E_SUCCESS: E_OUT_OF_RANGE);
    }
    else
    {
      /* If the simple attribute value is anything but a value within the invalid 
         range, return success */
      err_no = ((attrib_value <= hdrscmamac_invalid_range[attrib_ind].min) ||
                (attrib_value >= hdrscmamac_invalid_range[attrib_ind].max) )?
                E_SUCCESS: E_OUT_OF_RANGE;
    }

    if (err_no != E_SUCCESS)
    {
      HDR_MSG_PROT_2(MSG_LEGACY_ERROR, 
                     "Err: Recd invalid value 0x%x for attrib 0x%x",
                     attrib_value, attrib_id);
    }
  }

  return err_no;

} /* hdrscmamac_is_simple_attrib_valid */


 /* EJECT */
/*===========================================================================

FUNCTION HDRSCMAMAC_GET_ATTRIB_OFFSET_AND_SIZE

DESCRIPTION
  This function computes the byte offset of the attribute specified by the 
  attrib_id in the struct hdrscmamac_config_struct_type and stores it in 
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
  E_SUCCESS if the given attrib ID is valid.
  E_DATA_INVALID if the attrib ID is not valid

SIDE EFFECTS
  None

===========================================================================*/
hdrerrno_enum_type hdrscmamac_get_attrib_offset_and_size
(
  uint16 attrib_id, 
  uint16 *attrib_offset,
  uint16 *attrib_size,
  uint16 subtype
)
{
  uint8 attrib_index = 0;
    /* Attribute Index */

  hdrerrno_enum_type err_no = E_SUCCESS;
    /* Error Code */

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */ 

  /* Is the attribute simple? */
  if (HDRSCMAMAC_IS_ATTRIB_SIMPLE(attrib_id))
  {
    /* There are no simple attribs in Def. AMAC. Make sure we don't get here. */
    if (subtype == HDRSCP_DEFAULT_ACMAC)
    {
      HDR_MSG_PROT_1(MSG_LEGACY_ERROR, 
                     "Err: Got simple attrib 0x%x for Def. AMAC", attrib_id);
      err_no = E_DATA_INVALID;
    }
    else
    {
      /* Get the index */
      attrib_index = HDRSCMAMAC_SIMPLE_ATTRIB_ID_TO_IND(attrib_id);

      /* The attributes are arranged in the order of index in the config struct
         and since each simple attribute is of size uint8, the index could be
         used as the byte offset of that attribute in that config struct. */
      *attrib_offset = attrib_index;

      *attrib_size = sizeof(uint8);
    }
  }
  else
  {
    /* The attribute is probably complex... */
    switch(attrib_id)
    {
      /* Initial Configuration Attribute */
      case HDRSCMAMAC_INITIAL_CONFIG_ATTRIB_ID:

        *attrib_offset = FPOS(hdrscmamac_config_struct_type, init_config);
        *attrib_size = sizeof(hdrscmamac_initial_config_attrib_type);
        break;

      /* Power Params attribute */
      case HDRSCMAMAC_POWER_PARAMS_ATTRIB_ID:

        /* PowerParameters attribute is applicable only for default ACMAC. */
        if (subtype == HDRSCP_ENH_ACMAC)
        {
          HDR_MSG_PROT_1(MSG_LEGACY_ERROR, 
                         "Err: Got PowerParams attrib 0x%x for Enh. AMAC",
                         attrib_id);
          err_no = E_DATA_INVALID;
        }
        else
        {
          *attrib_offset = FPOS(hdrscmamac_config_struct_type, power_params);
          *attrib_size = sizeof(hdrscmamac_power_params_attrib_type);
        }
        break;

      default:

        HDR_MSG_PROT_1(MSG_LEGACY_ERROR, 
                       "Err: Invalid attrib ID: 0x%x; Ignored", attrib_id);
        err_no = E_DATA_INVALID;
    }
  }

  return err_no;

} /* hdrscmamac_get_attrib_offset_and_size */


/* EJECT */
/*===========================================================================

FUNCTION HDRSCMAMAC_GET_ATTRIB_VERNO_INDEX

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
hdrerrno_enum_type hdrscmamac_get_attrib_verno_index
(
  uint16 attrib_id,
  uint16 *index,
  uint16 subtype
)
{
  hdrerrno_enum_type err_no = E_SUCCESS;
    /* Error Code */

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */ 

  /* Is the attribute simple? */
  if (HDRSCMAMAC_IS_ATTRIB_SIMPLE(attrib_id))
  {
    /* There are no simple attribs in Def. AMAC. Make sure we don't get here. */
    if (subtype == HDRSCP_DEFAULT_ACMAC)
    {
      HDR_MSG_PROT_1(MSG_LEGACY_ERROR, 
                     "Err: Got simple attrib 0x%x for Def. AMAC", attrib_id);
      err_no = E_DATA_INVALID;
    }
    else
    {
      /* The attributes are arranged in the order of index in the Version number
         array */
      *index = HDRSCMAMAC_SIMPLE_ATTRIB_ID_TO_IND(attrib_id);
    }
  }
  else
  {
    switch(attrib_id)
    {
      /* Initial Configuration attribute 0x00 */
      case HDRSCMAMAC_INITIAL_CONFIG_ATTRIB_ID:

        *index = HDRSCMAMAC_IC_VERNO_INDEX;
        break;

      /* PowerParameters attribute 0x01 */
      case HDRSCMAMAC_POWER_PARAMS_ATTRIB_ID:

        /* PowerParameters attribute is applicable only for default ACMAC. */
        if (subtype == HDRSCP_ENH_ACMAC)
        {
          HDR_MSG_PROT_1(MSG_LEGACY_ERROR, 
                         "Err: Got PowerParams attrib 0x%x for Enh AMAC!",
                         attrib_id);
          err_no = E_DATA_INVALID;
        }
        else
        {
          *index = HDRSCMAMAC_PP_VERNO_INDEX;
        }
        break;

      default:
        HDR_MSG_PROT_1(MSG_LEGACY_ERROR, "Invalid attrib Id 0x%x", attrib_id);
        err_no = E_DATA_INVALID;
    }
  }

  return err_no;

} /* hdrscmamac_get_attrib_verno_index */


/* EJECT */
/*===========================================================================

FUNCTION HDRSCMAMAC_SET_CONFIG_TO_DEFAULT

DESCRIPTION
  Set the configuration attributes to its default values.

DEPENDENCIES
  None

PARAMETERS
  hdramac_cfg - Pointer to the config structure.
  
RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
void hdrscmamac_set_config_to_default
(
  hdrscmamac_config_struct_type *hdramac_cfg
)
{

  /* Set all simple attributes to default values */
  hdramac_cfg->at_class_override = HDRSCMAMAC_DEFAULT_AT_CLASS_OVERRIDE;
  hdramac_cfg->data_offset_38k4 = HDRSCMAMAC_DEFAULT_DATA_OFFSET_38K4;
  hdramac_cfg->data_offset_19k2 = HDRSCMAMAC_DEFAULT_DATA_OFFSET_19K2;
  hdramac_cfg->data_offset_9k6 = HDRSCMAMAC_DEFAULT_DATA_OFFSET_9K6;
  hdramac_cfg->data_offset_nom = HDRSCMAMAC_DEFAULT_DATA_OFFSET_NOM;
  hdramac_cfg->term_acc_rate_max = HDRSCMAMAC_DEFAULT_TERMINAL_ACCESS_RATE_MAX;
  hdramac_cfg->ap_override = HDRSCMAMAC_DEFAULT_APERSISTENCE_OVERRIDE;
  hdramac_cfg->pn_delay_supported = HDRSCMAMAC_DEFAULT_PN_DELAY_SUPPORT;

  /* Initial Configuration attribute */
  hdramac_cfg->init_config.probe_backoff = HDRSCMAMAC_DEFAULT_IC_PROBE_BACKOFF;
  hdramac_cfg->init_config.probe_seq_backoff = HDRSCMAMAC_DEFAULT_IC_PROBE_SEQ_BACKOFF;
  hdramac_cfg->init_config.probe_seq_max = HDRSCMAMAC_DEFAULT_IC_PROBE_SEQ_MAX;

  /* PowerParameters Attribute */
  hdramac_cfg->power_params.data_offset_9k6 = HDRSCMAMAC_DEFAULT_DATA_OFFSET_9K6;
  hdramac_cfg->power_params.data_offset_nom = HDRSCMAMAC_DEFAULT_DATA_OFFSET_NOM;

} /* hdrscmamac_set_config_to_default */


 /* EJECT */
/*===========================================================================

FUNCTION HDRSCMAMAC_GAUP_AP_OVERRIDE

DESCRIPTION
  This routine is used by Enhanced AMAC to send GAUP update request for 
  APersistenceOverride attribute.

DEPENDENCIES
  None.

PARAMETERS
  mode - ApersistenceOverride Mode 
  p_trans_id  - Address where the transaction ID is stored to be returned back.
  
RETURN VALUE
  E_SUCCESS       - If a GAUP Update was sent successfully.
  E_NOT_AVAILABLE - If we didn't have a free Rsp timer to send this GAUP update.
  E_NOT_ALLOWED   - If a GAUP update was already sent and pending, we deny the 
                    GAUP request.

SIDE EFFECTS
  None.
===========================================================================*/
hdrerrno_enum_type hdrscmamac_gaup_ap_override
(
  hdrscmamac_ap_override_mode_enum_type ap_mode,
  uint8 *p_trans_id
)
{

  dsm_item_type * send_msg_ptr = NULL;
    /* Pointer to gaup req. message to send */

  uint8 attrib_offset = 0;
    /* Attribute field offset from start of DSM item */

  uint8 timer_id;
    /* ID of the reserved timer */

  hdrerrno_enum_type err_no = E_SUCCESS;
    /* Error Code */

  uint8 ap_override;
    /* ap_override to GAUP */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (hdrscm_is_gaup_pending(HDRHAI_AC_MAC_PROTOCOL))
  {
    HDR_MSG_PROT(MSG_LEGACY_ERROR, "GAUP pending already!");
    return E_NOT_ALLOWED;
  }

  /* Reserve a Response timer for this GAUP update */
  if ( (err_no = hdrscm_reserve_rsp_timer(HDRHAI_AC_MAC_PROTOCOL, &timer_id, 
                           hdrscmamac.gaup_trans_id, TRUE)) != E_SUCCESS)
  {
    /* No free timer was available. Forward the error code to Apps! */
    return err_no;
  }

  if (ap_mode == HDRSCMAMAC_AP_OVERRIDE_AGGRESSIVE_MODE)
  {
    /* Save the current instance of ApersitenceOverride in order to be able to 
       revert back to current configuration. */
    (void) hdrscmdb_get_inuse_attrib(HDRHAI_AC_MAC_PROTOCOL, 
                                     HDRSCMAMAC_APERSISTENCE_OVERRIDE_ATTRIB, 
                                     &hdrscmamac.ap_override, 
                                     HDRSCP_ENH_ACMAC);

    ap_override = HDRSCMAMAC_AGGRESSIVE_AP_OVERRIDE;
  }
  else
  {
    ap_override = hdrscmamac.ap_override;
  }


  /* Get a new DSM buffer */
  send_msg_ptr = hdrutil_new_msg_buffer();

  /* Pack AttribUpdateRequest message's Message ID */
  HDRBIT_PACK8(send_msg_ptr, HDRSCM_ATTRIB_UPDATE_REQ_MSG,
               attrib_offset, HDRSCM_MESSAGE_ID_SIZE );

  /* Shift the offset to start of Transaction ID */
  attrib_offset += HDRSCM_MESSAGE_ID_SIZE;

  /* Pack the message's Transaction ID */
  HDRBIT_PACK8(send_msg_ptr, hdrscmamac.gaup_trans_id, attrib_offset, 
               HDRSCM_TRANS_ID_SIZE);

  /* Shift the offset to start of Attribute Record's Length */
  attrib_offset += HDRSCM_TRANS_ID_SIZE;

  /* Pack the Length field of the attribute Record */
  HDRBIT_PACK8(send_msg_ptr, HDRSCMAMAC_SIMPLE_ATTRIB_SIZE,
               attrib_offset, HDRSCMAMAC_LENGTH_FIELD_SIZE);

  /* Shift the offset to start of Attribute ID field */
  attrib_offset += HDRSCMAMAC_LENGTH_FIELD_SIZE;

  /* Pack the Attribute ID of the rate parameters */
  HDRBIT_PACK8(send_msg_ptr,
               HDRSCMAMAC_APERSISTENCE_OVERRIDE_ATTRIB,
               attrib_offset,
               HDRSCMAMAC_ATTRIB_ID_SIZE);

  /* Shift the offset to start of Value */
  attrib_offset += HDRSCMAMAC_ATTRIB_ID_SIZE;

  /* Pack the value of ApersistenceOverride */
  HDRBIT_PACK8(send_msg_ptr, ap_override, attrib_offset,
               HDRSCMAMAC_VALUE_ID_SIZE);

  /* Send AttribUpdateRequest msg on RTC. All config messages are reliable. */
  hdrhmp_send_msg(HDRHAI_AC_MAC_PROTOCOL,
                  HDRHAI_RTC,
                  TRUE,
                  HDRSCMAMAC_AR_MSG_PRIO,
                  HDRHAI_IN_USE,
                  send_msg_ptr,
                  hdrscm_tx_outcome_cb,
                  (void*) ((dword) timer_id) );

  /* A new transaction has been started. Increment the Transaction ID */
  hdrscmamac.gaup_trans_id++;

  return err_no;

} /* hdrscmamac_gaup_ap_override */


 /* EJECT */
/*===========================================================================

FUNCTION HDRSCMAMAC_IS_ATTRIB_GAUPABLE

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
hdrerrno_enum_type hdrscmamac_is_attrib_gaupable
(
  uint16 const attrib_id,
  uint16 const subtype
)
{
  uint8 gaup_ind = 0;
    /* Loop index */

  hdrerrno_enum_type err_no = E_NOT_ALLOWED;
    /* Error code */
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  switch (subtype)
  {
    case HDRSCP_ENH_ACMAC:
      /* Loop around the GAUPable attribute ID LUT to check if the given 
         attrib_id is one of them */
      for (gaup_ind = 0; (gaup_ind < HDRSCMAMAC_NUM_GAUPABLE_ATTRIBS) &&
                         (err_no != E_SUCCESS); ++gaup_ind)
      {
        if (attrib_id == hdrscmamac_gaup_attrib_ids[gaup_ind])
        {
          /* attrib_id is GAUPable */
          err_no = E_SUCCESS;
        }
      }
      break;

    case HDRSCP_DEFAULT_ACMAC:
    default:
      /* Nothing to be done. Just return E_NOT_ALLOWED */
      break;
  }

  return err_no;

} /* hdrscmamac_is_attrib_gaupable */


/* EJECT */
/*===========================================================================

FUNCTION HDRSCMAMAC_ATTRIB_IS_SUPPORTED

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
boolean hdrscmamac_attrib_is_supported
(
  uint16 const attrib_id,
  uint16 const subtype
)
{
  boolean known = FALSE;

/*- - - - - - - - - - - - -- - - - - - - - - - - - - - - - - - - - - - - - -*/

  switch(subtype)
  {

    case HDRSCP_ENH_ACMAC:
      known = (((attrib_id >= HDRSCMAMAC_AT_CLASS_OVERRIDE_ATTRIB) &&
                (attrib_id <= HDRSCMAMAC_APERSISTENCE_OVERRIDE_ATTRIB)) ||
               (attrib_id == HDRSCMAMAC_INITIAL_CONFIG_ATTRIB));
      break;
              
    case HDRSCP_DEFAULT_ACMAC:

      known = ((attrib_id == HDRSCMAMAC_INITIAL_CONFIG_ATTRIB) ||
               (attrib_id == HDRSCMAMAC_POWER_PARAMS_ATTRIB));
      break;

    default:
      HDR_MSG_PROT_1(MSG_LEGACY_ERROR, 
                     "AMAC subtype 0x%x not supported", subtype);
  }

  if (!known)
  {
    HDR_MSG_PROT_1(MSG_LEGACY_ERROR, 
                   "AMAC attribute 0x%x not supported", attrib_id);
  }

  return known;

} /* hdrscmamac_attrib_is_supported */



/* EJECT */
/*===========================================================================

FUNCTION HDRSCMAMAC_PROP_PN_DELAY_SUPPORTED

DESCRIPTION
  This function is used to propose PNDelaySupported attribute as 
  part of AT-init session configuration.

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
boolean hdrscmamac_prop_pn_delay_supported( void )
{
  uint8 timer_id = 0;
      /* Reserved timer Id */

  dsm_item_type * send_msg_ptr = NULL;
    /* Pointer to Config req. message to send */

  uint8 attrib_offset = 0;
    /* Attribute field offset from start of DSM item */

  boolean status = TRUE;
    /* Return code */

  uint16 pn_delay_supported;
    /* Simple attribute */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    pn_delay_supported = HDRSCMAMAC_PN_DELAY_SUPPORT_ENABLED;

    /* Reserve a Response timer for this Config Request message  */
    if ( (hdrscm_reserve_rsp_timer(HDRHAI_AC_MAC_PROTOCOL, &timer_id, 
                             hdrscmamac.trans_id, FALSE)) != E_SUCCESS)
    {
      /* No free timer was available. */
      status = FALSE;
    }

  if (status)
  {
    HDR_MSG_PROT(MSG_LEGACY_HIGH, 
                 "ACMAC: AT-init Proposal of PNRandomizationDelaySupported");
    
    /* Get a new DSM buffer */
    send_msg_ptr = hdrutil_new_msg_buffer();

    /* Pack AttribUpdateRequest message's Message ID */
    HDRBIT_PACK8(send_msg_ptr, HDRSCM_CONFIG_REQ_MSG,
                 attrib_offset, HDRSCM_MESSAGE_ID_SIZE );

    /* Shift the offset to start of Transaction ID */
    attrib_offset += HDRSCM_MESSAGE_ID_SIZE;

    /* Pack the message's Transaction ID */
    HDRBIT_PACK8(send_msg_ptr, hdrscmamac.trans_id, attrib_offset, 
                 HDRSCM_TRANS_ID_SIZE);

    /* Shift the offset to start of Attribute Record's Length */
    attrib_offset += HDRSCM_TRANS_ID_SIZE;

    HDRBIT_PACK8(send_msg_ptr, 
                 (HDRSCMAMAC_VALUE_ID_SIZE + HDRSCMAMAC_ATTRIB_ID_SIZE) / 8, 
                 attrib_offset, HDRSCM_LENGTH_SIZE);

    /* Shift the offset to start of AttributeID field */
    attrib_offset += HDRSCM_LENGTH_SIZE;

    HDRBIT_PACK16(send_msg_ptr, 
                  HDRSCMAMAC_PN_RANDOMIZATION_DELAY_SUPPORTED_ATTRIB, 
                  attrib_offset,
                  HDRSCMAMAC_ATTRIB_ID_SIZE);

    /* Shift the offset to start of Value ID field */
    attrib_offset += HDRSCMAMAC_ATTRIB_ID_SIZE;

    /* Pack Value ID */
    HDRBIT_PACK8(send_msg_ptr, pn_delay_supported, attrib_offset, 
                 HDRSCMAMAC_VALUE_ID_SIZE);

    /* Send ConfigRequest msg on RTC. All config messages are reliable. */
    hdrhmp_send_msg(HDRHAI_AC_MAC_PROTOCOL,
                    HDRHAI_RTC,
                    TRUE,
                    HDRSCM_CONFIG_REQ_MSG_PRI,
                    HDRHAI_IN_CONFIG,
                    send_msg_ptr,
                    hdrscm_tx_outcome_cb,
                    (void*) ((dword) timer_id) );

    HDR_MSG_PROT_1(MSG_LEGACY_HIGH, 
                   "ACMAC: AT Proposed. PNDelaySupported: %d", 
                    pn_delay_supported);

    /* Increment the transaction ID for next transaction */ 
    ++hdrscmamac.trans_id;
  }

  return status;

} /* hdrscmamac_prop_pn_delay_supported */

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
void hdrscmamac_fail_curr_config_neg
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
  session_fail_data.type = HDRHAI_AC_MAC_PROTOCOL;
  session_fail_data.subtype = HDRSCP_ENH_ACMAC;
  session_fail_data.attribute_id_length = HDRSCMAMAC_SIMPLE_ATTRIB_SIZE;
  session_fail_data.attribute_id = 
    HDRSCMAMAC_PN_RANDOMIZATION_DELAY_SUPPORTED_ATTRIB;
  session_fail_data.reason = close_reason;

  ind_data.scp_fail_ptr = (void *)&session_fail_data;

  hdrind_give_ind(HDRIND_AMAC_PROT_NEG_FAILED,(void *)&ind_data);

} /* hdrscmamac_fail_curr_config_neg */


/* EJECT */
/*===========================================================================

FUNCTION HDRSCMAMAC_GET_PROP_ATTRIB_VALUE

DESCRIPTION
  The function is used by SCM to check the proposed attribute value.

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
hdrerrno_enum_type hdrscmamac_get_prop_attrib_value
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
    /* Enhanced ACMAC */
    case HDRSCP_ENH_ACMAC:

      /* Make sure Attribute Information pointer is not NULL */
      ASSERT(a_aip != NULL);

      if (trans_id == (uint8)(hdrscmamac.trans_id-1))
      {
        switch(a_aip->attrib_id)
        {
          case HDRSCMAMAC_PN_RANDOMIZATION_DELAY_SUPPORTED_ATTRIB:
            a_aip->value_id = HDRSCMAMAC_PN_DELAY_SUPPORT_ENABLED;
            a_aip->is_accepted = TRUE;
            err_no = E_SUCCESS;
            break;

         default:
           /* unsupported attrib_id */
           HDR_MSG_PROT_1(MSG_LEGACY_ERROR, 
             "AMAC: Attrib id %d not supported", a_aip->attrib_id);
        }
      }
      else
      {
        HDR_MSG_PROT_2(MSG_LEGACY_ERROR, 
          "AMAC: trans_id %d mismatched proposed trans_id %d",
           trans_id, (uint8)(hdrscmamac.trans_id-1));
      }

      break;

    default:
      ERR("AMAC Sub: %d does not support AT-init proposal", subtype,0,0);
      err_no = E_NOT_SUPPORTED;
  }

  return err_no;

} /* hdrscmamac_get_prop_attrib_value */


 /* EJECT */
/*===========================================================================
FUNCTION HDRSCMAMAC_CONFIG_IS_NEEDED                                EXTERNAL

DESCRIPTION
  This function is used to find out if an AT-initiated Configuration is 
  needed by Enh. ACMAC

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
boolean hdrscmamac_config_is_needed( void )
{
  return ( hdrscm_config_is_needed(HDRHAI_AC_MAC_PROTOCOL) );
} /* hdrscmamac_config_is_needed */


