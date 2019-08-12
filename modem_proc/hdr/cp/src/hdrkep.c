
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

              D H   K E Y   E X C H A N G E   P R O T O C O L

GENERAL DESCRIPTION
  This module contains code to perform the Diffie-Helman key exchange
  and other functions defined in the DH Key Exchange Protocol of the
  security layer of IS-856.

    AN                   AT
    |                    |
    |-- ConfigRequest -->|
    | (session key len)  |    The generic configuration protocol is used to
    |                    |    establish the key length before exchanging keys
    |<- ConfigResponse --|    (not necessary if default key length is desired)
    |                    |
    |---- KeyRequest --->|    The AN computes g^x mod p and sends it to the AT
    | (w/AN public key)  |
    |                    |
    |<-- KeyResponse _---|    The AT computes g^y mod p and sends it to the AN
    | (w/AT public key)  |
    |                    |
    |-- ANKeyComplete -->|    The AT and AN compute SKey (session key =
    | (w/key signature)  |    g^xy mod p) and also compute a 160 bit message
    |                    |    digest of SKey.  The AN sends the message digest
    |<-- ATKeyComplete --|    to the AT to confirm they have the same SKey.
    |                    |

EXTERNALIZED FUNCTIONS (Global)
   hdrkep_powerup_init - One time powerup initialization

EXTERNALIZED FUNCTIONS (Regional)
   hdrkep_process_msg      - Handle messages for this protocol
   hdrkep_process_ind      - Handle indications for this protocol

IDLEIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2001 - 2014 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* <EJECT> */ 
/*===========================================================================

                        EDIT HISTORY FOR MODULE

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/hdr/cp/src/hdrkep.c#1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
07/09/14   cnx     Support NV refresh without reset.
04/15/14   cnx     Void RPC authentication key if key length is unexpected.
12/10/13   sat     Changed memcpy() to assignment or memscpy() (safer version).
06/04/13   cnx     Removed ps_in_alias header file.
10/05/12   ukl     Resolved compilation errors after removing FEATURE_BCMCS.
03/19/12   cnx     Resolved TCB compiler errors for Dime. 
10/10/11   cnx     Fixed typo.
09/24/11   cnx     Retrieved CDMA system time from LTE when HDR in tunnel mode.
05/13/11   wsh     Fixed ERR_FATAL when secret key generation fails.
10/08/10   arm     Fixed Klocwork errors. 
10/07/10   cnx     Fixed a potential crash caused by non-standard-compliant msg ID.  
08/02/10   pba     Replaced seccrypt with secapi (as seccrypt.h is deprecated).
06/21/10   cnx     Fixed compiler warnings.
09/30/09   wsh     Fixed KW warnings
03/23/09   wsh     CMI: featurized CMI under FEATURE_CMI
02/18/09   wsh     Updated file to include private header hdrmc_v.h
12/10/08   smd     Replaced assert.h with amssassert.h
10/01/08   wsh     Fixed critical/high lint warnings
07/06/07   yll     Fixed a problem in key calculation when TimeStampShort
                   wraps around.
04/03/07   wsh     Fixed error where dsm_item is not freed
02/12/07   mpa     Fixed compiler warnings.
01/29/07   hal     Fixed memory leak and memory management errors (found in 6550)
11/29/06   mpa     (for df) Set execution platform of the DH calculations to ARM
09/08/06   hal     Changed calls to hdrutil_new_msg_buffer() to hdrutil_new_dsm_buffer()
06/12/06   mpa     Updates for 7500 RPCing of HDR rev0
04/07/06   hal     Changed hdrutil_new_buffer() to use hdrutil_new_msg_buffer()
                   Changed dsm_pushdown_tail() to use hdrutil_pushdown_tail()
12/13/05   pba     Changes to support multiple personality negotiation
11/07/05   pba     Modified SCMDB calls to use updated API
10/04/05   pba     Added support to read/write config parameters from SCMDB/EFS
04/04/05   pba     Removed Lint errors
03/24/05   pba     Lint cleanup
12/07/04   hal     Changed dsm_offset_new_buffer to hdrutil_new_buffer.
01/16/04   mpa     (for om) The common base length must in units of bytes
                   in the new SEC API (3.0)
09/24/03   mpa     (for om) Merged in new SEC API
09/10/03   mpa     Converted F3 messages to use new MSG2.0 SSIDs
03/19/03   mpa     Allocated session_fail_data out of the heap because data
                   may be used in different task contexts
03/12/03   mpa     Added new session close reason ind params to hdrkep_bummer()
12/06/02   sjy     Updated the Diffie-Hellman parameter data type names to
                   correspond to the changes made in seccrypt.h
09/12/02   om      Updated seccrypt_delete() calls for double ref.
07/17/02   dna     Queue timer expiration events on hdrmc.event_q because timer
                   callback runs as a separate thread and causes problems.
06/20/02   om      Added registration of hdrmc_wait() to SEC to avoid
                   watchdog timeouts, removed call to seccrypt_init().
03/19/02   om      Fixed get_previous_rpc_auth_key() to return key length
03/19/02   dna     Allow for AN not changing session key length
03/18/02   nmw     Fixed NV interaction
11/29/01   om      First integrated version
10/19/01   dna     Created module

===========================================================================*/

/* <EJECT> */ 
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "hdr_variation.h"
#include "err.h"
#include "amssassert.h"
#include "task.h"
#include "dsm.h"
#include "secerrno.h"
#include "secutil.h"
#include "secapi.h"
#include "sys.h"

#ifdef FEATURE_MEMCPY_REMOVAL
#include "stringl.h"
#endif /* FEATURE_MEMCPY_REMOVAL */

#ifdef FEATURE_CMI
#include "sys_v.h"
#endif


#include "hdrerrno.h"
#include "hdrdebug.h"
#include "hdrts.h"

#ifdef FEATURE_CMI
#include "hdrmc_v.h"
#endif

#include "ps_in.h"

#include "hdrmci.h"
#include "hdrhmp.h"
#include "hdrind.h"
#include "hdrutil.h"
#include "hdrscp.h"
#include "hdrbit.h"
#include "hdrtrace.h"
#ifdef FEATURE_HDRMC_DEBUG
  #error code not present
#endif

#include "hdrkep.h"
#include "hdrkepi.h"

#include "nv.h"
#include "hdrscmdb.h"

#ifdef FEATURE_LTE_TO_HDR_OH
#include "hdrsap.h"
#endif /* FEATURE_LTE_TO_HDR_OH */

/* <EJECT> */ 
/*===========================================================================

            DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains definitions for constants, macros, types,
variables and other items needed by this module.

===========================================================================*/

/* Default is 96-octet (768-bit) Diffie-Hellman key. KeyLength = 768 */
#define HDRKEP_DEFAULT_KEY_LEN          96
#define HDRKEP_MAX_KEY_LEN              128

/* Maximum time in seconds that the access terminal requires for calculation
 * of the AT Public Key and the session key
 */
#define HDRKEP_MAX_AT_PUB_KEY_CALC_TIME 3
#define HDRKEP_MAX_SKEY_CALC_TIME       10
#define HDRKEP_BITS_PER_BYTE            8
#define HDRKEP_KEY_LEN_ATTRIB_ID        0x00

/* Number of octets in the fixed length portion of the KeyRequest message */
#define HDRKEP_KEY_REQ_FIXED_LEN        3

/* Number of octets expected in the ANKeyComplete message */
#define HDRKEP_AN_KEY_COMP_LEN          26

#define HDRKEP_MSG_ID_SIZE              8
#define HDRKEP_TRANSACTION_ID_SIZE      8
#define HDRKEP_ATTRIB_LEN_SIZE          8
#define HDRKEP_ATTRIB_ID_SIZE           8
#define HDRKEP_KEY_LEN_ATTRIB_VAL_SIZE  8

/* For a complex attribute, ATTRIB_VAL_SIZE is the size of the
 * attribute value structure, including the ValueID
 */

/* The minimum attribute has Length, attribute ID and one value */
#define HDRKEP_KEY_LEN_MIN_ATTRIB_LEN \
        ( HDRKEP_ATTRIB_LEN_SIZE + \
        HDRKEP_ATTRIB_ID_SIZE + \
        HDRKEP_KEY_LEN_ATTRIB_VAL_SIZE )

/* The smallest attribute is a length with an AttributeID */
#define HDRKEP_MIN_ATTRIB_LEN \
        ( HDRKEP_ATTRIB_LEN_SIZE + \
        HDRKEP_ATTRIB_ID_SIZE )

/* ---------------------------------------
 * Packing and unpacking for common fields
 * --------------------------------------- */

/* Transaction ID */
#define HDRKEP_UNPACK_MSG_ID( ptr, val_ptr )  \
        HDRBIT_UNPACK8( (ptr), (val_ptr), 0, 8 )

#define HDRKEP_UNPACK_TRANSACTION_ID( ptr, val_ptr )  \
          HDRBIT_UNPACK8( (ptr), (val_ptr), 8, 8 )

/* Message ID  */
#define HDRKEP_PACK_MSG_ID( ptr, val )  \
          HDRBIT_PACK8( (ptr), (val), 0, 8 )

/* Transaction ID  */
#define HDRKEP_PACK_TRANSACTION_ID( ptr, val )  \
          HDRBIT_PACK8( (ptr), (val), 8, 8 )


