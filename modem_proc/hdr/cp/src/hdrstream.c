/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                S T R E A M   C O N F I G U R AT I O N 

GENERAL DESCRIPTION
  The Stream Configuration module configures the binding between the
  services and the streams

EXTERNALIZED FUNCTIONS (Global)

  hdrstream_msg_cb()      - Callback registered with HMP to deliver 
                              messages to STREAM
  hdrstream_ind_cb()   - Callback registered for delivery of indications
  hdrstream_init() - Initialization command

EXTERNALIZED FUNCTIONS (Regional)
  hdrstream_process_cmd()     - Function called by hdrmc task to process
                              a command for STREAM
  hdrstream_process_ind()     - Function called by hdrmc task to process
                              an indication for STREAM
  hdrstream_process_msg()     - Function called by hdrmc task to process
                              a message for STREAM
  hdrstream_process_sig()     - Function called by hdrmc task to process
                              a signal for STREAM
  
INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

Copyright (c) 2000 - 2012 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* <EJECT> */ 
/*===========================================================================
                                                                                      
                        EDIT HISTORY FOR MODULE

$PVCSPath: L:/src/asw/COMMON/vcs/hdrstream.c_v   1.37   24 Apr 2003 11:42:26   maldinge  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/hdr/cp/src/hdrstream.c#1 $ $DateTime: 2016/12/13 07:58:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/09/16   vko     Reduce excessive F3 logging
07/09/14   cnx     Support NV refresh without reset.
12/10/13   sat     Changed memcpy() to assignment or memscpy() (safer version).
03/19/12   cnx     Resolved TCB compiler errors for Dime. 
06/21/10   cnx     Fixed compiler warnings.
03/23/09   wsh     CMI: featurized CMI under FEATURE_CMI
02/18/09   wsh     Updated file to include private header hdrmc_v.h
12/10/08   smd     Replaced assert.h with amssassert.h
10/17/07   pba     Updates based on code review comments.
09/11/07   cws     Added support for MCTAP
09/11/07   wsh     Removed lint high warnings
08/16/07   wsh     Removed compiler and lint critical warnings
08/08/07   pba     Initialize the inconfig instance of a stream to default
                   values of agreed upon application subtype for that stream.
07/30/07   pba     Featurized MMPA code under FEATURE_HDR_REVB.
07/17/07   pba     RelB MMPA support.
06/05/07   pba     In Rev0 mode do not send stream configuration request message
                   if session negotiation was initiated through ConfigStart.
04/03/07   wsh     Fixed error where dsm_item is not freed
03/27/07   pba     Fixed compile error.
03/21/07   pba     For ConfigStart case propose all stream config combinations
                   in AT INIT phase of session negotiation. AT will also not 
                   reject a stream config request from AN, even if the pktapp
                   bound to SN is already configured in another personality.
03/21/07   pba     Reverted the check to include MPA/EMPA in stream configreq
                   message only when RMAC3 is negotiated.
03/06/07   wsh     Removal of RVCT compiler warnings
03/07/07   yll     Removed SCP custom config, get App subtype settings from
                   SCP.
03/05/07   sju     Added hdrstream_get_stream_for_protocol() used by slp to get 
                   stream_id. 
01/24/07   pba     Updated Restrict Stream Configuration functionality to allow
                   hardlinking of MPA/EMPA.
01/10/07   yll     Control EMPA feature through an NV item
01/08/07   yll     Added support for CSNA
12/20/06   pba     Ignore ConfigRequest from AN if Stream protocol subtype is
                   hardlink.
11/21/06   wsh     Fixed potential problem that if a AN proposed value is 
                   rejected by AT during AN_INIT state, it might be rolled
                   by to the AT_INIT stage value, instead of the fallback.
10/02/06   pba     Modified conditions for Stream to propose in AT_INIT phase.
08/21/06   yll     Featurized code to propose EMPA in stream configuration.
05/24/06   pba     Re-designed and fixed issues with preferred stream 
                   configuration proposed by AT in AT_INIT phase.
05/10/06   yll/ksu Added support for EMPA
04/28/06   pba     Fixed Lint warnings
04/24/06   mpa     Removed featurization of hdrstream_stream_is_negotiated().
04/20/06   pba     Fixed issue with proposing stream configurations.
04/17/06   mpa     Propose all subtypes supported by the AT during session config.
04/17/06   pba     Added support to accept/propose only AT supported stream 
                   configurations in multiple personality scenario.
04/07/06   hal     Changed hdrutil_new_buffer() to use hdrutil_new_msg_buffer()
01/14/06   dna     Initialize in_config to in_use on power up because
                   this module references in_config while SCP is OPEN.
12/13/05   pba     Changes to support multiple personality negotiation
11/14/05   dna     Phase 0 support for multiple personalities
10/04/05   pba     Added support to read/write config parameters from SCMDB/EFS
09/16/05   mpa     Clear the session config timer on session close.
09/13/05   mpa     Fixed AT initiated MFPA negotiation.
09/12/05   vas     Cleanup of comments
08/07/05   vas     temp fix of casting in hdrstream_stream_is_assigned().
07/11/05   dna     Added support for HDRHAI_ENH_TEST_APP_IS890A
07/07/05   dna     Added Release A support for Stream configuration
07/07/05   mpa     Added support to propose all non-default subtypes during 
                   AT initiated config and an NV item to selectively enable/
                   disable support for non-default protocol subtypes.
04/04/05   pba     Removed Lint errors
03/24/05   pba     Lint cleanup
01/10/05   hal     Changed hdrstream_powerup_init to negotiate QoS
                   enabled streams.
12/07/04   hal     Changed dsm_offset_new_buffer to hdrutil_new_buffer.
09/09/04   mpa     Added accessor functions for QOS.
09/10/03   mpa     Converted F3 messages to use new MSG2.0 HDRPROT SSID
03/19/03   mpa     Allocated session_fail_data out of the heap because data
                   may be used in different task contexts
03/12/03   mpa     Added hdrstream_fail_cur_config_neg() to send PROT_NEG_FAILED 
                   indication with the session close reason
07/10/01   dna     Some initialization that should only be done at powerup
                   was happening when we re-enter HDR after power save.
                   Also merged the 1.30.1.1 branch to the tip.
06/09/02   vas     Added hdrstream_get_apptype_for_neg_stream()
04/29/02   vas     Changed ASSERT to print ERR and return from configure cmd
                   if awaiting SLP ack for Config Request.
04/02/02   dna     Add E_CANCELED as possible outcome for config req. msg.
09/11/01   om      Added IS-890 application ID (0x03)
09/11/01   dna     Initialize is_accepted to false for config req processing
09/05/01   dna     Added separate handling for in-use vs. in_config messages.
09/01/01   dna     Make config request/response handling identical to others. 
08/16/01   vas     Various corrections in Config message processing. Rewrote
                   hdrstream_config_valid(). Changed config_needed() to 
                   return false if either Packet App AN or SN configured 
                   instead of requiring both to be configured.
08/06/01   vas     Renamed function data_has_stream() to stream_is_assigned()
                   Renamed Application subtype enum for consistency
08/02/01   nmn     Moved Application Subtype enum to HAI
08/02/01   nmn     Updated config req msg processing
07/10/01   nmn     fixed config outcome_cb for FAILURE
06/06/01   dna     new param for hmp_send_msg()
04/12/01   nmn     Initial revision

===========================================================================*/

/* <EJECT> */ 
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "hdr_variation.h"
#include "comdef.h"
#include "target.h"
#include "dsm.h"
#include "hdrdebug.h"
#include "err.h"
#include "amssassert.h"
#include "nv.h"

#include "hdrstream.h"
#include "hdrstreami.h"
#include "hdrutil.h"
#include "hdrind.h"
#include "hdrhai.h"

#ifdef FEATURE_CMI
#include "hdrmc_v.h"
#endif

#include "hdrmci.h"
#include "hdrerrno.h"
#include "hdrhmp.h"
#include "hdrtrace.h"
#include "hdrscp.h"
#include "hdrscmdb.h"
#ifdef FEATURE_HDRMC_DEBUG
#error code not present
#endif /* FEATURE_HDRMC_DEBUG */

/* <EJECT> */ 
/*===========================================================================

            DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

===========================================================================*/
/*===========================================================================

             DEFINITIONS  FROM  THE  HDR  AIR  INTERFACE  SPEC

===========================================================================*/

/* Address of the TCB for the task in which this protocol runs */
#define HDRSTREAM_TASK_PTR hdrmc_tcb_ptr

#define HDRSTREAM_MSG_PRIORITY 40

#define HDRSTREAM_CONFIG_REQ_MSG  0x50  /* Configuration Request Message ID */
#define HDRSTREAM_CONFIG_RSP_MSG  0x51  /* Configuration Response Message ID */

#define HDRSTREAM_DEFAULT_STREAM0 0x0000
#define HDRSTREAM_DEFAULT_STREAM1 0xFFFF
#define HDRSTREAM_DEFAULT_STREAM2 0xFFFF
#define HDRSTREAM_DEFAULT_STREAM3 0xFFFF

/* <EJECT> */ 
/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/

#define HDRSTREAM_BITS_PER_BYTE 8
#define HDRSTREAM_STREAM_CONFIG_ATTRIB_ID 0x00

#define HDRSTREAM_MSG_ID_SIZE              8
#define HDRSTREAM_TRANSACTION_ID_SIZE      8
#define HDRSTREAM_ATTRIB_LEN_SIZE          8
#define HDRSTREAM_ATTRIB_ID_SIZE           8

/* For a complex attribute, ATTRIB_VAL_SIZE is the size of the 
 * attribute value structure, including the ValueID
 */
#define HDRSTREAM_STREAM_CONFIG_ATTRIB_VAL_SIZE 72

/* The minimum attribute has Length, attribute ID and one value */
#define HDRSTREAM_STREAM_CONFIG_MIN_ATTRIB_LEN \
        (HDRSTREAM_ATTRIB_LEN_SIZE + \
        HDRSTREAM_ATTRIB_ID_SIZE + \
        HDRSTREAM_STREAM_CONFIG_ATTRIB_VAL_SIZE)

/* The smallest attribute is a length with an AttributeID */
#define HDRSTREAM_MIN_ATTRIB_LEN \
        (HDRSTREAM_ATTRIB_LEN_SIZE + \
        HDRSTREAM_ATTRIB_ID_SIZE)

/* Config response attributes are always just Attribute ID, AttributeValue */
#define HDRSTREAM_CRSP_ATTRIB_VAL_SIZE 8
#define HDRSTREAM_CRSP_ATTRIB_LEN     ( HDRSTREAM_ATTRIB_LEN_SIZE + \
                                       HDRSTREAM_ATTRIB_ID_SIZE + \
                                       HDRSTREAM_CRSP_ATTRIB_VAL_SIZE )

/* The field in the config response is length in bytes,
 * excluding the length of length field.
 */
#define HDRSTREAM_CRSP_ATTRIB_LEN_BYTES  ( (HDRSTREAM_ATTRIB_ID_SIZE + \
          HDRSTREAM_CRSP_ATTRIB_VAL_SIZE) / HDRSTREAM_BITS_PER_BYTE )        

#define HDRSTREAM_MAX_NUM_APP_SUBTYPES 12

