/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

       H D R   P A C K E T   A P P L I C A T I O N   C O N T R O L L E R 
       
                P R O T O C O L   C O N F I G U R A T I O N     

GENERAL DESCRIPTION
  The packet application controller manages the Protocol Configuration of
  all Packet Application related protocols.
    
EXTERNALIZED FUNCTIONS
    
INITIALIZATION AND SEQUENCING REQUIREMENTS
  The protocol must be initialized after system startup prior to activating 
  the protocol.  
  
  Copyright (c) 2004 - 2014 by Qualcomm Technologies, Inc.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* EJECT */
/*===========================================================================

                           EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/hdr/cp/src/hdrpacc.c#1 $ $DateTime: 2016/12/13 07:58:05 $ $Author: mplcsds1 $
   
when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/09/14   cnx     Support NV refresh without reset.
06/17/14   vko     Race Condition in HDRCP in processing SLP ACK for Config Req and Config Response 
12/10/13   sat     Changed memcpy() to assignment or memscpy() (safer version).
03/19/12   cnx     Resolved TCB compiler errors for Dime. 
09/18/09   wsh     CMI: support changes from modem DATA
07/22/09   wsh     Check FlowNNTimers and MaxAbortTimer before send ConfResp
12/04/08   pba     Lint and Compiler warning fixes
09/11/07   wsh     Removed lint high warnings
07/30/07   pba     Featurized MMPA code under FEATURE_HDR_REVB.
07/30/07   pba     Obsoleted feature FEATURE_HDR_EMPA_SUPPORT_STD_V115_ATTRIB_ID
07/17/07   pba     RelB MMPA support.
07/02/07   cc      Added Subtype 4 RTCMAC Support.
04/03/07   wsh     Fixed error where dsm_item is not freed
01/08/07   yll     Added support for CSNA
11/30/06   yll     MaxReservation is not GAUPable.
11/16/06   yll     Don't include EMPA attributes in AT-initiated Config 
                   Request if EMPA is not negotiated.
10/18/06   wsh     Added logic to ignore ReservationKKQosFwd/RevReq in GAUP
10/04/06   pba     Use updated SCMDB API.
10/04/06   yll     Don't send HDRIND_SCM_MRLP_GAUP when GAUP contains no
                   attributes.
10/03/06   pba     Fixed bug in ConfigRequest message unpacking routine.
10/02/06   pba     Cancel configresponse timer on session close.
08/08/06   ksu/yll EMPA: v2.0 session negotiation
07/24/06   mpa     Replaced hdrds_queue_ind with hdrds_queue_ind_ext.
07/06/06   yll     Changed the attrib_len from uint8 to uint16.
06/20/06   mpa     Removed HDRPACC_UPDATE_FCP_STATE_IN_DB_CMD.
06/05/06   pba     Added new command HDRPACC_UPDATE_FCP_STATE_IN_DB_CMD
06/02/06   dna     Don't send AttributeUpdateAccept on AC
05/17/06   etv     Added hdrpacc_update_flow_mapping.
05/10/06   yll     Added EMPA support.
05/10/06   vas     Moved hdrpac_init() to hdrpacc_powerup_init() from 
                   hdrpac_powerup_init(). This will ensure a command is queued 
                   for DS task to do powerup initialization only after the 
                   hdrstream_powerup_init() is done. This is because 
                   hdrpacc_powerup_init() is called from the hdrmc task after 
                   hdrstream_powerup_init. Hdrpac_powerup_init() is called 
                   during dstask powerup initialization & not guaranteed to 
                   execute after stream init
04/07/06   hal     Changed hdrutil_new_buffer() to use hdrutil_new_msg_buffer()
03/22/06   pba     Send Session close if AT could not accept AN's AUR message.
02/28/06   yll     Removed the obsolete attributes. 
12/13/05   pba     Removed obsoleted SCP_RECONFIGURED indication
11/16/05   yll     Added support for new attributes: XmitAbortTimer, 
                   DOSAllowed, MaxRLPFlows and MaxReservations.
10/04/05   pba     Added support to read/write config parameters from SCMDB/EFS
09/06/05   mpa     Fixed RanHandoff negotiation for default app.
08/19/05   mpa     Make HDRMRLPC_AN_SUPPORTED_QOS_PROFILES_ATTRIB_ID gaupable
08/07/05   mpa     Added GAUP support for MRLP + updated to new resKK format
03/07/05   pba     Separated handling of In-Use and In-Config messages for PAC.
03/07/05   pba     Process all indications to PAC in DS task context. Session 
                   related indications need to be processed both by PAC in DS 
                   task context and PACC in MC task context.
03/07/05   sy      Fixed Lint errors.
12/10/04   hal     Added header file hdrutil.h.
12/07/04   hal     Changed dsm_offset_new_buffer to hdrutil_new_buffer.
11/04/04   mpa     Added GAUP support.
04/04/04   mpa     Created.

===========================================================================*/

/* EJECT */
/*===========================================================================

                          INCLUDE FILES FOR MODULE
                     
===========================================================================*/
#include "hdr_variation.h"
#include "dsm.h"
#include "comdef.h"
#include "err.h"

#include "hdrbit.h" 
#include "hdrhai.h"
#include "hdrtrace.h"
#include "hdrdebug.h"
#include "hdrutil.h"
#include "hdrmci.h"
#include "hdrdsm.h"

#include "hdrhmp.h"
#include "hdrscp.h"
#include "hdrstream.h"

#include "hdrpacc.h"

#ifdef FEATURE_HDR_QOS
#include "hdrds_api.h"
#include "hdrmrlpc.h"
#include "hdrmrlpci.h" 
#endif /* FEATURE_HDR_QOS */
#include "hdrlupc.h"
#include "hdrfcp.h"
#include "hdrdos.h"
#include "hdrscmdb.h"
#include "hdrscm.h"

/*===========================================================================

             DEFINITIONS  FROM  THE  HDR  AIR  INTERFACE  SPEC

===========================================================================*/

/* Configuration Messages  */
#define HDRPACC_CONFIG_REQ_MSG 0x50
#define HDRPACC_CONFIG_RSP_MSG 0x51
#define HDRPACC_ATTRIB_UPDATE_REQ_MSG  0x52
#define HDRPACC_ATTRIB_UPDATE_ACPT_MSG 0x53
#define HDRPACC_ATTRIB_UPDATE_REJ_MSG  0x54

#define HDRPACC_MESSAGE_PRIORITY 40

/*===========================================================================
            DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

===========================================================================*/

/*---------------------------------------------------------------------------
                   MESSAGE PACKING AND UNPACKING MACROS

  Following macros are used for unpacking received message and packing
  message to be sent
---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------
               Packing and unpacking for common fields
---------------------------------------------------------------------------*/

/* Unpack */
#define HDRPACC_UNPACK_MSG_ID( ptr, val_ptr )  \
        HDRBIT_UNPACK8( (ptr), (val_ptr), 0, 8 )

#define HDRPACC_UNPACK_TRANS_ID( ptr, val_ptr )  \
          HDRBIT_UNPACK8( (ptr), (val_ptr), 8, 8 )

/* Pack  */
#define HDRPACC_PACK_MSG_ID( ptr, val )  \
          HDRBIT_PACK8( (ptr), (val), 0, 8 )

#define HDRPACC_PACK_TRANSACTION_ID( ptr, val )  \
          HDRBIT_PACK8( (ptr), (val), 8, 8 )

/*---------------------------------------------------------------------------
  MACROS FOR EXTRACTING CONFIG REQUEST MESSAGE FIELDS (MULTIFLOW PACKET APP)
---------------------------------------------------------------------------*/
#define HDRPACC_BITS_PER_BYTE 8

#define HDRPACC_MSG_ID_SIZE               8
#define HDRPACC_TRANSACTION_ID_SIZE       8
#define HDRPACC_ATTRIB_LEN_SIZE           8

/* All MRLP/LUP simple attributes have the same size */
#define HDRPACC_SIMPLE_ATTRIB_VAL_SIZE   8

#define HDRPACC_UNPACK_CON_REQ_LEN( msg_ptr, val_ptr, aoffset) \
          (HDRBIT_UNPACK8((msg_ptr), (val_ptr), ((aoffset) + 0), 8))

/*-----------------------------------------
Default Packet App Configuration
-----------------------------------------*/
#define HDRPACC_ATTRIB_ID_DEFAULT_SIZE    8

#define HDRPACC_UNPACK_CON_REQ_DEFAULT_ATTRIB_ID( msg_ptr, val_ptr, aoffset) \
          (HDRBIT_UNPACK8((msg_ptr), (val_ptr), ((aoffset) + 8), 8))
          
#define HDRPACC_MIN_DEFLT_ATTRIB_LEN (HDRPACC_ATTRIB_LEN_SIZE + \
                                     HDRPACC_ATTRIB_ID_DEFAULT_SIZE)
                                        
/*-----------------------------------------
Multiflow Packet App Configuration
-----------------------------------------*/
#define HDRPACC_ATTRIB_ID_MULTIFLOW_SIZE 16           

#define HDRPACC_UNPACK_CON_REQ_MULTIFLOW_ATTRIB_ID( msg_ptr, val_ptr, aoffset) \
          (HDRBIT_UNPACK16((msg_ptr), (val_ptr), ((aoffset) + 8), 16))

#define HDRPACC_MIN_MULTIFLOW_ATTRIB_LEN (HDRPACC_ATTRIB_LEN_SIZE + \
                                         HDRPACC_ATTRIB_ID_MULTIFLOW_SIZE)

/*---------------------------------------------------------------------------
              MACROS FOR EXTRACTING CONFIG RESPONSE MESSAGE FIELDS 
---------------------------------------------------------------------------*/
#define HDRPACC_UNPACK_CON_RSP_LEN( msg_ptr, val_ptr, aoffset) \
          (HDRBIT_UNPACK8((msg_ptr), (val_ptr), ((aoffset) + 0), 8))

#define HDRPACC_UNPACK_CON_RSP_ATTRIB_ID( msg_ptr, val_ptr, aoffset) \
          (HDRBIT_UNPACK16((msg_ptr), (val_ptr), ((aoffset) + 8), 16))

#define HDRPACC_UNPACK_CON_RSP_ATTRIB_VAL( msg_ptr, val_ptr, aoffset) \
          (HDRBIT_UNPACK8((msg_ptr), (val_ptr), ((aoffset) + 24), 8))

/*---------------------------------------------------------------------------
                MACROS FOR PACKING CONFIG RESPONSE MESSAGE FIELDS 
---------------------------------------------------------------------------*/

/*-----------------------------------------
Multiflow Packet App Configuration
-----------------------------------------*/
#define HDRPACC_CON_RSP_MULTIFLOW_ATTRIB_LEN (HDRPACC_ATTRIB_LEN_SIZE      + \
                                    HDRPACC_ATTRIB_ID_MULTIFLOW_SIZE       + \
                                    HDRPACC_ATTRIB_LEN_SIZE)

typedef enum
{
  HDRPACC_CONFIG_RSP_TIMER, /* HDRPAC Config Response Timer */
  HDRPACC_GAUP_RSP_TIMER    /* GAUP Response Timer */
} hdrpacc_timer_enum_type;


/* The following structure contains all the global variables used by PACC */

LOCAL struct
{
  /* Flag indicating whether Idle is waiting for a config req msg TX callback
     from SLP */
  boolean await_slp_ack; 

  /* Transaction ID for the Config Request msg (assuming we are 
     always sending everything in one configuration request message)  */
  uint8 transaction_id;

  /* Waiting for Config Rsp Message timer */
  rex_timer_type config_rsp_timer; 

} hdrpacc;
          
/*===========================================================================

                    LOCAL FUNCTION DECLARATIONS
                    
===========================================================================*/
LOCAL void hdrpacc_timer_cb
(
  uint32 timer_id
);

LOCAL void hdrpacc_process_default_msg 
(
  dsm_item_type   *msg_ptr, 
  uint8           msg_id,
  hdrhai_stream_enum_type stream_id
);

LOCAL void hdrpacc_process_default_config_req_msg
(
  dsm_item_type *item_ptr,
  hdrhai_stream_enum_type stream_id
);

