/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

        S H A - 1   A U T H E N T I C A T I O N   P R O T O C O L   
                    C O N F I G U R A T I O N

GENERAL DESCRIPTION
  This module contains code to perform the functions of the SHA-1 
  Authentication Protocol as defined in the security layer of IS-856.

EXTERNALIZED FUNCTIONS (Global)
   hdrauth_powerup_init - One time powerup initialization

EXTERNALIZED FUNCTIONS (Regional)
   hdrauth_process_msg      - Handle messages pertaining to configuration
   hdrauth_process_ind      - Handle indications pertaining to configuration

IDLEIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2001 - 2014 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* <EJECT> */ 
/*===========================================================================

                        EDIT HISTORY FOR MODULE

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/hdr/cp/src/hdrauth.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/09/14   cnx     Support NV refresh without reset.
12/10/13   sat     Changed memcpy() to assignment or memscpy() (safer version).
10/29/13   sat     Reduced F3 messages (or/and) ASSERTs
03/19/12   cnx     Resolved TCB compiler errors for Dime. 
10/12/11   cnx     Retrieve CDMA system from LTE in the tunnel mode.
10/10/11   cnx     Retrieve CDMA system from LTE in the tunnel mode.
08/02/10   pba     Replaced seccrypt with secapi (as seccrypt.h is deprecated).
03/23/09   wsh     CMI: featurized CMI under FEATURE_CMI
02/18/09   wsh     Updated file to include private header hdrmc_v.h
12/10/08   smd     Replaced assert.h with amssassert.h
10/01/08   wsh     Fixed critical/high lint warnings
09/11/07   wsh     Removed lint high warnings
04/17/07   wsh     Added tracing to report SHA-1 calculation time (previous fix
                   didn't work)
04/03/07   wsh     Fixed error where dsm_item is not freed
03/27/07   wsh     Added support for new prior session handling
02/13/07   wsh     Added tracing to report SHA-1 calculation time
11/30/06   hal     Fixed cryptosync issue during prior session negotiation
11/27/06   mpa     (for om) Use ARM for SHA-1 computation to avoid access probe delays
09/08/06   hal     Changed calls to hdrutil_new_msg_buffer() to hdrutil_new_dsm_buffer()
06/26/06   pba     Used the new API to get the sector_id from the overhead
                   information, even if it is not marked as being current.
06/12/06   mpa     Updates for 7500 RPCing of HDR rev0
04/27/06   hal     Pulled in Don's changes to sector_id values during previous
                   session negotiation
04/26/06   hal     Fixed cryptosync value during previous session negotiation
04/07/06   hal     Changed hdrutil_new_buffer() to use hdrutil_new_msg_buffer()
                   Changed dsm_pushdown_tail() to use hdrutil_pushdown_tail()
01/26/06   mpa     Pack the LSB of ACPAC in the prior session data.
12/13/05   pba     Changes to support multiple personality negotiation
11/14/05   pba     Accept only ACAuthKeyLength values less than or equal to 160
10/04/05   pba     Added support to read/write config parameters from SCMDB/EFS
04/04/05   pba     Removed Lint errors
03/24/05   pba     Lint cleanup
12/07/04   hal     Changed dsm_offset_new_buffer to hdrutil_new_buffer.
09/24/04   mpa     (for om) Merged in new SEC api.
09/10/03   mpa     Converted F3 messages to use new MSG2.0 SSID
09/12/02   om      Updated seccrypt_delete() calls for double ref.
06/21/02   om      Added registration of hdrmc_wait() to SEC.
06/20/02   om      Added registration of hdrtx_wait() to SEC to avoid
                   watchdog timeouts, removed call to seccrypt_init().
04/29/02   vas     Changed ERR_FATAL to ERR if unknown MsgID received.
03/21/02   om      Optimized auth key padding
03/20/02   om      Fixed bit/byte mismatch in variable keylength
03/19/02   om      Fixed get_acpac() to differentiate prev. & curr. session
03/18/02   nmw     Fixed NV interaction
03/08/02   om      Fixed variable size AuthKeyLength
12/06/01   om      Fixed some MSG_HIGH to MSG_LOW
11/29/01   om      First integrated version
10/19/01   dna     Created module

===========================================================================*/

/* <EJECT> */ 
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "hdr_variation.h"
#include "err.h"
#include "nv.h"
#include "amssassert.h"
#include "secerrno.h"
#include "secapi.h"
#ifdef FEATURE_HDR_SEC_DEBUG
#error code not present
#endif /* FEATURE_HDR_SEC_DEBUG */
#include "sys.h"

#ifdef FEATURE_MEMCPY_REMOVAL
#include "stringl.h"
#endif /* FEATURE_MEMCPY_REMOVAL */

#ifdef FEATURE_CMI
#include "sys_v.h"
#endif


#include "hdrdebug.h"

#ifdef FEATURE_CMI
#include "hdrmc_v.h"
#endif

#include "hdrmci.h"
#include "hdrerrno.h"
#include "hdrsec.h"
#include "hdrseci.h"
#include "hdrkep.h"
#include "hdrhmp.h"
#include "hdrpcp.h"
#include "hdrind.h"
#include "hdrutil.h"
#include "hdrscp.h"
#include "hdrbit.h"
#include "hdrtrace.h"
#include "hdrts.h"
#ifdef FEATURE_HDRMC_DEBUG
#error code not present
#endif

#include "hdrauth.h"
#include "hdrauthi.h"

#include "nv.h"
#include "hdrscmdb.h"

#ifdef FEATURE_LTE_TO_HDR_OH
#include "hdrsap.h"
#endif /* FEATURE_LTE_TO_HDR_OH */


/* <EJECT> */ 
/*===========================================================================

            DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains defidleions for constants, macros, types,
variables and other items needed by this module.

===========================================================================*/

#define HDRAUTH_DEFAULT_AC_AUTH_KEY_LEN           0x00A0
#define HDRAUTH_BITS_PER_BYTE                     8
#define HDRAUTH_AC_AUTH_KEY_LEN_ATTRIB_ID         0x00

#define HDRAUTH_MSG_ID_SIZE                       8
#define HDRAUTH_TRANSACTION_ID_SIZE               8
#define HDRAUTH_ATTRIB_LEN_SIZE                   8
#define HDRAUTH_ATTRIB_ID_SIZE                    8
#define HDRAUTH_AC_AUTH_KEY_LEN_ATTRIB_VAL_SIZE   16

#define HDRAUTH_SHA1_DIGEST_LEN                   20

/* The minimum attribute has Length, attribute ID and one value */
#define HDRAUTH_AC_AUTH_KEY_LEN_MIN_ATTRIB_LEN \
        ( HDRAUTH_ATTRIB_LEN_SIZE + \
          HDRAUTH_ATTRIB_ID_SIZE + \
          HDRAUTH_AC_AUTH_KEY_LEN_ATTRIB_VAL_SIZE )

