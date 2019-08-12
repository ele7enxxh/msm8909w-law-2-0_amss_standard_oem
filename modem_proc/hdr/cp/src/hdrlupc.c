
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

           L O C A T I O N   U P D A T E   P R O T O C O L   
                    C O N F I G U R A T I O N

GENERAL DESCRIPTION
  This module contains code related to the configuration negotiation of the
  Location Update Protocol.

EXTERNALIZED FUNCTIONS (Global)
   hdrlupc_powerup_init - One time powerup initialization

EXTERNALIZED FUNCTIONS (Regional)
   hdrlupc_process_ind      - Handle indications pertaining to configuration

IDLEIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2001 - 2011 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* <EJECT> */ 
/*===========================================================================

                        EDIT HISTORY FOR MODULE

$PVCSPath: O:/src/asw/COMMON/vcs/hdrlupc.c_v   1.14   12 Jun 2002 00:57:08   vsali  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/hdr/cp/src/hdrlupc.c#1 $ $DateTime: 2016/12/13 07:58:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
12/10/13   sat     Changed memcpy() to assignment or memscpy() (safer version).
05/31/11   cnx     Added hdrlupc_get_config_result_handle();
                   Moved hdrlupc_config_result_type to hdrlupc.h.
12/10/08   smd     Replaced assert.h with amssassert.h
07/06/06   yll     Changed the attrib_len from uint8 to uint16.
12/13/05   pba     Changes to support multiple personality negotiation
10/04/05   pba     Added support to read/write config parameters from SCMDB/EFS
09/20/04   mpa     Fixed Config Rsp message packing
09/09/04   mpa     Preliminary QOS support.
06/12/02   vas     If ConfigRequest is received for any other app than
                   PacketApp bound to the service n/w, then send ConfigResp
                   not accepting any values so that the default is used.
09/11/01   dna     Make config req processing consistent with hdrstream.c
08/17/01   vas     Changes for messages to be sent for correct streams
08/01/01   dna     Changed config_req message to not send a response if we 
                   aren't sure what the AN intended
07/10/01   dna     Change LUP to not do AT initiated negotiation
06/06/01   dna     new param for hmp_send_msg()
03/25/01   dna     Created module

===========================================================================*/

/* <EJECT> */ 
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "hdr_variation.h"
#include "err.h"
#include "hdrerrno.h"
#include "hdrlupc.h"
#include "hdrind.h"
#include "hdrutil.h"
#include "hdrscp.h"
#include "amssassert.h"
#include "hdrbit.h"
#include "hdrstream.h"
#include "hdrscmdb.h"

/* <EJECT> */ 
/*===========================================================================

            DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains definitions for constants, macros, types,
variables and other items needed by this module.

===========================================================================*/

#define HDRLUPC_DEFAULT_RAN_HANDOFF FALSE

#define HDRLUPC_BITS_PER_BYTE 8

#define HDRLUPC_UNPACK_CON_REQ_RAN_HANDOFF( ptr, val_ptr, aoffset, voffset )  \
          HDRBIT_UNPACK8( (ptr), (val_ptr), ((aoffset) + (voffset) + 0), 8 )

/* Length of Config Response  */
#define HDRLUPC_PACK_CON_RSP_LEN( ptr, val, offset )  \
          HDRBIT_PACK8( (ptr), (val), ((offset) + 0), 8 )

/* Default Packet App */

/* Attribute ID  */
#define HDRLUPC_PACK_CON_RSP_DEFAULT_ATTRIB_ID( ptr, val, offset )  \
          HDRBIT_PACK8( (ptr), (val), ((offset) + 8), 8 )

/* Value or Value ID (complex attributes) */
#define HDRLUPC_PACK_CON_RSP_DEFAULT_VALUE( ptr, val, offset )  \
          HDRBIT_PACK8( (ptr), (val), ((offset) + 16), 8 )

#define HDRLUPC_CON_RSP_DEFAULT_ATTRIB_LEN (HDRLUPC_ATTRIB_LEN_SIZE      + \
                                    HDRLUPC_DEFAULT_ATTRIB_ID_SIZE       + \
                                    HDRLUPC_ATTRIB_VAL_SIZE)
                                    