#ifdef FEATURE_HDR_QOS
LOCAL void hdrpacc_process_multiflow_msg 
(
  dsm_item_type   *msg_ptr, 
  uint8           msg_id,
  hdrhai_stream_enum_type stream_id
);

LOCAL void hdrpacc_process_mf_config_req_msg
(
  dsm_item_type *item_ptr,
  hdrhai_stream_enum_type stream_id
);

LOCAL void hdrpacc_process_mf_config_rsp_msg
(
  dsm_item_type *item_ptr,
  hdrhai_stream_enum_type stream_id
);

LOCAL void hdrpacc_process_mf_attrib_update_req_msg
(
  dsm_item_type *item_ptr,
  hdrhai_stream_enum_type stream_id
);
#endif /* FEATURE_HDR_QOS */

LOCAL void hdrpacc_send_default_config_rsp_msg
(
  dsm_item_type *item_ptr,
  hdrhai_stream_enum_type stream_id
);

LOCAL void hdrpacc_config_tx_outcome_cb
(
  void                 *data_ptr,
  hdrerrno_enum_type    err_val
);

LOCAL void hdrpacc_fail_cur_config_neg
(
  hdrhai_session_close_reason_enum_type close_reason
);

/* EJECT */
/*============================================================================

                        FUNCTION DEFINITIONS

=============================================================================*/
/*=============================================================================
FUNCTION HDRPACC_POWERUP_INIT

DESCRIPTION
  This function should be called once upon system startup.  The protocol 
  configuration is initialized. 

DEPENDENCIES
  None.

PARAMETERS
  None
  
RETURN VALUE
  Returns a pointer to the current config data (always valid)

SIDE EFFECTS
  None.
=============================================================================*/
void hdrpacc_powerup_init( void )
{
  if ( hdrmc_is_first_init() )
  {
    /* Initialize timers. */
    rex_def_timer_ex(&hdrpacc.config_rsp_timer,
                     hdrpacc_timer_cb,
                     HDRPACC_CONFIG_RSP_TIMER);
  }

  hdrpacc.await_slp_ack  = FALSE;
  hdrpacc.transaction_id = 0;

  hdrlupc_powerup_init();
#ifdef FEATURE_HDR_QOS
  hdrmrlpc_powerup_init();
#endif /* FEATURE_HDR_QOS */
} /* hdrpacc_powerup_init() */
  
/* <EJECT> */
/*===========================================================================

FUNCTION HDRPACC_PROCESS_CONFIGURE

DESCRIPTION
  This function processes the Configure Command.

DEPENDENCIES
  Assumes that the Streams have already been negotiated.

PARAMETERS
  app       - application to Configure
  stream    - ID of the stream this application is bound to 
              (negotiated value, not necessarily current)

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void hdrpacc_process_configure 
( 
  hdrhai_app_subtype_enum_type app,
  hdrhai_stream_enum_type stream
)
{
#ifdef FEATURE_HDR_QOS
  dsm_item_type * send_msg_ptr; /* Pointer to config req. message to send */
  uint16 aoffset; /* Bit offset from start of message of current attrib */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If already waiting for ack from SLP for a ConfigureRequest message,
     ignore this Configure command & do not sent another ConfigureRequest */  
  if( hdrpacc.await_slp_ack == TRUE )
  {
    ERR("Configure cmd ignored. Awaiting SLP ack",0,0,0);
    return;
  }

  (void) rex_clr_timer( &hdrpacc.config_rsp_timer );

  send_msg_ptr = hdrutil_new_msg_buffer();

  /* Remember what we are requesting in a regional structure */
  hdrpacc.transaction_id++;

  hdrpacc.await_slp_ack = TRUE;

  /* Build the config request messge */
  HDRPACC_PACK_MSG_ID( send_msg_ptr, HDRPACC_CONFIG_REQ_MSG);
  HDRPACC_PACK_TRANSACTION_ID( send_msg_ptr, hdrpacc.transaction_id);
    
  /* Set aoffset to the offset where the first attribute is to be packed in */
  aoffset = HDRPACC_MSG_ID_SIZE + HDRPACC_TRANSACTION_ID_SIZE;
  
  switch (app)
  {
    case HDRHAI_MULTIFLOW_PACKET_APP_SN:

    #ifdef FEATURE_HDR_EMPA
    case HDRHAI_ENHANCED_MULTIFLOW_PACKET_APP_SN:
    #endif /* FEATURE_HDR_EMPA */
    #ifdef FEATURE_HDR_REVB
    case HDRHAI_MULTILINK_MULTIFLOW_PACKET_APP_SN:
    #endif /* FEATURE_HDR_REVB */
      /* Initialize the In Config params for the MRLP attributes */
      hdrmrlpc_config_init();

      /* Pack the Configuration Attributes for MRLP */
      hdrmrlpc_pack_config_req_attribs(app, send_msg_ptr, &aoffset);
      break;

    default:
      ERR("App. is not supported for AT neg.",0,0,0);
      return; 
  }

  hdrhmp_app_send_msg(HDRHAI_PACKET_APPLICATION_CONTROLLER,
                      stream,
                      HDRHAI_RTC,
                      TRUE,
                      HDRPACC_MESSAGE_PRIORITY,
                      HDRHAI_IN_CONFIG, /* Instance of protocol sending message */
                      send_msg_ptr,
                      hdrpacc_config_tx_outcome_cb,
                      NULL);

#endif /* FEATURE_HDR_QOS */
} /* hdrpacc_process_configure */

/*===========================================================================

FUNCTION HDRPACC_PROCESS_DEFAULT_MSG

DESCRIPTION
  This function processes default messages for PACC.
  
DEPENDENCIES
  None

PARAMETERS
  msg_ptr   - Pointer to the DSM item chain holding the message
  msg_id    - The message ID already extracted from the DSM item
  stream_id - stream the message was received on
  
RETURN VALUE
  None
  
SIDE EFFECTS
  None

===========================================================================*/
void hdrpacc_process_default_msg 
(
  dsm_item_type   *msg_ptr, 
  uint8           msg_id,
  hdrhai_stream_enum_type stream_id
)
{
  switch (msg_id)
  {

    case HDRPACC_CONFIG_REQ_MSG:
  /*---------------------------------------------------------------------
      ConfigRequest message
  ---------------------------------------------------------------------*/
      HDR_MSG_PROT (MSG_LEGACY_MED, "Config request message received");
      hdrpacc_process_default_config_req_msg( msg_ptr, stream_id );
    break;

    default:
      /*-----------------------------------------------------------------------
      Default: message not recognized.
      -----------------------------------------------------------------------*/
      HDR_MSG_PROT_1( MSG_LEGACY_ERROR, "Unknown message (%d)", msg_id );
    break;
  }

} /* hdrpacc_process_default_msg */

/* EJECT */
/*=============================================================================
FUNCTION HDRPACC_PROCESS_DEFAULT_CONFIG_REQ_MSG

DESCRIPTION
  This function processes the Configuration Request message.
  
DEPENDENCIES
  Assumes that the config request message was received on a stream negotiated
  for a default packet apps bound to the SN.

PARAMETERS
  item_ptr  - Pointer to the DSM item chain holding the message.
  stream_id - Stream the message was received on
  
RETURN VALUE
  None.
  
SIDE EFFECTS
  None.
=============================================================================*/
void hdrpacc_process_default_config_req_msg
(
  dsm_item_type *item_ptr,
  hdrhai_stream_enum_type stream_id
)
{

  int total_bits_remaining = 0; /* Number of bits remaining in the message */
  uint8  creq_transaction_id;   /* Transaction ID of the config req */

  uint8  attrib_len_in_bytes = 0;
  uint16 attrib_len;            /* Bit length of attribute, including the 
                                   length field */
  uint16 attrib_offset = 0;     /* Bit offset of current attribute */
  uint8  attrib_id;             /* ID field of attribute */

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

  dsm_item_type *send_msg_ptr; /* dsm buffer for sent message */

/*- - - - - - - - - - - - -- - - - - - - - - - - - - - - - - - - - - - - - -*/
  
  total_bits_remaining = (dsm_length_packet(item_ptr) * 
                               HDRPACC_BITS_PER_BYTE );

  /*---------------------------------------------------------------------
  Extract the Transaction ID or Sequence Number from the message 
  ---------------------------------------------------------------------*/
  if( HDRPACC_UNPACK_TRANS_ID(item_ptr,&creq_transaction_id ) 
      != E_SUCCESS )
  {
    ERR("Error extracting TransactionID from ConfigReq Msg", 0, 0, 0 );
    /* Without the transaction ID, it makes no sense to send a response */
    return;
  }
    
  /* Initialize attrib_len so the initial attrib_offset will be
   * immediately after the transaction ID of the config request.
   */
  attrib_len = HDRPACC_MSG_ID_SIZE + HDRPACC_TRANSACTION_ID_SIZE; 

  /*---------------------------------------------------------------------
  Unpack the configuration attributes 
  ---------------------------------------------------------------------*/
  hdrlupc_config_init();

  /* While there are at least enough bits for the smallest attribute... */ 
  while ((total_bits_remaining - attrib_len) >= HDRPACC_MIN_DEFLT_ATTRIB_LEN)
  {
    /* Move to the next attribute record */
    attrib_offset += attrib_len;
    total_bits_remaining -= attrib_len;

    /* Get the length of the new attribute */
    if( HDRPACC_UNPACK_CON_REQ_LEN( item_ptr, &attrib_len_in_bytes, 
                                    attrib_offset ) != 
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
    attrib_len = attrib_len_in_bytes * HDRPACC_BITS_PER_BYTE;
    attrib_len += HDRPACC_ATTRIB_LEN_SIZE;

    /* Any attribute record must have at least an attribute ID and a value */
    if ((attrib_len < HDRPACC_MIN_DEFLT_ATTRIB_LEN) ||
        (attrib_len > total_bits_remaining ))
    {
      /* If the length is something unexpected, we can't be sure of what 
       * the AN intended and it is safest to not send a response
       */
        ERR("Error, invalid total len vs bits in message. Ignoring",0,0,0);
        send_response = FALSE;
        break;
    }

    /* There is at least one more attribute record in the message */

    if( HDRPACC_UNPACK_CON_REQ_DEFAULT_ATTRIB_ID( item_ptr, &attrib_id, 
        attrib_offset ) != E_SUCCESS )
    {
      ERR("Error extracting AttributeID from ConfigReq Msg. Ignore", 0, 0, 0 );
      /* If we can't get the attribute_id, don't send a response */
      send_response = FALSE;
      break;
    }
    
    switch( attrib_id )
    {
      /*-----------------------------------------------------------------------
      Unpack Ran Handoff Attribute (LUP)
      -----------------------------------------------------------------------*/
      case HDRLUPC_RAN_HANDOFF_DEFAULT_ATTRIB_ID: 
        send_response = hdrlupc_unpack_default_config_req_attrib(item_ptr,
                                                                 attrib_len,
                                                                 attrib_offset);
        break;
      
      default:
        ERR("Unknown Attribute ID (0x%x)", 
             attrib_id, 0, 0 );
        break;
    }

    if (!send_response)
    {
      /* No need to continue parsing once we decide not to send a response */
      break;
    }

  } /* while( there are more attributes in the config request ) */

  if (send_response)
  {
    /* get new buffer */
    send_msg_ptr = hdrutil_new_msg_buffer();

    /*---------------------------------------------------------------------
    Pack Message ID
    ---------------------------------------------------------------------*/
    HDRPACC_PACK_MSG_ID( send_msg_ptr, HDRPACC_CONFIG_RSP_MSG);

    /*---------------------------------------------------------------------
    Pack Transaction ID.
    ---------------------------------------------------------------------*/
    HDRPACC_PACK_TRANSACTION_ID( send_msg_ptr, 
                                 creq_transaction_id);

    attrib_offset = HDRPACC_TRANSACTION_ID_SIZE + HDRPACC_MSG_ID_SIZE;

    /*---------------------------------------------------------------------
    Pack LUPC Attribute(s)
    ---------------------------------------------------------------------*/
    hdrlupc_pack_default_config_rsp_attribs( send_msg_ptr, &attrib_offset);

    /* Send config response on RTC.  All config messages are reliable. 
     */
    hdrhmp_app_send_msg( HDRHAI_PACKET_APPLICATION_CONTROLLER,
                         stream_id,
                         HDRHAI_RTC,
                         TRUE,
                         HDRPACC_MESSAGE_PRIORITY,
                         HDRHAI_IN_CONFIG, /* Instance of protocol sending message */
                         send_msg_ptr,
                         NULL,
                         NULL );

  } /* end if (send_response) */

} /* hdrpacc_process_default_config_req_msg() */

/* EJECT */
#ifdef FEATURE_HDR_QOS
/*===========================================================================

FUNCTION HDRPACC_PROCESS_MULTIFLOW_MSG

DESCRIPTION
  This function processes multiflow messages for PACC.

DEPENDENCIES
  None

PARAMETERS
  msg_ptr   - Pointer to the DSM item chain holding the message
  msg_id    - The message ID already extracted from the DSM item
  stream_id - stream the message was received on

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void hdrpacc_process_multiflow_msg 
(
  dsm_item_type   *msg_ptr, 
  uint8           msg_id,
  hdrhai_stream_enum_type stream_id
)
{
  switch (msg_id)
  {

    case HDRPACC_CONFIG_REQ_MSG:
      /*---------------------------------------------------------------------
      ConfigRequest message
      ---------------------------------------------------------------------*/
      HDR_MSG_PROT (MSG_LEGACY_MED, "Config request message received");
      hdrpacc_process_mf_config_req_msg( msg_ptr, stream_id );
      break;

    case HDRPACC_CONFIG_RSP_MSG:
      /*---------------------------------------------------------------------
      ConfigResponse message
      ---------------------------------------------------------------------*/
      HDR_MSG_PROT (MSG_LEGACY_MED, "Config response message received");
      hdrpacc_process_mf_config_rsp_msg( msg_ptr, stream_id );
      break;

    case HDRPACC_ATTRIB_UPDATE_REQ_MSG:
      /*---------------------------------------------------------------------
      AttributeUpdateRequest message
      ---------------------------------------------------------------------*/
      HDR_MSG_PROT(MSG_LEGACY_MED, "Attribute Update Request message received");
      hdrpacc_process_mf_attrib_update_req_msg( msg_ptr, stream_id );
      break;

    case HDRPACC_ATTRIB_UPDATE_ACPT_MSG:
      /*---------------------------------------------------------------------
      AttributeUpdateAccept message
      ---------------------------------------------------------------------*/
      HDR_MSG_PROT(MSG_LEGACY_MED, "Attribute Update Accept message received");
      hdrmrlpc_process_attrib_update_accept_msg( msg_ptr, stream_id );
      break;

    case HDRPACC_ATTRIB_UPDATE_REJ_MSG:
      /*---------------------------------------------------------------------
      AttributeUpdateReject message
      ---------------------------------------------------------------------*/
      HDR_MSG_PROT(MSG_LEGACY_MED, "Attribute Update Reject message received");
      hdrmrlpc_process_attrib_update_reject_msg( msg_ptr, stream_id );
      break;


    default:
      /*-----------------------------------------------------------------------
      Default: message not recognized.
      -----------------------------------------------------------------------*/
      ERR_FATAL( "Unknown message (%d) ", msg_id, 0, 0 );
      break;
  }

} /* hdrpacc_process_multiflow_msg */