/* The smallest attribute is a length with an AttributeID */
#define HDRAUTH_MIN_ATTRIB_LEN \
        ( HDRAUTH_ATTRIB_LEN_SIZE + \
          HDRAUTH_ATTRIB_ID_SIZE )

/* ---------------------------------------
 * Packing and unpacking for common fields 
 * --------------------------------------- */

/* Message ID  */
#define HDRAUTH_UNPACK_MSG_ID( ptr, val_ptr )  \
        HDRBIT_UNPACK8( (ptr), (val_ptr), 0, 8 )

/* Transaction ID */
#define HDRAUTH_UNPACK_TRANSACTION_ID( ptr, val_ptr )  \
          HDRBIT_UNPACK8( (ptr), (val_ptr), 8, 8 )

/* Message ID  */
#define HDRAUTH_PACK_MSG_ID( ptr, val )  \
          HDRBIT_PACK8( (ptr), (val), 0, 8 )

/* Transaction ID  */
#define HDRAUTH_PACK_TRANSACTION_ID( ptr, val )  \
          HDRBIT_PACK8( (ptr), (val), 8, 8 )

/* ------------------------
 * Unpacking Config Request
 * ------------------------ */
/* Attribute fields are offset from the beginning of the attribute record */
#define HDRAUTH_UNPACK_CON_REQ_LEN( ptr, val_ptr, aoffset )  \
          HDRBIT_UNPACK8( (ptr), (val_ptr), ((aoffset) + 0), 8 )

#define HDRAUTH_UNPACK_CON_REQ_ATTRIBUTE_ID( ptr, val_ptr, aoffset )  \
          HDRBIT_UNPACK8( (ptr), (val_ptr), ((aoffset) + 8), 8 )

#define HDRAUTH_UNPACK_CON_REQ_AC_AUTH_KEY_LEN( ptr, val_ptr, aoffset, voffset )  \
          HDRBIT_UNPACK16( (ptr), (val_ptr), ((aoffset) + (voffset) + 0), 16 )

/* -----------------------
 * Packing Config Response
 * ----------------------- */
/* Config response attributes are always just Attribute ID, AttributeValue */
#define HDRAUTH_CRSP_ATTRIB_VAL_SIZE   16
#define HDRAUTH_CRSP_ATTRIB_LEN        ( HDRAUTH_ATTRIB_LEN_SIZE + \
                                         HDRAUTH_ATTRIB_ID_SIZE + \
                                         HDRAUTH_CRSP_ATTRIB_VAL_SIZE )

/* The field in the config response is length in bytes,
 * excluding the length of length field.
 */
#define HDRAUTH_CRSP_ATTRIB_LEN_BYTES  ((HDRAUTH_ATTRIB_ID_SIZE + \
          HDRAUTH_CRSP_ATTRIB_VAL_SIZE) / HDRAUTH_BITS_PER_BYTE)        

/* Length of Config Response  */
#define HDRAUTH_PACK_CON_RSP_LEN( ptr, val, offset )  \
          HDRBIT_PACK8( (ptr), (val), ((offset) + 0), 8 )

/* Attribute ID  */
#define HDRAUTH_PACK_CON_RSP_ATTRIBUTE_ID( ptr, val, offset )  \
          HDRBIT_PACK8( (ptr), (val), ((offset) + 8), 8 )

/* Value */
#define HDRAUTH_PACK_CON_RSP_VALUE( ptr, val, offset )  \
          HDRBIT_PACK16( (ptr), (val), ((offset) + 16), 16 )

struct
{
  hdrauth_config_params_type current; /* Current configuration */
  hdrauth_config_params_type negotiated; /* Negotiated configuration */
} hdrauth;

/* Forward declaration of the HDR Tx and MC task's wait function */
rex_sigs_type  hdrtx_wait( rex_sigs_type );

/* <EJECT> */
/*===========================================================================

FUNCTION HDRAUTH_POWERUP_INIT

DESCRIPTION
  This function performs the initialization of the Location Update 
  configuration negotiation module.  This funciton initializes all elements
  of the current and negotiated instances of hdrauth_config_params_type.

DEPENDENCIES
  This function must called only once after powerup

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  All elements of the current and negotiated instances of 
  hdrauth_config_params_type are initialized.

===========================================================================*/
void hdrauth_powerup_init( void )
{

  rex_tcb_type *tcb_ptr; /* temp ptr */

  /* Get current configuration parameters from SCMDB */
  hdrscmdb_get_inuse_config(HDRHAI_AUTHENTICATION_PROTOCOL, &hdrauth.current);

#ifdef FEATURE_SEC
  if ( hdrmc_is_first_init() )
  {
    /* Register the TCBs we can call SEC services from */
    tcb_ptr = rcinit_lookup_rextask("hdrtx");
    (void) secapi_register_wait_func( tcb_ptr, hdrtx_wait );
    (void) secapi_register_wait_func( HDRMC_TASK_PTR, hdrmc_wait );
  }
#endif /* FEATURE_SEC */

} /* hdrauth_powerup_init() */

/* <EJECT> */
/*===========================================================================

FUNCTION HDRAUTH_PROCESS_AC_AUTH_KEY_LEN_ATTRIB

DESCRIPTION
  This function processes the ACAuthKeyLength attribute of a Configuration 
  Request message.

DEPENDENCIES
  None

PARAMETERS
  item_ptr - Pointer to the DSM item chain holding the message
  attrib_len - Length, in bits, of the attribute, including the length field
  attrib_offset - Bit offset of the attribute from the beginning of the message
  accepted_key_len_ptr - Pointer to struct to put any negotiated key length in

RETURN VALUE
  TRUE if a new, acceptable value for the attribute was found, FALSE otherwise.

SIDE EFFECTS
  None

===========================================================================*/
LOCAL boolean hdrauth_process_ac_auth_key_len_attrib
( 
  dsm_item_type*  item_ptr,
  uint8           attrib_len, 
  uint16          attrib_offset,
  uint16*         accepted_key_len_ptr
)
{
  /* Bit offset of current attribute value from the beginning of the 
   * current attribute */
  uint16 attrib_val_offset; 
  uint16 attrib_val; /* A suggested value for the attribute */
  boolean val_is_acceptable = FALSE;  /* If we accept the attribute */
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* The first attribute value is after the attribute length and ID fields */
  attrib_val_offset = HDRAUTH_ATTRIB_LEN_SIZE + HDRAUTH_ATTRIB_ID_SIZE;

  /* Loop through all the values given for this attribute. */
  while ((attrib_val_offset + HDRAUTH_AC_AUTH_KEY_LEN_ATTRIB_VAL_SIZE) <=
         attrib_len)
  {
    /* There is at least one more attribute value in this attribute */

    if ( HDRAUTH_UNPACK_CON_REQ_AC_AUTH_KEY_LEN( item_ptr, 
           &attrib_val, attrib_offset, attrib_val_offset ) != E_SUCCESS )
    {
      ERR("Error extracting attrib_val from ConfigReq Msg", 0, 0, 0 );
      /* If we can't get this attribute value, there is no reason to
       * try to get more values for this attribute.
       */
      break;
    }

    /* Decide if we want to use this value */
    if (attrib_val <= 160)
    {
      /* Accept only ACAuthKeyLength <= 160 the reason being, "message digest"
       * computation is implemented in DSP, which doesn't accept a key larger
       * than 160 bits and also padding the key with zeros does not improve
       * the cryptographic strength of the result */
      *accepted_key_len_ptr = attrib_val;
      val_is_acceptable = TRUE;
      break;
    }

    attrib_val_offset += HDRAUTH_AC_AUTH_KEY_LEN_ATTRIB_VAL_SIZE;
  }

  return (val_is_acceptable);

} /* hdrauth_process_ac_auth_key_len_attrib() */