/* Structure to hold info about each individual stream configuration value */
typedef struct 
{
  uint8 preferred_config_index;
    /* Index in the preferred stream configuration array */
  uint8 value_id;
    /* Value Id associated with the stream configuration that AT proposed */

}hdrstream_requested_config_info_type;

#define HDRSTREAM_CONFIG_COMBINATIONS_MAX 6

/* Structure to hold info on last config request we sent */
typedef struct
{
  uint8 transaction_id;
  /* Number of attribute values that AT included in the stream CfgReq msg */
  uint8   num_attrib_vals;
    /* Number of different values proposed for stream configuration in the 
       configrequest message */
  boolean awaiting_slp_ack;
  /* Preferred Stream Configurations */
  hdrstream_requested_config_info_type psc[HDRSTREAM_CONFIG_COMBINATIONS_MAX];
    /* This strcuture points to the stream configuration combinations that was included
       in the configrequest message */

} hdrstream_config_req_info_type;

typedef struct
{
  hdrscp_app_subtype_enable_mask_type app_config_mask;

} hdrstream_custom_config_type;

/* All static data for this file is in this structure */
static struct
{
  hdrstream_config_params_type current;
    /* Current Stream configuation */
  hdrstream_config_params_type negotiated;
    /* Inconfig Stream configuation */
  hdrstream_config_req_info_type config_req_info;
    /* Information about the stream configurations that 
       were included in the configrequest message */
  hdrstream_custom_config_type custom_app_config;
    /* Flags indicating packet applications that are enabled/disabled */
  rex_timer_type hdrstream_config_timer;
    /* Configuration response timer */

} hdrstream;

/* Structure that holds AT's preferred stream configurations */
typedef struct
{
  /* The AT's preferred stream configuration */
  hdrstream_config_params_type config;

}hdrstream_preferred_config_type;

/* Stream ID's to which SNPacket application and CSNA are tied to in the 
   configuration request proposed by the AT */
#define HDRSTREAM_SNPKTAPP_STREAM_ID HDRHAI_STREAM2
#define HDRSTREAM_CSNA_STREAM_ID     HDRHAI_STREAM3

/* This static array defines the preferred stream configurations that AT could
   propose in AT_INIT phase. Items in this array have to be in listed in order
   of "priority", index 0 being highest priority. If the stream id's for which 
   packet application bound to SN or CSNA are proposed are changed, the above 
   definition's should be changed too */
/*lint -e{651} suppress potentially confusing initializer */
static hdrstream_config_params_type 
         hdrstream_preferred_config_table[HDRSTREAM_CONFIG_COMBINATIONS_MAX] =
{ 
  {{{ 0x0000, 0xFFFF, HDRHAI_MULTILINK_MULTIFLOW_PACKET_APP_SN, 0xFFFF }}},
  {{{ 0x0000, 0xFFFF, HDRHAI_ENHANCED_MULTIFLOW_PACKET_APP_SN, HDRHAI_3G1X_CSN_APP }}},
  {{{ 0x0000, 0xFFFF, HDRHAI_ENHANCED_MULTIFLOW_PACKET_APP_SN, 0xFFFF }}},
  {{{ 0x0000, 0xFFFF, HDRHAI_MULTIFLOW_PACKET_APP_SN, HDRHAI_3G1X_CSN_APP }}},
  {{{ 0x0000, 0xFFFF, HDRHAI_MULTIFLOW_PACKET_APP_SN, 0xFFFF }}},
  {{{ 0x0000, 0xFFFF, HDRHAI_DEF_PACKET_APP_SN, 0xFFFF }}}
};

/* <EJECT> */ 
/*===========================================================================

                   MESSAGE PACK / UNPACK MACROS

===========================================================================*/

/****************************************************************************
** All Pack routines take a pointer to a dsm_item and
** a variable of the type belonging to the appropriate message
****************************************************************************/

/*-- Configuration Request Message --*/
#define HDRSTREAM_PACK_CON_REQ_MSG_ID( msg_ptr, val) \
          (HDRBIT_PACK8((msg_ptr), (val), 0, 8))
#define HDRSTREAM_PACK_CON_REQ_TRANS_ID( msg_ptr, val) \
          (HDRBIT_PACK8((msg_ptr), (val), 8, 8))

#define HDRSTREAM_PACK_CON_REQ_LEN( msg_ptr, val, aoffset) \
          (HDRBIT_PACK8((msg_ptr),(uint8) (val), ((aoffset) + 0), 8))
#define HDRSTREAM_PACK_CON_REQ_ATTRIBUTE_ID( msg_ptr, val, aoffset) \
          (HDRBIT_PACK8((msg_ptr), (val), ((aoffset) + 8), 8)) 

  /*- Stream Configuration Attribute (SC) -*/
#define HDRSTREAM_PACK_CON_REQ_SC_VALUE_ID( msg_ptr, val, aoffset, voffset) \
          (HDRBIT_PACK8((msg_ptr), (val), ((aoffset) + (voffset) + 0), 8))

#define HDRSTREAM_PACK_CON_REQ_SC_STREAM0(msg_ptr, val, aoffset, voffset) \
          (HDRBIT_PACK16((msg_ptr), (val), ((aoffset) + (voffset) + 8), 16))
#define HDRSTREAM_PACK_CON_REQ_SC_STREAM1(msg_ptr, val, aoffset, voffset) \
          (HDRBIT_PACK16((msg_ptr), (val), ((aoffset) + (voffset) + 24), 16))
#define HDRSTREAM_PACK_CON_REQ_SC_STREAM2(msg_ptr, val, aoffset, voffset) \
          (HDRBIT_PACK16((msg_ptr), (val), ((aoffset) + (voffset) + 40), 16))
#define HDRSTREAM_PACK_CON_REQ_SC_STREAM3(msg_ptr, val, aoffset, voffset) \
          (HDRBIT_PACK16((msg_ptr), (val), ((aoffset) + (voffset) + 56), 16))

/*-- Configuration Response Message --*/
#define HDRSTREAM_PACK_CON_RSP_MSG_ID( msg_ptr, val) \
          (HDRBIT_PACK8((msg_ptr), (val), 0, 8))
#define HDRSTREAM_PACK_CON_RSP_TRANS_ID( msg_ptr, val) \
          (HDRBIT_PACK8((msg_ptr), (val), 8, 8))

#define HDRSTREAM_PACK_CON_RSP_LEN( msg_ptr, val, aoffset) \
          (HDRBIT_PACK8((msg_ptr), (val), ((aoffset) + 0), 8))
#define HDRSTREAM_PACK_CON_RSP_ATTRIBUTE_ID( msg_ptr, val, aoffset) \
          (HDRBIT_PACK16((msg_ptr), (val), ((aoffset) + 8), 8))
#define HDRSTREAM_PACK_CON_RSP_VALUE_ID( msg_ptr, val, aoffset) \
          (HDRBIT_PACK8((msg_ptr), (val), ((aoffset) + 16), 8))

/***************************************************************************
** Unpack routines.  All take a pointer to a DSM item and a reference to
** a variable of the appropriate type
***************************************************************************/
#define HDRSTREAM_UNPACK_MSG_ID( msg_ptr, msg_id) \
          (HDRBIT_UNPACK8(msg_ptr, (msg_id), 0, 8))
#define HDRSTREAM_UNPACK_TRANS_ID( msg_ptr, trans_id) \
          (HDRBIT_UNPACK8(msg_ptr, (trans_id), 8, 8))

/*-- Configuration Request Message --*/
#define HDRSTREAM_UNPACK_CON_REQ_LEN( msg_ptr, val_ptr, aoffset) \
          (HDRBIT_UNPACK8((msg_ptr), (val_ptr), ((aoffset) + 0), 8))
#define HDRSTREAM_UNPACK_CON_REQ_ATTRIBUTE_ID( msg_ptr, val_ptr, aoffset) \
          (HDRBIT_UNPACK8((msg_ptr), (val_ptr), ((aoffset) + 8), 8))

  /*- Stream Configuration Attribute (SC) -*/
#define HDRSTREAM_UNPACK_CON_REQ_SC_VALUE_ID( msg_ptr, val_ptr, aoffset, voffset ) \
          (HDRBIT_UNPACK8((msg_ptr), (val_ptr), ((aoffset) + (voffset) + 0), 8))
#define HDRSTREAM_UNPACK_CON_REQ_SC_STREAM0( msg_ptr, val_ptr, aoffset, voffset ) \
          (HDRBIT_UNPACK16((msg_ptr), (val_ptr), ((aoffset) + (voffset) + 8), 16))
#define HDRSTREAM_UNPACK_CON_REQ_SC_STREAM1( msg_ptr, val_ptr, aoffset, voffset ) \
          (HDRBIT_UNPACK16((msg_ptr), (val_ptr), ((aoffset) + (voffset) + 24), 16))
#define HDRSTREAM_UNPACK_CON_REQ_SC_STREAM2( msg_ptr, val_ptr, aoffset, voffset ) \
          (HDRBIT_UNPACK16((msg_ptr), (val_ptr), ((aoffset) + (voffset) + 40), 16))
#define HDRSTREAM_UNPACK_CON_REQ_SC_STREAM3( msg_ptr, val_ptr, aoffset, voffset ) \
          (HDRBIT_UNPACK16((msg_ptr), (val_ptr), ((aoffset) + (voffset) + 56), 16))

/*-- Configuration Response Message --*/
#define HDRSTREAM_UNPACK_CON_RSP_LEN( msg_ptr, val, aoffset) \
          (HDRBIT_UNPACK8((msg_ptr), (val), ((aoffset) + 0), 8))
#define HDRSTREAM_UNPACK_CON_RSP_ATTRIBUTE_ID( msg_ptr, val, aoffset) \
          (HDRBIT_UNPACK8((msg_ptr), (val), ((aoffset) + 8), 8))

#define HDRSTREAM_UNPACK_CON_RSP_VALUE_ID( msg_ptr, val, aoffset) \
          (HDRBIT_UNPACK8((msg_ptr), (val), ((aoffset) + 16), 8)) 

/* header for the tx_outcome_cb function used as a pointer for hmp_send_msg */
LOCAL void hdrstream_config_tx_outcome_cb
(
  void * data_ptr,
  hdrerrno_enum_type err_val
);

LOCAL void hdrstream_fail_cur_config_neg
(
  hdrhai_session_close_reason_enum_type close_reason,
  uint8 attrib_id
);

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRSTREAM_GET_STREAM_CONFIG

DESCRIPTION
  This accessor function returns the configured applications for each
    stream.

DEPENDENCIES
  None
 
PARAMETERS
  stream_ptr - first element of a four-application array.

RETURN VALUE
  TRUE - streams were returned
  FALSE - streams were not returned
  
SIDE EFFECTS
  None

===========================================================================*/
boolean hdrstream_get_stream_config 
(
  hdrhai_app_subtype_enum_type * stream_ptr
)
{
  if (stream_ptr == NULL)
  {
    HDR_MSG_PROT (MSG_LEGACY_ERROR,
                  "NULL pointer passed to hdrstream_get_stream_config");
    return FALSE;
  }

  stream_ptr[0] = (hdrhai_app_subtype_enum_type) 
      hdrstream.current.stream_config.stream_application[0];
  stream_ptr[1] = (hdrhai_app_subtype_enum_type)
      hdrstream.current.stream_config.stream_application[1];
  stream_ptr[2] = (hdrhai_app_subtype_enum_type)
      hdrstream.current.stream_config.stream_application[2];
  stream_ptr[3] = (hdrhai_app_subtype_enum_type)
      hdrstream.current.stream_config.stream_application[3];

  return TRUE;
} /* hdrstream_get_stream_config */

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRSTREAM_STREAM_IS_ASSIGNED