/* Default attribute length in bytes, not including the length field */
#define HDRLUPC_CON_RSP_DEFAULT_ATTRIB_LEN_BYTES \
          ((HDRLUPC_DEFAULT_ATTRIB_ID_SIZE + HDRLUPC_ATTRIB_VAL_SIZE) \
            / HDRLUPC_BITS_PER_BYTE)


#ifdef FEATURE_HDR_QOS
/* Multiflow Packet App */

/* Attribute ID  */
#define HDRLUPC_PACK_CON_RSP_MULTIFLOW_ATTRIB_ID( ptr, val, offset )  \
          HDRBIT_PACK16( (ptr), (val), ((offset) + 8), 16 )

/* Value or Value ID (complex attributes) */
#define HDRLUPC_PACK_CON_RSP_MULTIFLOW_VALUE( ptr, val, offset )  \
          HDRBIT_PACK8( (ptr), (val), ((offset) + 24), 8 )

#define HDRLUPC_CON_RSP_MULTIFLOW_ATTRIB_LEN (HDRLUPC_ATTRIB_LEN_SIZE      + \
                                    HDRLUPC_MULTIFLOW_ATTRIB_ID_SIZE       + \
                                    HDRLUPC_ATTRIB_VAL_SIZE)

/* Multiflow attribute length in bytes, not including the length field */
#define HDRLUPC_CON_RSP_MULTIFLOW_ATTRIB_LEN_BYTES \
          ((HDRLUPC_MULTIFLOW_ATTRIB_ID_SIZE + HDRLUPC_ATTRIB_VAL_SIZE) \
           / HDRLUPC_BITS_PER_BYTE)
#endif /* FEATURE_HDR_QOS */

LOCAL struct
{
  hdrlupc_config_params_type in_config;  /* Used while processing config msg */
  hdrlupc_config_params_type current;    /* Current configuration */
  hdrlupc_config_params_type negotiated; /* Negotiated configuration */

  hdrlupc_config_result_type config_result;

} hdrlupc;

/*===========================================================================

                    LOCAL FUNCTION DECLARATION
           
===========================================================================*/
LOCAL void hdrlupc_process_ran_handoff_attrib
( 
  dsm_item_type *item_ptr,
  uint16 attrib_val_offset,
  uint16 attrib_len, 
  uint16 attrib_offset
);

/* <EJECT> */
/*===========================================================================

FUNCTION HDRLUPC_POWERUP_INIT

DESCRIPTION
  This function performs the initialization of the Location Update 
  configuration negotiation module.  This funciton initializes all elements
  of the current and negotiated instances of hdrlupc_config_params_type.

DEPENDENCIES
  This function must called only once after powerup

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  All elements of the current and negotiated instances of 
  hdrlupc_config_params_type are initialized.

===========================================================================*/
void hdrlupc_powerup_init ( void )
{

  hdrlupc.config_result.ran_ho.is_accepted = FALSE;
   
  /* Get the current configuration parameters from SCMDB */
  hdrscmdb_get_inuse_config(HDRHAI_LOCATION_UPDATE_PROTOCOL, &hdrlupc.current);

  ASSERT(( hdrlupc.current.unsolicited_is_allowed == TRUE ) ||
         ( hdrlupc.current.unsolicited_is_allowed == FALSE ));
} /* hdrlupc_powerup_init() */

/* <EJECT> */
/*===========================================================================
FUNCTION HDRLUPC_CONFIG_INIT_DEFAULT

DESCRIPTION
  Initializes the in-config values to "negotiated"
                          
DEPENDENCIES
  None.

PARAMETERS
  None.
    
RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void hdrlupc_config_init(void)
{
#ifdef FEATURE_MEMCPY_REMOVAL
  hdrlupc.in_config = hdrlupc.negotiated;
#else
  memcpy( &hdrlupc.in_config, 
          &hdrlupc.negotiated, 
          sizeof(hdrlupc_config_params_type) );  
#endif /* FEATURE_MEMCPY_REMOVAL */

  hdrlupc.config_result.ran_ho.is_accepted = FALSE;
  hdrlupc.config_result.ran_ho.value = 0;

} /* hdrmrlpc_config_init() */