/* <EJECT> */ 
/*=============================================================================
FUNCTION HDRPACC_PROCESS_MF_CONFIG_REQ_MSG

DESCRIPTION
  This function processes the Configuration Request message. (AN INIT)
  
DEPENDENCIES
  Assumes that the config request message was received on a stream negotiated
  for a multiflow packet apps bound to the SN.

PARAMETERS
  item_ptr  - Pointer to the DSM item chain holding the message.
  stream_id - Stream the message was received on
  
RETURN VALUE
  None.
  
SIDE EFFECTS
  None.
=============================================================================*/
void hdrpacc_process_mf_config_req_msg
(
  dsm_item_type *item_ptr,
  hdrhai_stream_enum_type stream_id
)
{
  int total_bits_remaining = 0; /* Number of bits remaining in the message */
  uint8  creq_transaction_id;   /* Transaction ID of the config req */

  uint16 attrib_len;            /* Bit length of attribute, including the 
                                   length field */
  uint16 attrib_offset = 0;     /* Bit offset of current attribute */
  uint16 attrib_id;             /* ID field of attribute */
  uint8  attrib_idx;            /* Attribute Index for FlowNN and ResKK 
                                   attributes */
  uint8  attrib_len_in_bytes = 0;
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
   *
   * A local copy of the negotiated parameters is made in LUPC/MRLPC
   * before we start parsing.  As new parameters are accepted, the copy
   * is updated.  It is moved to negotiated if we don't encounter any
   * errors.
   */

  dsm_item_type *send_msg_ptr; /* dsm buffer for sent message */

  hdrhai_app_subtype_enum_type app_subtype;

/*- - - - - - - - - - - - -- - - - - - - - - - - - - - - - - - - - - - - - -*/

  total_bits_remaining = (dsm_length_packet(item_ptr) * 
                          HDRPACC_BITS_PER_BYTE );

  /*---------------------------------------------------------------------
  Extract the Transaction ID or Sequence Number from the message 
  ---------------------------------------------------------------------*/
  if( HDRPACC_UNPACK_TRANS_ID(item_ptr,&creq_transaction_id ) 
      != E_SUCCESS )
  {
    ERR("Error extracting TransactionID from ConfigReq Msg", 0, 0, 0 );
    /* Without the transaction ID, it makes no sense to send a response */
    return;
  }
    
  /* Initialize attrib_len so the initial attrib_offset will be
   * immediately after the transaction ID of the config request.
   */
  attrib_len = HDRPACC_MSG_ID_SIZE + HDRPACC_TRANSACTION_ID_SIZE; 

  /*---------------------------------------------------------------------
  Unpack the configuration attributes 
  ---------------------------------------------------------------------*/
  /* Make a copy of the negotiated parameters */
  hdrlupc_config_init();
  hdrmrlpc_config_init();

  /* While there are at least enough bits for the smallest attribute... */ 
  while ((total_bits_remaining - attrib_len) >= HDRPACC_MIN_MULTIFLOW_ATTRIB_LEN)
  {
    /* Move to the next attribute record */
    attrib_offset += attrib_len;
    total_bits_remaining -= attrib_len;

    /* Get the length of the new attribute */
    if( HDRPACC_UNPACK_CON_REQ_LEN( item_ptr, &attrib_len_in_bytes, attrib_offset ) != 
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
    attrib_len = attrib_len_in_bytes * HDRPACC_BITS_PER_BYTE;
    attrib_len += HDRPACC_ATTRIB_LEN_SIZE;

    /* Any attribute record must have at least an attribute ID and a value */
    if ((attrib_len < HDRPACC_MIN_MULTIFLOW_ATTRIB_LEN) ||
        (attrib_len > total_bits_remaining ))
    {
      /* If the length is something unexpected, we can't be sure of what 
       * the AN intended and it is safest to not send a response
       */
      ERR("Error, invalid total len vs bits in message. Ignoring",0,0,0);
      send_response = FALSE;
      break;
    }

    /* There is at least one more attribute record in the message */
    if( HDRPACC_UNPACK_CON_REQ_MULTIFLOW_ATTRIB_ID( item_ptr, &attrib_id, 
                                                    attrib_offset ) != E_SUCCESS )
    {
      ERR("Error extracting AttributeID from ConfigReq Msg. Ignore", 0, 0, 0 );
      /* If we can't get the attribute_id, don't send a response */
      send_response = FALSE;
      break;
    }
    
    /*-------------------------------------------------------------------------
    Extract Flow ID from Attribute ID (for MRLP attributes)
    -------------------------------------------------------------------------*/
    if ( ((attrib_id & 0xFF00) == 0xFF00) || 
         ((attrib_id & 0xFF00) == 0x0000) )
    {
      attrib_idx = 0;
    }
    else
    {
      attrib_idx = attrib_id & 0x00FF;
      attrib_id = attrib_id & 0xFF00;
    }

    app_subtype = hdrstream_get_apptype_for_neg_stream( stream_id );

    switch( attrib_id )
    {
      /*-----------------------------------------------------------------------
      LUP Attributes
      -----------------------------------------------------------------------*/
      case HDRLUPC_RAN_HANDOFF_MULTIFLOW_ATTRIB_ID: 
        send_response = hdrlupc_unpack_multiflow_config_req_attrib( item_ptr,
                                                                    attrib_len,
                                                                    attrib_offset );
        break;

      /*-----------------------------------------------------------------------
      MRLP Attributes
      -----------------------------------------------------------------------*/
      /* Note:- SeqLenFwd simple attrib (HDRMRLPC_FLOW_NN_SEQ_LEN_FWD_ATTRIB_ID)
         is defined only for MFPA/EMPA. In MMPA SeqLengthFwd is a complex attrib
         (HDRMRLPC_MMPA_FLOW_NN_SEQ_LEN_FWD_ATTRIB_ID) and it has the same attrib
         id as HDRMRLPC_FLOW_NN_IDENTIFICATION_REV_ATTRIB_ID */
      case HDRMRLPC_MAX_ABORT_TIMER_ATTRIB_ID: 
      case HDRMRLPC_PPP_FREE_AUTHENTICATION_SUPPORT_ATTRIB_ID:
      case HDRMRLPC_TWO_ROUTES_SUPPORTED_ATTRIB_ID:
      case HDRMRLPC_AT_NAKDELAY_SUPPORTED_ATTRIB_ID:
      case HDRMRLPC_FLOW_NN_NAK_ENABLE_FWD_ATTRIB_ID: 
      case HDRMRLPC_FLOW_NN_NAK_ENABLE_REV_ATTRIB_ID: 
      case HDRMRLPC_FLOW_NN_HIGHER_LAYER_PROT_FWD_ATTRIB_ID: 
      case HDRMRLPC_FLOW_NN_HIGHER_LAYER_PROT_REV_ATTRIB_ID:
      case HDRMRLPC_FLOW_NN_PHYS_LAYER_NAK_ENABLED_REV_ATTRIB_ID: 
      case HDRMRLPC_RES_KK_IDLE_STATE_FWD_ATTRIB_ID: 
      case HDRMRLPC_RES_KK_IDLE_STATE_REV_ATTRIB_ID: 
      case HDRMRLPC_FLOW_NN_TRANSMIT_ABORT_TIMER_REV_ATTRIB_ID:
      case HDRMRLPC_FLOW_NN_DATA_OVER_SIG_ALLOWED_REV_ATTRIB_ID:
      case HDRMRLPC_FLOW_NN_ACTIVATED_FWD_ATTRIB_ID:
      case HDRMRLPC_FLOW_NN_ACTIVATED_REV_ATTRIB_ID:
      case HDRMRLPC_FLOW_NN_SEQ_LEN_FWD_ATTRIB_ID:
      case HDRMRLPC_FLOW_NN_SEQ_LEN_REV_ATTRIB_ID:
      case HDRMRLPC_FLOW_NN_FLOW_PROT_SDU_FWD_ATTRIB_ID:
      case HDRMRLPC_FLOW_NN_FLOW_PROT_SDU_REV_ATTRIB_ID:
      case HDRMRLPC_FLOW_NN_DATA_UNIT_FWD_ATTRIB_ID:
      case HDRMRLPC_FLOW_NN_DATA_UNIT_REV_ATTRIB_ID:
      case HDRMRLPC_FLOW_NN_ROUTE_PROT_SDU_FWD_ATTRIB_ID:
      case HDRMRLPC_FLOW_NN_ROUTE_PROT_SDU_REV_ATTRIB_ID:
      case HDRMRLPC_FLOW_NN_SIM_DEL_BOTH_ROUTES_FWD_ATTRIB_ID:
      case HDRMRLPC_FLOW_NN_OOO_DEL_ROUTE_PROT_FWD_ATTRIB_ID:
      case HDRMRLPC_FLOW_NN_NAK_DELAY_TIME_FWD_ATTRIB_ID:

      /* Complex Attributes */
      case HDRMRLPC_FLOW_NN_IDENTIFICATION_FWD_ATTRIB_ID: 
      case HDRMRLPC_FLOW_NN_ID_REV_OR_SEQ_LEN_FWD_ATTRIB_ID: 
      case HDRMRLPC_FLOW_NN_TIMER_FWD_ATTRIB_ID: 
      case HDRMRLPC_FLOW_NN_TIMER_REV_ATTRIB_ID: 
      case HDRMRLPC_FLOW_NN_RESERVATION_FWD_ATTRIB_ID: 
      case HDRMRLPC_FLOW_NN_RESERVATION_REV_ATTRIB_ID:
  
      case HDRMRLPC_RES_KK_QOS_REQ_FWD_ATTRIB_ID: 
      case HDRMRLPC_RES_KK_QOS_REQ_REV_ATTRIB_ID: 
      case HDRMRLPC_RES_KK_QOS_RSP_FWD_ATTRIB_ID:
      case HDRMRLPC_RES_KK_QOS_RSP_REV_ATTRIB_ID:

      case HDRMRLPC_FLOW_NN_FLOW_PROT_PARM_FWD_ATTRIB_ID:
      case HDRMRLPC_FLOW_NN_FLOW_PROT_PARM_REV_ATTRIB_ID:
      case HDRMRLPC_FLOW_NN_ROUTE_PROT_PARM_FWD_ATTRIB_ID:
      case HDRMRLPC_FLOW_NN_ROUTE_PROT_PARM_REV_ATTRIB_ID:

      case HDRMRLPC_AT_SUP_FLOW_PROT_PARM_PP_ATTRIB_ID:
      case HDRMRLPC_AT_SUP_ROUTE_PROT_PARM_PP_ATTRIB_ID:

      case HDRMRLPC_SUPPORTED_HIGHER_LAYER_PROT_ATTRIB_ID: 
      case HDRMRLPC_AT_SUPPORTED_QOS_PROFILES_ATTRIB_ID:
      case HDRMRLPC_AN_SUPPORTED_QOS_PROFILES_ATTRIB_ID:
      case HDRMRLPC_MAX_RLP_FLOWS_ATTRIB_ID:
      case HDRMRLPC_MAX_RESERVATIONS_ATTRIB_ID:

        /* Make sure that for MFPA stream, only MFPA attributes are
         * valid, and for EMPA stream, only EMPA attributes are valid */
        if (hdrmrlpc_is_attrib_valid_for_appsubtype(app_subtype, attrib_id))
        {
          send_response = hdrmrlpc_unpack_config_req_attrib( item_ptr,
                                                             attrib_id,
                                                             attrib_idx,
                                                             attrib_len,
                                                             attrib_offset );
        }
        else
        {
          HDR_MSG_PROT_2( MSG_LEGACY_ERROR, 
                          "Invalid attrib ID (0x%x for app_subtype %d",
                          attrib_id | attrib_idx, app_subtype );
        }
        break;

      default:
        ERR("Unknown Attribute ID (0x%x)", 
             attrib_id | attrib_idx, 0, 0 );
        break;

    } /* end switch */

    if (!send_response)
    {
      /* No need to continue parsing once we decide not to send a response */
      break;
    }

  } /* while( there are more attributes in the config request ) */

  if (send_response)
  {
    /* always check if flow timers are valid */
    if (!hdrmrlpc_flownn_timers_are_valid())
    {
      HDR_MSG_PROT(MSG_LEGACY_ERROR,
                   "Invalid FlowNNTimer rejected");
    }

    /* get new buffer */
    send_msg_ptr = hdrutil_new_msg_buffer();

    /*---------------------------------------------------------------------
    Pack Message ID
    ---------------------------------------------------------------------*/
    HDRPACC_PACK_MSG_ID( send_msg_ptr, HDRPACC_CONFIG_RSP_MSG);

    /*---------------------------------------------------------------------
    Pack Transaction ID.
    ---------------------------------------------------------------------*/
    HDRPACC_PACK_TRANSACTION_ID( send_msg_ptr, 
                                 creq_transaction_id);

    attrib_offset = HDRPACC_TRANSACTION_ID_SIZE + HDRPACC_MSG_ID_SIZE;

    /*---------------------------------------------------------------------
    Pack LUP Attributes
    ---------------------------------------------------------------------*/
    hdrlupc_pack_mf_config_rsp_attribs( send_msg_ptr, &attrib_offset);
                                                     
    /*---------------------------------------------------------------------
    Pack MRLP Attributes
    ---------------------------------------------------------------------*/
    hdrmrlpc_pack_config_rsp_attribs( send_msg_ptr, &attrib_offset, stream_id);

    /* Send config response on RTC.  All config messages are reliable. 
     */
    hdrhmp_app_send_msg( HDRHAI_PACKET_APPLICATION_CONTROLLER,
                         stream_id,
                         HDRHAI_RTC,
                         TRUE,
                         HDRPACC_MESSAGE_PRIORITY,
                         HDRHAI_IN_CONFIG, /* Instance of protocol sending message */
                         send_msg_ptr,
                         NULL,
                         NULL );

  }
  else
  {   
    /* The AT is rejecting the current configuration request message */
    hdrmrlpc_config_init();
  } /* end if (send_response) */


} /* hdrpacc_process_mf_config_req_msg() */

/* EJECT */
/*=============================================================================
FUNCTION HDRPACC_PROCESS_MF_CONFIG_RSP_MSG

DESCRIPTION
  This function processes the Configuration Response message for a multiflow
  app. (AT Init)
  
DEPENDENCIES
  Assumes that the Streams have already been negotiated.

PARAMETERS
  item_ptr  - Pointer to the DSM item chain holding the message.
  stream_id - Stream the message was received on
  
RETURN VALUE
  None.
  
SIDE EFFECTS
  _mpa_ CURRENTLY UNUSED - KEEPING THE HOOKS IN FOR FUTURE EMFPA USE.
=============================================================================*/
/*lint -save -e715 stream_id*/
void hdrpacc_process_mf_config_rsp_msg
(
  dsm_item_type *item_ptr,
  hdrhai_stream_enum_type stream_id
)
{
  uint8  crsp_transaction_id; /* Transaction ID of the message */
  uint32 total_bits_remaining; /* Number of bits left to be unpacked */
  uint16 attrib_len; /* Bit length of attribute, including the length field */
  uint16 attrib_offset = 0; /* Bit offset of current attribute */
  uint16  attrib_id; /* ID field of current attribute being unpacked */
  uint8 value_id; /* Value ID of current attribute being unpacked */

  /* Flag if we should fail negotiation based on this config response */
  boolean negotiation_failed = FALSE; 

  /* Flag if there was an error with the message structure */
  boolean message_parsing_error = FALSE;

  /* sum of errno return vals (E_SUCCESS is zero) */
  int err; 

  /* Flow ID for a FlowNN MRLP attribute */
  uint8 flow_id; 

  /* Data for session config complete indication */
  hdrind_ind_data_union_type ind_data;

  hdrhai_app_subtype_enum_type app_subtype;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  total_bits_remaining =(uint32) (dsm_length_packet(item_ptr) * HDRPACC_BITS_PER_BYTE);

  /* Extract the Transaction ID from the message */
  if (HDRPACC_UNPACK_TRANS_ID(item_ptr, &crsp_transaction_id) != E_SUCCESS)
  {
    ERR("Error extracting TransactionID from ConfigRsp Msg", 0, 0, 0 );
    return;
  }

  /* if transaction ID doesn't match, ignore message */
  if (crsp_transaction_id != hdrpacc.transaction_id)
  {
    HDRTRACE_RXMSG_IGNORED_SEQ(HDRHAI_PACKET_APPLICATION_CONTROLLER,
                               HDRPACC_CONFIG_RSP_MSG, 0); //lint !e506
    HDR_MSG_PROT_2 (MSG_LEGACY_HIGH, "IgnoreConfigRsp-TransID exp =%d rcv=%d",
                    hdrpacc.transaction_id, crsp_transaction_id);
    return;
  }

  /* Initialize attrib_len so the initial attrib_offset will be
   * immediately after the transaction ID of the config response.
   */
  attrib_len = HDRPACC_MSG_ID_SIZE + HDRPACC_TRANSACTION_ID_SIZE;

  /* While there are at least enough bits for the smallest attribute... */ 
  while ((total_bits_remaining - attrib_len) >= 
         HDRPACC_CON_RSP_MULTIFLOW_ATTRIB_LEN)
  {
    /* Move to the next attribute record */
    attrib_offset += attrib_len;
    total_bits_remaining -= attrib_len;

    err  = HDRPACC_UNPACK_CON_RSP_LEN(item_ptr, (uint8 *)&attrib_len, attrib_offset);
    err |= HDRPACC_UNPACK_CON_RSP_ATTRIB_ID(item_ptr,&attrib_id,attrib_offset);
    err |= HDRPACC_UNPACK_CON_RSP_ATTRIB_VAL(item_ptr,&value_id,attrib_offset);
                                                 
    if (err != E_SUCCESS)
    {
      ERR("Error unpacking Configuration Response message, ignoring",0,0,0);
      message_parsing_error = TRUE;
      break;
    }

    /* The AttributeLength field in the message is the attribute length in 
     * bytes, excluding the length of the length field.  For our purposes, 
     * we want attrib_len to be the total length of the attribute in bits, 
     * including the length field.
     */
    attrib_len *= HDRPACC_BITS_PER_BYTE;
    attrib_len += HDRPACC_ATTRIB_LEN_SIZE;

    /* In a config response, all attributes are length, attrib_id, value_id */
    if ((attrib_len < HDRPACC_CON_RSP_MULTIFLOW_ATTRIB_LEN) ||
        (attrib_len > total_bits_remaining))
    {
      ERR("Error, invalid total len vs bits in message. ignoring",0,0,0);
      message_parsing_error = TRUE;
      break;
    }

    /*-------------------------------------------------------------------------
    Process Multi-RLP Attributes
    -------------------------------------------------------------------------*/
    if ( ((attrib_id & 0xFF00) == 0xFF00) || 
         ((attrib_id & 0xFF00) == 0x0000) )
    {
      flow_id = 0;
    }
    else
    {
      flow_id = attrib_id & 0x00FF;
      attrib_id = attrib_id & 0xFF00;
    }
      
    app_subtype = hdrstream_get_apptype_for_neg_stream( stream_id );

    switch( attrib_id )
    {
      /* Simple Attributes */
      case HDRMRLPC_MAX_ABORT_TIMER_ATTRIB_ID: 
      case HDRMRLPC_PPP_FREE_AUTHENTICATION_SUPPORT_ATTRIB_ID:
      case HDRMRLPC_TWO_ROUTES_SUPPORTED_ATTRIB_ID:
      case HDRMRLPC_AT_NAKDELAY_SUPPORTED_ATTRIB_ID:
      case HDRMRLPC_FLOW_NN_NAK_ENABLE_FWD_ATTRIB_ID: 
      case HDRMRLPC_FLOW_NN_NAK_ENABLE_REV_ATTRIB_ID: 
      case HDRMRLPC_FLOW_NN_HIGHER_LAYER_PROT_FWD_ATTRIB_ID: 
      case HDRMRLPC_FLOW_NN_HIGHER_LAYER_PROT_REV_ATTRIB_ID:
      case HDRMRLPC_FLOW_NN_PHYS_LAYER_NAK_ENABLED_REV_ATTRIB_ID: 
      case HDRMRLPC_RES_KK_IDLE_STATE_FWD_ATTRIB_ID: 
      case HDRMRLPC_RES_KK_IDLE_STATE_REV_ATTRIB_ID: 
      case HDRMRLPC_FLOW_NN_TRANSMIT_ABORT_TIMER_REV_ATTRIB_ID:
      case HDRMRLPC_FLOW_NN_DATA_OVER_SIG_ALLOWED_REV_ATTRIB_ID:
      case HDRMRLPC_FLOW_NN_ACTIVATED_FWD_ATTRIB_ID:
      case HDRMRLPC_FLOW_NN_ACTIVATED_REV_ATTRIB_ID:
      case HDRMRLPC_FLOW_NN_SEQ_LEN_FWD_ATTRIB_ID:
      case HDRMRLPC_FLOW_NN_SEQ_LEN_REV_ATTRIB_ID:
      case HDRMRLPC_FLOW_NN_FLOW_PROT_SDU_FWD_ATTRIB_ID:
      case HDRMRLPC_FLOW_NN_FLOW_PROT_SDU_REV_ATTRIB_ID:
      case HDRMRLPC_FLOW_NN_DATA_UNIT_FWD_ATTRIB_ID:
      case HDRMRLPC_FLOW_NN_DATA_UNIT_REV_ATTRIB_ID:
      case HDRMRLPC_FLOW_NN_ROUTE_PROT_SDU_FWD_ATTRIB_ID:
      case HDRMRLPC_FLOW_NN_ROUTE_PROT_SDU_REV_ATTRIB_ID:
      case HDRMRLPC_FLOW_NN_SIM_DEL_BOTH_ROUTES_FWD_ATTRIB_ID:
      case HDRMRLPC_FLOW_NN_OOO_DEL_ROUTE_PROT_FWD_ATTRIB_ID:
      case HDRMRLPC_FLOW_NN_NAK_DELAY_TIME_FWD_ATTRIB_ID:

      /* Complex Attributes */
      case HDRMRLPC_FLOW_NN_IDENTIFICATION_FWD_ATTRIB_ID: 
      case HDRMRLPC_FLOW_NN_ID_REV_OR_SEQ_LEN_FWD_ATTRIB_ID:
      case HDRMRLPC_FLOW_NN_TIMER_FWD_ATTRIB_ID: 
      case HDRMRLPC_FLOW_NN_TIMER_REV_ATTRIB_ID: 
      case HDRMRLPC_FLOW_NN_RESERVATION_FWD_ATTRIB_ID: 
      case HDRMRLPC_FLOW_NN_RESERVATION_REV_ATTRIB_ID:

      case HDRMRLPC_RES_KK_QOS_REQ_FWD_ATTRIB_ID: 
      case HDRMRLPC_RES_KK_QOS_REQ_REV_ATTRIB_ID: 
      case HDRMRLPC_RES_KK_QOS_RSP_FWD_ATTRIB_ID:
      case HDRMRLPC_RES_KK_QOS_RSP_REV_ATTRIB_ID:

      case HDRMRLPC_FLOW_NN_FLOW_PROT_PARM_FWD_ATTRIB_ID:
      case HDRMRLPC_FLOW_NN_FLOW_PROT_PARM_REV_ATTRIB_ID:
      case HDRMRLPC_FLOW_NN_ROUTE_PROT_PARM_FWD_ATTRIB_ID:
      case HDRMRLPC_FLOW_NN_ROUTE_PROT_PARM_REV_ATTRIB_ID:

      case HDRMRLPC_AT_SUP_FLOW_PROT_PARM_PP_ATTRIB_ID:
      case HDRMRLPC_AT_SUP_ROUTE_PROT_PARM_PP_ATTRIB_ID:

      case HDRMRLPC_SUPPORTED_HIGHER_LAYER_PROT_ATTRIB_ID: 
      case HDRMRLPC_AT_SUPPORTED_QOS_PROFILES_ATTRIB_ID:
      case HDRMRLPC_AN_SUPPORTED_QOS_PROFILES_ATTRIB_ID:
      case HDRMRLPC_MAX_RLP_FLOWS_ATTRIB_ID:
      case HDRMRLPC_MAX_RESERVATIONS_ATTRIB_ID:

        /* Make sure that for MFPA stream, only MFPA attributes are
         * valid, and for EMPA stream, only EMPA attributes are valid */
        if (hdrmrlpc_is_attrib_valid_for_appsubtype(app_subtype, attrib_id))
        {
          negotiation_failed = 
            hdrmrlpc_validate_config_rsp_attrib(attrib_id, flow_id, value_id);
        }
        else
        {
          HDR_MSG_PROT_2( MSG_LEGACY_ERROR, 
                          "Invalid attrib ID (0x%x for app_subtype %d",
                          attrib_id | flow_id, app_subtype );

          /* Accepting an attribute we never negotiated is grounds for failure */
          negotiation_failed = TRUE;
        }
        break;

      default:
        ERR("Unknown Attribute ID (0x%x)", attrib_id | flow_id, 0, 0 );
               
        /* Accepting an attribute we never negotiated is grounds for failure */
        negotiation_failed = TRUE;
        break;

    } /* End MRLP attribs switch */
      

    if ((negotiation_failed) || (message_parsing_error))
    {
      break;
    }
  } /* end while */

  if (message_parsing_error)
  {
    /* If we had an unexpected problem parsing the message then it is better
     * to just ignore it.  If the AN wants to send a valid response it still 
     * can, or else our config response timer will expire and we will abort.
     */
  }
  else if (negotiation_failed)
  {
    hdrpacc_fail_cur_config_neg(HDRHAI_CLOSE_PROT_NEG_FAIL);
  }
  else
  {
    if (hdrpacc.await_slp_ack)
    {
      /* The flag must be set first because hdrhmp_abort_msg() causes
       * the callback to be called, and if this flag is not FALSE
       * the callback will set the config response timer, for example.
       */
      hdrpacc.await_slp_ack = FALSE;
      (void) hdrhmp_abort_msg(hdrpacc_config_tx_outcome_cb, NULL);
    }

    /* Message is a response to our config request. Stop and clear timer */
    (void) rex_clr_timer( &hdrpacc.config_rsp_timer );

    /* Submit the parameters explicitely accepted by the AN 
       (others are set to fallback (negotiated) value) */
    /* _mpa_ removed */
    /* hdrmrlpc_config_rsp_submit();  */

    ind_data.prot_name = HDRHAI_PACKET_APPLICATION_CONTROLLER;
    hdrind_give_ind(HDRIND_SCM_CONFIG_COMPLETE, &ind_data);

  } /* end else (negotiation successful)*/
} /* hdrpacc_process_mf_config_rsp_msg() */
/*lint -restore*/

/* EJECT */
/*=============================================================================
FUNCTION HDRPACC_PROCESS_MF_ATTRIB_UPDATE_REQ_MSG

DESCRIPTION
  This function processes an AttributeUpdateRequest message for the
  Multiflow packet app.
    
DEPENDENCIES
  None

PARAMETERS
  item_ptr  - Pointer to the DSM item chain holding the message.
  stream_id - Stream the message was received on
  
RETURN VALUE
  None.
  
SIDE EFFECTS
  None.
=============================================================================*/
void hdrpacc_process_mf_attrib_update_req_msg
(
  dsm_item_type *item_ptr,
  hdrhai_stream_enum_type stream_id
)
{
  int total_bits_remaining = 0; /* Number of bits remaining in the message */
  uint8  areq_transaction_id;   /* Transaction ID of the config req */

  uint8  attrib_len_in_bytes = 0;
  uint16 attrib_len;            /* Bit length of attribute, including the 
                                   length field */
  uint16 attrib_offset = 0;     /* Bit offset of current attribute */
  uint16 attrib_id;             /* ID field of attribute */

  /* During AN Initiated GAUP, the AT is not reject the attributes the AN
   * is proposing (the AttributeUpdateReject message is reserved for the AN).
   * The AT will not send a response (AttributeUpdateAccept) message if it 
   * finds values that it cannot validate/use. 
   */
  boolean send_response = TRUE;

  /* As we parse the message we update the negotiated parameters in a 
   * local copy as we agree to accept new values.  That way, if we 
   * later encounter an error and decide not to send a response, we don't 
   * corrupt the real negotiated parameters.
   *
   * A local copy of the negotiated parameters is made in MRLPC
   * before we start parsing.  As new parameters are accepted, the copy
   * is updated.  It is moved to negotiated if we don't encounter any
   * errors.
   */

  dsm_item_type *send_msg_ptr; /* dsm buffer for sent message */

  uint8 flow_id; /* iterator */
  
  boolean gaup_is_accepted = TRUE;
    /* Flag to indicate if GAUP message from AN is accepted or not */
  static hdrscp_scp_fail_ind_type gaup_fail_data;
    /* Data for AN GAUP failed indication */
  hdrind_ind_data_union_type ind_data;
  hdrhai_app_subtype_enum_type  app_subtype; /* Check if MFPA or EMPA */
  boolean gaup_is_empty = FALSE;

/*- - - - - - - - - - - - -- - - - - - - - - - - - - - - - - - - - - - - - -*/

  total_bits_remaining = (dsm_length_packet(item_ptr) * 
                               HDRPACC_BITS_PER_BYTE );

  /*---------------------------------------------------------------------
  Extract the Transaction ID from the message 
  ---------------------------------------------------------------------*/
  if( HDRPACC_UNPACK_TRANS_ID(item_ptr,&areq_transaction_id ) 
      != E_SUCCESS )
  {
    ERR("Error extracting TransactionID from ConfigReq Msg", 0, 0, 0 );
    /* Without the transaction ID, it makes no sense to send a response */
    return;
  }
    
  /* Initialize attrib_len so the initial attrib_offset will be
   * immediately after the transaction ID of the config request.
   */
  attrib_len = HDRPACC_MSG_ID_SIZE + HDRPACC_TRANSACTION_ID_SIZE; 

  /*---------------------------------------------------------------------
  Unpack the configuration attributes 
  ---------------------------------------------------------------------*/
  /* Make a copy of the negotiated parameters */
  /* _mpa_ since we could have both AT Initiated GAUP and AN Initiated
     GAUP occuring at the same time, we don't reset the in_config instance
     It is set when the session is reconfigured instead and we keep using
     the same structure thereafter. */
  //hdrmrlpc_config_init();

  if ((total_bits_remaining - attrib_len) < HDRPACC_MIN_MULTIFLOW_ATTRIB_LEN)
  {
    gaup_is_empty = TRUE;
    HDR_MSG_PROT(MSG_LEGACY_ERROR, "Gaup is empty!!!");
  }

  /* While there are at least enough bits for the smallest attribute... */ 
  while ((total_bits_remaining - attrib_len) >= HDRPACC_MIN_MULTIFLOW_ATTRIB_LEN)
  {
    /* Move to the next attribute record */
    attrib_offset += attrib_len;
    total_bits_remaining -= attrib_len;

    /* Get the length of the new attribute */
    if( HDRPACC_UNPACK_CON_REQ_LEN( item_ptr, &attrib_len_in_bytes, attrib_offset ) != 
        E_SUCCESS )
    {
      ERR("Error unpacking AttributeUpdateReq Msg", 0, 0, 0 );
      /* Without the length we can't continue to parse the message. */
      send_response = FALSE;
      break;
    }

    /* The AttributeLength field in the message is the attribute length in 
     * bytes, excluding the length of the length field.  For our purposes, 
     * we want attrib_len to be the total length of the attribute in bits, 
     * including the length field.
     */
    attrib_len = attrib_len_in_bytes * HDRPACC_BITS_PER_BYTE;
    attrib_len += HDRPACC_ATTRIB_LEN_SIZE;

    /* Any attribute record must have at least an attribute ID and a value */
    if ((attrib_len < HDRPACC_MIN_MULTIFLOW_ATTRIB_LEN) ||
        (attrib_len > total_bits_remaining ))
    {
      /* If the length is something unexpected, we can't be sure of what 
       * the AN intended and it is safest to not send a response
       */
        ERR("Error, invalid total len vs bits in message. Ignoring",0,0,0);
        send_response = FALSE;
        break;
    }

    /* There is at least one more attribute record in the message */
    if( HDRPACC_UNPACK_CON_REQ_MULTIFLOW_ATTRIB_ID( item_ptr, &attrib_id, 
        attrib_offset ) != E_SUCCESS )
    {
      ERR("Error extracting AttributeID from AttribUpdateReq Msg. Ignore", 0, 0, 0 );
      /* If we can't get the attribute_id, don't send a response */
      send_response = FALSE;
      break;
    }
    
    /*-------------------------------------------------------------------------
    Extract Flow ID from Attribute ID (for MRLP attributes)
    -------------------------------------------------------------------------*/
    if ( ((attrib_id & 0xFF00) == 0xFF00) || 
         ((attrib_id & 0xFF00) == 0x0000) )
    {
      flow_id = 0;
    }
    else
    {
      flow_id = attrib_id & 0x00FF;
      attrib_id = attrib_id & 0xFF00;
    }

    app_subtype = hdrstream_get_apptype_for_neg_stream( stream_id );

    switch( attrib_id )
    {
      /*-----------------------------------------------------------------------
      MRLP Attributes that may be GAUPed
      -----------------------------------------------------------------------*/
      case HDRMRLPC_FLOW_NN_NAK_ENABLE_FWD_ATTRIB_ID: 
      case HDRMRLPC_FLOW_NN_NAK_ENABLE_REV_ATTRIB_ID: 
      case HDRMRLPC_FLOW_NN_HIGHER_LAYER_PROT_FWD_ATTRIB_ID: 
      case HDRMRLPC_FLOW_NN_HIGHER_LAYER_PROT_REV_ATTRIB_ID:
      case HDRMRLPC_FLOW_NN_PHYS_LAYER_NAK_ENABLED_REV_ATTRIB_ID: 
      case HDRMRLPC_RES_KK_IDLE_STATE_FWD_ATTRIB_ID: 
      case HDRMRLPC_RES_KK_IDLE_STATE_REV_ATTRIB_ID: 
      case HDRMRLPC_FLOW_NN_TRANSMIT_ABORT_TIMER_REV_ATTRIB_ID:
      case HDRMRLPC_FLOW_NN_DATA_OVER_SIG_ALLOWED_REV_ATTRIB_ID:
      case HDRMRLPC_FLOW_NN_ACTIVATED_FWD_ATTRIB_ID:
      case HDRMRLPC_FLOW_NN_ACTIVATED_REV_ATTRIB_ID:
      case HDRMRLPC_FLOW_NN_SEQ_LEN_FWD_ATTRIB_ID:
      case HDRMRLPC_FLOW_NN_SEQ_LEN_REV_ATTRIB_ID:
      case HDRMRLPC_FLOW_NN_FLOW_PROT_SDU_FWD_ATTRIB_ID:
      case HDRMRLPC_FLOW_NN_FLOW_PROT_SDU_REV_ATTRIB_ID:
      case HDRMRLPC_FLOW_NN_DATA_UNIT_FWD_ATTRIB_ID:
      case HDRMRLPC_FLOW_NN_DATA_UNIT_REV_ATTRIB_ID:
      case HDRMRLPC_FLOW_NN_ROUTE_PROT_SDU_FWD_ATTRIB_ID:
      case HDRMRLPC_FLOW_NN_ROUTE_PROT_SDU_REV_ATTRIB_ID:
      case HDRMRLPC_FLOW_NN_SIM_DEL_BOTH_ROUTES_FWD_ATTRIB_ID:
      case HDRMRLPC_FLOW_NN_OOO_DEL_ROUTE_PROT_FWD_ATTRIB_ID:
      case HDRMRLPC_FLOW_NN_NAK_DELAY_TIME_FWD_ATTRIB_ID:
  
      case HDRMRLPC_FLOW_NN_IDENTIFICATION_FWD_ATTRIB_ID: 
      case HDRMRLPC_FLOW_NN_ID_REV_OR_SEQ_LEN_FWD_ATTRIB_ID:
      case HDRMRLPC_FLOW_NN_TIMER_FWD_ATTRIB_ID: 
      case HDRMRLPC_FLOW_NN_TIMER_REV_ATTRIB_ID: 
      case HDRMRLPC_FLOW_NN_RESERVATION_FWD_ATTRIB_ID: 
      case HDRMRLPC_FLOW_NN_RESERVATION_REV_ATTRIB_ID:
      case HDRMRLPC_RES_KK_QOS_RSP_FWD_ATTRIB_ID:
      case HDRMRLPC_RES_KK_QOS_RSP_REV_ATTRIB_ID:
      case HDRMRLPC_FLOW_NN_FLOW_PROT_PARM_FWD_ATTRIB_ID:
      case HDRMRLPC_FLOW_NN_FLOW_PROT_PARM_REV_ATTRIB_ID:
      case HDRMRLPC_FLOW_NN_ROUTE_PROT_PARM_FWD_ATTRIB_ID:
      case HDRMRLPC_FLOW_NN_ROUTE_PROT_PARM_REV_ATTRIB_ID:
      case HDRMRLPC_AN_SUPPORTED_QOS_PROFILES_ATTRIB_ID:

        /* Make sure that for MFPA stream, only MFPA attributes are
         * valid, and for EMPA stream, only EMPA attributes are valid */
        if (hdrmrlpc_is_attrib_valid_for_appsubtype(app_subtype, attrib_id))
        {
          /* _mpa_ reuse the unpacking routine from the config req msg */
          /* Optimize... Assuming that the AN is really sending only one possible
           * value per attribute and that they are valid. 
           * Will need to check for this later (there is no fallback here). */

          send_response = hdrmrlpc_unpack_config_req_attrib( item_ptr,
                                                             attrib_id,
                                                             flow_id,
                                                             attrib_len,
                                                             attrib_offset );
        }
        else
        {
          HDR_MSG_PROT_2( MSG_LEGACY_ERROR, 
                          "Invalid attrib ID (0x%x for app_subtype %d",
                          attrib_id | flow_id, app_subtype );
        }
        break;

      case HDRMRLPC_MAX_RESERVATIONS_ATTRIB_ID:
        /* 3GPP2 C.S0024-A Version 3.0 specifies that MaxReservations 
         * attribute is not GAUPable. Don't respond to it and send a
         * session close message */

      case HDRMRLPC_RES_KK_QOS_REQ_FWD_ATTRIB_ID: 
      case HDRMRLPC_RES_KK_QOS_REQ_REV_ATTRIB_ID: 
        /*-------------------------------------------------------------------
         * According to standard, AN is not allowed to send those 2 
         * attributes. If AT receives them, the correct behavior is not
         * to respond to AttributeUpdate message, and send a SessionClose
         *-----------------------------------------------------------------*/
        send_response = FALSE;
        gaup_is_accepted = FALSE;
        break;

      default:
        ERR("Unknown GAUP Attribute ID (0x%x)", 
             attrib_id | flow_id, 0, 0 );
        break;

    } /* end switch */

    if (!send_response)
    {
      /* No need to continue parsing once we decide not to send a response */
      break;
    }

  } /* while( there are more attributes in the config request ) */

  /* No need to perform GAUP config response validations if
   * GAUP is empty */
  if (!gaup_is_empty &&
      hdrmrlpc_validate_gaup_config_rsp_attribs() != E_SUCCESS)
  {
    /* If AttributeUpdateRequest message is not accepted send
     * session close message */
    send_response = FALSE;
    gaup_is_accepted = FALSE;
  }

  if (send_response)
  {
    
    /* get new buffer */
    send_msg_ptr = hdrutil_new_msg_buffer();

    /*---------------------------------------------------------------------
    Pack Message ID
    ---------------------------------------------------------------------*/
    HDRPACC_PACK_MSG_ID( send_msg_ptr, HDRPACC_ATTRIB_UPDATE_ACPT_MSG);

    /*---------------------------------------------------------------------
    Pack Transaction ID.
    ---------------------------------------------------------------------*/
    HDRPACC_PACK_TRANSACTION_ID( send_msg_ptr, 
                                 areq_transaction_id);

    /*---------------------------------------------------------------------
    Send AttributeUpdateAccept message. 
    ---------------------------------------------------------------------*/
    hdrhmp_app_send_msg( HDRHAI_PACKET_APPLICATION_CONTROLLER,
                         stream_id,
                         HDRHAI_RTC,
                         TRUE,
                         HDRPACC_MESSAGE_PRIORITY,
                         HDRHAI_IN_USE, /* Instance of protocol sending message */
                         send_msg_ptr,
                         NULL,
                         NULL );

    /*---------------------------------------------------------------------
    Notify data that GAUP is done.
    ---------------------------------------------------------------------*/
    if (!gaup_is_empty)
    {
      /* No real payload - pass in gaup transaction id */
      ind_data.gaup_trans_id = 0;

      hdrds_queue_ind_ext(HDRHAI_PACKET_APPLICATION_CONTROLLER,
                          HDRIND_SCM_MRLP_GAUP, ind_data );
    }
    else
    {   
        /* For empty GAUP, still send GAUP accept back to AN, 
         * but don't send any indiations to apps */
        HDR_MSG_PROT(MSG_LEGACY_HIGH, 
                     "Empty GAUP, don't send HDRIND_SCM_MRLP_GAUP Ind");
    }
  }
  else
  {   
    /* The AT is rejecting the current AttributeUpdate Request message */

    /* _mpa_ TODO: can't reset here.  We could have an AT Initiated GAUP
       message pending as well so we can't overwrite the in_config instance again.  
       Shouldn't happen though */
    hdrmrlpc_config_init();

    if (!gaup_is_accepted)
    {
      /* AT could not accept the AttributeUpdateRequest message so send session
       * close as AT is not allowed to send AttributeUpdateReject */
      gaup_fail_data.reason = HDRHAI_CLOSE_AN_GAUP_FAIL;
      ind_data.scp_fail_ptr = (void *)&gaup_fail_data;

      hdrind_give_ind(HDRIND_PACC_AN_GAUP_FAILED, (void*)&ind_data);
    }
  } /* end if (send_response) */

} /* hdrpacc_process_mf_attrib_update_req_msg() */

/* EJECT */
#endif /* FEATURE_HDR_QOS */

/* EJECT */
/*=============================================================================
FUNCTION HDRPACC_PROCESS_CONFIG_RSP_TIMEOUT

DESCRIPTION
  This function processes the Configuration Response Timer Timeout.
  
DEPENDENCIES
  None.

PARAMETERS
  None.
  
RETURN VALUE
  None.
  
SIDE EFFECTS
  None.
=============================================================================*/
void hdrpacc_process_config_rsp_timeout( void )
{
   
  hdrpacc_fail_cur_config_neg(HDRHAI_CLOSE_CFG_RSP_EXP);

} /* hdrpacc_process_config_rsp_timeout() */

/* EJECT */
/*=============================================================================
FUNCTION HDRPACC_PROCESS_GAUP_RSP_TIMEOUT

DESCRIPTION
  This function processes the GAUP Response Timer Timeout.
  
DEPENDENCIES
  None.

PARAMETERS
  None.
  
RETURN VALUE
  None.
  
SIDE EFFECTS
  None.
=============================================================================*/
void hdrpacc_process_gaup_rsp_timeout( void )
{
   
  

} /* hdrpacc_process_config_rsp_timeout() */

/* EJECT */
/*===========================================================================

FUNCTION HDRPACC_FAIL_CUR_CONFIG_NEG

DESCRIPTION
  This function fails the current config negotiation

DEPENDENCIES
  None

PARAMETERS
  close_reason - Session Close Reason

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void hdrpacc_fail_cur_config_neg
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
  
  session_fail_data.reason = close_reason;

  ind_data.scp_fail_ptr = (void *)&session_fail_data;
  hdrind_give_ind(HDRIND_SCM_PROT_NEG_FAILED,(void *)&ind_data);

} /* hdrpacc_fail_cur_config_neg() */

/* EJECT */
/*===========================================================================

FUNCTION HDRPACC_CONFIG_REQ_MSG_TX_OUTCOME_CB

DESCRIPTION
  This function is as a callback funtion for transmission outcome of the
  config request message.

DEPENDENCIES
  None

PARAMETERS
  data        - user data
  status      - message transmission status


RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
/*lint -save -e715 data_ptr*/
void hdrpacc_config_tx_outcome_cb
(
  void                 *data_ptr,
  hdrerrno_enum_type    err_val
)
{

  if (!hdrpacc.await_slp_ack)
  {
    HDR_MSG_PROT (MSG_LEGACY_LOW, "Ignore config req tx outcome callback");
    return;
  }


  switch(err_val)
  {
    case E_SUCCESS:

      /* message transmitted successfully */
      HDRTRACE_TXMSG_ACKED(HDRHAI_PACKET_APPLICATION_CONTROLLER,
      HDRPACC_CONFIG_REQ_MSG, 0);  /*lint !e506 */
	  if(hdrpacc.await_slp_ack == TRUE)
	  {
          HDR_MSG_PROT (MSG_LEGACY_MED, "Starting config rsp timer");
      (void)rex_set_timer(&hdrpacc.config_rsp_timer,
                          HDRHAI_CONFIG_RSP_TIMER_MSEC);
	  }
      break;

    case E_FAILURE:

      HDRTRACE_TXMSG_FAILED(HDRHAI_PACKET_APPLICATION_CONTROLLER,
      HDRPACC_CONFIG_REQ_MSG, 0); /*lint !e506 */
      hdrpacc_fail_cur_config_neg(HDRHAI_CLOSE_CFG_MSG_FAIL);
      break;

    case E_AGAIN:   /* given after max retries on the RTC */
    case E_CANCELED:/* given if connection closes before max retries */
      /* ignore, configure command will be given again by SCP */
    case E_NOT_ALLOWED:
      break;

    default:
      ERR( "Unknown err val for Config outcome %d",
            err_val, 0, 0);
      break;
  }

  /* This flag is set to TRUE when the message is sent, and because it is
   * guaranteed that this callback is called regardless of the outcome of
   * the message, the flag will not get out of sync.
   */
  hdrpacc.await_slp_ack = FALSE;
  
} /* hdrpacc_config_tx_outcome_cb() */
/*lint -restore*/

/* EJECT */
/*=============================================================================
FUNCTION HDRPACC_SEND_DEFAULT_CONFIG_RSP_MSG

DESCRIPTION
  This function sends a default Configuration Response Message.
  
DEPENDENCIES
  None.

PARAMETERS
  item_ptr  - Pointer to the DSM item chain holding the message.
  stream_id - Stream the message was received on
  
RETURN VALUE
  None.
  
SIDE EFFECTS
  None.
=============================================================================*/
void hdrpacc_send_default_config_rsp_msg
(
  dsm_item_type *item_ptr,
  hdrhai_stream_enum_type stream_id
)
{
  uint8  creq_transaction_id;  /* Transaction ID of the config req */

  dsm_item_type *send_msg_ptr; /* dsm buffer for sent message */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*---------------------------------------------------------------------
  Extract the Transaction ID or Sequence Number from the message 
  ---------------------------------------------------------------------*/
  if( HDRPACC_UNPACK_TRANS_ID(item_ptr, &creq_transaction_id ) 
      != E_SUCCESS )
  {
    ERR("Error extracting TransactionID from ConfigReq Msg", 0, 0, 0 );
    /* Without the transaction ID, it makes no sense to send a response */
    return;
  }

  /* get new buffer */
  send_msg_ptr = hdrutil_new_msg_buffer();

  /*---------------------------------------------------------------------
  Pack Message ID
  ---------------------------------------------------------------------*/
  HDRPACC_PACK_MSG_ID( send_msg_ptr, HDRPACC_CONFIG_RSP_MSG);

  /*---------------------------------------------------------------------
  Pack Transaction ID.
  ---------------------------------------------------------------------*/
  HDRPACC_PACK_TRANSACTION_ID( send_msg_ptr, 
                               creq_transaction_id);
  
  /*---------------------------------------------------------------------
  Send config response on RTC.  All config messages are reliable. 
  ---------------------------------------------------------------------*/
  hdrhmp_app_send_msg( HDRHAI_PACKET_APPLICATION_CONTROLLER,
                       stream_id,
                       HDRHAI_RTC,
                       TRUE,
                       HDRPACC_MESSAGE_PRIORITY,
                       HDRHAI_IN_CONFIG, 
                       send_msg_ptr,
                       NULL,
                       NULL );

} /* hdrpacc_send_default_config_rsp_msg() */

/* EJECT */
/*===========================================================================

FUNCTION HDRPACC_PROCESS_IND

DESCRIPTION
  This function processes indications given to PACC in MC task context.

DEPENDENCIES
  None

PARAMETERS
  ind_name     - Name of the indication to process
  ind_data_ptr - Any data associated with the indication
  
RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void hdrpacc_process_ind
(
  hdrind_ind_name_enum_type ind_name,
  hdrind_ind_data_union_type * ind_data_ptr
)
{
  switch (ind_name)
  {
    case HDRIND_SCM_SESSION_CHANGED:
      if (hdrpacc.await_slp_ack)
      {
        /* The flag must be set first because hdrhmp_abort_msg() causes
         * the callback to be called, and if this flag is not FALSE
         * the callback will set the config response timer, for example.
         */
        hdrpacc.await_slp_ack = FALSE;
        (void) hdrhmp_abort_msg(hdrpacc_config_tx_outcome_cb, NULL);
      }

      hdrfcp_process_ind(ind_name, ind_data_ptr);
      hdrlupc_process_ind(ind_name,ind_data_ptr);