/* <EJECT> */
/*===========================================================================

FUNCTION HDRAUTH_PROCESS_CONFIG_REQ_MSG

DESCRIPTION
  This function processes the Configuration Request message.

DEPENDENCIES
  None

PARAMETERS
  item_ptr - Pointer to the DSM item chain holding the message

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
LOCAL void hdrauth_process_config_req_msg
( 
  dsm_item_type*  item_ptr
)
{
  int total_bits_remaining; /* Number of bits remaining in the message */
  uint8  creq_transaction_id; /* Transaction ID of the message */

  uint8  attrib_len; /* Bit length of attribute, including the length field */
  uint16 attrib_offset = 0; /* Bit offset of current attribute */
  uint8  attrib_id; /* ID field of attribute */

  /* If during processing we end up in a situation where we don't know what
   * attribute ID the AN included, or aren't sure if they meant to include
   * one, we can't send a response because we may be accepting to use the 
   * default value for an attribute that we are not actually using the 
   * default value for (e.g. if we negotiated a non-default value during
   * AT initiated negotiation).  
   */
  boolean send_response = TRUE;

  /* As we parse the message we update the negotiated parameters in a 
   * local copy as we agree to accept new values.  That way, if we 
   * later encounter an error and decide not to send a response, or if 
   * the same attribute is negotiated twice and we accept the first but
   * decline the second, we don't corrupt the real negotiated parameters.
   */

  /* For each simple attribute we need a flag if we are accpeting it
   * and the value we are accepting.
   */
  boolean accept_ac_auth_key_len = FALSE; 
  uint16 accepted_ac_auth_key_len_val = 0;

  /* For each complex attribute we need a flag if we are accpeting it,
   * the ValueID we are accepting, and a structure holding the accepted value.
   */


  dsm_item_type *send_msg_ptr; /* dsm buffer for sent message */
  /*- - - - - - - - - - - -- - - - - - - - - - - - - - - - - - - - - - - - -*/

  total_bits_remaining = (dsm_length_packet(item_ptr) * 
                               HDRAUTH_BITS_PER_BYTE );

  /* Extract the Transaction ID from the message */
  if( HDRAUTH_UNPACK_TRANSACTION_ID( item_ptr, &creq_transaction_id ) 
      != E_SUCCESS )
  {
    ERR("Error extracting TransactionID from ConfigReq Msg", 0, 0, 0 );
    /* Without the transaction ID, it makes no sense to send a response */
    return;
  }

  /* Initialize attrib_len so the initial attrib_offset will be
   * immediately after the transaction ID of the config request.
   */
  attrib_len = HDRAUTH_MSG_ID_SIZE + HDRAUTH_TRANSACTION_ID_SIZE; 

  /* Unpack the configuration attributes */

  /* While there are at least enough bits for the smallest attribute... */ 
  while ((total_bits_remaining - attrib_len) >= HDRAUTH_MIN_ATTRIB_LEN)
  {
    /* Move to the next attribute record */
    attrib_offset += attrib_len;
    total_bits_remaining -= attrib_len;

    /* Get the length of the new attribute */
    if( HDRAUTH_UNPACK_CON_REQ_LEN( item_ptr, &attrib_len, attrib_offset ) != 
        E_SUCCESS )
    {
      ERR("Error unpacking ConfigReq Msg", 0, 0, 0 );
      /* Without the length we can't continue to parse the message. */
      send_response = FALSE;
      break;
    }

    /* The AttributeLength field in the message is the attribute length in 
     * bytes, excluding the length of the length field.  For our purposes, 
     * we want attrib_len to be the total length of the attribute in bits, 
     * including the length field.
     */
    attrib_len *= HDRAUTH_BITS_PER_BYTE;
    attrib_len += HDRAUTH_ATTRIB_LEN_SIZE;

    /* Any attribute record must have at least an attribute ID */
    if ((attrib_len < HDRAUTH_MIN_ATTRIB_LEN) ||
        (attrib_len > total_bits_remaining ))
    {
      /* If the length is something unexpected, we can't be sure of what 
       * the AN intended and it is safest to not send a response
       */
      send_response = FALSE;
      break;
    }

    /* There is at least one more attribute record in the message */

    if( HDRAUTH_UNPACK_CON_REQ_ATTRIBUTE_ID( item_ptr, &attrib_id, 
        attrib_offset ) != E_SUCCESS )
    {
      ERR("Error extracting AttributeID from ConfigReq Msg", 0, 0, 0 );
      /* If we can't get the attribute_id, don't send a response */
      send_response = FALSE;
      break;
    }
    
    /* Process the attribute.  Note that if the same attribute appears 
     * multiple times in the message, the last one overwrites the others.
     */

    if (hdrscp_get_config_subtype( HDRHAI_AUTHENTICATION_PROTOCOL ) ==
        HDRSCP_SHA1_AUTH)
    {
      /* Handle attributes for SHA-1 Authentication Protocol */
      switch( attrib_id )
      {
        case HDRAUTH_AC_AUTH_KEY_LEN_ATTRIB_ID: 
          if (attrib_len >= HDRAUTH_AC_AUTH_KEY_LEN_MIN_ATTRIB_LEN)
          {
            accept_ac_auth_key_len = hdrauth_process_ac_auth_key_len_attrib( 
                    item_ptr, attrib_len, attrib_offset, 
                    &accepted_ac_auth_key_len_val );
          }
          else
          {
            ERR("Not enough bits for attribute %d",attrib_id,0,0);
            /* If the AN didn't include enough bits for at least one complete
             * attribute value we don't know what the AN intended, so it is 
             * safest not to send a response.
             */
            send_response = FALSE;
          }
          break;
  
        default:
          ERR("Unknown Attribute ID", 0, 0, 0 );
          break;
      }
    }
    else
    {
      /* There are no config attributes defined for any other subtypes
       * of this protocol.
       */
      ERR("Unknown Attribute ID", 0, 0, 0 );
    }

    if (!send_response)
    {
      /* No need to continue parsing once we decide not to send a response */
      break;
    }
  } /* while( there are more attributes in the config request ) */

  /* Form and send the Configuration Response message. */

  if (send_response)
  {
    /* get new buffer */
    send_msg_ptr = hdrutil_new_msg_buffer();
  
    /* pack fields */
    HDRAUTH_PACK_MSG_ID( send_msg_ptr, HDRAUTH_CONFIG_RESP_MSG);
    HDRAUTH_PACK_TRANSACTION_ID( send_msg_ptr, creq_transaction_id);
  
    /* Now pack in the attribute ID and value of all attributes we have 
     * flagged that we want to accept.
     */
  
    /* Re-use attrib_offset variable for message packing */
    attrib_offset = HDRAUTH_MSG_ID_SIZE + HDRAUTH_TRANSACTION_ID_SIZE;
  
    if (accept_ac_auth_key_len)
    {
      /* Include that we are accepting the ac_auth_key_len attribute */
      HDRAUTH_PACK_CON_RSP_LEN( send_msg_ptr, HDRAUTH_CRSP_ATTRIB_LEN_BYTES,
                             attrib_offset );
      HDRAUTH_PACK_CON_RSP_ATTRIBUTE_ID( send_msg_ptr, 
        HDRAUTH_AC_AUTH_KEY_LEN_ATTRIB_ID, attrib_offset );
      HDRAUTH_PACK_CON_RSP_VALUE( send_msg_ptr, accepted_ac_auth_key_len_val,
        attrib_offset );
  
      attrib_offset += HDRAUTH_CRSP_ATTRIB_LEN;

      /* Update our regional structure to reflect the newly accepted values.
       * This is done per attribute because if an attribute was included twice
       * and the second was not accepted, negotiated_values still has the
       * values from the first one, but those were not actually negotiated.
       */
      hdrauth.negotiated.ac_auth_key_len = accepted_ac_auth_key_len_val;
    }
  
    /* if (accept_some_other_attribute) ... */
  
    /* Send config response on RTC.  All config messages are reliable. 
     */
    hdrhmp_send_msg( HDRHAI_AUTHENTICATION_PROTOCOL,
                   HDRHAI_RTC,
                   TRUE,
                   HDRAUTH_MESSAGE_PRIORITY,
                   HDRHAI_IN_CONFIG, /* Instance of protocol sending message */
                   send_msg_ptr,
                   NULL,
                   NULL );

  } /* end if (send_response) */

} /* hdrauth_process_config_req_msg() */