DESCRIPTION
  This boolean accessor function determines whether there has been
    at least one stream allocated for a packet application.

DEPENDENCIES
  None.

PARAMETERS
  app_subtype -  Application sub-type to check streams for

RETURN VALUE
  TRUE  - Stream has been allocation for a Packet Application
  FALSE - Otherwise.

SIDE EFFECTS
  None

===========================================================================*/
boolean hdrstream_stream_is_assigned
(
  hdrhai_app_subtype_enum_type   app_subtype
)
{
  boolean ret_val;

  /* shorthand pointer to current stream config attribute */
  hdrstream_stream_config_attrib_type *csc_ptr = 
    &hdrstream.current.stream_config;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Check if the passed in app type is negotiated on any stream  */
  /* or for HDRHAI_APP_PACKET_APP_ANY if any non-signaling app    */
  /* is negotiated.                                               */
  ret_val = 
    ( ( (csc_ptr->stream_application[1] == (uint16) app_subtype)
        ||
        (csc_ptr->stream_application[2] == (uint16) app_subtype)
        ||
        (csc_ptr->stream_application[3] == (uint16) app_subtype) )
      ||
      ( (app_subtype == HDRHAI_APP_PACKET_APP_ANY)
        &&
        ( (csc_ptr->stream_application[1] != (uint16) HDRHAI_APP_STREAM_NOT_USED)
          ||
          (csc_ptr->stream_application[2] != (uint16) HDRHAI_APP_STREAM_NOT_USED)
          ||
          (csc_ptr->stream_application[3] != (uint16) HDRHAI_APP_STREAM_NOT_USED) 
        )
      )
    );
          

  HDR_MSG_PROT_2 (MSG_LEGACY_LOW, "hdrstream_is_assigned: %d (%d)",
                  ret_val,app_subtype);

  return ret_val;

} /* hdrstream_stream_is_assigned */

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRSTREAM_STREAM_IS_NEGOTIATED

DESCRIPTION
  This boolean accessor function determines whether a stream was negotiated
  for a application subtype.

DEPENDENCIES
  None.

PARAMETERS
  app_subtype -  Application sub-type to check streams for

RETURN VALUE
  TRUE  - This packet application was bound to a stream during negotiation
          (may not be current yet)
  FALSE - Otherwise.

SIDE EFFECTS
  None

===========================================================================*/
boolean hdrstream_stream_is_negotiated
(
  hdrhai_app_subtype_enum_type   app_subtype
)
{
  boolean ret_val;

  /* shorthand pointer to current stream config attribute */
  hdrstream_stream_config_attrib_type *nsc_ptr = 
    &hdrstream.negotiated.stream_config;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Check if the passed in app type is negotiated on any stream  */
  /* or for HDRHAI_APP_PACKET_APP_ANY if any non-signaling app    */
  /* is negotiated.                                               */
  ret_val = 
    ( ( (nsc_ptr->stream_application[1] == app_subtype)
        ||
        (nsc_ptr->stream_application[2] == app_subtype)
        ||
        (nsc_ptr->stream_application[3] == app_subtype) )
      ||
      ( (app_subtype == HDRHAI_APP_PACKET_APP_ANY)
        &&
        ( (nsc_ptr->stream_application[1] != HDRHAI_APP_STREAM_NOT_USED)
          ||
          (nsc_ptr->stream_application[2] != HDRHAI_APP_STREAM_NOT_USED)
          ||
          (nsc_ptr->stream_application[3] != HDRHAI_APP_STREAM_NOT_USED) 
        )
      )
    );

  HDR_MSG_PROT_2 (MSG_LEGACY_LOW, "hdrstream_is_negotiated: %d (%d)",
                  ret_val,app_subtype);

  return ret_val;

} /* hdrstream_stream_is_negotiated */

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRSTREAM_CSNA_IS_ALLOWED

DESCRIPTION
  This function returns if AT is allowed to negotiate CSNA or not.

DEPENDENCIES
  None
 
PARAMETERS
  None

RETURN VALUE
  TRUE if CSNA negotiation is allowed, FALSE otherwise
  
SIDE EFFECTS
  None

===========================================================================*/
static boolean hdrstream_csna_is_allowed( void )
{
  return hdrstream.custom_app_config.app_config_mask.enable_csna;
} /* hdrstream_csna_is_allowed() */

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRSTREAM_QOS_NEGOTIATION_IS_ALLOWED

DESCRIPTION
  This function returns if AT is allowed to negotiate QOS or not.

DEPENDENCIES
  None
 
PARAMETERS
  app_subtype - Name of the application subtype.

RETURN VALUE
  TRUE if the given app subtype negotiation is allowed, FALSE otherwise
  
SIDE EFFECTS
  None

===========================================================================*/
static boolean hdrstream_qos_negotiation_is_allowed
( 
  hdrhai_app_subtype_enum_type app_subtype
)
{
  boolean app_subtype_is_allowed = FALSE;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

#ifdef FEATURE_HDR_QOS
  if (app_subtype == HDRHAI_MULTIFLOW_PACKET_APP_SN)
  {
    if (hdrstream.custom_app_config.app_config_mask.enable_mfpa)
    {
      app_subtype_is_allowed = TRUE;
    }
  }
#ifdef FEATURE_HDR_EMPA
  else if (app_subtype == HDRHAI_ENHANCED_MULTIFLOW_PACKET_APP_SN)
  {
    if (hdrstream.custom_app_config.app_config_mask.enable_empa)
    {
      app_subtype_is_allowed = TRUE;
    }
  }
#endif /* FEATURE_HDR_EMPA */
#ifdef FEATURE_HDR_REVB
  else if (app_subtype == HDRHAI_MULTILINK_MULTIFLOW_PACKET_APP_SN)
  {
    if (hdrstream.custom_app_config.app_config_mask.enable_mmpa)
    {
      app_subtype_is_allowed = TRUE;
    }
  }
#endif /* FEATURE_HDR_REVB */
#endif /* FEATURE_HDR_QOS */

  return app_subtype_is_allowed;
}

/* <EJECT> */ 
/*=============================================================================
FUNCTION HDRSTREAM_SNPKTAPP_CONFIG_IS_ALLOWED

DESCRIPTION
  This function checks if a given application type bound to SN is allowed to 
  be negotiated. This function should be called only for checking if a given 
  packet application bound to SN is allowed.
    
DEPENDENCIES
  None.

PARAMETERS
  app_subtype - Name of the application subtype
    
RETURN VALUE
  TRUE if the given apptype is allowed to be negotiated FALSE otherwise. 

SIDE EFFECTS
  None
=============================================================================*/
static boolean hdrstream_snpktapp_config_is_allowed
( 
  hdrhai_app_subtype_enum_type app_subtype
)
{
  /* List of negotiated app subtypes */
  uint16 neg_app_subtypes[HDRSTREAM_MAX_NUM_APP_SUBTYPES];
  /* Number of negotiated app subtypes, included in the above array */
  uint8  num_app_subtypes;
  /* Flag to indicate if a given pktapp bound to SN is allowed to be
     negotiated */
  boolean app_subtype_is_allowed = TRUE;
  /* Loop Index */
  int8 index = 0;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (app_subtype == HDRHAI_APP_STREAM_NOT_USED)
  {
    app_subtype_is_allowed = FALSE;
  }
  else if (((app_subtype == HDRHAI_MULTIFLOW_PACKET_APP_SN) ||
            (app_subtype == HDRHAI_ENHANCED_MULTIFLOW_PACKET_APP_SN) ||
            (app_subtype == HDRHAI_MULTILINK_MULTIFLOW_PACKET_APP_SN)) && 
            (!hdrstream_qos_negotiation_is_allowed(app_subtype)))
  {
    /* If MPA/EMPA is disabled through NV item then it is not allowed */
    app_subtype_is_allowed = FALSE;
  }

  /* Check if the given App subtype has been configured in any of the other 
     personalities */
  if (app_subtype_is_allowed) 
  {
    /* If the session is already open it means that session negotiation was
       initiated through ConfigStart, in which case we want to propose all 
       SNPktApps and do the check for stream configuration is valid at the 
       time of SCP receiving SCC with continue = 0 */
    if (hdrstream_stream_is_assigned(HDRHAI_APP_PACKET_APP_ANY))
    {
      app_subtype_is_allowed = TRUE;
    }
    /* Otherwise for intial session negotiation, check if the given App subtype
       has been configured in any of the other personalities */
    else 
    {
      /* Get all negotiated app subtypes from personalities that so far have
         been configured */
      hdrscmdb_get_negotiated_app_subtypes(neg_app_subtypes, &num_app_subtypes);
  
      /* Sanity check to make sure "neg_app_subtypes" did not buffer overflow */
      ASSERT (num_app_subtypes <= HDRSTREAM_MAX_NUM_APP_SUBTYPES);
  
      /* Loop through and check that the app subtype is not negotiated in any 
        other personality */
      for (index = 0; index < num_app_subtypes ; index++)
      {
        if (app_subtype == neg_app_subtypes[index])
        {
          app_subtype_is_allowed = FALSE;
          break;
        }
      }
    }
  }

  return app_subtype_is_allowed;
} /* hdrstream_snpktapp_config_is_allowed() */

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRSTREAM_STREAM_CONFIG_IS_ALLOWED

DESCRIPTION
  This helper function determines if AT is allowed to propose a given stream
  configuration.
  
DEPENDENCIES
  None
  
PARAMETERS
  stream_cfg - Stream Configuration that needs to be checked if AT is allowed
               to propose or not.

RETURN VALUE
  TRUE - If the given stream configuration is allowed, FALSE otherwise.

SIDE EFFECTS
  None

===========================================================================*/
boolean hdrstream_stream_config_is_allowed
(
  hdrstream_config_params_type stream_cfg
)
{
  boolean stream_cfg_is_allowed = FALSE;
/*--------------------------------------------------------------------------*/

  if (hdrstream_snpktapp_config_is_allowed(
        (hdrhai_app_subtype_enum_type)
        stream_cfg.stream_config.stream_application[HDRSTREAM_SNPKTAPP_STREAM_ID]))
  {
    stream_cfg_is_allowed = TRUE;

    /* If the packet application bound to service network in the desired stream 
       configuration is allowed, however if the stream configruation includes 
       CSNA and CSNA is not allowed AT should not propose that stream 
       configuration.  */
    if ((stream_cfg.stream_config.stream_application[HDRSTREAM_CSNA_STREAM_ID] ==
         HDRHAI_3G1X_CSN_APP) && !hdrstream_csna_is_allowed())
    {
      stream_cfg_is_allowed = FALSE;
    }
  }

  return stream_cfg_is_allowed;

} /* hdrstream_stream_config_is_allowed() */

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRSTREAM_ALL_SNPKTAPPS_NEGOTIATED

DESCRIPTION
  This helper function determines if all the Packet Application bound to 
  Service netowrk have already been negotiated or not.