/* ------------------------
 * Unpacking Config Request
 * ------------------------ */
/* Attribute fields are offset from the beginning of the attribute record */
#define HDRKEP_UNPACK_CON_REQ_LEN( ptr, val_ptr, aoffset )  \
          HDRBIT_UNPACK8( (ptr), (val_ptr), ((aoffset) + 0), 8 )

#define HDRKEP_UNPACK_CON_REQ_ATTRIBUTE_ID( ptr, val_ptr, aoffset )  \
          HDRBIT_UNPACK8( (ptr), (val_ptr), ((aoffset) + 8), 8 )

#define HDRKEP_UNPACK_CON_REQ_KEY_LEN( ptr, val_ptr, aoffset, voffset )  \
          HDRBIT_UNPACK8( (ptr), (val_ptr), ((aoffset) + (voffset) + 0), 8 )

/* Config response attributes are always just Attribute ID, AttributeValue */
#define HDRKEP_CRSP_ATTRIB_VAL_SIZE   8
#define HDRKEP_CRSP_ATTRIB_LEN        ( HDRKEP_ATTRIB_LEN_SIZE + \
                                        HDRKEP_ATTRIB_ID_SIZE + \
                                        HDRKEP_CRSP_ATTRIB_VAL_SIZE )

/* The field in the config response is length in bytes,
 * excluding the length of length field.
 */
#define HDRKEP_CRSP_ATTRIB_LEN_BYTES  ((HDRKEP_ATTRIB_ID_SIZE + \
          HDRKEP_CRSP_ATTRIB_VAL_SIZE) / HDRKEP_BITS_PER_BYTE)

/* -----------------------
 * Packing Config Response
 * ----------------------- */
/* Length of Config Response  */
#define HDRKEP_PACK_CON_RSP_LEN( ptr, val, offset )  \
          HDRBIT_PACK8( (ptr), (val), ((offset) + 0), 8 )

/* Attribute ID  */
#define HDRKEP_PACK_CON_RSP_ATTRIBUTE_ID( ptr, val, offset )  \
          HDRBIT_PACK8( (ptr), (val), ((offset) + 8), 8 )

/* Value ID  */
#define HDRKEP_PACK_CON_RSP_VALUE_ID( ptr, val, offset )  \
          HDRBIT_PACK8( (ptr), (val), ((offset) + 16), 8 )

/* ---------------------
 * Unpacking Key Request
 * --------------------- */
#define HDRKEP_UNPACK_KEY_REQ_TIMEOUT( ptr, val_ptr )  \
          HDRBIT_UNPACK8( (ptr), (val_ptr), 16, 8 )

/* --------------------
 * Packing Key Response
 * -------------------- */
#define HDRKEP_PACK_KEY_RSP_TIMEOUT( ptr, val )  \
          HDRBIT_PACK8( (ptr), (val), 16, 8 )

/* -------------------------
 * Unpacking AN Key Complete
 * ------------------------- */
#define HDRKEP_UNPACK_AN_KEY_COMP_NONCE( ptr, val_ptr )  \
          HDRBIT_UNPACK16( (ptr), (val_ptr), 16, 16 )

#define HDRKEP_UNPACK_AN_KEY_COMP_TIMESTAMP( ptr, val_ptr )  \
          HDRBIT_UNPACK16( (ptr), (val_ptr), 32, 16 )

/* -----------------------
 * Packing AT Key Complete
 * ----------------------- */
#define HDRKEP_PACK_AT_KEY_COMP_RESULT( ptr, val )  \
          HDRBIT_PACK8( (ptr), (val), 16, 1 )

/* -----------------------
 * Individual key fields
 * ----------------------- */
#define HDRKEP_FAC_AUTH_KEY_OFFSET( k_len )  (( k_len / 8 ) * 0 )
#define HDRKEP_RAC_AUTH_KEY_OFFSET( k_len )  (( k_len / 8 ) * 1 )
#define HDRKEP_FAC_ENC_KEY_OFFSET( k_len )   (( k_len / 8 ) * 2 )
#define HDRKEP_RAC_ENC_KEY_OFFSET( k_len )   (( k_len / 8 ) * 3 )
#define HDRKEP_FPC_AUTH_KEY_OFFSET( k_len )  (( k_len / 8 ) * 4 )
#define HDRKEP_RPC_AUTH_KEY_OFFSET( k_len )  (( k_len / 8 ) * 5 )
#define HDRKEP_FPC_ENC_KEY_OFFSET( k_len )   (( k_len / 8 ) * 6 )
#define HDRKEP_RPC_ENC_KEY_OFFSET( k_len )   (( k_len / 8 ) * 7 )


LOCAL void hdrkep_timer_cb ( uint32 timer_id );

LOCAL void hdrkep_send_at_key_comp_msg ( boolean result );

LOCAL hdrerrno_enum_type hdrkep_create_keys( void );

#ifdef FEATURE_SEC
LOCAL void hdrkep_keygen_cb( void*               udata_ptr,
                             secerrno_enum_type  err_num
                           );
#endif /* FEATURE_SEC */

LOCAL void hdrkep_bummer( hdrerrno_enum_type  err_num,
                          char*               err_msg_ptr,
                          int                 line
                        );

typedef enum
{
  HDRKEP_INACTIVE,
  HDRKEP_KEYGEN_DH_PRIVATE_KEY_WAIT_STATE,
  HDRKEP_KEYGEN_DH_PUBLIC_KEY_WAIT_STATE,
  HDRKEP_KEYGEN_DH_SECRET_KEY_WAIT_STATE,
  HDRKEP_KEYGEN_DH_SECRET_KEY_AN_WAIT_STATE,
  HDRKEP_KEYGEN_DH_FINISHED,
  HDRKEP_KEYGEN_FAILED
} hdrkep_state_type;

typedef enum
{
  HDRKEP_AN_SKEY_CALC_TIMER,
  HDRKEP_AT_SKEY_CALC_DEBUG_TIMER,
  HDRKEP_AT_PUB_KEY_CALC_DEBUG_TIMER
} hdrkep_timer_enum_type;

struct
{
  /* Array containing the common base (must be in units of bytes for SEC3.0) */
  #define COMMON_BASE_LEN   1
  uint8   common_base[COMMON_BASE_LEN];

  /* Arrays containing the prime modulo for 768 and 1024 bit keys */
  #define COMMON_MOD_768_BIT_LEN    768
  #define COMMON_MOD_1024_BIT_LEN   1024
  uint8   common_mod_768_bit[COMMON_MOD_768_BIT_LEN/8];
  uint8   common_mod_1024_bit[COMMON_MOD_1024_BIT_LEN/8];

} hdrkep_dh_params =
{
  /* Array containing the common base */
  { 0x02 },

  /* Arrays containing the prime modulo for 768 bit keys */
  { 0xff, 0xff, 0xff, 0xff,  0xff, 0xff, 0xff, 0xff,  0xc9, 0x0f, 0xda, 0xa2,
    0x21, 0x68, 0xc2, 0x34,  0xc4, 0xc6, 0x62, 0x8b,  0x80, 0xdc, 0x1c, 0xd1,
    0x29, 0x02, 0x4e, 0x08,  0x8a, 0x67, 0xcc, 0x74,  0x02, 0x0b, 0xbe, 0xa6,
    0x3b, 0x13, 0x9b, 0x22,  0x51, 0x4a, 0x08, 0x79,  0x8e, 0x34, 0x04, 0xdd,
    0xef, 0x95, 0x19, 0xb3,  0xcd, 0x3a, 0x43, 0x1b,  0x30, 0x2b, 0x0a, 0x6d,
    0xf2, 0x5f, 0x14, 0x37,  0x4f, 0xe1, 0x35, 0x6d,  0x6d, 0x51, 0xc2, 0x45,
    0xe4, 0x85, 0xb5, 0x76,  0x62, 0x5e, 0x7e, 0xc6,  0xf4, 0x4c, 0x42, 0xe9,
    0xa6, 0x3a, 0x36, 0x20,  0xff, 0xff, 0xff, 0xff,  0xff, 0xff, 0xff, 0xff
  },

  /* Arrays containing the prime modulo for 1024 bit keys */
  { 0xff, 0xff, 0xff, 0xff,  0xff, 0xff, 0xff, 0xff,  0xc9, 0x0f, 0xda, 0xa2,
    0x21, 0x68, 0xc2, 0x34,  0xc4, 0xc6, 0x62, 0x8b,  0x80, 0xdc, 0x1c, 0xd1,
    0x29, 0x02, 0x4e, 0x08,  0x8a, 0x67, 0xcc, 0x74,  0x02, 0x0b, 0xbe, 0xa6,
    0x3b, 0x13, 0x9b, 0x22,  0x51, 0x4a, 0x08, 0x79,  0x8e, 0x34, 0x04, 0xdd,
    0xef, 0x95, 0x19, 0xb3,  0xcd, 0x3a, 0x43, 0x1b,  0x30, 0x2b, 0x0a, 0x6d,
    0xf2, 0x5f, 0x14, 0x37,  0x4f, 0xe1, 0x35, 0x6d,  0x6d, 0x51, 0xc2, 0x45,
    0xe4, 0x85, 0xb5, 0x76,  0x62, 0x5e, 0x7e, 0xc6,  0xf4, 0x4c, 0x42, 0xe9,
    0xa6, 0x37, 0xed, 0x6b,  0x0b, 0xff, 0x5c, 0xb6,  0xf4, 0x06, 0xb7, 0xed,
    0xee, 0x38, 0x6b, 0xfb,  0x5a, 0x89, 0x9f, 0xa5,  0xae, 0x9f, 0x24, 0x11,
    0x7c, 0x4b, 0x1f, 0xe6,  0x49, 0x28, 0x66, 0x51,  0xec, 0xe6, 0x53, 0x81,
    0xff, 0xff, 0xff, 0xff,  0xff, 0xff, 0xff, 0xff
  }
};