#ifdef FEATURE_HDR_QOS
      hdrdos_process_ind(ind_name, ind_data_ptr);
      hdrmrlpc_process_ind(ind_name,ind_data_ptr);
#endif /* FEATURE_HDR_QOS */

      if ( ind_data_ptr->change_reason == HDRHAI_SCHG_CLOSED_NORMAL || 
           ind_data_ptr->change_reason == HDRHAI_SCHG_CLOSED_RESTORE_PRIOR )
      {
        /* Stop and clear timer */
        (void) rex_clr_timer( &hdrpacc.config_rsp_timer );
      }
    break;

    default:
      ERR_FATAL( "Unknown indication (%d)", ind_name, 0, 0 );
    break;
  }

} /* hdrpacc_process_ind() */

/* EJECT */
/*===========================================================================
FUNCTION HDRPACC_IND_CB

DESCRIPTION
  This function is registered to be called whenever a protocol gives an
  indication of interest to HDRAPP1, HDRAPP2, HDRAPP3. Indications given 
  to PACC are processed in HDRMC task context.

DEPENDENCIES
  None.

PARAMETERS
  ind_name - Indication to process, is a global enum for all
             possible indications in HDR signaling
  ind_data - Optional data pointer for indication specific data

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void hdrpacc_ind_cb
(
  hdrind_ind_name_enum_type ind_name,
  void *ind_data_ptr
)
{
  if ( rex_self() == HDRMC_TASK_PTR )
  {
    hdrpacc_process_ind( ind_name, (hdrind_ind_data_union_type*)ind_data_ptr );
  }
  else
  {
    hdrmc_queue_ind( HDRHAI_PACKET_APPLICATION_CONTROLLER, ind_name, 
                     ind_data_ptr );
  }
} /* hdrpacc_ind_cb() */