DEPENDENCIES
  None
  
PARAMETERS
  None

RETURN VALUE
  TRUE - All SNPktApps are negotiated, FALSE otherwise

SIDE EFFECTS
  None

===========================================================================*/
static boolean hdrstream_all_snpktapps_negotiated( void )
{
  uint8  loop_i;
  hdrstream_config_params_type stream_cfg;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* For each PktApp in preferred stream configuration array, check if it
     has already been negotiated */
  for (loop_i = 0; loop_i < HDRSTREAM_CONFIG_COMBINATIONS_MAX; loop_i++)
  {
    stream_cfg = hdrstream_preferred_config_table[loop_i];

    /* If atleast one of the app subtype is allowed then return FALSE */
    if (hdrstream_snpktapp_config_is_allowed(
          (hdrhai_app_subtype_enum_type)
          stream_cfg.stream_config.stream_application[HDRSTREAM_SNPKTAPP_STREAM_ID]))
    {
      return FALSE; 
    }
  }

  return TRUE;
}

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRSTREAM_STREAM_CONFIG_IS_VALID

DESCRIPTION
  This helper function determines whether a configuration is valid.

  Valid configuration is:

  stream0: SIG APP
  stream1: PACKET_APP_AN, PACKET_APP_SN, NOT_USED
  stream2: PACKET_APP_AN, PACKET_APP_SN, NOT_USED
  stream3: PACKET_APP_AN, PACKET_APP_SN, NOT_USED

DEPENDENCIES
  None.

PARAMETERS
  config_attrib_ptr - Pointer to stream config attribute to validate

RETURN VALUE
  TRUE - configuration is valid
  FALSE - configuration is not valid

SIDE EFFECTS
  None

===========================================================================*/

LOCAL boolean hdrstream_stream_config_is_valid
(
  hdrstream_stream_config_attrib_type *config_attrib_ptr
)
{
  boolean config_is_valid = TRUE; 
    /* Is configuration valid */
  int i; 
    /* Index counter */
  hdrhai_app_subtype_enum_type csa; 
    /* Temporary storage for current stream application */
  hdrhai_app_subtype_enum_type is890_subtype  = HDRHAI_APP_STREAM_NOT_USED; 
    /* Test app subtype currently configured */
  hdrhai_app_subtype_enum_type sn_pktapp_in_creq = HDRHAI_APP_STREAM_NOT_USED;
    /* SN packet application that the AN included in the configrequest message */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If Stream 0 is not the Signaling App, the Configuration is not valid */
  if (config_attrib_ptr->stream_application[0]  != HDRHAI_DEF_SIG_APP)
  {
    return FALSE;
  }

  /* Validate Stream 1, 2 and 3 */
  for (i = 1; i < HDRSTREAM_NUM_STREAMS; i++)
  {
    csa = (hdrhai_app_subtype_enum_type) 
        config_attrib_ptr->stream_application[i];

    if (((csa == HDRHAI_DEF_PACKET_APP_SN) || 
         (csa == HDRHAI_MULTIFLOW_PACKET_APP_SN) ||
         (csa == HDRHAI_ENHANCED_MULTIFLOW_PACKET_APP_SN) ||
         (csa == HDRHAI_MULTILINK_MULTIFLOW_PACKET_APP_SN)) &&
         (sn_pktapp_in_creq == HDRHAI_APP_STREAM_NOT_USED) &&
         hdrstream_snpktapp_config_is_allowed(csa))
    {
      /* AT allows only one packet application bound to SN to be included 
         in a given configuration request, so if we find two SN PacketApps 
         in the same configrequest AT will reject the message */
      sn_pktapp_in_creq = csa;
    }
    else if ( (csa == HDRHAI_3G1X_CSN_APP) && 
           (sn_pktapp_in_creq == HDRHAI_MULTIFLOW_PACKET_APP_SN) &&
           (sn_pktapp_in_creq == HDRHAI_ENHANCED_MULTIFLOW_PACKET_APP_SN) &&
              hdrstream_csna_is_allowed())
    {
      /* CSNA is acceptable only when MFPA or EMPA is also negotiated */
    }
    else if (( (csa == HDRHAI_DEF_PACKET_APP_IS890) || 
               (csa == HDRHAI_ENH_TEST_APP_IS890A)  || 
               (csa == HDRHAI_MC_TEST_APP_IS890B)  )   && 
             (is890_subtype == HDRHAI_APP_STREAM_NOT_USED))
    {
      /* Save test app subtype */
      is890_subtype = csa;
    }
    else if (( csa == HDRHAI_DEF_PACKET_APP_AN ) ||
             ( csa == HDRHAI_APP_STREAM_NOT_USED ))
    {
      /* These are always acceptable */
    }
    else
    {
      /* If none of the above conditinos were true, something is  wrong */
      HDR_MSG_PROT_2 (MSG_LEGACY_ERROR,  
                      "STREAM: Got invalid app for str (%d -> %d)", csa, i);
        config_is_valid = FALSE;
        break;
    }

    /* This stream application is acceptable.  Go on to next one */
    HDR_MSG_PROT_2 (MSG_LEGACY_HIGH,  
                    "STREAM: Got valid app for str (%d -> %d)", csa, i);
  }

  return config_is_valid;

} /* hdrstream_stream_config_is_valid */

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRSTREAM_CONFIG_IS_NEEDED

DESCRIPTION
  This helper function determines whether configuration is necessary.

  Configuration is needed when AN Packet or SN Packet are not assigned
  to a stream at least once.

DEPENDENCIES
  This function assumes that the "config valid" function only accepts
  a configuration with signaling assigned to stream 0.  If that function
  allows the signaling to be assigned to a different stream, or another
  application to be assied to stream 0, this should be checked as well.

PARAMETERS
  None

RETURN VALUE
  TRUE - configuration is necessary
  FALSE - configuration is not necessary

SIDE EFFECTS
  None

===========================================================================*/
boolean hdrstream_config_is_needed( void )
{
  boolean config_is_needed = FALSE;
    /* Flag indicating if Stream needs to configure or not */
/*-------------------------------------------------------------------------*/

  if (hdrstream_stream_is_assigned(HDRHAI_APP_PACKET_APP_ANY))
  {
    if (!hdrscp_phone_is_set_to_rev0_mode())
    {
      /* In Rev A mode for session negotiation initiated through ConfigStart 
         we propose all SNpktapps all the time and do the check at the end to
         see if configuration is valid */
      config_is_needed = TRUE;
    }
    else
    {
      /* In Rev 0 mode we maintain the call flow same as before i.e. do not 
         propose stream configuration request message if the session was 
         initiated through ConfigStart, otherwise some old test base station
         fails to negotiate the session */
    }
  }
  else if (!hdrscp_config_subtype_is_hardlink(HDRHAI_STREAM_PROTOCOL) &&
           !hdrstream_all_snpktapps_negotiated())
  {
    /* For initial session negotiation stream, configuration is needed if
       1) Stream protocol's negotiated subtype is not hardlink.
       2) Not all the SN packet application types have already been configured
          in personalities. */
    config_is_needed = TRUE;
  }

  return config_is_needed;
} /* hdrstream_config_is_needed */

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRSTREAM_GET_STREAM

DESCRIPTION
  This helper function returns the stream ID of the stream associated with the
  app subtype specified.

DEPENDENCIES

PARAMETERS
  app_subtype - The application subtype who's stream is desired.
  stream_id - Pointer to location in which to return the stream_id, if 
              one is found.

RETURN VALUE
  TRUE - There is a stream configured with the Service Network
  FALSE - There is no stream configured with the Service Network

SIDE EFFECTS
  None

===========================================================================*/

boolean hdrstream_get_stream  
( 
  hdrhai_app_subtype_enum_type app_subtype,
  hdrhai_stream_enum_type *stream_id
)
{
  uint8 index = 0;
  uint16 csa; /* Temporary storage for current stream application */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  for (index = 0; index < HDRSTREAM_NUM_STREAMS; index++)
  {
    csa = hdrstream.current.stream_config.stream_application[index];
    if (csa == app_subtype)
    {
      /* Depends on hdrhai_stream_enum_type using 0 for HDRHAI_STREAM0, etc. */
      *stream_id = (hdrhai_stream_enum_type) index;
      return TRUE;
    }
  }

  return FALSE;

} /* hdrstream_get_stream */

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRSTREAM_GET_STREAM_FOR_PROTOCOL

DESCRIPTION
  This helper function get stream id from protocol id.

DEPENDENCIES
  None
  
PARAMETERS
  protocol_id   - Protocol ID

RETURN VALUE
  stream_id

SIDE EFFECTS
  None
===========================================================================*/
hdrhai_stream_enum_type hdrstream_get_stream_for_protocol
( 
  uint8   protocol_id
)
{
  hdrhai_stream_enum_type  stream_id;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Get the stream ID (if applicable) from the protocol ID */
  switch ( (hdrhai_protocol_type_enum_type)protocol_id )
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
      stream_id = HDRHAI_STREAM0;
      break;
  }
  
  return stream_id;

} /* hdrstream_get_stream_for_protocol() */

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRSTREAM_GET_NEGOTIATED_STREAM

DESCRIPTION
  This helper function returns the stream ID of the stream associated with the
  app subtype specified.

DEPENDENCIES

PARAMETERS
  app_subtype - The application subtype who's stream is desired.
  stream_id - Pointer to location in which to return the stream_id, if 
              one is found.

RETURN VALUE
  TRUE - There is a stream configured with the Service Network
  FALSE - There is no stream configured with the Service Network

SIDE EFFECTS
  None

===========================================================================*/

boolean hdrstream_get_negotiated_stream  
( 
  hdrhai_app_subtype_enum_type app_subtype,
  hdrhai_stream_enum_type *stream_id_ptr /* output */
)
{
  uint8 index = 0;
  uint16 nsa; /* Temporary storage for current stream application */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  for (index = 0; index < HDRSTREAM_NUM_STREAMS; index++)
  {
    nsa = hdrstream.negotiated.stream_config.stream_application[index];
    if (nsa == app_subtype)
    {
      /* Depends on hdrhai_stream_enum_type using 0 for HDRHAI_STREAM0, etc. */

      if (stream_id_ptr != NULL)
      {
        *stream_id_ptr = (hdrhai_stream_enum_type) index;
      }
      return TRUE;
    }
  }

  return FALSE;

} /* hdrstream_get_negotiated_stream */

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRSTREAM_GET_APPTYPE_FOR_NEG_STREAM

DESCRIPTION
  This function returns the negotiated App Subtype for the particular Stream 
  ID. Note that this value is the negotiated value, and may not have been
  committed.

DEPENDENCIES
  None.

PARAMETERS

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
hdrhai_app_subtype_enum_type hdrstream_get_apptype_for_neg_stream
( 
  hdrhai_stream_enum_type stream_id 
)
{
  return (hdrhai_app_subtype_enum_type)
      hdrstream.negotiated.stream_config.stream_application[(uint8)stream_id];
} /* hdrstream_get_apptype_for_neg_stream() */

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRSTREAM_PROCESS_CONFIGURE

DESCRIPTION
  This function processes the Configure Command.

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