struct
{
  hdrkep_config_params_type current; /* Current configuration */
  hdrkep_config_params_type negotiated; /* Negotiated configuration */
  
  /* For the temporary 128 byte items, use DSM items */
  dsm_item_type *an_pub_key;
  dsm_item_type *at_pub_key;
  dsm_item_type *skey;

  uint8 an_key_sig[HDRKEP_KEY_SIG_LEN];
  uint8 at_key_sig[HDRKEP_KEY_SIG_LEN];

  uint8 key_req_trans_id;  /* KeyRequest message transaction ID */
  uint8 key_comp_trans_id; /* KeyComplete message transaction ID */
  uint8 an_skey_timeout;   /* Time for AN to compute SKey */
  uint16 nonce;  /* Nonce - Picked by AN, used to compute the KeySignature */

  /* TimeStampShort - the 16 least significant bits of the SystemTimeLong
   * used in computing the KeySignature
   */
  uint16 key_sig_timestamp_short;

  /* Time after which a ANKeyComplete is invalid */
  rex_timer_type an_skey_calc_timer;

  /* Debug timers to make sure our calculations are within the limits */
  rex_timer_type at_skey_calc_debug_timer;
  rex_timer_type at_pub_key_calc_debug_timer;

#ifdef FEATURE_SEC
  /* Handle to the crypto instance used for the DH operations */
  secapi_handle_type pkx_handle;
#endif /* FEATURE_SEC */

  /* If the AN and/or AT have finished computing the session key */
  hdrkep_state_type state;

} hdrkep;

/* <EJECT> */
/*===========================================================================

FUNCTION HDRKEP_POWERUP_INIT

DESCRIPTION
  This function performs the initialization of the Key Exchange Protocol.

DEPENDENCIES
  This function must called only once after powerup

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  All elements of the current and negotiated instances of
  hdrkep_config_params_type are initialized.

===========================================================================*/
void hdrkep_powerup_init( void )
{
  if ( hdrmc_is_first_init() )
  {
    rex_def_timer_ex( &hdrkep.an_skey_calc_timer,
                      hdrkep_timer_cb,
                      HDRKEP_AN_SKEY_CALC_TIMER );

    rex_def_timer_ex( &hdrkep.at_skey_calc_debug_timer,
                      hdrkep_timer_cb,
                      HDRKEP_AT_SKEY_CALC_DEBUG_TIMER );

    rex_def_timer_ex( &hdrkep.at_pub_key_calc_debug_timer,
                      hdrkep_timer_cb,
                      HDRKEP_AT_PUB_KEY_CALC_DEBUG_TIMER );

#ifdef FEATURE_SEC                    
    /* Register the TCB we can call SEC services from */
    (void) secapi_register_wait_func( HDRKEP_TASK_PTR, hdrmc_wait );
#endif /* FEATURE_SEC */
  }

  hdrkep.state = HDRKEP_INACTIVE;

  /* Get current configuration parameters from SCMDB */
  hdrscmdb_get_inuse_config(HDRHAI_KEY_EXCHANGE_PROTOCOL, &hdrkep.current);

  if ( (hdrkep.current.key_len != HDRKEP_DEFAULT_KEY_LEN) &&
       (hdrkep.current.key_len != HDRKEP_MAX_KEY_LEN) )
  {
    HDR_MSG_PROT_1( MSG_LEGACY_ERROR, "Key length (%d) is incorrect", 
                    hdrkep.current.key_len );
    hdrkep_set_config_to_default(&hdrkep.current);
  }

} /* hdrkep_powerup_init() */

/* <EJECT> */
#ifdef FEATURE_SEC
/*===========================================================================

FUNCTION HDRKEP_FINISH_KEY_EXCHANGE

DESCRIPTION
  This function compares the key signature sent by the AN to the one
  we computed and sends the AT Key Complete message to accept or reject
  the key.

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
LOCAL void hdrkep_finish_key_exchange( void )
{
  if ( ! secutil_byte_compare( hdrkep.an_key_sig,
                               hdrkep.at_key_sig,
                               HDRKEP_KEY_SIG_LEN ) )
  {
    /* Report that the key exchange was successful */
    hdrkep_send_at_key_comp_msg( TRUE );
    hdrkep.negotiated.keys_are_valid = TRUE;

    /* We are finished so we can free the DSM items we used */
    dsm_free_packet( &hdrkep.at_pub_key );
    dsm_free_packet( &hdrkep.an_pub_key );
    dsm_free_packet( &hdrkep.skey );
  }
  else
  {
    /* Report that the key exchange failed */
    hdrkep_send_at_key_comp_msg( FALSE );

    /* Fail negotiation */
    hdrkep_bummer( E_BAD_DATA,
                   "KEP: Key signature mismatch",
                   __LINE__ );
  }

  hdrkep.state = HDRKEP_INACTIVE;

} /* hdrkep_finish_key_exchange() */

/* <EJECT> */
/*===========================================================================

FUNCTION HDRKEP_PROCESS_KEY_LEN_ATTRIB

DESCRIPTION
  This function processes the KeyLength attribute of a Configuration
  Request message.

DEPENDENCIES
  None

PARAMETERS
  item_ptr              - Pointer to the DSM item chain holding the message
  attrib_len            - Length, in bits, of the attribute, including
                          the length field
  attrib_offset         - Bit offset of the attribute from the
                          beginning of the message
  negotiated_values_ptr - Pointer to struct to put any negotiated values into

RETURN VALUE
  TRUE if a new, acceptable value for the attribute was found, FALSE
  otherwise.

SIDE EFFECTS
  None

===========================================================================*/
LOCAL boolean hdrkep_process_key_len_attrib
(
  dsm_item_type *item_ptr,
  uint8  attrib_len,
  uint16 attrib_offset,
  uint8 *accepted_key_len_val_ptr
)
{
  /* Bit offset of current attribute value from the beginning of the
   * current attribute */
  uint16 attrib_val_offset;
  uint8  attrib_val; /* A suggested value for the attribute */
  boolean val_is_acceptable = FALSE;  /* If we accept the attribute */
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* The first attribute value is after the attribute length and ID fields */
  attrib_val_offset = HDRKEP_ATTRIB_LEN_SIZE + HDRKEP_ATTRIB_ID_SIZE;

  /* Loop through all the values given for this attribute. */
  while ((attrib_val_offset + HDRKEP_KEY_LEN_ATTRIB_VAL_SIZE) <=
         attrib_len)
  {
    /* There is at least one more attribute value in this attribute */

    if ( HDRKEP_UNPACK_CON_REQ_KEY_LEN( item_ptr,
           &attrib_val, attrib_offset, attrib_val_offset ) != E_SUCCESS )
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
      *accepted_key_len_val_ptr = attrib_val;
      val_is_acceptable = TRUE;
      break;
    }
    else
    {
      ERR("Unexpected value for RAN Handoff", 0, 0, 0 );
    }
    attrib_val_offset += HDRKEP_KEY_LEN_ATTRIB_VAL_SIZE;
  }

  return (val_is_acceptable);

} /* hdrkep_process_key_len_attrib() */