/*===========================================================================

FUNCTION HDRPACC_PROCESS_MSG

DESCRIPTION
  This function processes messages for PACC.

DEPENDENCIES
  None

PARAMETERS
  msg_ptr   - Pointer to the DSM item chain holding the message
  protocol_instance - Instance of the protocol the message is for
RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void hdrpacc_process_msg
(
  dsm_item_type   *msg_ptr, 
  hdrhai_protocol_instance_enum_type protocol_instance
)
{
  uint8 msg_id;      /* Message ID */
  uint8 protocol_id; /* Protocol ID (stream the message was received on)*/
  hdrhai_stream_enum_type stream_id; /* stream the message was rxed on */
  boolean free_item = TRUE;


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
  Ensure the msg_ptr is valid.  If not, log an error and just return,
  since it is already NULL.
  -------------------------------------------------------------------------*/
  if (msg_ptr == NULL)
  {
    ERR("NULL msg_ptr passed to hdrpacc_process_msg()",0,0,0);
    return;
  }   

  /* Extract the Message ID */
  if( HDRPACC_UNPACK_MSG_ID( msg_ptr, &msg_id ) != E_SUCCESS )
  {
    ERR("Error Unpacking Msg ID", 0, 0,0 );
    dsm_free_packet( &msg_ptr );
    return;
  }
  
  /*---------------------------------------------------------------------------
  Get the protocol id (stream) associated with the message.   
  ---------------------------------------------------------------------------*/
  protocol_id = (uint8)HDRDSM_GET_PROT_ID(msg_ptr) ;

  switch( protocol_id )
  {
    case HDRHAI_N_APP1_TYPE:
      stream_id = HDRHAI_STREAM1;
      break;

    case HDRHAI_N_APP2_TYPE:
      stream_id = HDRHAI_STREAM2;
      break;

    case HDRHAI_N_APP3_TYPE:
      stream_id = HDRHAI_STREAM3;
      break;

    default:
      /* Should never happen, since Stream 0 message should never be routed
      ** to PAC */
      stream_id = HDRHAI_STREAM0;
      ERR_FATAL("Invalid Protocol ID %d", protocol_id, 0, 0 );
      break;
  }

  /*---------------------------------------------------------------------------
  In Config instance   
  ---------------------------------------------------------------------------*/
  if (protocol_instance == HDRHAI_IN_CONFIG)
  {
  HDRTRACE_MSG( HDRHAI_PACKET_APPLICATION_CONTROLLER, msg_id, 0 );

     switch (msg_id)
     {
       case HDRPACC_CONFIG_REQ_MSG:
       case HDRPACC_CONFIG_RSP_MSG:

         /* Route the message based on which protocol it is received for */

         switch( hdrstream_get_apptype_for_neg_stream( stream_id ))
         {
           case HDRHAI_DEF_PACKET_APP_SN:
           case HDRHAI_DEF_PACKET_APP_AN:
             hdrpacc_process_default_msg(msg_ptr, msg_id, stream_id);
             break;

#ifdef FEATURE_HDR_QOS
           case HDRHAI_MULTIFLOW_PACKET_APP_SN:
           case HDRHAI_MULTIFLOW_PACKET_APP_AN:

           #ifdef FEATURE_HDR_EMPA
           case HDRHAI_ENHANCED_MULTIFLOW_PACKET_APP_SN:
           case HDRHAI_ENHANCED_MULTIFLOW_PACKET_APP_AN:
           #endif /* FEATURE_HDR_EMPA */

           #ifdef FEATURE_HDR_REVB
           case HDRHAI_MULTILINK_MULTIFLOW_PACKET_APP_SN:
           case HDRHAI_MULTILINK_MULTIFLOW_PACKET_APP_AN:
           #endif /* FEATURE_HDR_REVB */

             hdrpacc_process_multiflow_msg(msg_ptr, msg_id, stream_id);
             break;
#endif /* FEATURE_HDR_QOS */

           #ifdef FEATURE_HDR_CSNA
           case HDRHAI_3G1X_CSN_APP:
             /* Route the message to SCM which processes the Config
              * Request/Response messages for CSNA. Since both
              * CSNA and PACC are within the HDRMC task, we can
              * make direct function calls */
             hdrscm_process_msg(msg_ptr, protocol_instance);
             free_item = FALSE;
             break;
           #endif /* FEATURE_HDR_CSNA */

           default:
             ERR( "AppSubtype: %d Invalid msg %d",
                hdrstream_get_apptype_for_neg_stream( stream_id ), msg_id, 0 );
             break;
         }
         break;

       default:
         ERR("Unknown InConfig message (0x%x)",msg_id,0,0);
         break;
     }
  } 
  /*---------------------------------------------------------------------------
  In Use instance - GAUP messages
  ---------------------------------------------------------------------------*/
  else if (protocol_instance == HDRHAI_IN_USE)
  {
    HDRTRACE_MSG( HDRHAI_PACKET_APPLICATION_CONTROLLER, msg_id, 0 );

    switch (msg_id)
    {
       case HDRPACC_ATTRIB_UPDATE_REQ_MSG:
       case HDRPACC_ATTRIB_UPDATE_ACPT_MSG:
       case HDRPACC_ATTRIB_UPDATE_REJ_MSG:
        
         /* Route the message based on which protocol it is received for */
         
         switch( hdrstream_get_apptype_for_neg_stream( stream_id ))
         {
           case HDRHAI_DEF_PACKET_APP_SN:
             hdrpacc_process_default_msg(msg_ptr, msg_id, stream_id);
             break;
#ifdef FEATURE_HDR_QOS
           case HDRHAI_MULTIFLOW_PACKET_APP_SN:

           #ifdef FEATURE_HDR_EMPA
           case HDRHAI_ENHANCED_MULTIFLOW_PACKET_APP_SN:
           #endif /* FEATURE_HDR_EMPA */
           
           #ifdef FEATURE_HDR_REVB
           case HDRHAI_MULTILINK_MULTIFLOW_PACKET_APP_SN:
           #endif /* FEATURE_HDR_REVB */
             hdrpacc_process_multiflow_msg(msg_ptr, msg_id, stream_id);
             break;
#endif /* FEATURE_HDR_QOS */

           #ifdef FEATURE_HDR_CSNA
           case HDRHAI_3G1X_CSN_APP:
             /* Route the message to SCM which processes the GAUP
              * Request/Accept/Reject messages for CSNA. Since both
              * CSNA and PACC are within the HDRMC task, we can
              * make direct function calls */
             hdrscm_process_msg(msg_ptr, protocol_instance);
             free_item = FALSE;
             break;
           #endif /* FEATURE_HDR_CSNA */
           
           default:
             ERR( "AppSubtype: %d Invalid msg %d",
             hdrstream_get_apptype_for_neg_stream( stream_id ), msg_id, 0 );
             break;
         }
      break;

       default:
         ERR("Unknown InUse message (0x%x)",msg_id,0,0);
         break;
    }
  } /* end - protocol_instance */
  if (free_item)
  {
    dsm_free_packet( &msg_ptr );
  }
} /* hdrpacc_process_msg() */