static void hdrstream_process_configure ( void )
{
  /* Pointer to config req. message to send */
  dsm_item_type * send_msg_ptr;
  /* Bit offset from start of message of current attrib */
  uint16 aoffset;
  /* Bit offset from start of attrib of current value */
  uint16 voffset; 
  /* Loop index */
  int8  index = 0; 
  /* Number of attribute values to be included in config request msg */
  uint8 attrib_val_count = 0; 
  /* Shorthand pointer to preferred STREAM configuration parameters */
  hdrstream_config_params_type *psc_ptr;
  uint8 creq_len_bytes = 0;
  uint8 index_in_psc = 0;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If already waiting for ack from SLP for a ConfigureRequest message,
     ignore this Configure command & do not sent another ConfigureRequest */  
  if( hdrstream.config_req_info.awaiting_slp_ack == TRUE )
  {
    ERR("Configure cmd ignored. Awaiting SLP ack",0,0,0);
    return;
  }

  /* Remember what we are requesting in a regional structure */
  hdrstream.config_req_info.transaction_id++;
  hdrstream.config_req_info.num_attrib_vals = 0;
  hdrstream.config_req_info.awaiting_slp_ack = TRUE;

  /* Loop for each entry in the preferred configuration array, to check if the
     packet application can be proposed in the current configuration request */
  for (index_in_psc = 0; index_in_psc < HDRSTREAM_CONFIG_COMBINATIONS_MAX; index_in_psc++)
  {
    if (hdrstream_stream_config_is_allowed(
          hdrstream_preferred_config_table[index_in_psc]))
    {
      /* Remember the index of the stream configuration value in the 
         "hdrstream_preferred_config_table" array that AT is proposing
         and also the associated value_id */
      hdrstream.config_req_info.psc[attrib_val_count].preferred_config_index =
          index_in_psc;
      hdrstream.config_req_info.psc[attrib_val_count].value_id = 
          attrib_val_count;

      attrib_val_count++;
    }
  }

  hdrstream.config_req_info.num_attrib_vals = attrib_val_count;

  send_msg_ptr = hdrutil_new_msg_buffer();

  /* Build the config request messge */
  HDRSTREAM_PACK_CON_REQ_MSG_ID( send_msg_ptr, HDRSTREAM_CONFIG_REQ_MSG);
  HDRSTREAM_PACK_CON_REQ_TRANS_ID( send_msg_ptr, 
    hdrstream.config_req_info.transaction_id);

  /* Set aoffset to the offset where the first attribute is to be packed in */
  aoffset = HDRSTREAM_MSG_ID_SIZE + HDRSTREAM_TRANSACTION_ID_SIZE;

  /* Length field in the config request is length in bytes, excluding the
     length of length field */
  creq_len_bytes = (HDRSTREAM_ATTRIB_ID_SIZE + 
                  (attrib_val_count*HDRSTREAM_STREAM_CONFIG_ATTRIB_VAL_SIZE)) /
                    HDRSTREAM_BITS_PER_BYTE;

  HDRSTREAM_PACK_CON_REQ_LEN( send_msg_ptr, creq_len_bytes, aoffset);

  HDRSTREAM_PACK_CON_REQ_ATTRIBUTE_ID(send_msg_ptr, 
    HDRSTREAM_STREAM_CONFIG_ATTRIB_ID, aoffset);

  /* Set voffset to offset inside of attrib where first value is to be put */
  voffset = HDRSTREAM_ATTRIB_LEN_SIZE + HDRSTREAM_ATTRIB_ID_SIZE;

  for (index = 0; index < hdrstream.config_req_info.num_attrib_vals; index++)
  {
    index_in_psc = hdrstream.config_req_info.psc[index].preferred_config_index;

    psc_ptr = &hdrstream_preferred_config_table[index_in_psc];

    HDRSTREAM_PACK_CON_REQ_SC_VALUE_ID(send_msg_ptr,
      hdrstream.config_req_info.psc[index].value_id, aoffset, voffset);

    HDRSTREAM_PACK_CON_REQ_SC_STREAM0(send_msg_ptr,
      psc_ptr->stream_config.stream_application[0], aoffset, voffset);
    HDRSTREAM_PACK_CON_REQ_SC_STREAM1(send_msg_ptr,
      psc_ptr->stream_config.stream_application[1], aoffset, voffset);
    HDRSTREAM_PACK_CON_REQ_SC_STREAM2(send_msg_ptr,
      psc_ptr->stream_config.stream_application[2], aoffset, voffset);
    HDRSTREAM_PACK_CON_REQ_SC_STREAM3(send_msg_ptr,
      psc_ptr->stream_config.stream_application[3], aoffset, voffset);

    /* Move voffset to the next value */
    voffset += HDRSTREAM_STREAM_CONFIG_ATTRIB_VAL_SIZE;
  }

  hdrhmp_send_msg(HDRHAI_STREAM_PROTOCOL,
                  HDRHAI_RTC,
                  TRUE,
                  HDRSTREAM_MSG_PRIORITY,
                  HDRHAI_IN_CONFIG, /* Instance of protocol sending message */
                  send_msg_ptr,
                  hdrstream_config_tx_outcome_cb,
                  NULL);
} /* hdrstream_process_configure */

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRSTREAM_PROCESS_SESSION_CHANGED

DESCRIPTION
  This function processes the SCM.SessionChanged Indication.

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

static void hdrstream_process_session_changed ( void )
{

  if (hdrstream.config_req_info.awaiting_slp_ack)
  {
    /* The flag must be set first because hdrhmp_abort_msg() causes
     * the callback to be called, and if this flag is not FALSE
     * the callback will set the config response timer, for example.
     */
    hdrstream.config_req_info.awaiting_slp_ack = FALSE;
    (void) hdrhmp_abort_msg(hdrstream_config_tx_outcome_cb, NULL);
  }

  (void) rex_clr_timer( &hdrstream.hdrstream_config_timer );
  (void) rex_clr_sigs(HDRSTREAM_TASK_PTR, HDRMC_STREAM_CONFIG_RSP_TIMER_SIG );

  /* Update the local copy of the current configuration from SCMDB */
  hdrscmdb_get_inuse_config(HDRHAI_STREAM_PROTOCOL, &hdrstream.current);

  /* Because this module references its in_config instance even while
   * SCP is in OPEN state, we need to initialize the in_config to match
   * in_use.
   */
#ifdef FEATURE_MEMCPY_REMOVAL
  hdrstream.negotiated = hdrstream.current;
#else
  memcpy( &hdrstream.negotiated, &hdrstream.current,
          sizeof(hdrstream_config_params_type));
#endif /* FEATURE_MEMCPY_REMOVAL */

} /* hdrstream_process_session_changed */

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRSTREAM_PROCESS_SC_ATTRIB

DESCRIPTION
  This function processes the stream configuration attribute of the 
  Configuration Request Message

DEPENDENCIES
  None

PARAMETERS
  item_ptr - Pointer to the DSM item chain holding the message
  attrib_len - Length, in bits, of the attribute, including the length field
  attrib_offset - Bit offset of the attribute from the beginning of the message
  value_id_ptr - Pointer to location to write value ID of accepted attribute
  stream_config_ptr - Pointer to struct to put the negotiated value into

RETURN VALUE
  TRUE if a new, acceptable value for the attribute was found, FALSE otherwise.
  If return value is TRUE, value_id is returned through value_id_ptr, and the
  acceptable value is returned through stream_config_ptr.

SIDE EFFECTS
  None.

===========================================================================*/

LOCAL boolean hdrstream_process_stream_config
(
  dsm_item_type * item_ptr,
  uint16 attrib_len,
  uint16 attrib_offset,
  uint8 *value_id_ptr,
  hdrstream_stream_config_attrib_type *stream_config_ptr
)
{
  /* Bit offset of current attribute value from the beginning of the 
   * current attribute */
  uint16 attrib_val_offset; 
  boolean val_is_acceptable = FALSE;  /* If we accept the attribute */
  uint8 cur_value_id; /* Value ID associated with current value */
  hdrstream_stream_config_attrib_type cur_value; /* current attribute value */
  int err; /* sum of errno return vals, depends that E_SUCCESS is zero */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* The first attribute value is after the attribute length and ID fields */
  attrib_val_offset = HDRSTREAM_ATTRIB_LEN_SIZE + HDRSTREAM_ATTRIB_ID_SIZE;

  while ((attrib_val_offset + HDRSTREAM_STREAM_CONFIG_ATTRIB_VAL_SIZE) <= 
         attrib_len)
  {
    /* There is at least one more attribute value in this attribute */
    err  = HDRSTREAM_UNPACK_CON_REQ_SC_VALUE_ID(
             item_ptr, &cur_value_id, 
             attrib_offset, attrib_val_offset );

    err |= HDRSTREAM_UNPACK_CON_REQ_SC_STREAM0(
             item_ptr, &cur_value.stream_application[0], 
             attrib_offset, attrib_val_offset );

    err |= HDRSTREAM_UNPACK_CON_REQ_SC_STREAM1(
             item_ptr, &cur_value.stream_application[1], 
             attrib_offset, attrib_val_offset );

    err |= HDRSTREAM_UNPACK_CON_REQ_SC_STREAM2(
             item_ptr, &cur_value.stream_application[2], 
             attrib_offset, attrib_val_offset );

    err |= HDRSTREAM_UNPACK_CON_REQ_SC_STREAM3(
             item_ptr, &cur_value.stream_application[3], 
             attrib_offset, attrib_val_offset );

    if (err)
    {
      ERR("Error unpacking Configuration Request message, ignoring",
            0,0,0);
      break;
    }

    /* Verify that the received attribute is acceptable */
    if (hdrstream_stream_config_is_valid(&cur_value))
    {
      *value_id_ptr = cur_value_id;
#ifdef FEATURE_MEMCPY_REMOVAL
      *stream_config_ptr = cur_value;
#else
      memcpy(stream_config_ptr, &cur_value, 
             sizeof(hdrstream_stream_config_attrib_type));
#endif /* FEATURE_MEMCPY_REMOVAL */
      val_is_acceptable = TRUE;
      break;
    }

    attrib_val_offset += HDRSTREAM_STREAM_CONFIG_ATTRIB_VAL_SIZE;
  }
  return (val_is_acceptable);
} /* hdrstream_process_stream_config */

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRSTREAM_INIT_APP_PROTOCOLS_INCONFIG_TO_DEFAULT

DESCRIPTION
  This function sets the inconfig instance of application layer protocols
  to default values.

DEPENDENCIES
  None

PARAMETERS
  None
                     
RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
static void hdrstream_init_app_protocols_inconfig_to_default( void )
{
  uint8 stream_id;
/*-------------------------------------------------------------------------*/

  for (stream_id = 1; stream_id < HDRSTREAM_NUM_STREAMS; stream_id++)
  {
    /* Since stream 0 is always signaling app skip streamid 1. For each of 
       the other stream check if the packet application had changed in the
       current negotiattion */

    if (hdrstream.current.stream_config.stream_application[stream_id] !=
        hdrstream.negotiated.stream_config.stream_application[stream_id])
    {

      /* If the packet application had changed to MPA/EMPA/MMPA reset all 
         of the packet application protocol's configuration */
      if ((hdrstream.negotiated.stream_config.stream_application[stream_id]
           == HDRHAI_MULTIFLOW_PACKET_APP_SN) ||
          (hdrstream.negotiated.stream_config.stream_application[stream_id]
           == HDRHAI_ENHANCED_MULTIFLOW_PACKET_APP_SN) ||
          (hdrstream.negotiated.stream_config.stream_application[stream_id]
           == HDRHAI_MULTILINK_MULTIFLOW_PACKET_APP_SN))
      {
        /* Reset LUP, MRLP, FCP, DOS */
        hdrscmdb_set_pktapp_protocols_to_default();
      }
      /* If stream is configured fr CSNA, init CSNA with defaults */
      else if ((hdrstream.negotiated.stream_config.stream_application[stream_id]
           == HDRHAI_3G1X_CSN_APP))
      {
        /* Reset CSNA */
        hdrscmdb_set_csna_protocol_to_default();
      }

    }
  }

} /* hdrstream_init_app_protocols_inconfig_to_default() */

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRSTREAM_PROCESS_CONFIG_REQ_MSG