/* <EJECT> */
/*===========================================================================

FUNCTION HDRAUTH_PROCESS_MSG

DESCRIPTION
  This function processes messages for AUTH.

DEPENDENCIES
  None

PARAMETERS
  msg_ptr - Pointer to the DSM item chain holding the message
  protocol_instance - Instance of the protocol the message is for

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void hdrauth_process_msg 
(
  dsm_item_type*                      msg_ptr, 
  hdrhai_protocol_instance_enum_type  protocol_instance
)
{
  uint8 msg_id;           /* Message identifier */
 /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ASSERT((protocol_instance == HDRHAI_IN_USE) ||
         (protocol_instance == HDRHAI_IN_CONFIG)); 

  /* MessageID is the first uint8 */
  if (HDRAUTH_UNPACK_MSG_ID(msg_ptr, &(msg_id)) != E_SUCCESS)
  {
    ERR( "Could not extract message_id", 0, 0, 0 );
    dsm_free_packet( &msg_ptr );
    return;
  }

  /* Currently only the in config instance receives any messages */
  if (protocol_instance != HDRHAI_IN_CONFIG)
  {
    ERR("Msg for wrong instance %d", msg_id, 0, 0 );
    dsm_free_packet( &msg_ptr );
    return;
  }

  HDRTRACE_MSG( HDRHAI_AUTHENTICATION_PROTOCOL, msg_id, 0 );
#ifdef FEATURE_HDRMC_DEBUG
  #error code not present
#endif

  if (hdrscp_get_config_subtype( HDRHAI_AUTHENTICATION_PROTOCOL ) ==
      HDRSCP_SHA1_AUTH)
  {
    /* Process messages for the in-config instance of the authentication
     * protocol, which is currently SHA-1 authentication.
     */
    switch (msg_id)
    {
      case HDRAUTH_CONFIG_REQ_MSG:
        hdrauth_process_config_req_msg( msg_ptr );
        break;
  
      default:
        ERR( "Unknown msg for SHA-1 auth", 0, 0, 0 );
        break;
    }
  }
  else /* (protocol subtype == HDRSCP_DEFAULT_AUTH ) */
  {
    switch (msg_id)
    {
      case HDRAUTH_CONFIG_REQ_MSG:
        hdrauth_process_config_req_msg( msg_ptr );
        break;
  
      default:
        ERR( "Unknown msg for default auth", 0, 0, 0 );
        break;
    }
  }
  dsm_free_packet(&msg_ptr);
#ifdef FEATURE_HDRMC_DEBUG
  #error code not present
#endif
} /* hdrauth_process_msg() */

/* <EJECT> */
/*===========================================================================

FUNCTION HDRAUTH_PROCESS_IND

DESCRIPTION
  This function processes indications given to AUTH.

DEPENDENCIES
  None

PARAMETERS
  ind_name     - Name of the indication for AUTH to process
  ind_data_ptr - Any data associated with the indication
  
RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void hdrauth_process_ind 
(
  hdrind_ind_name_enum_type    ind_name,        
  hdrind_ind_data_union_type*  ind_data_ptr
)
{
  SYS_ARG_NOT_USED(ind_data_ptr); /* remove this line when ind_data_ptr
                                   * is used */

  HDRTRACE_IND( HDRHAI_AUTHENTICATION_PROTOCOL, ind_name, 0 );
#ifdef FEATURE_HDRMC_DEBUG
  #error code not present
#endif
  switch (ind_name)
  {
    case HDRIND_SCM_SESSION_CHANGED:
      /* Update the local copy of current configuration from SCMDB */      
      hdrscmdb_get_inuse_config(HDRHAI_AUTHENTICATION_PROTOCOL,
                                &hdrauth.current);
      break;

    default:
      ERR_FATAL( "Unknown indication", 0, 0, 0 );
      break;
  }
#ifdef FEATURE_HDRMC_DEBUG
  #error code not present
#endif
} /* hdrauth_process_ind() */