/* <EJECT> */
/*===========================================================================
FUNCTION HDRPACC_CONFIG_MSG_CB

DESCRIPTION
  This function is registered to be called whenever a protocol gives a
  message to HDRAPP1, HDRAPP2, HDRAPP3 in-config instance. All Config 
  messages are processed in the HDRMC task. 

DEPENDENCIES
  None

PARAMETERS
  msg_ptr - Pointer to the DSM item chain holding the message

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void hdrpacc_config_msg_cb
(
  dsm_item_type *msg_ptr
)
{
  /*-----------------------------------------------------------------------
  Queue the message & tag it for the PAC. Messages for the In Config instance
  should run in the HDRMC task
  -----------------------------------------------------------------------*/
  hdrmc_queue_msg( HDRHAI_PACKET_APPLICATION_CONTROLLER, 
                   HDRHAI_IN_CONFIG, msg_ptr );

} /* hdrpacc_config_msg_cb() */

/*===========================================================================
FUNCTION HDRPACC_MSG_CB

DESCRIPTION
  This function is registered to be called whenever a protocol gives a
  message to HDRAPP1, HDRAPP2, HDRAPP3 in-use instance. 
  
  All InUse Config messages are processed in the HDRMC task. 

  
DEPENDENCIES
  None

PARAMETERS
  msg_ptr - Pointer to the DSM item chain holding the message

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void hdrpacc_msg_cb
(
  dsm_item_type *msg_ptr
)
{
  /*-----------------------------------------------------------------------
  Queue the message & tag it for the PAC. Messages for the In Config instance
  should run in the HDRMC task
  -----------------------------------------------------------------------*/
  hdrmc_queue_msg( HDRHAI_PACKET_APPLICATION_CONTROLLER, 
                   HDRHAI_IN_USE, msg_ptr );

} /* hdrpacc_msg_cb() */