/* <EJECT> */
/*===========================================================================

FUNCTION HDRLUPC_PROCESS_DEFAULT_RAN_HO_ATTRIB

DESCRIPTION
  This function processes the RAN Handoff attribute of a Configuration 
  Request message for the Default Packet Application.

DEPENDENCIES
  None

PARAMETERS
  item_ptr - Pointer to the DSM item chain holding the message
  attrib_len - Length, in bits, of the attribute, including the length field
  attrib_offset - Bit offset of the attribute from the beginning of the message

RETURN VALUE
  None
  
SIDE EFFECTS
  None

===========================================================================*/
void hdrlupc_process_default_ran_ho_attrib
( 
  dsm_item_type *item_ptr,
  uint16 attrib_len, 
  uint16 attrib_offset
)
{
  /* If we've processed this parameter successfully in the current round
     of negotiation, we don't process it again */
  if (hdrlupc.config_result.ran_ho.is_accepted)
  {
    return;
  }

  hdrlupc_process_ran_handoff_attrib(
    item_ptr,
    HDRLUPC_ATTRIB_LEN_SIZE + HDRLUPC_DEFAULT_ATTRIB_ID_SIZE,
    attrib_len,
    attrib_offset);

} /* hdrlupc_process_default_ran_ho_attrib() */

#ifdef FEATURE_HDR_QOS
/*===========================================================================
FUNCTION HDRLUPC_PROCESS_MULTIFLOW_RAN_HO_ATTRIB

DESCRIPTION
  This function processes the RAN Handoff attribute of a Configuration 
  Request message for the Multiflow packet Application.

DEPENDENCIES
  None

PARAMETERS
  item_ptr - Pointer to the DSM item chain holding the message
  attrib_len - Length, in bits, of the attribute, including the length field
  attrib_offset - Bit offset of the attribute from the beginning of the message
  negotiated_values_ptr - Pointer to struct to put any negotiated values into

RETURN VALUE
  None
  
SIDE EFFECTS
  None

===========================================================================*/
void hdrlupc_process_multiflow_ran_ho_attrib
( 
  dsm_item_type *item_ptr,
  uint16 attrib_len, 
  uint16 attrib_offset
)
{
  /* If we've processed this parameter successfully in the current round
     of negotiation, we don't process it again */
  if (hdrlupc.config_result.ran_ho.is_accepted)
  {
    return;
  }

  hdrlupc_process_ran_handoff_attrib(
    item_ptr,
    HDRLUPC_ATTRIB_LEN_SIZE + HDRLUPC_MULTIFLOW_ATTRIB_ID_SIZE,
    attrib_len,
    attrib_offset);

} /* hdrlupc_process_multiflow_ran_ho_attrib() */
#endif /* FEATURE_HDR_QOS */