/* <EJECT> */
/*===========================================================================

FUNCTION HDRAUTH_PROCESS_CMD

DESCRIPTION
  This function processes commands given to AUTH.

DEPENDENCIES
  None

PARAMETERS
  auth_cmd_ptr - Pointer to the command for AUTH to process

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void hdrauth_process_cmd 
(
  hdrauth_cmd_type*  auth_cmd_ptr
)
{
  switch (auth_cmd_ptr->name)
  {
    default:
      ERR_FATAL( "Unknown command", 0, 0, 0 );
      break;
  }
} /* hdrauth_process_cmd() */

/* <EJECT> */ 
/*===========================================================================
FUNCTION HDRAUTH_PROCESS_SIG

DESCRIPTION
  This function processes signals for AUTH.
  
DEPENDENCIES
  Only one signal is sent at a time, so only one bit of the sig input 
  parameter is set.  If multiple signals need to be delivered, they must
  be delivered by multiple calls.

PARAMETERS
  sig - The signal that belongs to this protocol and was set.  This is not
        a signal mask.  Only one bit of the value is set
  
RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void hdrauth_process_sig
( 
  rex_sigs_type sig 
)
{

  SYS_ARG_NOT_USED(sig); /* remove this line when sig is used */

#ifdef FEATURE_HDRMC_DEBUG
  #error code not present
#endif

  /* Signal not recognized. */
  ERR("Invalid protocol signal!", 0, 0, 0);

#ifdef FEATURE_HDRMC_DEBUG
  #error code not present
#endif
} /* hdrauth_process_sig() */

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRAUTH_CONFIG_MSG_CB

DESCRIPTION
  This function is registered to be called whenever a protocol gives a
  message to HDRAUTH in-config instance.

DEPENDENCIES
  None
  
PARAMETERS
  msg - Pointer to the DSM item chain holding the message

RETURN VALUE
  None
  
SIDE EFFECTS
  None

===========================================================================*/
void hdrauth_config_msg_cb
(
  dsm_item_type * msg_ptr 
)
{
  hdrmc_queue_msg( HDRHAI_AUTHENTICATION_PROTOCOL, HDRHAI_IN_CONFIG, msg_ptr );
} /* hdrauth_config_msg_cb */

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRAUTH_IND_CB

DESCRIPTION
  This function is registered to be called whenever a protocol gives an
  indication of interest to HDRAUTH.

DEPENDENCIES
  None
 
PARAMETERS
  ind_name - Indication to process, is a global enum for all
                    possible indications in HDR signaling
  ind_data - Optional data pointer for indication specific data
 
RETURN VALUE
  None
  
SIDE EFFECTS
  None

===========================================================================*/

void hdrauth_ind_cb
(
  hdrind_ind_name_enum_type  ind_name,
  void*                      ind_data_ptr 
)
{
  if (rex_self() == HDRAUTH_TASK_PTR)
  {
    hdrauth_process_ind(ind_name, (hdrind_ind_data_union_type *)ind_data_ptr);
  }
  else
  {
    hdrmc_queue_ind( HDRHAI_AUTHENTICATION_PROTOCOL, ind_name, ind_data_ptr );
  }
} /* hdrauth_ind_cb */

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRAUTH_GET_PREVIOUS_SESSION_DATA

DESCRIPTION
  This function performs the same SHA-1 hash used for the access channel
  but uses the Nonce value passed in as the payload instead of the access 
  channel packet.

          ---------- -------------------- ----------------
         |Cryptosync|        ACPAC       |    Security    |
         |   Short  |                    |      Nonce     |
          ---------- -------------------- ----------------
              16              64                 32           
       
DEPENDENCIES
  None
 
PARAMETERS
  security_nonce        - The random number to considered payload of an 
                          access channel message.
  sec_packet_len_ptr    - The length in octets of the security layer packet
  sec_packet_ptr        - The security layer packet consisting of the given
                          payload with the ACPAC and CryptosyncShort headers
  kep_protocol_subtype  - Subtype of key exchange active in prev. session
  sec_protocol_subtype  - Subtype of security prot. active in prev. session
  auth_protocol_subtype - Subtype of auth prot. active in prev. session
  send_on_tc            - The created data is to be sent on traffic channel
  
  Note that the protocol subtypes are kept as part of the session 
  configuration so they need to be given with this command in order 
  for the security layer to know what subtypes to use in the computation.
                      
RETURN VALUE
  None
  
SIDE EFFECTS
  None