/* <EJECT> */
/*===========================================================================
FUNCTION HDRPACC_PROCESS_TIMER

DESCRIPTION
  This function processes all timer expiration events for the Packet 
  Application Controller.

DEPENDENCIES
  None.

PARAMETERS
  timer_id - The ID of the timer that has expired

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void hdrpacc_process_timer
(
  uint32 timer_id
)
{
  switch ((hdrpacc_timer_enum_type)timer_id)
  {
    case HDRPACC_CONFIG_RSP_TIMER:
      hdrpacc_process_config_rsp_timeout();
      break;

    case HDRPACC_GAUP_RSP_TIMER:
      hdrpacc_process_gaup_rsp_timeout(); 
      break;

    default:
      ERR("Invalid PAC timer",timer_id,0,0);
      break;
  }
} /* hdrpacc_process_timer() */

/* <EJECT> */
/*===========================================================================
FUNCTION HDRPACC_TIMER_CB

DESCRIPTION
  This function is the callback for all PACC timers.
  This function is called by REX when the timer expires, and runs
  in the context of the task that defined the timer (HDRMC), however
  it runs as an independent thread and thus must queue an event for HDRMC.

DEPENDENCIES
  None.

PARAMETERS
  timer_id - The ID of the timer that has expired

RETURN VALUE
  None.

SIDE EFFECTS
  HDRMC task context
===========================================================================*/
void hdrpacc_timer_cb
(
  uint32 timer_id
)
{
  hdrmc_queue_timer(HDRHAI_PACKET_APPLICATION_CONTROLLER, timer_id);
} /* hdrpacc_timer_cb() */