/*===========================================================================

FUNCTION HDRLUPC_PROCESS_RAN_HANDOFF_ATTRIB

DESCRIPTION
  This function processes the RAN Handoff attribute of a Configuration 
  Request message.

DEPENDENCIES
  None

PARAMETERS
  item_ptr      - Pointer to the DSM item chain holding the message
  attrib_val_offset - Bit offset of current attribute value from the beginning 
                      of the current attribute
  attrib_len    - Length, in bits, of the attribute, including the length field
  attrib_offset - Bit offset of the attribute from the beginning of the message

RETURN VALUE
  None
  
SIDE EFFECTS
  None

===========================================================================*/
void hdrlupc_process_ran_handoff_attrib
( 
  dsm_item_type *item_ptr,
  uint16 attrib_val_offset,
  uint16 attrib_len, 
  uint16 attrib_offset
)
{
  uint8  attrib_val; /* A suggested value for the attribute */
  uint16 curr_attrib_val_offset = attrib_val_offset;

/*- - - - - - - - - - - - -- - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Loop through all the values given for this attribute. */
  while ((curr_attrib_val_offset + HDRLUPC_ATTRIB_VAL_SIZE) <=
         attrib_len)
  {
    /* There is at least one more attribute value in this attribute */

    if ( HDRLUPC_UNPACK_CON_REQ_RAN_HANDOFF( item_ptr, 
           &attrib_val, attrib_offset, curr_attrib_val_offset ) != E_SUCCESS )
    {
      ERR("Error extracting attrib_val from ConfigReq Msg", 0, 0, 0 );
      /* If we can't get this attribute value, there is no reason to
       * try to get more values for this attribute.
       */
      break;
    }

    /* Decide if we want to use this value */
    if ((attrib_val == 0) || (attrib_val == 1))
    {
      hdrlupc.in_config.unsolicited_is_allowed = TRUE;

      hdrlupc.config_result.ran_ho.value = attrib_val;
      hdrlupc.config_result.ran_ho.is_accepted = TRUE;
    }
    
    curr_attrib_val_offset += HDRLUPC_ATTRIB_VAL_SIZE;

    /* If we've accepted a value from this attribute list, no need to
       continue and look at the other ones (if any) */

    if (hdrlupc.config_result.ran_ho.is_accepted)
    {
      break;
    }

  }
  
  if (!hdrlupc.config_result.ran_ho.is_accepted)
  {
    ERR("Unexpected value for RAN Handoff", 0, 0, 0 );
  }

} /* hdrlupc_process_ran_handoff_attrib() */

/* <EJECT> */
/*===========================================================================

FUNCTION HDRLUPC_UNPACK_DEFAULT_CONFIG_REQ_ATTRIB

DESCRIPTION
  This function unpacks default Config Request attributes.

DEPENDENCIES
  None

PARAMETERS
  item_ptr      - Pointer to the DSM item chain holding the message
  attrib_len    - Length, in bits, of the attribute, including the length field
  attrib_offset - Bit offset of the attribute from the beginning of the message

RETURN VALUE
  TRUE  - Message unpacked successfully
  FALSE - otherwise

SIDE EFFECTS
  None

===========================================================================*/
boolean hdrlupc_unpack_default_config_req_attrib
( 
  dsm_item_type *item_ptr,
  uint16 attrib_len, 
  uint16 attrib_offset
)
{
  boolean send_rsp;

/*- - - - - - - - - - - - -- - - - - - - - - - - - - - - - - - - - - - - - -*/

  send_rsp = TRUE;

  if (attrib_len >= HDRLUPC_DEFAULT_RAN_HO_ATTRIB_SIZE)
  {
     hdrlupc_process_default_ran_ho_attrib( item_ptr, 
                                            attrib_len, 
                                            attrib_offset );
  }
  else
  {
    ERR("Not enough bits for RAN Handoff attribute",0,0,0);
    /* If the AN didn't include enough bits for at least one complete
     * attribute value we don't know what the AN intended, so it is 
     * safest not to send a response.
     */
    send_rsp = FALSE;
  }

  return send_rsp;


} /* hdrlupc_unpack_default_config_req_attrib() */