/* <EJECT> */
/*===========================================================================

FUNCTION HDRKEP_PROCESS_CONFIG_REQ_MSG

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
LOCAL void hdrkep_process_config_req_msg
(
  dsm_item_type *item_ptr
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
  boolean accept_key_len = FALSE;
  uint8 accepted_key_len_val = 0;

  dsm_item_type *send_msg_ptr; /* dsm buffer for sent message */
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  total_bits_remaining = (dsm_length_packet(item_ptr) *
                               HDRKEP_BITS_PER_BYTE );

  /* Extract the Transaction ID from the message */
  if( HDRKEP_UNPACK_TRANSACTION_ID( item_ptr, &creq_transaction_id )
      != E_SUCCESS )
  {
    ERR("Error extracting TransactionID from ConfigReq Msg", 0, 0, 0 );
    /* Without the transaction ID, it makes no sense to send a response */
    return;
  }

  /* Initialize attrib_len so the initial attrib_offset will be
   * immediately after the transaction ID of the config request.
   */
  attrib_len = HDRKEP_MSG_ID_SIZE + HDRKEP_TRANSACTION_ID_SIZE;

  /* Unpack the configuration attributes */

  /* While there are at least enough bits for the smallest attribute... */
  while ((total_bits_remaining - attrib_len) >= HDRKEP_MIN_ATTRIB_LEN)
  {
    /* Move to the next attribute record */
    attrib_offset += attrib_len;
    total_bits_remaining -= attrib_len;

    /* Get the length of the new attribute */
    if( HDRKEP_UNPACK_CON_REQ_LEN( item_ptr, &attrib_len, attrib_offset ) !=
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
    attrib_len *= HDRKEP_BITS_PER_BYTE;
    attrib_len += HDRKEP_ATTRIB_LEN_SIZE;

    /* Any attribute record must have at least an attribute ID */
    if ((attrib_len < HDRKEP_MIN_ATTRIB_LEN) ||
        (attrib_len > total_bits_remaining ))
    {
      /* If the length is something unexpected, we can't be sure of what
       * the AN intended and it is safest to not send a response
       */
      send_response = FALSE;
      break;
    }

    /* There is at least one more attribute record in the message */

    if( HDRKEP_UNPACK_CON_REQ_ATTRIBUTE_ID( item_ptr, &attrib_id,
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
    if (hdrscp_get_config_subtype( HDRHAI_KEY_EXCHANGE_PROTOCOL ) ==
        HDRSCP_DH_KEY_EXCHANGE )
    {
      /* Handle attributes for DH Key Exchange protocol */
      switch( attrib_id )
      {
        case HDRKEP_KEY_LEN_ATTRIB_ID:
          if (attrib_len >= HDRKEP_KEY_LEN_MIN_ATTRIB_LEN)
          {
            accept_key_len = hdrkep_process_key_len_attrib(
                    item_ptr, attrib_len, attrib_offset,
                    &accepted_key_len_val );
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
    HDRKEP_PACK_MSG_ID( send_msg_ptr, HDRKEP_CONFIG_RESP_MSG);
    HDRKEP_PACK_TRANSACTION_ID( send_msg_ptr, creq_transaction_id);

    /* Now pack in the attribute ID and value of all attributes we have
     * flagged that we want to accept.
     */

    /* Re-use attrib_offset variable for message packing */
    attrib_offset = HDRKEP_MSG_ID_SIZE + HDRKEP_TRANSACTION_ID_SIZE;

    if (accept_key_len)
    {
      /* Include that we are accepting the key_len attribute */
      HDRKEP_PACK_CON_RSP_LEN( send_msg_ptr, HDRKEP_CRSP_ATTRIB_LEN_BYTES,
                             attrib_offset );
      HDRKEP_PACK_CON_RSP_ATTRIBUTE_ID( send_msg_ptr,
        HDRKEP_KEY_LEN_ATTRIB_ID, attrib_offset );
      HDRKEP_PACK_CON_RSP_VALUE_ID( send_msg_ptr, accepted_key_len_val,
        attrib_offset );

      attrib_offset += HDRKEP_CRSP_ATTRIB_LEN;

      /* Update our regional structure to reflect the newly accepted values.
       * This is done per attribute because if an attribute was included twice
       * and the second was not accepted, negotiated_values still has the
       * values from the first one, but those were not actually negotiated.
       */
      hdrkep.negotiated.key_len =
        (accepted_key_len_val == 0x01) ? HDRKEP_MAX_KEY_LEN :
                                         HDRKEP_DEFAULT_KEY_LEN;
    }

    /* if (accept_some_other_attribute) ... */

    /* Send config response on RTC.  All config messages are reliable.
     */
    hdrhmp_send_msg( HDRHAI_KEY_EXCHANGE_PROTOCOL,
                   HDRHAI_RTC,
                   TRUE,
                   HDRKEP_MESSAGE_PRIORITY,
                   HDRHAI_IN_CONFIG, /* Instance of protocol sending message */
                   send_msg_ptr,
                   NULL,
                   NULL );

  } /* end if (send_response) */

} /* hdrkep_process_config_req_msg() */

/* <EJECT> */
/*===========================================================================

FUNCTION HDRKEP_PROCESS_KEY_REQ_MSG

DESCRIPTION
  This function processes the Key Request message.

DEPENDENCIES
  None

PARAMETERS
  item_ptr - Pointer to the DSM item chain holding the message

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
LOCAL void hdrkep_process_key_req_msg
(
  dsm_item_type *item_ptr
)
{
  secerrno_enum_type          err_num;
  secapi_pkx_param_data_type  pkx_param;
  int err; /* sum of errno return vals, depends that E_SUCCESS is zero */
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (dsm_length_packet(item_ptr) <
      (uint32)(HDRKEP_KEY_REQ_FIXED_LEN + hdrkep.negotiated.key_len))
  {
    ERR("Not enough bytes in KeyReq Msg", 0, 0, 0 );
    return;
  }

  if ( hdrkep.pkx_handle )
  {
    HDR_MSG_PROT (MSG_LEGACY_HIGH,  "KEP: Freeing old PKX handle");
    (void) secapi_delete( &(hdrkep.pkx_handle) );
  }

  if (hdrkep.state != HDRKEP_INACTIVE)
  {
    ERR("KeyRequest out of sequence",0,0,0);
    return;
  }

  /* This is the beginning of the key exchange sequence -  */
  /* allocate a new crypto instance for the DH operations. */
  if ( secapi_new( &(hdrkep.pkx_handle), SECAPI_DIFFIE_HELLMAN )
       !=
       E_SUCCESS )
  {
    ERR_FATAL( "No free D-H crypto instances !", 0, 0, 0 );
  }


  /* Extract the Transaction ID and Timeout fields from the message */
  err = HDRKEP_UNPACK_TRANSACTION_ID( item_ptr, &hdrkep.key_req_trans_id );
  err |= HDRKEP_UNPACK_KEY_REQ_TIMEOUT( item_ptr, &hdrkep.an_skey_timeout );

  /* Extract the ANPubKey field */
  hdrkep.an_pub_key = hdrutil_new_dsm_buffer();
  if (hdrkep.an_pub_key == NULL)
  {
    ERR_FATAL( "KEP: Out of DSM items", 0, 0, 0 );
  }

  err |= (dsm_extract( item_ptr, 3, hdrkep.an_pub_key->data_ptr,
           hdrkep.negotiated.key_len ) 
          != hdrkep.negotiated.key_len );/*lint !e514 */
  hdrkep.an_pub_key->used = hdrkep.negotiated.key_len;

  if (err)
  {
    ERR( "Error extracting from KeyReq Msg", 0, 0, 0 );
    hdrkep_bummer( E_NO_DATA,
                   "Error extracting from KeyReq Msg",
                   __LINE__ );
    return;
  }

  /* For debug, ensure computation of ATPubKey < T_KEPSigCompAT (3s) */
  (void) rex_set_timer( &hdrkep.at_pub_key_calc_debug_timer,
                 HDRKEP_MAX_AT_PUB_KEY_CALC_TIME * 1000);

  hdrkep.at_pub_key = hdrutil_new_dsm_buffer();
  hdrkep.skey = hdrutil_new_dsm_buffer();
  if ( (hdrkep.at_pub_key == NULL)
       ||
       (hdrkep.skey == NULL) )
  {
    ERR_FATAL( "KEP: Out of DSM items", 0, 0, 0 );
  }

  do
  {
    /* Set execution mode of the DH calculations to async */
    pkx_param.common.exec_mode = SECAPI_ASYNC_EXEC;
    err_num = secapi_pkx_set_param( hdrkep.pkx_handle,
                                    SECAPI_DIFFIE_HELLMAN,
                                    SECAPI_EXEC_MODE,
                                    &pkx_param );
    if ( err_num != E_SUCCESS )
    {
      hdrkep_bummer( err_num,
                     "Could not set DH to async mode",
                     __LINE__ );
      break;
    }

    /* Set execution platform of the DH calculations to ARM */
    pkx_param.common.exec_platform = SECAPI_EXEC_PLATFORM_ARM;
    err_num = secapi_pkx_set_param( hdrkep.pkx_handle,
                                    SECAPI_DIFFIE_HELLMAN,
                                    SECAPI_EXEC_PLATFORM,
                                    &pkx_param );
    if ( err_num != E_SUCCESS )
    {
      hdrkep_bummer( err_num,
                     "Could not set DH to ARM exec",
                     __LINE__ );
      break;
    }

    /* Set callback for async DH execution */
    pkx_param.common.async_cb.cb_fptr = hdrkep_keygen_cb;
    pkx_param.common.async_cb.usr_ptr = (void*) NULL;
    err_num = secapi_pkx_set_param( hdrkep.pkx_handle,
                                    SECAPI_DIFFIE_HELLMAN,
                                    SECAPI_ASYNC_CALLBACK,
                                    &pkx_param );
    if ( err_num != E_SUCCESS )
    {
      hdrkep_bummer( err_num,
                     "Could not set DH async cb",
                     __LINE__ );
      break;
    }

    /* Configure the pkx instance with:
    *  - base
    *  - modulus
    *  - exponent */
    pkx_param.dh.group.base_size    = COMMON_BASE_LEN;
    pkx_param.dh.group.base_ptr     = hdrkep_dh_params.common_base;
    pkx_param.dh.group.modulus_size = hdrkep.negotiated.key_len;
    if ( hdrkep.negotiated.key_len == HDRKEP_DEFAULT_KEY_LEN )
    {
      pkx_param.dh.group.modulus_ptr = hdrkep_dh_params.common_mod_768_bit;
    }
    else
    {
      pkx_param.dh.group.modulus_ptr = hdrkep_dh_params.common_mod_1024_bit;
    }

    err_num = secapi_pkx_set_param( hdrkep.pkx_handle,
                                    SECAPI_DIFFIE_HELLMAN,
                                    SECAPI_PKX_DH_GROUP,
                                    &pkx_param );
    if ( err_num != E_SUCCESS )
    {
      ERR( "KEP: Setting DH base -> error %d", err_num, 0, 0 );
      break;
    }

    /* Set the DH peer public key in the pkx instance */
    pkx_param.dh.peer_public_key_ptr = hdrkep.an_pub_key->data_ptr;
    err_num = secapi_pkx_set_param( hdrkep.pkx_handle,
                                    SECAPI_DIFFIE_HELLMAN,
                                    SECAPI_PKX_DH_PEER_PUBLIC_KEY,
                                    &pkx_param );
    if ( err_num != E_SUCCESS )
    {
      ERR( "KEP: Setting DH peer public key -> error %d", err_num, 0, 0 );
      break;
    }

    /* Kick off calculation of our private and public keys */
    err_num = secapi_pkx_create_key( hdrkep.pkx_handle,
                                     SECAPI_DIFFIE_HELLMAN,
                                     SECAPI_PKX_DH_PRIVATE_KEY,
                                     NULL );
    if ( err_num != E_SUCCESS )
    {
      hdrkep_bummer( err_num,
                     "Could not create private DH key",
                     __LINE__ );
      break;
    }

    hdrkep.state =  HDRKEP_KEYGEN_DH_PRIVATE_KEY_WAIT_STATE;

  } while( 0 ); /*lint !e717 */ 

} /* hdrkep_process_key_req_msg() */

/* <EJECT> */
/*===========================================================================

FUNCTION HDRKEP_SEND_KEY_RSP_MSG

DESCRIPTION
  This function sends the Key Response message after the ATPubKey calculation
  is completed.

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
LOCAL void hdrkep_send_key_rsp_msg ( void )
{
  dsm_item_type *send_msg_ptr; /* dsm buffer for sent message */
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* get new buffer */
  send_msg_ptr = hdrutil_new_msg_buffer();

  /* pack fields */
  HDRKEP_PACK_MSG_ID( send_msg_ptr, HDRKEP_KEY_RESP_MSG);
  HDRKEP_PACK_TRANSACTION_ID( send_msg_ptr, hdrkep.key_req_trans_id );
  HDRKEP_PACK_KEY_RSP_TIMEOUT( send_msg_ptr, HDRKEP_MAX_SKEY_CALC_TIME );
  (void) hdrutil_pushdown_tail( &send_msg_ptr,
                         hdrkep.at_pub_key->data_ptr,
                         hdrkep.negotiated.key_len );

  /* Send KeyResponse reliably on the RTC */
  hdrhmp_send_msg( HDRHAI_KEY_EXCHANGE_PROTOCOL,
                 HDRHAI_RTC,
                 TRUE,
                 HDRKEP_MESSAGE_PRIORITY,
                 HDRHAI_IN_CONFIG, /* Instance of protocol sending message */
                 send_msg_ptr,
                 NULL,
                 NULL );

} /* hdrkep_send_key_rsp_msg() */

/* <EJECT> */
/*===========================================================================

FUNCTION HDRKEP_PROCESS_AN_KEY_COMP_MSG

DESCRIPTION
  This function processes the AN Key Complete Message.

DEPENDENCIES
  None

PARAMETERS
  item_ptr - Pointer to the DSM item chain holding the message

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
LOCAL void hdrkep_process_an_key_comp_msg
(
  dsm_item_type *item_ptr
)
{
  int err_num = 0; /* sum of errno return vals, needs E_SUCCESS to be zero */
  rex_timer_cnt_type msec_remaining; /* Time left on timer */
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (dsm_length_packet(item_ptr) < HDRKEP_AN_KEY_COMP_LEN )
  {
    ERR( "KEP: Not enough bytes in ANKeyComp Msg", 0, 0, 0 );
    return;
  }

  switch( hdrkep.state )
  {
    case HDRKEP_KEYGEN_DH_SECRET_KEY_WAIT_STATE:
    case HDRKEP_KEYGEN_DH_SECRET_KEY_AN_WAIT_STATE:
    case HDRKEP_KEYGEN_DH_FINISHED:
      msec_remaining = rex_get_timer( &hdrkep.an_skey_calc_timer );
      HDR_MSG_PROT_1 (MSG_LEGACY_MED,  "KEP: AN took %dms to respond",
                     hdrkep.an_skey_timeout * 1000 - msec_remaining);
      break;

    default:
      ERR( "KEP: ANKeyComplete out of sequence", 0, 0, 0 );
      err_num = 1;
      break;
  }

  if ( err_num ) return;

  /* Cancel an_skey_timeout timer */
  ( void ) rex_clr_timer( &hdrkep.an_skey_calc_timer );

  /* Extract the Transaction ID, Nonce and TimeStampShort fields */
  err_num = HDRKEP_UNPACK_TRANSACTION_ID( item_ptr,
                                         &hdrkep.key_comp_trans_id );
  err_num |= HDRKEP_UNPACK_AN_KEY_COMP_NONCE( item_ptr, &hdrkep.nonce );
  err_num |= HDRKEP_UNPACK_AN_KEY_COMP_TIMESTAMP(
              item_ptr,
              &hdrkep.key_sig_timestamp_short );

  /* Extract the KeySignature field */
  err_num |= ( dsm_extract( item_ptr,
                           6,
                           hdrkep.an_key_sig,
                           HDRKEP_KEY_SIG_LEN )
              != HDRKEP_KEY_SIG_LEN ); /*lint !e514 */

  if (err_num)
  {
    ERR( "KEP: Error extracting ANKeyComp Msg", 0, 0, 0 );
    return;
  }

  if (hdrkep.state == HDRKEP_KEYGEN_DH_SECRET_KEY_AN_WAIT_STATE)
  {
    /* We still need to wait for our calculation to finish */
    hdrkep.state = HDRKEP_KEYGEN_DH_SECRET_KEY_WAIT_STATE;
  }
  else if (hdrkep.state == HDRKEP_KEYGEN_DH_FINISHED)
  {
    /* We already finished and were just waiting for the AN */
    (void) hdrkep_create_keys();
    hdrkep_finish_key_exchange();
  }
} /* hdrkep_process_an_key_comp_msg() */


/* <EJECT> */
/*===========================================================================

FUNCTION HDRKEP_SEND_AT_KEY_COMP_MSG

DESCRIPTION
  This function sends the AT Key Complete message after the SKey calculation
  is completed.

DEPENDENCIES
  None

PARAMETERS
  result - TRUE if the key exchange was successful, FALSE otherwise

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
LOCAL void hdrkep_send_at_key_comp_msg
(
  boolean result
)
{
  dsm_item_type *send_msg_ptr; /* dsm buffer for sent message */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* get new buffer */
  send_msg_ptr = hdrutil_new_msg_buffer();

  /* pack fields */
  HDRKEP_PACK_MSG_ID( send_msg_ptr, HDRKEP_AT_KEY_COMP_MSG );
  HDRKEP_PACK_TRANSACTION_ID( send_msg_ptr, hdrkep.key_comp_trans_id);
  HDRKEP_PACK_AT_KEY_COMP_RESULT( send_msg_ptr, result );

  /* Send KeyResponse reliably on the RTC */
  hdrhmp_send_msg( HDRHAI_KEY_EXCHANGE_PROTOCOL,
                 HDRHAI_RTC,
                 TRUE,
                 HDRKEP_MESSAGE_PRIORITY,
                 HDRHAI_IN_CONFIG, /* Instance of protocol sending message */
                 send_msg_ptr,
                 NULL,
                 NULL );

} /* hdrkep_send_at_key_comp_msg() */

/* <EJECT> */
#endif /* FEATURE_SEC */
/*===========================================================================

FUNCTION HDRKEP_PROCESS_CMD

DESCRIPTION
  This function processes commands given to KEP.

DEPENDENCIES
  None

PARAMETERS
  kep_cmd_ptr - Pointer to the command for KEP to process

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void hdrkep_process_cmd (hdrkep_cmd_type *kep_cmd_ptr)
{
  switch (kep_cmd_ptr->name)
  {
    default:
      ERR_FATAL( "Unknown command", 0, 0, 0 );
      break;
  }
} /* hdrkep_process_cmd() */

/* <EJECT> */
/*===========================================================================

FUNCTION HDRKEP_PROCESS_MSG

DESCRIPTION
  This function processes messages for KEP.

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
void hdrkep_process_msg
(
  dsm_item_type * msg_ptr,
  hdrhai_protocol_instance_enum_type protocol_instance
)
{
#ifdef FEATURE_SEC
  uint8 msg_id; /* message ID of received message */
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ASSERT( (protocol_instance == HDRHAI_IN_USE)
          ||
          (protocol_instance == HDRHAI_IN_CONFIG) );

  /* MessageID is the first uint8 */
  if ( HDRKEP_UNPACK_MSG_ID( msg_ptr, &(msg_id) ) != E_SUCCESS )
  {
    ERR( "Could not extract message_id", 0, 0, 0 );
    dsm_free_packet( &msg_ptr );
    return;
  }

  /* Currently only the in config instance receives any messages */
  if (protocol_instance != HDRHAI_IN_CONFIG)
  {
    ERR("Msg for wrong instance %d", msg_id, 0, 0 );
    dsm_free_packet(&msg_ptr);
    return;
  }

  HDRTRACE_MSG( HDRHAI_KEY_EXCHANGE_PROTOCOL, msg_id, 0 );
#ifdef FEATURE_HDRMC_DEBUG
  #error code not present
#endif


  if (hdrscp_get_config_subtype( HDRHAI_KEY_EXCHANGE_PROTOCOL )
      == HDRSCP_DH_KEY_EXCHANGE )
  {
    /* Process messages for the in-config instance of the key exchange
     * protocol, which is currently DH Key Exchange
     */
    switch (msg_id)
    {
      case HDRKEP_CONFIG_REQ_MSG:
        hdrkep_process_config_req_msg( msg_ptr );
        break;

      case HDRKEP_KEY_REQ_MSG:
        hdrkep_process_key_req_msg( msg_ptr );
        break;

      case HDRKEP_AN_KEY_COMP_MSG:
        hdrkep_process_an_key_comp_msg( msg_ptr );
        break;

      default:
        ERR( "Unknown msg (%d) for DH KEP", msg_id, 0, 0 );
        break;
    }
  }
  else /* (protocol subtype == HDRSCP_DEFAULT_KEP ) */
  {
    switch (msg_id)
    {
      case HDRKEP_CONFIG_REQ_MSG:
        hdrkep_process_config_req_msg( msg_ptr );
        break;

      default:
        ERR( "Unknown msg (%d) for default KEP", msg_id, 0, 0 );
        break;
    }
  }
  dsm_free_packet(&msg_ptr);
#ifdef FEATURE_HDRMC_DEBUG
  #error code not present
#endif

#endif /* FEATURE_SEC */
} /* hdrkep_process_msg() */