===========================================================================*/
void hdrauth_get_previous_session_data 
(
  uint32  security_nonce,
  uint8*  sec_packet_len_ptr, 
  uint8*  sec_packet_ptr,
  uint16  sec_protocol_subtype,
  uint16  auth_protocol_subtype,
  boolean send_on_tc
)
{
  dsm_item_type*  acpac_ptr = NULL;
  dsm_item_type*  nonce_ptr = NULL;
  uint8           cryptosync[HDRSEC_CRYPTOSYNC_LEN];
  uint16          cryptosync_len;
  boolean         include_cryptosync = FALSE;
  uint8*          pkt_index;
  qword           sys_time; /* Cryptosync calculations rely on system time */
  int             index;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (auth_protocol_subtype != HDRSCP_SHA1_AUTH )
  {
    /* The previous session did not authenticate the AC, so there is nothing
     * to do.
     */
#ifdef FEATURE_MEMCPY_REMOVAL
    memscpy( sec_packet_ptr, sizeof(security_nonce), 
             &security_nonce, sizeof (security_nonce));
#else
    memcpy( sec_packet_ptr, &security_nonce, sizeof (security_nonce));
#endif /* FEATURE_MEMCPY_REMOVAL */
    *sec_packet_len_ptr = sizeof (security_nonce);
  }
  else
  {
    nonce_ptr = hdrutil_new_dsm_buffer();
    if ( nonce_ptr == NULL )
    {
      ERR_FATAL( "AUTH: Out of DSM items", 0, 0, 0 );
    }
#ifdef FEATURE_MEMCPY_REMOVAL
    memscpy( nonce_ptr->data_ptr, sizeof(security_nonce), 
             &security_nonce, sizeof( security_nonce ) );
#else
    memcpy( nonce_ptr->data_ptr, &security_nonce, sizeof( security_nonce ) );
#endif /* FEATURE_MEMCPY_REMOVAL */
    nonce_ptr->used = sizeof( security_nonce );

    /* Check the previous session's security subtype; if the previous subtype
       was default, then the cryptosync will be left as 0. Otherwise, the
       cryptosync value will be based off the current CDMA system time */
    if( sec_protocol_subtype == HDRSCP_DEFAULT_SEC )
    {
      memset( cryptosync, 0, HDRSEC_CRYPTOSYNC_LEN );
    }
    else if( sec_protocol_subtype == HDRSCP_GENERIC_SECURITY )
    {
      /* Update the cryptosync for this packet and change to network order */
#ifdef FEATURE_LTE_TO_HDR_OH
      if ( hdrsap_tunnel_mode_is_enabled( HDRHAI_AUTHENTICATION_PROTOCOL ) &&
           hdrscp_get_current_subtype( HDRHAI_SIGNALING_ADAPTATION_PROTOCOL ) 
             == HDRSCP_IRAT_SAP )
      {
        hdrsap_get_80ms_time( sys_time );
      }
      else
#endif /* FEATURE_LTE_TO_HDR_OH */
      {
        hdrts_get_80ms_time( sys_time );
      }

      for (index = 0; index <= 7; index++)
      {
        cryptosync[index] = QW_BYTE( sys_time, 7 - index );
      }

      /* get CryptoSync Short */
      include_cryptosync = TRUE;
    }

    /* Set the cryptosync length */
    cryptosync_len = HDRSEC_CRYPTOSYNC_LEN;

    /* Generate the ACPAC for this packet */
    if ( hdrauth_create_acpac( nonce_ptr, 
                               cryptosync,
                               cryptosync_len,
                               &acpac_ptr,
                               send_on_tc ) != E_SUCCESS )
    {
      *sec_packet_len_ptr = 0;
      return;
    }

    pkt_index = sec_packet_ptr;

    if (include_cryptosync)
    {
      /* Offset cryptosync by 6 to match up with cryptosync short 
         value used earlier */
#ifdef FEATURE_MEMCPY_REMOVAL
      memscpy( pkt_index, HDRSEC_CRYPTOSYNC_SHORT_LEN, cryptosync + 6, 
               HDRSEC_CRYPTOSYNC_SHORT_LEN );
#else
      memcpy( pkt_index, cryptosync + 6, HDRSEC_CRYPTOSYNC_SHORT_LEN );
#endif /* FEATURE_MEMCPY_REMOVAL */
      pkt_index += HDRSEC_CRYPTOSYNC_SHORT_LEN;
    }

#ifdef FEATURE_MEMCPY_REMOVAL
    memscpy( pkt_index, HDRAUTH_ACPAC_LEN, 
             acpac_ptr->data_ptr + (HDRAUTH_SHA1_DIGEST_LEN - HDRAUTH_ACPAC_LEN), 
             HDRAUTH_ACPAC_LEN );
#else 
    memcpy( pkt_index, 
            acpac_ptr->data_ptr + (HDRAUTH_SHA1_DIGEST_LEN - HDRAUTH_ACPAC_LEN), 
            HDRAUTH_ACPAC_LEN );
#endif /* FEATURE_MEMCPY_REMOVAL */

    pkt_index += HDRAUTH_ACPAC_LEN;

#ifdef FEATURE_MEMCPY_REMOVAL
    memscpy( pkt_index, sizeof(security_nonce), 
             &security_nonce, sizeof (security_nonce) );
#else
    memcpy( pkt_index, &security_nonce, sizeof (security_nonce) );
#endif /* FEATURE_MEMCPY_REMOVAL */

    /* Return the correct length of the computed security packet */
    *sec_packet_len_ptr = (include_cryptosync) ? 
                          HDRSEC_CRYPTOSYNC_SHORT_LEN + HDRAUTH_ACPAC_LEN + 
                          sizeof (security_nonce)  :
                          HDRAUTH_ACPAC_LEN + sizeof (security_nonce);
  }

  dsm_free_packet( &acpac_ptr );
  dsm_free_packet( &nonce_ptr );

} /* hdrauth_get_previous_session_data() */

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRAUTH_CREATE_ACPAC

DESCRIPTION
  This function generates the access channel authentication
  code for given message.

DEPENDENCIES
  None

PARAMETERS
  pkt_ptr        - Pointer to DSM item w/ the message to be authenticated
  cryptosync     - Pointer to byte array containing the cryptosync
  cryptosync_len - Length of cryptosync in bytes
  acpac_ptr      - Pointer to empty DSM item or NULL to put ACPAC into
  use_previous_session_context - Use keys from previous session context

RETURN VALUE
  HDR error code to indicate the outcome.

SIDE EFFECTS
  None