/* <EJECT> */
/*===========================================================================

FUNCTION HDRLUPPC_PACK_DEFAULT_CONFIG_RSP_ATTRIBS

DESCRIPTION
  This function packs the Multiflow RAN Handoff attributes in a Configuration 
  Response Message (AN INIT)
  
DEPENDENCIES
  None

PARAMETERS
  item_ptr - Pointer to the DSM item chain holding the message
  attrib_offset - Bit offset of the attribute from the beginning of the message

RETURN VALUE
  (attrib_offset is updated)
  
SIDE EFFECTS
  None

===========================================================================*/
void hdrlupc_pack_default_config_rsp_attribs
(
  dsm_item_type *item_ptr,
  uint16 *attrib_offset
)
{
  if (hdrlupc.config_result.ran_ho.is_accepted)
  {
    HDRLUPC_PACK_CON_RSP_LEN(item_ptr, 
                             HDRLUPC_CON_RSP_DEFAULT_ATTRIB_LEN_BYTES, 
                             *attrib_offset);
    HDRLUPC_PACK_CON_RSP_DEFAULT_ATTRIB_ID(
                             item_ptr,
                             HDRLUPC_RAN_HANDOFF_DEFAULT_ATTRIB_ID,
                             *attrib_offset);
    HDRLUPC_PACK_CON_RSP_DEFAULT_VALUE(
                             item_ptr,
                             hdrlupc.config_result.ran_ho.value,
                             *attrib_offset);
    *attrib_offset += HDRLUPC_CON_RSP_DEFAULT_ATTRIB_LEN;
  }

  /* Temporary in-config value is now negotiated */
#ifdef FEATURE_MEMCPY_REMOVAL
  hdrlupc.negotiated = hdrlupc.in_config;
#else
  memcpy( &hdrlupc.negotiated, 
          &hdrlupc.in_config, 
          sizeof(hdrlupc_config_params_type) ); 
#endif /* FEATURE_MEMCPY_REMOVAL */

} /* hdrlupc_pack_default_config_rsp_attribs() */

#ifdef FEATURE_HDR_QOS
/* <EJECT> */
/*===========================================================================

FUNCTION HDRLUPC_UNPACK_MULTIFLOW_CONFIG_REQ_ATTRIB

DESCRIPTION
  This function unpacks Config Request Attributes for the Multiflow Packet App.

DEPENDENCIES
  None

PARAMETERS
  item_ptr      - Pointer to the DSM item chain holding the message
  attrib_len    - Length, in bits, of the attribute, including the length field
  attrib_offset - Bit offset of the attribute from the beginning of the message

RETURN VALUE
  TRUE  - Message unpacked successfully
  FALSE - otherwise

SIDE EFFECTS
  None

===========================================================================*/
boolean hdrlupc_unpack_multiflow_config_req_attrib
( 
  dsm_item_type *item_ptr,
  uint16 attrib_len, 
  uint16 attrib_offset
)
{
  boolean send_rsp;

/*- - - - - - - - - - - - -- - - - - - - - - - - - - - - - - - - - - - - - -*/

  send_rsp = TRUE;

  if (attrib_len >= HDRLUPC_MULTIFLOW_RAN_HO_ATTRIB_SIZE)
  {
     hdrlupc_process_multiflow_ran_ho_attrib(
          item_ptr, attrib_len, attrib_offset );

  }
  else
  {
    ERR("Not enough bits for RAN Handoff attribute",0,0,0);
    /* If the AN didn't include enough bits for at least one complete
     * attribute value we don't know what the AN intended, so it is 
     * safest not to send a response.
     */
    send_rsp = FALSE;
  }

  return send_rsp;


} /* hdrlupc_unpack_multiflow_config_req_attrib() */

/* <EJECT> */
/*===========================================================================

FUNCTION HDRLUPPC_PACK_MF_CONFIG_RSP_ATTRIBS

DESCRIPTION
  This function packs the Multiflow RAN Handoff attributes in a Configuration 
  Response Message (AN INIT)
  
DEPENDENCIES
  None

PARAMETERS
  item_ptr - Pointer to the DSM item chain holding the message
  attrib_offset - Bit offset of the attribute from the beginning of the message

RETURN VALUE
  (attrib_offset is updated)
  
SIDE EFFECTS
  None

===========================================================================*/
void hdrlupc_pack_mf_config_rsp_attribs
(
  dsm_item_type *item_ptr,
  uint16 *attrib_offset
)
{
  if (hdrlupc.config_result.ran_ho.is_accepted)
  {
    HDRLUPC_PACK_CON_RSP_LEN(item_ptr, 
                             HDRLUPC_CON_RSP_MULTIFLOW_ATTRIB_LEN_BYTES, 
                             *attrib_offset);
    HDRLUPC_PACK_CON_RSP_MULTIFLOW_ATTRIB_ID(
                             item_ptr,
                             HDRLUPC_RAN_HANDOFF_MULTIFLOW_ATTRIB_ID,
                             *attrib_offset);
    HDRLUPC_PACK_CON_RSP_MULTIFLOW_VALUE(
                             item_ptr,
                             hdrlupc.config_result.ran_ho.value,
                             *attrib_offset);
    *attrib_offset += HDRLUPC_CON_RSP_MULTIFLOW_ATTRIB_LEN;
  }

  /* Temporary in-config value is now negotiated */
#ifdef FEATURE_MEMCPY_REMOVAL
  hdrlupc.negotiated = hdrlupc.in_config;
#else
  memcpy( &hdrlupc.negotiated, 
          &hdrlupc.in_config, 
          sizeof(hdrlupc_config_params_type) );  
#endif /* FEATURE_MEMCPY_REMOVAL */

} /* hdrlupc_pack_mf_config_rsp_attribs() */
#endif /* FEATURE_HDR_QOS */