/* <EJECT> */
/*===========================================================================

FUNCTION HDRKEP_PROCESS_IND

DESCRIPTION
  This function processes indications given to KEP.

DEPENDENCIES
  None

PARAMETERS
  ind_name     - Name of the indication for KEP to process
  ind_data_ptr - Any data associated with the indication

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void hdrkep_process_ind (hdrind_ind_name_enum_type ind_name,
        hdrind_ind_data_union_type * ind_data_ptr)
{

  SYS_ARG_NOT_USED(ind_data_ptr);/* remove this line when ind_data_ptr
                                  * is used */

  HDRTRACE_IND( HDRHAI_KEY_EXCHANGE_PROTOCOL, ind_name, 0 );
#ifdef FEATURE_HDRMC_DEBUG
  #error code not present
#endif
  switch (ind_name)
  {
    case HDRIND_SMP_SESSION_CLOSED:
      /* Cancel an_skey_timeout timer */
      ( void ) rex_clr_timer( &hdrkep.an_skey_calc_timer );

      hdrkep.state = HDRKEP_INACTIVE;
      break;

    case HDRIND_SCM_SESSION_CHANGED:
      /* Update local copy of current configuration from SCMDB */
      hdrscmdb_get_inuse_config(HDRHAI_KEY_EXCHANGE_PROTOCOL, &hdrkep.current);
      break;

    default:
      ERR( "Unknown indication (%d)", ind_name, 0, 0 );
      break;
  }