===========================================================================*/
hdrerrno_enum_type hdrauth_create_acpac
(
  dsm_item_type*   pkt_ptr,
  uint8*           cryptosync,
  uint16           cryptosync_len,
  dsm_item_type**  acpac_ptr,
  boolean          use_previous_session_context
)
{
  #ifndef FEATURE_SEC
  return E_NOT_SUPPORTED;
  #else
  hdrerrno_enum_type          err_num = E_SUCCESS;
  secapi_handle_type          sha_hash = NULL;
  secapi_hsh_param_data_type  sha_param;
  dsm_item_type*              hash_data1_ptr = NULL;
  dsm_item_type*              hash_data2_ptr = NULL;
  dsm_item_type*              tmp_ptr = NULL;
  uint16                      rpc_auth_key_size;
  uint16                      configured_rpc_auth_key_size;
  hdraddr_type                sector_id;
  uint8                       sector_subnet_mask[sizeof( hdraddr_type )];
  sys_link_type               current_link;
  qword                       start_time;
  qword                       current_time;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  
  hash_data1_ptr = hdrutil_new_dsm_buffer();
  hash_data2_ptr = hdrutil_new_dsm_buffer();
  if ( (hash_data1_ptr == NULL) || (hash_data2_ptr == NULL) )
  {
    ERR_FATAL( "HDRAUTH: Out of DSM items", 0, 0, 0 );
  }

  if ( *acpac_ptr && (*acpac_ptr)->used )
  {
    HDR_MSG_PROT (MSG_LEGACY_ERROR,  "AUTH: Stale data in ACPAC item");
    dsm_free_packet( acpac_ptr );
  }

  do
  {
    /* Get the access channel authentication key */
    if ( use_previous_session_context )
    {
      rpc_auth_key_size = 
        hdrkep_get_previous_rpc_auth_key( hash_data1_ptr->data_ptr );
      HDR_MSG_PROT_1 (MSG_LEGACY_MED,  "AUTH: Prev. ses. context is valid: %d",
                     rpc_auth_key_size != 0);
    }
    else
    {
      rpc_auth_key_size = 
        hdrkep_get_rpc_auth_key( hash_data1_ptr->data_ptr );
      HDR_MSG_PROT_1 (MSG_LEGACY_MED,  "AUTH: Curr. ses. context is valid: %d",
                     rpc_auth_key_size != 0);
    }

    if ( rpc_auth_key_size == 0 )
    {
      /* Use a default 0 key */
      HDR_MSG_PROT (MSG_LEGACY_HIGH,  "AUTH: RPC Auth Key not available");
      memset( hash_data1_ptr->data_ptr, 0, hash_data1_ptr->size );
      rpc_auth_key_size = HDRAUTH_DEFAULT_AC_AUTH_KEY_LEN /
                            HDRAUTH_BITS_PER_BYTE;
    }
    hash_data1_ptr->used += rpc_auth_key_size;

    /* Adjust the AC Auth Key length according to the negotiated value */
    configured_rpc_auth_key_size = 
      hdrauth.current.ac_auth_key_len / HDRAUTH_BITS_PER_BYTE;
    if ( configured_rpc_auth_key_size <= rpc_auth_key_size )
    {
      /* The AC Auth key is of equal or less length than    */
      /* the RPC Auth key - this should be the normal case. */
      hash_data1_ptr->data_ptr += (rpc_auth_key_size 
                                   - configured_rpc_auth_key_size);
      hash_data1_ptr->used -= (rpc_auth_key_size 
                               - configured_rpc_auth_key_size);
    }
    else
    {
      /* The AC Auth key needs to be padded with 0 */
      hash_data1_ptr->used = 
        MIN( configured_rpc_auth_key_size, hash_data1_ptr->size );
      configured_rpc_auth_key_size -= hash_data1_ptr->size;

      while( configured_rpc_auth_key_size > 0 )
      {
        tmp_ptr = hdrutil_new_dsm_buffer();
        if ( tmp_ptr == NULL )
        {
          ERR_FATAL( "HDRAUTH: Out of DSM items", 0, 0, 0 );
        }
        memset( tmp_ptr->data_ptr, 0, tmp_ptr->size );
        tmp_ptr->used = 
          MIN( configured_rpc_auth_key_size,
               tmp_ptr->size );
        configured_rpc_auth_key_size -= tmp_ptr->used;
        dsm_append( &hash_data1_ptr, &tmp_ptr );
      }
    }

    if (use_previous_session_context)
    {
      /* Section 7.4.6.1.5.1 of IS-856A says:
         The access terminal shall supply the Nonce, to the security layer of 
         the prior session as if the Nonce is the payload to be transmitted on 
         the Access Channel. The access terminal shall set all the unspecified 
         parameters needed by the protocols in the Security Layer to zero for 
         the purpose of generating this Security Layer Packet. */

      /* While in the connected state computing the SecurityPacket for the 
         PriorSession attribute, the SectorID is the "unspecified parameter"
         mentioned in the standard, because with multiple pilots possible in 
         the active set, no one sector can be chosen. */

      /* Set the sector ID to 0 */
      memset( sector_id, 0, sizeof( hdraddr_type ) );
    }
    else
    {
      /* Get the current sector ID and append it */
      if( hdrovhd_get_qc_link( &current_link, 
                               HDRHAI_AUTHENTICATION_PROTOCOL ) == E_SUCCESS )
      {
        if ( hdrovhd_get_sector_id2( sector_id,
                                     sector_subnet_mask,
                                     &current_link,
                                     HDRHAI_AUTHENTICATION_PROTOCOL ) 
             !=
             E_SUCCESS )
        {
          ERR( "AUTH: Sector ID not available", 0, 0, 0 );
          err_num = E_NOT_AVAILABLE;
          break;
        }
      } 
      else
      {
        err_num = E_NOT_AVAILABLE;
        break;
      }
    }

    (void) hdrutil_pushdown_tail( &hash_data2_ptr,
                           (uint8*) sector_id,
                           sizeof( hdraddr_type ) );

    /* Append the cryptosync */
    (void) hdrutil_pushdown_tail( &hash_data2_ptr,
                           cryptosync,
                           cryptosync_len );

    HDR_MSG_PROT(MSG_LEGACY_MED, "Create ACPAC: start SHA operation");
#ifdef FEATURE_LTE_TO_HDR_OH
    if ( hdrsap_tunnel_mode_is_enabled( HDRHAI_AUTHENTICATION_PROTOCOL ) &&
         hdrscp_get_current_subtype( HDRHAI_SIGNALING_ADAPTATION_PROTOCOL ) 
           == HDRSCP_IRAT_SAP )
    {
      hdrsap_get_80ms_time( start_time );
    }
    else
#endif /* FEATURE_LTE_TO_HDR_OH */
    {
      hdrts_get_80ms_time( start_time );
    }

    /* Allocate a new SHA instance from the crypto layer */
    err_num = (hdrerrno_enum_type) secapi_new( &sha_hash, SECAPI_SHA );
    if ( err_num != E_SUCCESS )
    {
      ERR( "AUTH: Could not allocate SHA instance", 0, 0, 0 );
      break;
    }
  
    /* Set the preferred platform to ARM to avoid access probe delays */
    sha_param.common.exec_platform = SECAPI_EXEC_PLATFORM_ARM;
    err_num = (hdrerrno_enum_type) secapi_hsh_set_param( sha_hash,
                                     SECAPI_SHA,
                                     SECAPI_EXEC_PLATFORM,
                                     &sha_param );
    if ( err_num != E_SUCCESS ) 
    {
      ERR( "AUTH: Could not set SECAPI_EXEC_PLATFORM_ARM", 0, 0, 0 );
      break;
    }

    /* Initialize the hash functions to hash input continuously */
    sha_param.mode = SECAPI_HSH_INPUT_MODE_CONTINUE;
    err_num = (hdrerrno_enum_type) secapi_hsh_set_param( sha_hash,
                                     SECAPI_SHA,
                                     SECAPI_HSH_INPUT_MODE,
                                     &sha_param );
    if ( err_num != E_SUCCESS ) 
    {
      ERR( "AUTH: Could not set SECAPI_HSH_INPUT_MODE_CONTINUE", 0, 0, 0 );
      break;
    }

    /* Calculate the SHA-1 hash over this data */
    err_num = (hdrerrno_enum_type) secapi_hsh_create_digest_dsm( sha_hash, 
                                         SECAPI_SHA,
                                         hash_data1_ptr, 
                                         NULL,
                                         dsm_length_packet(hash_data1_ptr),
                                         NULL );
    if ( err_num != E_SUCCESS )
    {
      ERR( "HDRAUTH: Could not create SHA digest (%d)", err_num, 0, 0 );
      break;
    }

    err_num = (hdrerrno_enum_type) secapi_hsh_create_digest_dsm( sha_hash, 
                                         SECAPI_SHA,
                                         pkt_ptr, 
                                         NULL,
                                         dsm_length_packet(pkt_ptr),
                                         NULL );
    if ( err_num != E_SUCCESS )
    {
      ERR( "HDRAUTH: Could not create SHA digest (%d)", err_num, 0, 0 );
      break;
    }

    sha_param.mode = SECAPI_HSH_INPUT_MODE_FINAL;
    err_num = (hdrerrno_enum_type) secapi_hsh_set_param( sha_hash,
                                     SECAPI_SHA,
                                     SECAPI_HSH_INPUT_MODE,
                                     &sha_param );
    if ( err_num != E_SUCCESS ) 
    {
      ERR( "AUTH: Could not set SECAPI_HSH_INPUT_MODE_FINAL", 0, 0, 0 );
      break;
    }

    err_num = (hdrerrno_enum_type) secapi_hsh_create_digest_dsm( sha_hash, 
                                         SECAPI_SHA,
                                         hash_data2_ptr, 
                                         NULL,
                                         dsm_length_packet(hash_data2_ptr),
                                         acpac_ptr );
    if ( err_num != E_SUCCESS )
    {
      ERR( "HDRAUTH: Could not create SHA digest (%d)", err_num, 0, 0 );
      break;
    }
#ifdef FEATURE_HDR_SEC_DEBUG
    #error code not present
#endif /* FEATURE_HDR_SEC_DEBUG */

#ifdef FEATURE_LTE_TO_HDR_OH
    if ( hdrsap_tunnel_mode_is_enabled( HDRHAI_AUTHENTICATION_PROTOCOL ) &&
         hdrscp_get_current_subtype( HDRHAI_SIGNALING_ADAPTATION_PROTOCOL ) 
           == HDRSCP_IRAT_SAP )
    {
      hdrsap_get_80ms_time( current_time );
    }
    else
#endif /* FEATURE_LTE_TO_HDR_OH */
    {
      hdrts_get_80ms_time( current_time );
    }
    HDR_MSG_PROT_1(MSG_LEGACY_MED, 
                  "ACPAC is created. Time taken to calculate SHA: %d ms",
                  (current_time-start_time)*80);
  }
  while( 0 );/*lint !e717 */ 

  /* Restore the original message and free temporary items */
  (void) secapi_delete( &sha_hash );
  dsm_free_packet( &hash_data1_ptr );
  dsm_free_packet( &hash_data2_ptr );

  return err_num;
  #endif /* FEATURE_SEC */
} /* hdrauth_create_acpac() */