/* <EJECT> */
/*===========================================================================

FUNCTION HDRLUPC_PROCESS_IND

DESCRIPTION
  This function processes indications given to LUPC.

DEPENDENCIES
  None

PARAMETERS
  ind_name     - Name of the indication for LUP to process
  ind_data_ptr - Any data associated with the indication
  
RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void hdrlupc_process_ind (hdrind_ind_name_enum_type ind_name,
        hdrind_ind_data_union_type * ind_data_ptr)
{

  switch (ind_name)
  {
    case HDRIND_SCM_SESSION_CHANGED:
      /* Update the local copy of current configuration from SCMDB */   
      hdrscmdb_get_inuse_config(HDRHAI_LOCATION_UPDATE_PROTOCOL, 
                                &hdrlupc.current);

      break;

    default:
      ERR_FATAL( "Unknown indication", 0, 0, 0 );
      break;
  }
} /* hdrlupc_process_ind() */

/* <EJECT> */
/*===========================================================================

FUNCTION HDRLUPC_UNSOLICITED_IS_ALLOWED

DESCRIPTION
  This function tells whether unsolicited locaiton notification is allowed
  by the current configuration.

DEPENDENCIES
  None

PARAMETERS
  None
  
RETURN VALUE
  TRUE if the current configuration allows the AT to send an unsolicited
  LocationNotification message, FALSE otherwise.

SIDE EFFECTS
  None

===========================================================================*/
boolean hdrlupc_unsolicited_is_allowed ( void )
{
  
  return hdrlupc.current.unsolicited_is_allowed;

} /* hdrlupc_unsolicited_is_allowed() */

/*===========================================================================

FUNCTION HDRLUPC_SET_CONFIG_TO_DEFAULT

DESCRIPTION
  This function sets Location Update Protocol's configuration parameters
  to default values. 

DEPENDENCIES
  None

PARAMETERS
  current_config - Pointer to Location Update Protocol's current configuration
                   structure in SCMDB.
                   
RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void hdrlupc_set_config_to_default
( 
  hdrlupc_config_params_type *current_config 
)
{
  current_config->unsolicited_is_allowed = HDRLUPC_DEFAULT_RAN_HANDOFF;
} /* hdrlupc_set_config_to_default() */

/*===========================================================================

FUNCTION HDRLUPC_GET_INCONFIG_HANDLE

DESCRIPTION
  This function returns a pointer to inconfig instance of LUP

DEPENDENCIES
  None

PARAMETERS
  None
                     
RETURN VALUE
  Pointer to inconfig instance of LUP

SIDE EFFECTS
  None

===========================================================================*/
void* hdrlupc_get_inconfig_handle(void)
{
  return (void*) &hdrlupc.negotiated;
} /* hdrlupc_get_inconfig_handle() */


/*===========================================================================
FUNCTION HDRLUPC_GET_CONFIG_RESULT_HANDLE

DESCRIPTION
  This function returns the pointer to the config result of LUP. 

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  Pointer to the config result.

SIDE EFFECTS
  None.
===========================================================================*/
void* hdrlupc_get_config_result_handle( void )
{
  return (void*)&hdrlupc.config_result;
} /* hdrlupc_get_config_result_handle */