#ifdef FEATURE_HDRMC_DEBUG
  #error code not present
#endif
} /* hdrkep_process_ind() */

/* <EJECT> */
/*===========================================================================

FUNCTION HDRKEP_PROCESS_SIG

DESCRIPTION
  This function processes signals for KEP.

DEPENDENCIES
  Only one signal is sent at a time, so only one bit of the sig input
  parameter is set.  If multiple signals need to be delivered, they must
  be delivered by multiple calls.

PARAMETERS
  sig - The signal that belongs to this protocol and was set.  This is not
        a signal mask.  Only one bit of the value is set

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void hdrkep_process_sig (rex_sigs_type sig)
{
#ifdef FEATURE_SEC
  rex_timer_cnt_type msec_remaining; /* Time left on timer */
  secerrno_enum_type err_num;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

#ifdef FEATURE_HDRMC_DEBUG
  #error code not present
#endif
  switch (sig)
  {
    case HDRMC_KEP_KEYGEN_DONE_SIG:

      /* Figure wich key we waited on and continue the DH calculation */
      switch( hdrkep.state )
      {
        case HDRKEP_KEYGEN_DH_PRIVATE_KEY_WAIT_STATE:
          HDR_MSG_PROT (MSG_LEGACY_HIGH,  "KEP: DH private key generated");
          err_num = secapi_pkx_create_key( hdrkep.pkx_handle,
                                           SECAPI_DIFFIE_HELLMAN,
                                           SECAPI_PKX_DH_PUBLIC_KEY,
                                           (hdrkep.at_pub_key)->data_ptr );
          if ( err_num != E_SUCCESS )
          {
            hdrkep_bummer( err_num,
                           "Could not create public DH key",
                           __LINE__ );
            return;
          }

          /* Set our state for the next callback */
          hdrkep.state = HDRKEP_KEYGEN_DH_PUBLIC_KEY_WAIT_STATE;
          break;

        case HDRKEP_KEYGEN_DH_PUBLIC_KEY_WAIT_STATE:
          msec_remaining = rex_get_timer(
                             &hdrkep.at_pub_key_calc_debug_timer);
          HDR_MSG_PROT_1 (MSG_LEGACY_HIGH,  "KEP: DH ATPubKey computed (%d ms)",
                      HDRKEP_MAX_AT_PUB_KEY_CALC_TIME * 1000 - msec_remaining);

          ( void ) rex_clr_timer( &hdrkep.at_pub_key_calc_debug_timer );

          (void) rex_set_timer( &hdrkep.an_skey_calc_timer,
                         hdrkep.an_skey_timeout * 1000 );

          /* For debug, ensure computation of  */
          /* SKey < HDRKEP_MAX_SKEY_CALC_TIME  */
          (void) rex_set_timer( &hdrkep.at_skey_calc_debug_timer,
                         HDRKEP_MAX_SKEY_CALC_TIME * 1000 );

          /* Kick off the secret key generation */
          err_num = secapi_pkx_create_key( hdrkep.pkx_handle,
                                           SECAPI_DIFFIE_HELLMAN,
                                           SECAPI_PKX_DH_SECRET_KEY,
                                           (hdrkep.skey)->data_ptr );
          if ( err_num != E_SUCCESS )
          {
            hdrkep_bummer( err_num,
                           "Could not create secret DH key",
                           __LINE__ );
            return;
          }

          hdrkep.state = HDRKEP_KEYGEN_DH_SECRET_KEY_AN_WAIT_STATE;

          /* Send KeyResponse message */
          hdrkep_send_key_rsp_msg();
          break;

        case HDRKEP_KEYGEN_DH_SECRET_KEY_WAIT_STATE:
        case HDRKEP_KEYGEN_DH_SECRET_KEY_AN_WAIT_STATE:
          msec_remaining = rex_get_timer( &hdrkep.at_skey_calc_debug_timer);
          HDR_MSG_PROT_1 (MSG_LEGACY_HIGH,  "KEP: DH AT SKey computed (%d ms)",
                         HDRKEP_MAX_SKEY_CALC_TIME * 1000 - msec_remaining);

          ( void ) rex_clr_timer( &hdrkep.at_skey_calc_debug_timer );

          if ( hdrkep.state == HDRKEP_KEYGEN_DH_SECRET_KEY_AN_WAIT_STATE )
          {
            /* We were faster than the AN to calculate SKey.
            * Wait until we receive the AN Key Complete to continue.
            */

            hdrkep.state = HDRKEP_KEYGEN_DH_FINISHED;
          }
          else if ( hdrkep.state == HDRKEP_KEYGEN_DH_SECRET_KEY_WAIT_STATE )
          {
            /* The AN already finished, so we can finish now, too. */
            (void) hdrkep_create_keys();
            hdrkep_finish_key_exchange();
          }
          else
          {
            ASSERT (hdrkep.state == HDRKEP_INACTIVE);
          }

          /* Free the crypto instance used for this key exchange */
          (void) secapi_delete( &(hdrkep.pkx_handle) );
          break;

        case HDRKEP_INACTIVE:
        case HDRKEP_KEYGEN_FAILED:
        case HDRKEP_KEYGEN_DH_FINISHED:
          ERR( "KEP: Got KeyGen Sig in state %d", hdrkep.state, 0, 0 );
          break;

        default:
          ERR_FATAL( "KEP: Found unknown state %d",
                     hdrkep.state, 0, 0 );
      }
      break;

    default:
      ERR("Unexpected Signal",0,0,0);
      break;
  }
#ifdef FEATURE_HDRMC_DEBUG
  #error code not present
#endif
#endif /* FEATURE_SEC */
} /* hdrkep_process_sig() */

/* <EJECT> */

/*===========================================================================

FUNCTION HDRKEP_TIMER_CB

DESCRIPTION
  This function is the callback function that gets called when a timer for
  the DH Key Exchange Protocol expires.

DEPENDENCIES
  None

PARAMETERS
  timer_id - The ID of the timer that expired.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
LOCAL void hdrkep_timer_cb
(
  uint32 timer_id
)
{
  hdrmc_queue_timer(HDRHAI_KEY_EXCHANGE_PROTOCOL, timer_id);
} /* hdrkep_timer_cb() */

/* <EJECT> */