/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRAUTH_GET_PKT

DESCRIPTION
  This function authenticates messages to be sent on the access channel.

DEPENDENCIES
  None

PARAMETERS
  ...
  
RETURN VALUE
  Length of AC packet in bytes

SIDE EFFECTS
  None

===========================================================================*/
uint16 hdrauth_get_pkt
(
  uint16                    max_bytes,
  hdrhai_channel_enum_type  channel,
  uint16                    time_since_prev_req,
  dsm_item_type**           pkt_ptr,
  boolean*                  is_fmt_b_ptr
)
{
  hdrerrno_enum_type  err_num;
  dsm_item_type*      acpac_ptr = NULL;
  uint16              pkt_len;
  uint16              prot_subtype = 0;
  uint8               cryptosync[HDRSEC_CRYPTOSYNC_LEN];
  uint16              cryptosync_len;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Only the AC Mac should call us as long as we don't */
  /* authenticate the dedicated channels.               */
  ASSERT( channel == HDRHAI_AC );

  prot_subtype = hdrscp_get_current_subtype( HDRHAI_AUTHENTICATION_PROTOCOL );

  /* Adjust the maximum message size for the authentication header */
  max_bytes = ( (prot_subtype == HDRSCP_SHA1_AUTH) ? 
                  max_bytes - HDRAUTH_ACPAC_LEN : max_bytes );

  /* Get the payload from PCP */
  pkt_len = hdrpcp_get_pkt( max_bytes,
                            channel,
                            time_since_prev_req,
                            pkt_ptr,
                            is_fmt_b_ptr );
  HDR_MSG_PROT_2 (MSG_LEGACY_LOW,  "AUTH: Rcvd %d/%d bytes from PCP", pkt_len, 
                 dsm_length_packet( *pkt_ptr ));

  /* Create the authentication header and prepend it */
  if ( prot_subtype == HDRSCP_SHA1_AUTH )
  {
    /* If HDRSEC doesn't provide a cryptosync, use a NULL one */
    if ( (cryptosync_len = hdrsec_get_cryptosync( cryptosync )) == 0 )
    {
      HDR_MSG_PROT (MSG_LEGACY_MED,  "AUTH: Got NULL cryptosync from SEC");
      memset( cryptosync, 0, HDRSEC_CRYPTOSYNC_LEN );
      cryptosync_len = HDRSEC_CRYPTOSYNC_LEN;
    }

    if ( pkt_len )
    {
      err_num = hdrauth_create_acpac( *pkt_ptr, 
                                     cryptosync, 
                                     cryptosync_len, 
                                     &acpac_ptr,
                                     FALSE );
      
      if ( err_num == E_SUCCESS )
      {
        /* Only use the 64 least significant bits from the digest */
        acpac_ptr->data_ptr += (HDRAUTH_SHA1_DIGEST_LEN - HDRAUTH_ACPAC_LEN);
        acpac_ptr->used -= (HDRAUTH_SHA1_DIGEST_LEN - HDRAUTH_ACPAC_LEN);

        /* Prepend the ACPAC to the message payload from PCP */
        acpac_ptr->pkt_ptr = *pkt_ptr;
        *pkt_ptr = acpac_ptr;
        pkt_len += HDRAUTH_ACPAC_LEN;
      }
      else
      {
        ERR( "HDRAUTH: Could not generate ACPAC (%d)", err_num, 0, 0 );
        dsm_free_packet( pkt_ptr );
        dsm_free_packet( &acpac_ptr );
        pkt_len = 0;
      }
    }
  }

  return pkt_len;

} /* hdrauth_get_pkt() */
/*===========================================================================

FUNCTION HDRAUTH_SET_CONFIG_TO_DEFAULT

DESCRIPTION
  This function sets authentication protocol configuration parameters to 
  default. 
  
DEPENDENCIES
  None

PARAMETERS
  current_config - Pointer to authentication protocol's current configuration 
                   structure in SCMDB.
  
RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void hdrauth_set_config_to_default
( 
  hdrauth_config_params_type *current_config 
)
{

  current_config->ac_auth_key_len = HDRAUTH_DEFAULT_AC_AUTH_KEY_LEN;

} /* hdrauth_set_config_to_default() */

/*===========================================================================

FUNCTION HDRAUTH_GET_INCONFIG_HANDLE

DESCRIPTION
  This function returns a pointer to inconfig instance of Authentication 
  Protocol

DEPENDENCIES
  None

PARAMETERS
  None
                     
RETURN VALUE
  Pointer to inconfig instance of Authentication Protocol

SIDE EFFECTS
  None

===========================================================================*/
void* hdrauth_get_inconfig_handle(void)
{
  return (void*) &hdrauth.negotiated;
} /* hdrauth_get_inconfig_handle() */