/* <EJECT> */
/*===========================================================================

FUNCTION HDRPACC_PROCESS_CMD

DESCRIPTION
  This function processes commands for the PACC

DEPENDENCIES
  None

PARAMETERS
  pac_cmd_ptr - Pointer to the stucture containing the command

RETURN VALUE
  None

SIDE EFFECTS
  For HDRMC task
===========================================================================*/
void hdrpacc_process_cmd
(
  hdrpacc_cmd_type *pac_cmd_ptr
)
{
  HDRTRACE_CMD( HDRHAI_PACKET_APPLICATION_CONTROLLER, pac_cmd_ptr->name,
                 pac_cmd_ptr->sender, 0 );

  /*-------------------------------------------------------------------------
  Determine which command has been received.
  -------------------------------------------------------------------------*/
  switch ( pac_cmd_ptr->name )
  {
    case HDRPACC_CONFIGURE_CMD:
      /*---------------------------------------------------------------------
      Configure Command      
      ---------------------------------------------------------------------*/
      hdrpacc_process_configure(pac_cmd_ptr->params.config.app,
                                pac_cmd_ptr->params.config.stream);
      break;

    case HDRPACC_UPDATE_FLOW_MAPPING_IN_SCMDB_CMD:
      /*---------------------------------------------------------------------
      Commit the flow mapping update to dB
      ---------------------------------------------------------------------*/
      HDR_MSG_PROT(MSG_LEGACY_HIGH, "Update DB for changed flow binding");
#ifdef FEATURE_HDR_REVB
      if (hdrscp_get_current_subtype( HDRHAI_RTC_MAC_PROTOCOL ) == HDRSCP_MC_RMAC)
      {
        hdrscmdb_set_gauped_attrib(pac_cmd_ptr->sender, 
                                   pac_cmd_ptr->params.update.attrib_id, 
                                   0, 
                                   &pac_cmd_ptr->params.update.flow_mapping, 
                                   HDRSCP_MC_RMAC);
      }
      else
      {
#endif /* FEATURE_HDR_REVB */
      hdrscmdb_set_gauped_attrib(pac_cmd_ptr->sender, 
                                 pac_cmd_ptr->params.update.attrib_id, 
                                 0, 
                                 &pac_cmd_ptr->params.update.flow_mapping, 
                                 HDRSCP_SUB3_RMAC);
#ifdef FEATURE_HDR_REVB
      }
#endif /* FEATURE_HDR_REVB */
      break;

    default:
      /*---------------------------------------------------------------------
      Unrecognized command.
      ---------------------------------------------------------------------*/
      ERR("PAC: Received Unrecognized command:%d", pac_cmd_ptr->name, 0, 0 );
      break;
  }
} /* hdrpacc_process_cmd() */

/* <EJECT> */
/*===========================================================================

FUNCTION HDRPACC_CONFIGURE

DESCRIPTION
  This function conditionally commands HDRPAC to configure 
  (will configure LUP, RLP, FCP and RLP as needed)

DEPENDENCIES
  hdrpacc_configure shall only be called after the AT Initiated stream is 
  complete, or there is already a valid stream configuration.
  HDRMC task context

PARAMETERS
  sender - The protocol giving this command (only for software trace)

RETURN VALUE
  TRUE - the configure command is accepted
  FALSE - the configure command is rejected

SIDE EFFECTS
  None

===========================================================================*/
boolean hdrpacc_configure
(
  hdrhai_protocol_name_enum_type sender 
)
{
  boolean config_required = FALSE;
    /* Flag indicating whether PAC module configuration is required or not */
#ifdef FEATURE_HDR_QOS
  hdrpacc_cmd_type hdrpac_cmd; 
    /* Build command into local struct */
  hdrhai_stream_enum_type stream_id;
    /* Stream associated with the given SN packet app subtype */
  hdrhai_app_subtype_enum_type app_subtype = HDRHAI_APP_STREAM_NOT_USED;
    /* App subtype bound to SN negotiated in the given stream config */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  app_subtype = hdrstream_get_negotiated_snpktapp();

  if (((app_subtype == HDRHAI_MULTIFLOW_PACKET_APP_SN)
  #ifdef FEATURE_HDR_EMPA
        ||
       (app_subtype == HDRHAI_ENHANCED_MULTIFLOW_PACKET_APP_SN)
#endif /* FEATURE_HDR_EMPA */
#ifdef FEATURE_HDR_REVB
        ||
       (app_subtype == HDRHAI_MULTILINK_MULTIFLOW_PACKET_APP_SN)
#endif /* FEATURE_HDR_REVB */
      )&&
      hdrmrlpc_config_is_needed()
     )
  {
    config_required = TRUE;

    HDR_MSG_PROT (MSG_LEGACY_MED, "PAC Config is required");

    (void) hdrstream_get_negotiated_stream(app_subtype, &stream_id);

    /* Fill in all members of the hdrpac_cmd_type for the Configure command */
    hdrpac_cmd.name   = HDRPACC_CONFIGURE_CMD;
    hdrpac_cmd.sender = sender;

    hdrpac_cmd.params.config.app = app_subtype;
    hdrpac_cmd.params.config.stream = stream_id;

    if (rex_self() == HDRMC_TASK_PTR)
    {
      /* Call the processing function directly if in the same task */
      hdrpacc_process_cmd( &hdrpac_cmd );
    }
    else
    {
      /* Currently no other task should ever issue this command,
       * so we should never get here.
       */
      ERR("Configure command not expected from other task",0,0,0);

      hdrmc_queue_cmd( HDRHAI_PACKET_APPLICATION_CONTROLLER, &hdrpac_cmd, 
                       sizeof (hdrpac_cmd) );
    }
  }
  else
  {
    /* No negotiation needed for - HDRHAI_DEF_PACKET_APP_SN  */
  }
#endif /*FEATURE_HDR_QOS */

  return config_required;

} /* hdrpacc_configure */

/* <EJECT> */
/*===========================================================================

FUNCTION HDRPACC_UPDATE_FLOW_MAPPING_IN_SCMDB

DESCRIPTION
  If RLP flow x is bound to MAC flow 1, and a GAUP update binds the same RLP 
  flow x to a different MAC flow (say 2), RMAC unbinds the RLP flow x from 
  MAC flow 1 before binding it to MAC flow 2. We need to update the SCM DB
  in order to keep it consistent for this implicit unbind action.

DEPENDENCIES
  None.

PARAMETERS
  sender       - protocol calling this function.
  flow_mapping - Pointer to Asso flows. struct containing the updated 
                 mapping.
  attrib_id    - Id of the RMAC3 attribute that needs to be updated.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
void hdrpacc_update_flow_mapping_in_scmdb
(
  hdrhai_protocol_name_enum_type sender,
  hdrscmrmac3_associated_flows_struct_type *flow_mapping,
  uint16 attrib_id
)
{
  hdrpacc_cmd_type hdrpac_cmd; /* Build command into local struct */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Fill in PAC cmd fields for Update flow mapping command */
  hdrpac_cmd.name = HDRPACC_UPDATE_FLOW_MAPPING_IN_SCMDB_CMD;
  hdrpac_cmd.sender = sender;
  hdrpac_cmd.params.update.attrib_id = attrib_id;
#ifdef FEATURE_MEMCPY_REMOVAL
  hdrpac_cmd.params.update.flow_mapping = *flow_mapping;
#else
  memcpy(&hdrpac_cmd.params.update.flow_mapping, 
         flow_mapping,
         sizeof(hdrscmrmac3_associated_flows_struct_type));
#endif /* FEATURE_MEMCPY_REMOVAL */

  /* If the current task is not HDRMC, queue cmd to hdrmc */
  if (rex_self() != HDRMC_TASK_PTR)
  {
    hdrmc_queue_cmd( HDRHAI_PACKET_APPLICATION_CONTROLLER, &hdrpac_cmd, 
                     sizeof (hdrpac_cmd) );
  }
  else
  {
    hdrpacc_process_cmd(&hdrpac_cmd);
  }

} /* hdrpacc_update_flow_mapping_in_scmdb */