/*===========================================================================

FUNCTION HDRKEP_PROCESS_TIMER

DESCRIPTION
  This function processes all timer expiration events for the key exchange
  protocol.

DEPENDENCIES
  None

PARAMETERS
  timer_id - The ID of the timer that expired.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void hdrkep_process_timer
(
  uint32 timer_id
)
{
#ifdef FEATURE_SEC
  switch ((hdrkep_timer_enum_type) timer_id )
  {
    case HDRKEP_AN_SKEY_CALC_TIMER:
      HDR_MSG_PROT (MSG_LEGACY_HIGH,
                   "T_KEPSigCompAT expired (no KeyCompleteMsg from AN)");

      hdrkep.state = HDRKEP_INACTIVE;

      hdrkep_bummer( E_TIMER_EXP,
                     "T_KEPSigCompAT expired (no KeyCompleteMsg from AN)",
                     __LINE__ );
      break;

    case HDRKEP_AT_SKEY_CALC_DEBUG_TIMER:
      ERR("AT SKey calc took too long!",0,0,0);
      break;

    case HDRKEP_AT_PUB_KEY_CALC_DEBUG_TIMER:
      ERR("ATPubKey calc took too long!",0,0,0);
      break;

    default:
      ERR("Invalid Timer ID",0,0,0);
      break;
  }
#endif /* FEATURE_SEC */
} /* hdrkep_process_timer() */

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRKEP_CONFIG_MSG_CB

DESCRIPTION
  This function is registered to be called whenever a protocol gives a
  message to HDRKEP in-config instance.

DEPENDENCIES
  None

PARAMETERS
  msg - Pointer to the DSM item chain holding the message

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void hdrkep_config_msg_cb
(
  dsm_item_type * msg_ptr
)
{
  hdrmc_queue_msg( HDRHAI_KEY_EXCHANGE_PROTOCOL, HDRHAI_IN_CONFIG, msg_ptr );
} /* hdrkep_config_msg_cb */

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRKEP_IND_CB

DESCRIPTION
  This function is registered to be called whenever a protocol gives an
  indication of interest to HDRKEP.

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
void hdrkep_ind_cb
(
  hdrind_ind_name_enum_type  ind_name,
  void*                      ind_data_ptr
)
{
  if (rex_self() == HDRKEP_TASK_PTR)
  {
    hdrkep_process_ind(ind_name, (hdrind_ind_data_union_type *)ind_data_ptr);
  }
  else
  {
    hdrmc_queue_ind( HDRHAI_KEY_EXCHANGE_PROTOCOL, ind_name, ind_data_ptr );
  }
} /* hdrkep_ind_cb */

/* <EJECT> */ 
#ifdef FEATURE_SEC
/*===========================================================================

FUNCTION HDRKEP_KEYGEN_CB

DESCRIPTION
  This function kis called by the crypto layer whenever
  a DH key operation is done.

DEPENDENCIES
  None

PARAMETERS
  udata_ptr  - User data for callback (not used for IS-856)
  err_num     - Outcome of the Crypto layer operation

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
LOCAL void hdrkep_keygen_cb
(
  void*               udata_ptr,
  secerrno_enum_type  err_num
)
{

  SYS_ARG_NOT_USED(udata_ptr); /* remove this line when udata_ptr is used */

  if ( err_num != E_SUCCESS )
  {
    hdrkep_bummer( err_num,
                   "Key operation failed",
                   __LINE__ );
  }

  (void) rex_set_sigs( HDRKEP_TASK_PTR, HDRMC_KEP_KEYGEN_DONE_SIG );
} /* hdrkep_keygen_cb() */

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRKEP_CREATE_KEYS