DESCRIPTION
  This function processes the Configuration Request Message.

DEPENDENCIES
  The MSG_ID is not checked.  This function assumes that the given message
    is the Configuration Request Message.

PARAMETERS
  item_ptr - pointer to the received message

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

LOCAL void hdrstream_process_config_req_msg
(
  dsm_item_type * item_ptr
)
{
  int total_bits_remaining; /* number of bits remaining in the message */
  uint8  creq_transaction_id; /* Transaction ID of the message */

  uint8 attrib_len; /* Bit length of attribute, including the length field */
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

  /* boolean flag needed to indicate if config_req from AN included
   * HDRSTREAM_STREAM_CONFIG_ATTRIB_ID. This test is needed to roll
   * back the value to the fallback value if proposal from AN is
   * rejected.
   */
  boolean stream_config_included = FALSE;

  /* As we parse the message we update the negotiated parameters in a 
   * local copy as we agree to accept new values.  That way, if we 
   * later encounter an error and decide not to send a response, or if 
   * the same attribute is negotiated twice and we accept the first but
   * decline the second, we don't corrupt the real negotiated parameters.
   */

  /* For each simple attribute we need a flag if we are accpeting it
   * and the value we are accepting.
   */

  /* For each complex attribute we need a flag if we are accpeting it,
   * the ValueID we are accepting, and a structure holding the accepted value.
   */
  struct
  {
    boolean is_accepted;
    uint8 value_id;
    hdrstream_stream_config_attrib_type value;
  } stream_config_attrib;

  dsm_item_type * send_msg_ptr; /* dsm buffer for sent message */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  total_bits_remaining = (dsm_length_packet(item_ptr) *
                         HDRSTREAM_BITS_PER_BYTE);

  /* Extract the Transaction ID */
  if (HDRSTREAM_UNPACK_TRANS_ID(item_ptr, &creq_transaction_id ) !=
      E_SUCCESS)
  {
    ERR("Error extracting TransactionID from ConfigReq Msg",0,0,0);
    /* Without the transaction ID, it makes no sense to send a response */
    return;
  }

  /* Initialize all "is_accepted" flags that are inside structures */
  stream_config_attrib.is_accepted = FALSE;

  /* Initialize attrib_len so the initial attrib_offset will be
   * immediately after the transaction ID of the config request.
   */
  attrib_len = HDRSTREAM_MSG_ID_SIZE + HDRSTREAM_TRANSACTION_ID_SIZE;

  /* Unpack the configuration attributes */

  /* While there are at least enough bits for the smallest attribute... */ 
  while ((total_bits_remaining - attrib_len) >= HDRSTREAM_MIN_ATTRIB_LEN)
  {
    /* Move to the next attribute record */
    attrib_offset += attrib_len;
    total_bits_remaining -= attrib_len;

    /* Get the length of the new attribute */
    if (HDRSTREAM_UNPACK_CON_REQ_LEN(item_ptr, &attrib_len, 
        attrib_offset) != E_SUCCESS)
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
    attrib_len *= HDRSTREAM_BITS_PER_BYTE;
    attrib_len += HDRSTREAM_ATTRIB_LEN_SIZE;

    /* Any attribute record must have at least an attribute ID */
    if ((attrib_len < HDRSTREAM_MIN_ATTRIB_LEN) ||
        (attrib_len > total_bits_remaining))
    {
      /* If the length is something unexpected, we can't be sure of what 
       * the AN intended and it is safest to not send a response
       */
      ERR("Error, invalid total len vs bits in message. ignoring",0,0,0);
      send_response = FALSE;
      break;
    }

    /* There is at least one more attribute record in the message */

    if (HDRSTREAM_UNPACK_CON_REQ_ATTRIBUTE_ID(item_ptr, &(attrib_id), 
        attrib_offset) != E_SUCCESS)
    {
      ERR("Error unpacking Configuration Request message, ignoring",0,0,0);
      /* If we can't get the attribute_id, don't send a response */
      send_response = FALSE;
      break;
    }

    switch (attrib_id)
    {
      case HDRSTREAM_STREAM_CONFIG_ATTRIB_ID:
        stream_config_included = TRUE;
        if (attrib_len >= HDRSTREAM_STREAM_CONFIG_MIN_ATTRIB_LEN)
        {
          stream_config_attrib.is_accepted = 
            hdrstream_process_stream_config(
              item_ptr, attrib_len, attrib_offset, 
               &stream_config_attrib.value_id,
               &stream_config_attrib.value );
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
        ERR("Unknown Attribute ID %d",attrib_id,0,0);
        break;
    } /* switch */

    if (!send_response)
    {
      /* No need to continue parsing once we decide not to send a response */
      break;
    }
  } /* while */

  /**************************************************************************
   * Per EV-DO standard, if AT rejected AN's proposal for a particular      *
   * attribute, AT needs to set the value to the fallback value for         *
   * that attribute, ie, that of the current in_use stance value.           *
   * The hdrstream.negotiated structures contains the value after AT_INIT   *
   * negotiation, and might be different to the in_use value. The           *
   * negotiated variable will be copied into hdrstreamc.current structure,  *
   * overwriting the in_use stance value after Processing of ConfigReq.     *
   * Therefore, rolling back is needed if 1) AT proposed a value during     *
   * AT_INIT state, 2) AT does not use scmdb functions to manage            *
   * configuration.                                                         *
   **************************************************************************/

  if (stream_config_included && 
      !stream_config_attrib.is_accepted)
  {
#ifdef FEATURE_MEMCPY_REMOVAL
    hdrstream.negotiated.stream_config = hdrstream.current.stream_config;
#else
    memcpy(&hdrstream.negotiated.stream_config, 
      &hdrstream.current.stream_config,
      sizeof(hdrstream.negotiated.stream_config));
#endif /* FEATURE_MEMCPY_REMOVAL */

    hdrstream_init_app_protocols_inconfig_to_default();

  }

  /* Form and send the Configuration Response message. */

  if (send_response)
  {
    /* get new buffer */
    send_msg_ptr = hdrutil_new_msg_buffer();

    /* pack fields */
    HDRSTREAM_PACK_CON_RSP_MSG_ID( send_msg_ptr, HDRSTREAM_CONFIG_RSP_MSG);
    HDRSTREAM_PACK_CON_RSP_TRANS_ID( send_msg_ptr, creq_transaction_id);

    /* Now pack in the attribute ID and value of all attributes we have 
     * flagged that we want to accept.
     */
  
    /* attrib_offset is now offset for response message */
    attrib_offset = HDRSTREAM_MSG_ID_SIZE + HDRSTREAM_TRANSACTION_ID_SIZE;

    /* If it was _both_ requested & accepted (need to keep track)*/
    if (stream_config_attrib.is_accepted)
    {
      /* Include that we are accepting the stream config attribute */
      HDRSTREAM_PACK_CON_RSP_LEN( send_msg_ptr, HDRSTREAM_CRSP_ATTRIB_LEN_BYTES,
                               attrib_offset);
      HDRSTREAM_PACK_CON_RSP_ATTRIBUTE_ID( send_msg_ptr, 
        HDRSTREAM_STREAM_CONFIG_ATTRIB_ID, attrib_offset );
      HDRSTREAM_PACK_CON_RSP_VALUE_ID( send_msg_ptr, 
        stream_config_attrib.value_id, attrib_offset);

      attrib_offset += HDRSTREAM_CRSP_ATTRIB_LEN;

      /* Update our regional structure to reflect the newly accepted values */
#ifdef FEATURE_MEMCPY_REMOVAL
      hdrstream.negotiated.stream_config = stream_config_attrib.value;
#else
      memcpy( &hdrstream.negotiated.stream_config,
              &stream_config_attrib.value, 
              sizeof( hdrstream_stream_config_attrib_type ));
#endif /* FEATURE_MEMCPY_REMOVAL */
    }
    /* Else, if it was requested, we've set the negotiated instance back to its 
       fallback/in_use value before sending a rsp.
       If we didn't want to accept and we don't want to use default/fallback, then
       we shouldn't send a config response (session will be closed, and
       we would go back to default) */

    /* if (accept_some_other_attribute) ... */
  
    /* Send config response on RTC.  All config messages are reliable. 
     */
    hdrhmp_send_msg( HDRHAI_STREAM_PROTOCOL,
                     HDRHAI_RTC,
                     TRUE, /* Reliable */
                     HDRSTREAM_MSG_PRIORITY,
                     HDRHAI_IN_CONFIG,/* Instance of protocol sending message */
                     send_msg_ptr,
                     NULL,
                     NULL );
    
  } /* end if (send_response) */

} /* hdrstream_process_config_req_msg */

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRSTREAM_PROCESS_CONFIG_RSP_MSG

DESCRIPTION
  This function processes the Configuration Response Message.

DEPENDENCIES
  The MSG_ID is not checked.  This function assumes that the given message
    is the Configuration Response Message.

PARAMETERS
  item_ptr - pointer to the received message

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

LOCAL void hdrstream_process_config_rsp_msg
(
  dsm_item_type * item_ptr
)
{
  uint8  crsp_transaction_id; /* Transaction ID of the message */
  uint16 total_bits_remaining; /* Number of bits left to be unpacked */
  uint8 attrib_len; /* Bit length of attribute, including the length field */
  uint16 attrib_offset = 0; /* Bit offset of current attribute */
  uint8  attrib_id = 0; /* ID field of current attribute being unpacked */
  uint8 value_id; /* Value ID of current attribute being unpacked */

  /* Flags if this response accepts our proposed stream config attribute */
  boolean stream_config_attrib_is_accepted = FALSE; 

  /* Flag if we should fail negotiation based on this config response */
  boolean negotiation_failed = FALSE; 

  /* Flag if there was an error with the message structure */
  boolean message_parsing_error = FALSE; 

  /* Index of the stream configuration in the preferred stream configuration
     array that AN accepted */
  uint8 accepted_config_index = 0;

  /* Loop Index */
  uint8 index = 0;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  total_bits_remaining = dsm_length_packet(item_ptr) * HDRSTREAM_BITS_PER_BYTE;

  /* Extract the Transaction ID from the message */
  if (HDRSTREAM_UNPACK_TRANS_ID(item_ptr, &crsp_transaction_id) != E_SUCCESS)
  {
    ERR("Error extracting TransactionID from ConfigRsp Msg", 0, 0, 0 );
    return;
  }

  /* if transaction ID doesn't match, ignore message */
  if (crsp_transaction_id != hdrstream.config_req_info.transaction_id)
  {
     HDRTRACE_RXMSG_IGNORED_SEQ(HDRHAI_STREAM_PROTOCOL,
      HDRSTREAM_CONFIG_RSP_MSG, 0); //lint !e506
    return;
  }

  /* Initialize attrib_len so the initial attrib_offset will be
   * immediately after the transaction ID of the config response.
   */
  attrib_len = HDRSTREAM_MSG_ID_SIZE + HDRSTREAM_TRANSACTION_ID_SIZE;

  /* Unpack the configuration attributes */

  /* While there are at least enough bits for the smallest attribute... */ 
  while ((total_bits_remaining - attrib_len) >= HDRSTREAM_CRSP_ATTRIB_LEN)
  {
    /* Move to the next attribute record */
    attrib_offset += attrib_len;
    total_bits_remaining -= attrib_len;

    if (HDRSTREAM_UNPACK_CON_RSP_LEN(item_ptr, &attrib_len, attrib_offset) !=
        E_SUCCESS)
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
    attrib_len *= HDRSTREAM_BITS_PER_BYTE;
    attrib_len += HDRSTREAM_ATTRIB_LEN_SIZE;

    /* In a config response, all attributes are length, attrib_id, value_id */
    if ((attrib_len < HDRSTREAM_CRSP_ATTRIB_LEN) ||
        (attrib_len > total_bits_remaining))
    {
      ERR("Error, invalid total len vs bits in message. ignoring",0,0,0);
      message_parsing_error = TRUE;
      break;
    }

    if (HDRSTREAM_UNPACK_CON_RSP_ATTRIBUTE_ID(item_ptr, &attrib_id,
        attrib_offset) != E_SUCCESS)
    {
      ERR("Error unpacking creq_attrib_id",0,0,0);
      message_parsing_error = TRUE;
      break;
    }

    if (HDRSTREAM_UNPACK_CON_RSP_VALUE_ID(item_ptr, &value_id,
        attrib_offset) != E_SUCCESS)
    {
      ERR("Error unpacking creq_value_id",0,0,0);
      message_parsing_error = TRUE;
      break;
    }

    switch (attrib_id)
    {
      case HDRSTREAM_STREAM_CONFIG_ATTRIB_ID:
        for (index = 0; index < hdrstream.config_req_info.num_attrib_vals; 
             index++)
        {
          if (hdrstream.config_req_info.psc[index].value_id == value_id)
          {
            stream_config_attrib_is_accepted = TRUE;
            accepted_config_index = 
                   hdrstream.config_req_info.psc[index].preferred_config_index;
            break;
          }
        }

        if (!stream_config_attrib_is_accepted)
        {
          /* Accepting a different ValueID than we proposed is a failure */
          negotiation_failed = TRUE;
        }
        break;

      default:
        ERR("Unknown Attribute ID %d",attrib_id,0,0);
        /* Accepting an attribute we never negotiated is grounds for failure */
        negotiation_failed = TRUE;
        break;
    } /* switch */

    if ((negotiation_failed) || (message_parsing_error))
    {
      break;
    }
  }

  if (message_parsing_error)
  {
    /* If we had an unexpected problem parsing the message then it is better
     * to just ignore it.  If the AN wants to send a valid response it still 
     * can, or else our config response timer will expire and we will abort.
     */
  }
  else if (negotiation_failed)
  {
    hdrstream_fail_cur_config_neg(HDRHAI_CLOSE_PROT_NEG_FAIL, attrib_id);
  }
  else
  {
    if (hdrstream.config_req_info.awaiting_slp_ack)
    {
      /* The flag must be set first because hdrhmp_abort_msg() causes
       * the callback to be called, and if this flag is not FALSE
       * the callback will set the config response timer, for example.
       */
      hdrstream.config_req_info.awaiting_slp_ack = FALSE;
      (void) hdrhmp_abort_msg(hdrstream_config_tx_outcome_cb, NULL);
    }

    /* Message is a response to our config request. Stop and clear timer */
    (void) rex_clr_timer( &hdrstream.hdrstream_config_timer );
    (void) rex_clr_sigs(HDRSTREAM_TASK_PTR, HDRMC_STREAM_CONFIG_RSP_TIMER_SIG );

    if (stream_config_attrib_is_accepted)
    {
#ifdef FEATURE_MEMCPY_REMOVAL
      hdrstream.negotiated.stream_config = 
         hdrstream_preferred_config_table[accepted_config_index].stream_config;
#else
      memcpy( &hdrstream.negotiated.stream_config,
              &hdrstream_preferred_config_table[accepted_config_index].stream_config,
              sizeof(hdrstream_stream_config_attrib_type));
#endif /* FEATURE_MEMCPY_REMOVAL */

      hdrstream_init_app_protocols_inconfig_to_default();
    }
    else
    {
      /* A config response was received with the correct transaction ID but 
       * it did not accept stream config attribute, so it implicitly accepts 
       * the default value (last value negotiated) for this attribute.
       */
#ifdef FEATURE_MEMCPY_REMOVAL
      hdrstream.negotiated.stream_config = hdrstream.current.stream_config;
#else
      memcpy( &hdrstream.negotiated.stream_config,
              &hdrstream.current.stream_config,
              sizeof(hdrstream_stream_config_attrib_type));
#endif /* FEATURE_MEMCPY_REMOVAL */
    }
  
    hdrind_give_ind(HDRIND_STREAM_CONFIG_COMPLETE, NULL);
  }

} /* hdrstream_process_config_rsp_msg */

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRSTREAM_CONFIG_TX_OUTCOME_CB

DESCRIPTION
  This function tells SCP that the given config complete msg was sent.

DEPENDENCIES
  If hdrhmp_abort_msg() has been called previously to abort this message,
  the flag hdrstream.config_req_info.awaiting_slp_ack should be set to
  FALSE and any "message outcome" related processing done at that point.
  This callback only handles the case where hdrhmp_abort_msg() has not
  been previously called.

PARAMETERS
  data_ptr - pointer to any callback data.  ignored.
  err_val - shows the status of the sent message.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

LOCAL void hdrstream_config_tx_outcome_cb
(
  void * data_ptr,
  hdrerrno_enum_type err_val
)
{

  if (!hdrstream.config_req_info.awaiting_slp_ack)
  {
    HDR_MSG_PROT (MSG_LEGACY_LOW, "Ignore config req tx outcome callback");
    return;
  }

  /* This flag is set to TRUE when the message is sent, and because it is
   * guaranteed that this callback is called regardless of the outcome of
   * the message, the flag will not get out of sync.
   */
  hdrstream.config_req_info.awaiting_slp_ack = FALSE;

  switch (err_val)
  {
    case E_SUCCESS:
      /* message transmitted successfully */
      HDRTRACE_TXMSG_ACKED(HDRHAI_STREAM_PROTOCOL, 
        HDRSTREAM_CONFIG_REQ_MSG, 0); 
      (void) rex_set_timer( &hdrstream.hdrstream_config_timer,
                            HDRHAI_CONFIG_RSP_TIMER_MSEC );
      break;

    case E_FAILURE: /* given when lose acquisition */

      HDRTRACE_TXMSG_FAILED(HDRHAI_STREAM_PROTOCOL,
        HDRSTREAM_CONFIG_REQ_MSG, 0);
      hdrstream_fail_cur_config_neg(HDRHAI_CLOSE_CFG_MSG_FAIL,
                                    HDRSTREAM_STREAM_CONFIG_ATTRIB_ID);
      break;

    case E_AGAIN:   /* given when we lose the connection */
    case E_CANCELED:/* given if connection closes before max retries */
      /* ignore, configure command will be given again by SCP */
    case E_NOT_ALLOWED:
      break;

    default:
      ERR( "Unknown err val for Config outcome %d",
            err_val, 0, 0);
  }
} /* hdrstream_config_tx_outcome_cb */ //lint !e715 data_ptr not used
   
/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRSTREAM_CONFIG_MSG_CB

DESCRIPTION
  This function is registered to be called whenever a protocol gives a
  message to HDRSTREAM in-config instance.

DEPENDENCIES
  None
  
PARAMETERS
  msg - Pointer to the DSM item chain holding the message

RETURN VALUE
  None
  
SIDE EFFECTS
  None

===========================================================================*/

void hdrstream_config_msg_cb
(
  dsm_item_type * msg_ptr 
)
{
  hdrmc_queue_msg( HDRHAI_STREAM_PROTOCOL, HDRHAI_IN_CONFIG, msg_ptr );
} /* hdrstream_config_msg_cb */

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRSTREAM_IND_CB

DESCRIPTION
  This function is registered to be called whenever a protocol gives an
  indication of interest to HDRSTREAM.

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

void hdrstream_ind_cb
(
  hdrind_ind_name_enum_type ind_name,
  void * ind_data_ptr 
)
{
  if (rex_self() == HDRSTREAM_TASK_PTR)
  {
    hdrstream_process_ind(ind_name, (hdrind_ind_data_union_type *)ind_data_ptr);
  }
  else
  {
    hdrmc_queue_ind( HDRHAI_STREAM_PROTOCOL, ind_name, ind_data_ptr );
  }
} /* hdrstream_ind_cb */

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRSTREAM_POWERUP_INIT

DESCRIPTION
  This function commands STREAM to initialize.  This function is only called
    during powerup.

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void hdrstream_powerup_init ( void )
{

  if ( hdrmc_is_first_init() )
  {
    rex_def_timer( &hdrstream.hdrstream_config_timer,
                   HDRSTREAM_TASK_PTR,
                   HDRMC_STREAM_CONFIG_RSP_TIMER_SIG );
  }

  /* Get current configuration parameters from SCMDB */
  hdrscmdb_get_inuse_config(HDRHAI_STREAM_PROTOCOL, &hdrstream.current);

  /* Because this module references its in_config instance even while
   * SCP is in OPEN state, we need to initialize the in_config to match
   * in_use.
   */
#ifdef FEATURE_MEMCPY_REMOVAL
  hdrstream.negotiated = hdrstream.current;
#else
  memcpy( &hdrstream.negotiated, &hdrstream.current,
          sizeof(hdrstream_config_params_type));
#endif /* FEATURE_MEMCPY_REMOVAL */

  hdrscp_get_app_subtype_config(&hdrstream.custom_app_config.app_config_mask);

  HDR_MSG_PROT_4(MSG_LEGACY_HIGH, 
                 "Stream app subtype MFPA=%d EMPA=%d CSNA=%d MMPA= %d",
                 hdrstream.custom_app_config.app_config_mask.enable_mfpa,
                 hdrstream.custom_app_config.app_config_mask.enable_empa,
                 hdrstream.custom_app_config.app_config_mask.enable_csna,
                 hdrstream.custom_app_config.app_config_mask.enable_mmpa);

} /* hdrstream_powerup_init */

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRSTREAM_CONFIGURE

DESCRIPTION
  This function conditionally commands HDRSTREAM to configure.

DEPENDENCIES
  None

PARAMETERS
  sender - The protocol giving this command (only for software trace)

RETURN VALUE
  TRUE - the configure command is accepted
  FALSE - the configure command is rejected

SIDE EFFECTS
  None

===========================================================================*/

boolean hdrstream_configure
(
  hdrhai_protocol_name_enum_type sender 
)
{
  hdrstream_cmd_type hdrstream_cmd; /* Build command into local struct */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (!hdrstream_config_is_needed())
  {
    return FALSE;
  }

  HDR_MSG_PROT (MSG_LEGACY_MED, "HDRSTREAM_CONFIGURE_CMD received");

  /* Stop and clear timer */
  (void) rex_clr_timer( &hdrstream.hdrstream_config_timer );
  (void) rex_clr_sigs(HDRSTREAM_TASK_PTR, HDRMC_STREAM_CONFIG_RSP_TIMER_SIG );

  /* Fill in all members of the hdrstream_cmd_type for the Activate command */
  hdrstream_cmd.name = HDRSTREAM_CONFIGURE_CMD;
  hdrstream_cmd.sender = sender;

  if (rex_self() == HDRSTREAM_TASK_PTR)
  {
    /* call the processing function directly if in the same task */
    hdrstream_process_cmd( &hdrstream_cmd );
  }
  else
  {
    /* Currently no other task should ever issue this command,
     * so we should never get here.
     */
    ERR("Configure command not expected from other task",0,0,0);

    hdrmc_queue_cmd( HDRHAI_STREAM_PROTOCOL, &hdrstream_cmd, 
                     sizeof (hdrstream_cmd) );
  }
  return TRUE;
} /* hdrstream_configure */

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRSTREAM_FAIL_CUR_CONFIG_NEG

DESCRIPTION
  This function fails the current config negotiation

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
LOCAL void hdrstream_fail_cur_config_neg
(
  hdrhai_session_close_reason_enum_type close_reason,
  uint8 attrib_id
)
{
  hdrind_ind_data_union_type ind_data;
  /* Allocate out of the heap because it is possible for this indication to
  be queued from a different task context. Otherwise, the payload may be 
  corrupted which could cause a null pointer de-reference. */ 
  static hdrscp_scp_fail_ind_type session_fail_data;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* fill the indication data */
  session_fail_data.type = HDRHAI_STREAM_PROTOCOL;
  session_fail_data.subtype = hdrscp_get_config_subtype(HDRHAI_STREAM_PROTOCOL) ;
  session_fail_data.attribute_id_length = HDRSTREAM_ATTRIBUTE_ID_LENGTH;
  session_fail_data.attribute_id = attrib_id;
  session_fail_data.reason = close_reason;

  ind_data.scp_fail_ptr = (void *)&session_fail_data;
  hdrind_give_ind(HDRIND_STREAM_PROT_NEG_FAILED,(void *)&ind_data);

} /* hdrstream_fail_cur_config_neg() */

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRSTREAM_PROCESS_CMD

DESCRIPTION
  This function processes commands given to STREAM.

DEPENDENCIES
  None

PARAMETERS
  stream_cmd_ptr - Pointer to the command for STREAM to process

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void hdrstream_process_cmd
(
  hdrstream_cmd_type * stream_cmd_ptr 
)
{
   HDRTRACE_CMD( HDRHAI_STREAM_PROTOCOL, stream_cmd_ptr->name, 
    stream_cmd_ptr->sender, 0 );
#ifdef FEATURE_HDRMC_DEBUG
  #error code not present
#endif

  switch ( stream_cmd_ptr->name )
  {
    case HDRSTREAM_CONFIGURE_CMD:  /* Configure Command */
      HDR_MSG_PROT (MSG_LEGACY_LOW, "Process Configure Command");
      hdrstream_process_configure( );
      break;

    default:
      ERR("Unrecognized command %d",stream_cmd_ptr->name,0,0);
      break;
  }
#ifdef FEATURE_HDRMC_DEBUG
  #error code not present
#endif
} /* hdrstream_process_cmd */

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRSTREAM_PROCESS_IND

DESCRIPTION
  This function processes indications given to STREAM.

DEPENDENCIES
  None

PARAMETERS
  ind_name     - Name of the indication for STREAM to process
  ind_data_ptr - Any data associated with the indication
  
RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void hdrstream_process_ind
(
  hdrind_ind_name_enum_type ind_name, 
  hdrind_ind_data_union_type * ind_data_ptr 
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

   HDRTRACE_IND( HDRHAI_STREAM_PROTOCOL, ind_name, 
    0 );
#ifdef FEATURE_HDRMC_DEBUG
  #error code not present
#endif
  switch ( ind_name )
  {
    case HDRIND_SCM_SESSION_CHANGED:
      HDR_MSG_PROT (MSG_LEGACY_LOW, "SCP session changed received");
      hdrstream_process_session_changed();
      break;

    default:
      ERR ("Unrecognized Indication %d", ind_name, 0, 0);
      break;
  }
#ifdef FEATURE_HDRMC_DEBUG
  #error code not present
#endif
} /* hdrstream_process_ind */ //lint !e715 ind_data_ptr not used

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRSTREAM_PROCESS_MSG

DESCRIPTION
  This function processes messages for STREAM.

DEPENDENCIES
  None

PARAMETERS
  item_ptr - Pointer to the DSM item chain holding the message
  protocol_instance - Instance of the protocol the message is for

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void hdrstream_process_msg
( 
  dsm_item_type * item_ptr, 
  hdrhai_protocol_instance_enum_type protocol_instance
)
{
  uint8 msg_id;           /* Message identifier */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ASSERT((protocol_instance == HDRHAI_IN_USE) ||
         (protocol_instance == HDRHAI_IN_CONFIG)); 

  /* MessageID is the first uint8 */
  if (HDRSTREAM_UNPACK_MSG_ID(item_ptr, &(msg_id)) != E_SUCCESS)
  {
    ERR( "Could not extract message_id", 0, 0, 0 );
    dsm_free_packet( &item_ptr );
    return;
  }

  /* Currently only the in config instance receives any messages */
  if (protocol_instance != HDRHAI_IN_CONFIG)
  {
    ERR("Msg for wrong instance %d", msg_id, 0, 0 );
    dsm_free_packet(&item_ptr);
    return;
  }

  HDRTRACE_MSG( HDRHAI_STREAM_PROTOCOL, msg_id, 0 );
#ifdef FEATURE_HDRMC_DEBUG
  #error code not present
#endif
  switch ( msg_id )
  {
    case HDRSTREAM_CONFIG_REQ_MSG:
      HDR_MSG_PROT (MSG_LEGACY_LOW, "Config Request Msg received");
      hdrstream_process_config_req_msg(item_ptr);
      break;

    case HDRSTREAM_CONFIG_RSP_MSG:
      HDR_MSG_PROT (MSG_LEGACY_LOW, "Config Response Msg received");
      hdrstream_process_config_rsp_msg(item_ptr);
      break;

    default:
      ERR ("HDRSTREAM unknown message %d", msg_id, 0, 0);
      break;
  }
  dsm_free_packet( &item_ptr );
#ifdef FEATURE_HDRMC_DEBUG
  #error code not present
#endif
} /* hdrstream_process_msg */

/* <EJECT> */ 
/*=============================================================================
FUNCTION HDRSTREAM_PROCESS_SIG

DESCRIPTION
  This function processes signals for STREAM.
  
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
=============================================================================*/
void hdrstream_process_sig
( 
  rex_sigs_type sig 
)
{
#ifdef FEATURE_HDRMC_DEBUG
  #error code not present
#endif

  /*---------------------------------------------------------------------------
  Determine which signal has been received.
  ---------------------------------------------------------------------------*/
  switch ( sig )
  {
    case HDRMC_STREAM_CONFIG_RSP_TIMER_SIG:
      /*-----------------------------------------------------------------------
      Configuration Response timer expired: 
      -----------------------------------------------------------------------*/
      HDR_MSG_PROT (MSG_LEGACY_HIGH, 
                    "sig:HDRMC_STREAM_CONFIG_RSP_TIMER_SIG for HDRSTREAM");
      hdrstream_fail_cur_config_neg(HDRHAI_CLOSE_CFG_RSP_EXP, 
                                    HDRSTREAM_STREAM_CONFIG_ATTRIB_ID);      
      break;

    default:
      /*-----------------------------------------------------------------------
      Default: signal not recognized.
      -----------------------------------------------------------------------*/
      ERR("Invalid overhead protocol signal!", 0, 0, 0);
      break;
  }
#ifdef FEATURE_HDRMC_DEBUG
  #error code not present
#endif
} /* hdrstream_process_sig() */

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRSTREAM_SET_CONFIG_TO_DEFAULT

DESCRIPTION
  This function initializes STREAM configuration parameters to default. 

DEPENDENCIES
  None

PARAMETERS
  current_cfg - Pointer to STREAM protocol's current configuration 
                structure in SCMDB.
                   
RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void hdrstream_set_config_to_default
( 
  hdrstream_config_params_type *current_cfg 
)
{
  /* The current configuration is now back to being the defaults */
  current_cfg->stream_config.stream_application[0] = HDRSTREAM_DEFAULT_STREAM0;
  current_cfg->stream_config.stream_application[1] = HDRSTREAM_DEFAULT_STREAM1;
  current_cfg->stream_config.stream_application[2] = HDRSTREAM_DEFAULT_STREAM2;
  current_cfg->stream_config.stream_application[3] = HDRSTREAM_DEFAULT_STREAM3;
} /* hdrstream_set_config_to_default() */

/*===========================================================================

FUNCTION HDRSTREAM_GET_INCONFIG_HANDLE

DESCRIPTION
  This function returns a pointer to inconfig instance of STREAM protocol

DEPENDENCIES
  None

PARAMETERS
  None
                     
RETURN VALUE
  Pointer to inconfig instance of STREAM protocol

SIDE EFFECTS
  None

===========================================================================*/
void* hdrstream_get_inconfig_handle(void)
{
  return (void*) &hdrstream.negotiated;
} /* hdrstream_get_inconfig_handle() */

/*===========================================================================

FUNCTION HDRSTREAM_GET_NEGOTIATED_SNPKTAPP

DESCRIPTION
  This function returns the negotiated packet application bound to SN if any.
  otherwise returns HDRHAI_APP_STREAM_NOT_USED.

DEPENDENCIES
  None

PARAMETERS
  None
                     
RETURN VALUE
  Returns negotiated packet application bound to SN or 
  HDRHAI_APP_STREAM_NOT_USED.

SIDE EFFECTS
  None

===========================================================================*/
hdrhai_app_subtype_enum_type hdrstream_get_negotiated_snpktapp(void)
{
  uint8 i;
    /* Loop index */
  hdrhai_app_subtype_enum_type app_type = HDRHAI_APP_STREAM_NOT_USED;
    /* App subtype bound to SN negotiated in the given stream config */
/*---------------------------------------------------------------------------*/

  for (i = 1; i < HDRSTREAM_NUM_STREAMS; i++)
  {
    app_type = (hdrhai_app_subtype_enum_type) 
        hdrstream.negotiated.stream_config.stream_application[i];

    if ((app_type == HDRHAI_DEF_PACKET_APP_SN) ||
        (app_type == HDRHAI_MULTIFLOW_PACKET_APP_SN) ||
        (app_type == HDRHAI_ENHANCED_MULTIFLOW_PACKET_APP_SN) ||
        (app_type == HDRHAI_MULTILINK_MULTIFLOW_PACKET_APP_SN))
    {
      /* Found the packet application bound to SN */
      break;
    }
  }

  return app_type;
}