DESCRIPTION
  This function derives the individual authentication and
  encryption keys from the DH-generated secret key.
  Currently only the reverse public channel authentication
  key is used.

  Standard specifies the calculation of TimeStampLong as follows:
  TimeStampLong = 
    (SystemTime - (SystemTime[15:0] - TimeStampShort) mod 2^16) mod 2^64

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
LOCAL hdrerrno_enum_type hdrkep_create_keys( void )
{
  secapi_handle_type          sha_hash = NULL;
  secapi_hsh_param_data_type  sha_param;
  secerrno_enum_type          err_num = E_SUCCESS;
  dsm_item_type*              scratch_ptr = NULL;
  dsm_item_type*              hash_output = NULL;
  qword                       sys_time;
  uint32                      sys_time_upper;
  uint32                      sys_time_lower;
  uint16                      nonce;
  qword                       tmp_qw;
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Get a scratch buffer to assemble the input for the SHA-1 hash */
  scratch_ptr = hdrutil_new_dsm_buffer();
  if ( scratch_ptr == NULL )
  {
    ERR_FATAL( "KEP: Out of DSM items", 0, 0, 0 );
  }

  do
  {
    /* Allocate a new SHA instance from the crypto layer */
    err_num = secapi_new( &sha_hash, SECAPI_SHA );
    if ( err_num != E_SUCCESS )
    {
      ERR( "KEP: Could not allocate SHA instance", 0, 0, 0 );
      break;
    }

    /* Initialize the hash functions to hash all input at once */
    sha_param.mode = SECAPI_HSH_INPUT_MODE_ATOMIC;
    err_num = secapi_hsh_set_param( sha_hash,
                                    SECAPI_SHA,
                                    SECAPI_HSH_INPUT_MODE,
                                    &sha_param );
    if ( err_num != E_SUCCESS )
    {
      ERR( "KEP: Could not set SECHSH_SHA_HMODE_ATOMIC", 0, 0, 0 );
      break;
    }

    /* First calculate the signature to verify the secret key */
    (hdrkep.skey)->used = hdrkep.negotiated.key_len;

#ifdef FEATURE_LTE_TO_HDR_OH
  if ( hdrsap_tunnel_mode_is_enabled( HDRHAI_KEY_EXCHANGE_PROTOCOL ) &&
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

    /* The calculation of TimeStampLong is optimized for the case where 
     * the lower 16 bits of system time did not roll over since the AN 
     * sent the message e.g. since the AN took its snapshot of 
     * TimeStampShort.  If it did roll over, however, we just roll back
     * system time by 0x10000 units (by doing a costly 64 bit subtraction).
     * After that the same optimized logic can be re-used. 
     */
    if (( qw_lo(sys_time) & ((uint32) 0x0000ffff )) < 
          (uint32) hdrkep.key_sig_timestamp_short )
    {
      /* Handle the TimeStampShort wrap around: 
       * The upper 48 bits of the system time needs 
       * to be decremented by 1, and the lower 16
       * bits will be replaced by key_sig_timestamp_short
       */
      qw_set( tmp_qw, ((uint32) 0x0), ((uint32) 0x00010000 ));
      qw_sub( sys_time, sys_time, tmp_qw );

      HDR_MSG_PROT_2(MSG_LEGACY_HIGH, 
                 "HDRKEP: TimeStampShort wrap around. Rcvd 0x%x, Curr 0x%x",
                 hdrkep.key_sig_timestamp_short, 
                 (qw_lo(sys_time) & ((uint32) 0x0000ffff)) );
    }

    sys_time_upper = qw_hi( sys_time );
    sys_time_lower = qw_lo( sys_time );
    sys_time_lower = (sys_time_lower & ((uint32) 0xffff0000))
                     | ((uint32) hdrkep.key_sig_timestamp_short);
    sys_time_upper = ps_htonl( sys_time_upper );
    sys_time_lower = ps_htonl( sys_time_lower );
    nonce = ps_htons( hdrkep.nonce );

    (void) hdrutil_pushdown_tail( &scratch_ptr,
                       &(hdrkep.key_comp_trans_id),
                       1 );
    (void) hdrutil_pushdown_tail( &scratch_ptr,
                       (uint8*) &(nonce),
                       2 );
    (void) hdrutil_pushdown_tail( &scratch_ptr,
                       (uint8*) &sys_time_upper,
                       4 );
    (void) hdrutil_pushdown_tail( &scratch_ptr,
                       (uint8*) &sys_time_lower,
                       4 );

    ASSERT( scratch_ptr );

    (hdrkep.skey)->pkt_ptr = scratch_ptr;
   

/* For debugging purposes only */
#ifdef FEATURE_HDR_SEC_DEBUG
    #error code not present
#endif /* FEATURE_HDR_SEC_DEBUG */

    err_num = secapi_hsh_create_digest_dsm( sha_hash,
                                            SECAPI_SHA,
                                            hdrkep.skey,
                                            NULL,
                                            dsm_length_packet(hdrkep.skey),
                                            &hash_output );
    ASSERT(hash_output);

    if ( err_num != E_SUCCESS )
    {
      hdrkep_bummer( err_num,
                     "Key Sign. digest calc. failed",
                     __LINE__ );
      /* We do not need to free the scratch_ptr as it is
         already freed in hdrkep_bummer() because it was
         manually chained to hdrkep.skey */
      break;
    }
#ifdef FEATURE_MEMCPY_REMOVAL
    memscpy( hdrkep.at_key_sig, 
             sizeof(hdrkep.at_key_sig), 
             hash_output->data_ptr,
             HDRKEP_KEY_SIG_LEN );
#else
    memcpy( hdrkep.at_key_sig,
            hash_output->data_ptr,
            HDRKEP_KEY_SIG_LEN );
#endif /* FEATURE_MEMCPY_REMOVAL */
    hash_output->used = 0;

    /* Now adjust the hashing data to exclude the transaction ID (8 bit) */
    scratch_ptr->data_ptr++;
    scratch_ptr->used--;

    /* --------------- Beginning of subkey generation ---------------- */
    /* Here we loop over all subkeys needed:                           */

    /* Forward AUTH assigned channel (adjust pointer to subkey index 0) */

    /* Reverse AUTH assigned channel (adjust pointer to subkey index 1) */

    /* Forward ENC assigned channel (adjust pointer to subkey index 2) */

    /* Reverse ENC assigned channel (adjust pointer to subkey index 3) */

    /* Forward AUTH public channel (adjust pointer to subkey index 4) */

    /* Reverse AUTH public channel (adjust pointer to subkey index 5) */
    (hdrkep.skey)->data_ptr +=
    HDRKEP_RPC_AUTH_KEY_OFFSET( hdrkep.negotiated.key_len );
    (hdrkep.skey)->used = hdrkep.negotiated.key_len / 8;

    /* Calculate the RPC key; instead of setting the hash_output DSM
       pointer directly to NULL, we call dsm_free_packet() */
    dsm_free_packet( &hash_output );
    err_num = secapi_hsh_create_digest_dsm( sha_hash,
                                            SECAPI_SHA,
                                            hdrkep.skey,
                                            NULL,
                                            dsm_length_packet(hdrkep.skey),
                                            &hash_output );
    if ( err_num != E_SUCCESS )
    {
      hdrkep_bummer( err_num,
                     "RPC key digest calc. failed",
                     __LINE__ );
      /* We do not need to free the scratch_ptr as it is
         already freed in hdrkep_bummer() because it was
         manually chained to hdrkep.skey */
      break;
    }
#ifdef FEATURE_MEMCPY_REMOVAL
    memscpy( hdrkep.negotiated.rpc_auth_key, 
             sizeof(hdrkep.negotiated.rpc_auth_key), 
             hash_output->data_ptr,
             HDRKEP_KEY_SIG_LEN );
#else
    memcpy( hdrkep.negotiated.rpc_auth_key,
            hash_output->data_ptr,
            HDRKEP_KEY_SIG_LEN );
#endif /* FEATURE_MEMCPY_REMOVAL */
    hash_output->used = 0;
    /* The hash_output DSM item is no longer used after this point so
       we perform proper cleanup by free'ing the packet */
    dsm_free_packet( &hash_output );

    /* Forward ENC public channel (adjust pointer to subkey index 6) */

    /* Reverse ENC public channel (adjust pointer to subkey index 7) */

    /* ------------------ End of subkey generation ------------------- */

    /* Now restore the secret key buffer and free the scratch buffer */
    (hdrkep.skey)->data_ptr -=
    HDRKEP_RPC_AUTH_KEY_OFFSET( hdrkep.negotiated.key_len );
    (hdrkep.skey)->used = hdrkep.negotiated.key_len;
    (hdrkep.skey)->pkt_ptr = NULL;
    dsm_free_packet( &scratch_ptr );

  } while( 0 ); /*lint !e717 */ 

  (void) secapi_delete( &sha_hash );

  return err_num;

} /* hdrkep_create_keys() */
#endif /* FEATURE_SEC */

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRKEP_GET_RPC_AUTH_KEY

DESCRIPTION
  This function returns the RPC Auth Key, if it has been negotiated.

DEPENDENCIES
  None

PARAMETERS
  rpc_auth_key - Pointer to an array of HDRKEP_KEY_SIG_LEN octets in which
                 the current value of rpc_auth_key is returned, if available.

RETURN VALUE
  Size of the key in bytes, 0 if non-valid.

SIDE EFFECTS
  None

===========================================================================*/
uint16 hdrkep_get_rpc_auth_key
(
  uint8* rpc_auth_key
)
{
  if (hdrkep.current.keys_are_valid)
  {
    /* The current seeeion key is divided into 8 subkeys: */
#ifdef FEATURE_MEMCPY_REMOVAL
    memscpy( rpc_auth_key, 
             HDRKEP_KEY_SIG_LEN, 
             hdrkep.current.rpc_auth_key,
             HDRKEP_KEY_SIG_LEN );
#else
    memcpy( rpc_auth_key,
            hdrkep.current.rpc_auth_key,
            HDRKEP_KEY_SIG_LEN );
#endif /* FEATURE_MEMCPY_REMOVAL */
    return HDRKEP_KEY_SIG_LEN;
  }

  return 0;

} /* hdrkep_get_rpc_auth_key */

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRKEP_GET_PREV_RPC_AUTH_KEY

DESCRIPTION
  This function returns the RPC Auth Key of the previous session,
  if it has been negotiated.

DEPENDENCIES
  None

PARAMETERS
  rpc_auth_key - Pointer to an array of HDRKEP_KEY_SIG_LEN octets in which
                 the current value of rpc_auth_key is returned, if available.

RETURN VALUE
  Size of the key in bytes, 0 if non-valid.

SIDE EFFECTS
  None

===========================================================================*/
uint16 hdrkep_get_previous_rpc_auth_key
(
  uint8 *rpc_auth_key
)
{
  hdrkep_config_params_type previous_config;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  hdrscmdb_get_previous_config(HDRHAI_KEY_EXCHANGE_PROTOCOL, 
                               (void *) &previous_config);

  if (previous_config.keys_are_valid)
  {
    /* The current session key is divided into 8 subkeys: */
#ifdef FEATURE_MEMCPY_REMOVAL
    memscpy( rpc_auth_key,
             HDRKEP_KEY_SIG_LEN, 
             previous_config.rpc_auth_key,
             HDRKEP_KEY_SIG_LEN );
#else
    memcpy( rpc_auth_key,
            previous_config.rpc_auth_key,
            HDRKEP_KEY_SIG_LEN );
#endif /* FEATURE_MEMCPY_REMOVAL */
    return HDRKEP_KEY_SIG_LEN;
  }

  return 0;

} /* hdrkep_get_prev_rpc_auth_key */

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRKEP_SET_CONFIG_TO_DEFAULT

DESCRIPTION
  This function initializes the Key Exchange Protocol configuration parameters
  to default values.

DEPENDENCIES
  None

PARAMETERS
  current_config - Pointer to Key Exchange Protocol's current configuration 
                   structure in SCMDB.
                   
RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void hdrkep_set_config_to_default( hdrkep_config_params_type *current_config )
{

  current_config->key_len = HDRKEP_DEFAULT_KEY_LEN;
  current_config->keys_are_valid = FALSE;

} /* hdrkep_set_config_to_default() */
/* <EJECT> */ 

/*===========================================================================

FUNCTION HDRKEP_GET_INCONFIG_HANDLE

DESCRIPTION
  This function returns a pointer to inconfig instance of Key Exchange 
  Protocol 

DEPENDENCIES
  None

PARAMETERS
  None
                     
RETURN VALUE
  Pointer to inconfig instance of KEP

SIDE EFFECTS
  None

===========================================================================*/
void* hdrkep_get_inconfig_handle( void )
{
  return (void*) &hdrkep.negotiated;
} /* hdrkep_get_inconfig_handle() */
#ifdef FEATURE_SEC
/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRKEP_BUMMER

DESCRIPTION
  This function prints an error message to the F3 screen and notifies
  the session control protocol about the negotiation failure.

DEPENDENCIES
  None

PARAMETERS
  err_num       - Error type which occured
  err_msg_ptr  - Optional error message (requires string capability of MSG())
  line         - Line in which function was called (use __LINE__)

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
LOCAL void hdrkep_bummer
(
  hdrerrno_enum_type  err_num,
  char*               err_msg_ptr,
  int                 line
)
{
  hdrind_ind_data_union_type ind_data;  /* Data for negotiation failed ind */
  /* Allocate out of the heap because it is possible for this indication to
  be queued from a different task context. Otherwise, the payload may be
  corrupted which could cause a null pointer de-reference. */
  static hdrscp_scp_fail_ind_type session_fail_data;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SYS_ARG_NOT_USED(err_msg_ptr); /* remove this line when err_msg_ptr 
                                  * is used */

  ERR( "+++ Error %d in state %d at line %d +++",
       err_num, hdrkep.state, line );
  MSG_STR_ERROR( "+++ (%s)", err_msg_ptr, 0, 0, 0 );
  hdrkep.state = HDRKEP_KEYGEN_FAILED;

  /* fill the indication data */
  session_fail_data.reason = HDRHAI_CLOSE_PROT_NEG_FAIL;
  session_fail_data.type = HDRHAI_KEY_EXCHANGE_PROTOCOL;
  session_fail_data.subtype =
    hdrscp_get_config_subtype(HDRHAI_KEY_EXCHANGE_PROTOCOL) ;
  session_fail_data.attribute_id_length = HDRKEP_ATTRIBUTE_ID_LENGTH;

  /* Arbitrary number to represent key negotiation failure, since it */
  /* isn't a formal configuration attribute.                         */
  session_fail_data.attribute_id = 5;

  ind_data.scp_fail_ptr = (void *)&session_fail_data;
  hdrind_give_ind(HDRIND_KEP_PROT_NEG_FAILED,(void *)&ind_data);

  /* Free any allocated items */
  if ( hdrkep.at_pub_key ) dsm_free_packet( &hdrkep.at_pub_key );
  if ( hdrkep.an_pub_key ) dsm_free_packet( &hdrkep.an_pub_key );
  if ( hdrkep.skey       ) dsm_free_packet( &hdrkep.skey );

  /* Free the PKX instance used */
  (void) secapi_delete( &(hdrkep.pkx_handle) );

} /* hdrkep_bummer() */
#endif /* FEATURE_SEC */





