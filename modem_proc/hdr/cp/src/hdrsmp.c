/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

            S E S SI O N   M A N A G E M E N T   P R O T O C O L

GENERAL DESCRIPTION
  The Session Management Protocol (SMP) is the main state machine
  controlling the session layer of the HDR protocol stack.

EXTERNALIZED FUNCTIONS (Global)

  hdrsmp_msg_cb()        - Callback registered with HMP to deliver
                            messages to SMP
  hdrsmp_ind_cb()        - Callback registered for delivery of 
                            indications
  hdrsmp_activate()      - Command function to activate SMP
  hdrsmp_deactivate()    - Command function to deactivate SMP
  hdrsmp_powerup_init()  - Powerup Initialization command
  hdrsmp_session_is_open - Boolean accessor function to determine if the
                            session is open
  hdrsmp_session_is_closing - Boolean accessor function to determine if
                            the session is in the process of closing.

EXTERNALIZED FUNCTIONS (Regional)
  hdrsmp_process_cmd()     - Function called by hdrmc task to process
                              a command for SMP
  hdrsmp_process_ind()     - Function called by hdrmc task to process
                              an indication for SMP
  hdrsmp_process_msg()     - Function called by hdrmc task to process
                              a message for SMP
  hdrsmp_process_sig()     - Function called by hdrmc task to process
                              a signal for SMP

INITIALIZATION AND SEQUENCING REQUIREMENTS
  The Session Management Protocol must be initialized before any other
  1xEV-DO Protocols which have session-related data.

Copyright (c) 2000 - 2015 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* <EJECT> */ 
/*===========================================================================

                        EDIT HISTORY FOR MODULE

$PVCSPath: L:/src/asw/COMMON/vcs/hdrsmp.c_v   1.57   09 Apr 2003 09:26:38   maldinge  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/hdr/cp/src/hdrsmp.c#1 $ $DateTime: 2016/12/13 07:58:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
12/14/15   vko     Fixed KW issues 
10/15/14   vko     Fixed race condition while session close
07/09/14   cnx     Support NV refresh without reset.
03/14/14   vko/dsp FR19338:Reduce call setup time by skipping RF TX shutdown.
10/29/13   sat     Reduced F3 messages (or/and) ASSERTs
08/02/13   vko     Fixed SMP stuck in CLOSING state
05/06/13   cnx     Added hdrsmp_session_is_valid.
03/19/13   cnx     Send SessionClose when both NAI and eHRPD capability change
09/20/12   cnx     Save session to EFS when powerdown in tunnel.
09/11/12   cnx     Handle TX and RX messages during active handoff
12/02/11   cnx     Check gmmcdp rf setting when phone acquires HDR for the first time.
10/12/11   cnx     Moved closing non-irat session to HMP.
09/24/11   cnx     Support KeepAlive in tunnel mode.
07/27/11   cnx     Merged Optimized Handoff changes.
06/03/11   cnx     Renamed hdrsmp_process_ehrpd_credentials_changed().
05/31/11   cnx     Added hdrsmp_get_close_reason().
12/02/10   pba     Do not reset maintain EVDO session flag on session close 
                   for IRAT measurement feature.
06/10/10   pba     IRAT Measurement feature.
02/23/10   pba     Silently close the session when OTASP procedure for eHRPD 
                   credentials update happens.
01/05/10   pba     Replaced DS API to avoid unnecessary RPC.
10/30/09   pba     Added ehrpd support.
03/23/09   wsh     CMI: featurized CMI under FEATURE_CMI
02/27/09   wsh     Added run-time check for KDDI feature flags
02/18/09   wsh     Updated file to include private header hdrmc_v.h
12/10/08   smd     Replaced assert.h with amssassert.h
12/04/08   pba     Lint and Compiler warning fixes
11/21/08   pba     Added events for SLPQH feature.
08/29/08   wsh     Fixed double UATIReq when AT powers up w/ KA expired
10/02/08   sju     Updated KeepAlive feature based on review comments.
08/13/08   sju     Added support for SMP KeepAlive enhancement.
06/08/08   wsh     Fixed DCM can't turn on when in ConSetup
05/23/08   wsh     Fixed AT not able to open session if previous session 
                   negotiation is interrupted (DCM).
05/22/08   pba     Klockwork fixes.
05/17/08   wsh     Fixed not sending CM confirmation if in INACTIVE state
04/17/08   wsh     DCM optimization: not closing session unnecessarily when
                   moving across subnet
04/03/08   wsh     Merged fix for waiting UATIAssign onto 2H07
04/02/08   wsh     DCM related chagnes
08/15/07   etv     Added support for AT-initiated PHY config.
06/20/07   cc      Added support for AT-initiated FTC MAC config.
06/14/07   kss     Added support for AT-initiated AC MAC config.
05/29/07   mpa     Added hdrsmp_process_amp_failed_new_nai().
05/16/07   yll     Removed DS707 RPC calls from F3 message.
05/11/07   pba     When session changed indication is received check if the 
                   session is open and set the SMPKA timer.
03/27/07   wsh     Added support for new prior session handling
01/29/07   hal     Corrected CloseReason reporting in SessionClose message
                   Added macros for packing CloseReason 0x04 if necessary
01/08/07   yll     Added support for CSNA.
10/04/06   yll     Added several external functions
05/10/06   yll     Fixed a compiler warning.
05/09/06   yll     Added support for Minimal UATI Mode.
04/25/06   yll     Set the session close reason properly when Keep Alive 
                   Timer expired.
04/21/06   hal     Added session close reason information
04/07/06   hal     Changed hdrutil_new_buffer() to use hdrutil_new_msg_buffer()
04/06/06   mpa     Set KA timer on session open after connection close.
03/22/06   pba     Send Session close if AT could not accept AN's AUR message.
01/31/06   etv     Added support for AT-init Neg. for RMAC3.
12/13/05   pba     Changes to support multiple personality negotiation
12/06/05   etv     Added Generic MMCD protocol's Neg failed ind processing.
10/28/05   mpa     Use ALMP_CONNECTION_CLOSED ind instead of 
                   CONNECTED_CONNECTION_CLOSED to determine if SMP can close.
10/26/05   dna     Continue using HDRIND_RTCMAC_SHUTDOWN_COMPLETE, for now.
10/22/05   dna     SMP should close based on CONNECTED_CONNECTION_CLOSED ind,
                   not RMAC_SHUTDOWN_COMPLETE.  Also, ignore session_close msg
                   if we are already closing the session.
10/04/05   pba     Added support to read/write config parameters from SCMDB/EFS
08/22/05   etv     Calling SCM DB function to process Session close 
04/04/05   pba     Removed Lint errors
03/24/05   pba     Lint cleanup (More)
03/09/05   pba     Lint cleanup
01/17/05   mpa     Added hdrsmp_session_is_closing() accessor function.
12/07/04   hal     Changed dsm_offset_new_buffer to hdrutil_new_buffer.
10/19/04   mpa     Moved session_is_open check from hdrscp. 
09/09/04   mpa     Added new generic HDR_PROT_NEG_FAILED ind.
08/26/04   mpa     Added hdrsmp_process_amp_failed_new_esn().
02/25/04   mpa     New silent_close parameter in hdrsmp_deactivate() allows
                   silent or explicit internal session closure.
09/10/03   mpa     Converted F3 messages to use new MSG2.0 HDRPROT SSID
06/26/03   mpa     Added new session config failure reason
06/19/03   mpa     Don't try closing the session again on a session cfg failure
                   if we are already pending a session close
06/09/03   mpa     Added HDRTRACE_HDRSMP_SESSION_CLOSED event
06/06/03   mpa     Set connection closing flag to FALSE if we finish closing
                   the session
04/01/03   mpa     Don't process SC complete cmd if we are already inactive
03/12/03   mpa     Fixed attributeID packing in sessionClose msg
                   Updated all session close reasons
                   Centralized processing of session config failure indications
                   (added process_session_config_fail_related_ind )
02/12/03   mpa     Don't restart KA timer when rxing directed pkt in connected
01/30/03   mpa     Added explicit connection closing in session close procedure
                   Added FEATURE_HDR_KA_REQUEST_DISABLED
09/12/02   mpa     KA_OUTCOME and SC_COMPLETE cmds should always be queued.
09/11/02   mpa     Do not send SessionClose msg when processing AMP.failed inds
08/27/02   mpa     Allow Keep Alive Rsp to be sent on AC or RTC
06/07/02   dna     Change close reason to "SessionLost" for reacq on new subnet
04/15/02   mpa     Removed time_source param from process_keep_alive_timer_exp.
                   Modified hdrsmp_first_acq_init
04/11/02   dna     Removed "ERR" that can happen under normal conditions
03/11/02   nmw     Added check on ProtNegFailed indications to ignore when
                   SCP's session is closed (session in process of closing).
02/26/02   mpa     Added check for disabled ka timer in hdrsmp_first_acq_init
                   to prevent assertion error in hdrsmp_process_ka_timer_exp.
01/21/02   mpa     Added time_source param to process_keep_alive_timer_exp fn.
                   Added hdrsmp_first_acq_init.  Redesigned powerup_init.
01/17/02   dna     Remove hdrsmp_init() because it was never called.
                   Add initialization on first acquisition after powerup.
11/30/01   om      Added security layer support
09/11/01   dna     Initialize is_accepted to false for config req processing
09/05/01   dna     Added separate handling for in-use vs. in_config messages.
09/01/01   dna     Make config request/response handling identical to others.
08/02/01   nmn     Updated config req msg processing
07/02/01   nmn     fixed config req unpacking
06/06/01   dna     new param for hmp_send_msg()
03/30/01   nmn     added support for LUP
11/21/00   nmn     Initial revision

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

#include "hdrutil.h"
#include "hdrind.h"
#include "hdrhai.h"
#include "hdrlup.h"
#include "hdrstream.h"

#ifdef FEATURE_CMI
#include "hdrmc_v.h"
#endif

#include "hdrmci.h"
#include "hdrsmp.h"
#include "hdrsmpi.h"
#include "hdrsmps.h"
#include "hdrsmpka.h"
#include "hdramp.h"
#include "hdrscp.h"
#include "hdrscps.h"
#include "hdrerrno.h"
#include "hdrhmp.h"
#include "hdridle.h"
#include "hdrkep.h"
#include "hdrscmgmmcdp.h"
#include "hdrtrace.h"
#include "qw.h"
#include "ds707_extif.h"
#ifdef FEATURE_HDRMC_DEBUG
#error code not present
#endif

#include "hdrscmdb.h"

#ifdef FEATURE_LTE_TO_HDR_OH
#include "hdrsap.h"
#include "hdrsaps.h"
#endif /* FEATURE_LTE_TO_HDR_OH */

#if defined (FEATURE_DEDICATED_C2K_MODE) || defined (FEATURE_IRAT_PILOT_MEAS)
#include "cm.h"

#ifdef FEATURE_CMI
#include "cm_v.h"
#endif

#include "cmxll.h"
#include "hdramac.h"
#endif /* FEATURE_DEDICATED_C2K_MODE || FEATURE_IRAT_PILOT_MEAS */

#ifdef FEATURE_HDR_SLP_WAIT_FOR_UATI_ASSIGNMENT
extern void hdramp_done_waiting_for_uati_assignment
( 
  hdramp_clear_slp_holdoff_reason_type reason
);
#endif /* FEATURE_HDR_SLP_WAIT_FOR_UATI_ASSIGNMENT */
/* <EJECT> */ 
/*===========================================================================

            DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

===========================================================================*/
/*===========================================================================

             DEFINITIONS  FROM  THE  HDR  AIR  INTERFACE  SPEC

===========================================================================*/

#define HDRSMP_SESSION_CLOSE_MSG   0x01 /* Session Close Message ID */
#define HDRSMP_KEEP_ALIVE_REQ_MSG  0x02 /* Keep Alive Request Message ID */
#define HDRSMP_KEEP_ALIVE_RSP_MSG  0x03 /* Keep Alive Response Message ID */
#define HDRSMP_CONFIG_REQ_MSG      0x50 /* Configuration Request Message ID */
#define HDRSMP_CONFIG_RSP_MSG      0x51 /* Configuration Response Message ID */

#define HDRSMP_MESSAGE_PRIORITY    40     /* Priority for all messages */

typedef enum
{
  HDRSMP_CLOSE_NORMAL              = 0, /* Normal Close */
  HDRSMP_CLOSE_REPLY               = 1, /* Reply to Close */
  HDRSMP_CLOSE_ERROR               = 2, /* Protocol Error */
  HDRSMP_CLOSE_PROT_CON_FAIL       = 3, /* Protocol Configuration Failure */
  HDRSMP_CLOSE_PROT_NEG_FAIL       = 4, /* Protocol Negotiation Failure */
  HDRSMP_CLOSE_SESSION_CON_FAIL    = 5, /* Session Configuration Failure */
  HDRSMP_CLOSE_SESSION_LOST        = 6, /* Session Lost */
  HDRSMP_CLOSE_SESSION_UNREACHABLE = 7, /* Session Unreachable */
  HDRSMP_CLOSE_RESOURCES_BUSY      = 8  /* All Session resources busy */
} hdrsmp_close_reason_enum_type;

/* Default Keep Alive Attribute Value */
#define HDRSMP_DEFAULT_KEEP_ALIVE_EXPIRATION_MINS 0x0CA8UL

#define HDRSMP_CON_FAIL_MORE_INFO_LEN 3

/* Maintain EVDO session timer value: 180 seconds.
 * It's based on the estimate of two times of a typical max access probe
 * failure. An access attempt duration is about 30 seconds. AT typically
 * tries 3 attempts before declaring a max access probe failure, totaling
 * 90 seconds. And to leave some cushion, we double that to 180 seconds.*/
#define HDRSMP_MAINTAIN_EVDO_SESSION_TIMER_MS  180000UL

/* Define the timer enum here, to be used in the timer callback */
typedef enum
{
  /* Maintain EVDO session timer */
  HDRSMP_MAINTAIN_EVDO_SESSION_TIMER

} hdrsmp_timer_id_enum_type;

/* <EJECT> */ 
/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/

#define HDRSMP_BITS_PER_BYTE 8
#define HDRSMP_KEEP_ALIVE_ATTRIBUTE_ID 0xFFU

#define HDRSMP_MSG_ID_SIZE              8
#define HDRSMP_TRANSACTION_ID_SIZE      8
#define HDRSMP_ATTRIB_LEN_SIZE          8
#define HDRSMP_ATTRIB_ID_SIZE           8
#define HDRSMP_KA_ATTRIB_VAL_SIZE       16

/* The smallest attribute is a length with an AttributeID */
#define HDRSMP_MIN_ATTRIB_LEN \
        ( HDRSMP_ATTRIB_LEN_SIZE + \
        HDRSMP_ATTRIB_ID_SIZE )

/* The minimum attribute has Length, attribute ID and one value */
#define HDRSMP_KA_MIN_LEN \
        ( HDRSMP_ATTRIB_LEN_SIZE + \
        HDRSMP_ATTRIB_ID_SIZE + \
        HDRSMP_KA_ATTRIB_VAL_SIZE )

/*-- Constant sizes of fields of Configuration Response Message --*/
#define HDRSMP_CRSP_ATTRIB_VAL_SIZE 16
#define HDRSMP_CRSP_ATTRIB_LEN     ( HDRSMP_ATTRIB_LEN_SIZE + \
                                    HDRSMP_ATTRIB_ID_SIZE + \
                                    HDRSMP_CRSP_ATTRIB_VAL_SIZE )

/* The field in the config response is length in bytes,
 * excluding the length of length field.
 */
#define HDRSMP_CRSP_ATTRIB_LEN_BYTES  ((HDRSMP_ATTRIB_ID_SIZE + \
          HDRSMP_CRSP_ATTRIB_VAL_SIZE) / HDRSMP_BITS_PER_BYTE )

/*-- Protocol negotiation failed data structure */
typedef struct
{
  uint8 type;
  uint16 subtype;
  uint32 attribute_id;
  uint8 length;
} hdrsmp_prot_neg_fail_type;

/*-- Session Close Message structure --*/
typedef struct
{
  uint8 message_id;
  hdrsmp_close_reason_enum_type close_reason;
  uint8 more_info_len;
  uint8 type;
  uint16 subtype;
  uint8 attribute_id_length; /* in bytes */
  uint32 attribute_id;
} hdrsmp_session_close_msg_type;

/*-- Keep Alive Request and Response Message structure --*/
typedef struct
{
  uint8 message_id;
  uint8 transaction_id;
} hdrsmp_keep_alive_msg_type;

/*-- definitions for smp_data_type --*/

/* This structure contains all session attribute data. */
typedef struct
{
  hdrsmp_config_params_type current; /* currently used data */
  hdrsmp_config_params_type negotiated; /* negotiated session data */
} hdrsmp_session_params_type;

/* This structure contains information related to a sent
   Keep Alive Request message */
typedef struct
{
  boolean sending_msg;         /* flag indicating whether this message is
                                  attempting to be sent. */
  uint8 trans_id;              /* sent transaction ID */
} hdrsmp_keep_alive_req_msg_info_type;

typedef struct
{
  hdrhai_session_close_reason_enum_type close_reason;
    /* Internal reason for closing the session */
  boolean connection_closing;
    /* connection must be closed before we can finish session closing */
#if defined (FEATURE_DEDICATED_C2K_MODE) || defined (FEATURE_IRAT_PILOT_MEAS)
  boolean cm_sess_close_pending;
#endif
} hdrsmp_session_close_info_type;

/* Maintain EVDO session is true if any of the followings are TRUE:
   - Minimal UATI Mode is off
   - The user has attempted an EVDO data call within the last 3 minutes
   - PPP is established with an active or dormant data session
*/
typedef struct
{
  boolean         mum_is_enabled;
  rex_timer_type  maintain_session_timer;
  #if defined (FEATURE_DEDICATED_C2K_MODE) || defined (FEATURE_IRAT_PILOT_MEAS)
  boolean         maintain_evdo_session;
  #endif
} hdrsmp_session_maintenance_type;

LOCAL struct
{
  /* global store of session close info */
  hdrsmp_session_close_info_type session_close;
  /* global storage of keep alive request info */
  hdrsmp_keep_alive_req_msg_info_type keep_alive_req_msg_info;
  /* global storage of all session parameter data */
  hdrsmp_session_params_type session_params;
  /* global storage session maintenance parameters */
  hdrsmp_session_maintenance_type session_maint;

  hdrsmp_dup_act_detect_enum_type dup_act_detect;

  #ifdef FEATURE_IRAT_PILOT_MEAS
  boolean meas_mode_is_on;
  #endif
  /* this flag is set if the session is going to be closed soon */
  boolean session_closing;
} hdrsmp;

/* <EJECT> */ 
/*===========================================================================

                   MESSAGE PACK / UNPACK MACROS

===========================================================================*/

/****************************************************************************
** All Pack routines take a pointer to a dsm_item and
** a variable of the type belonging to the appropriate message
****************************************************************************/

/*-- Session Close Message --*/
#define HDRSMP_PACK_SC_MSG_ID( msg_ptr, message_id) \
          (HDRBIT_PACK8((msg_ptr), message_id, 0, 8))
#define HDRSMP_PACK_SC_CLOSE_REASON( msg_ptr, close_reason) \
          (HDRBIT_PACK8((msg_ptr), (uint8)close_reason, 8, 8))
#define HDRSMP_PACK_SC_MORE_INFO_LEN( msg_ptr, more_info_len) \
          (HDRBIT_PACK8((msg_ptr), more_info_len, 16, 8))
#define HDRSMP_PACK_SC_TYPE( msg_ptr, type) \
          (HDRBIT_PACK8((msg_ptr), type, 24, 8))
#define HDRSMP_PACK_SC_SUBTYPE( msg_ptr, subtype) \
          (HDRBIT_PACK16((msg_ptr), subtype, 32, 16))
#define HDRSMP_PACK_SC_ATTRIBUTE_ID(msg_ptr, attribute_id, attribute_id_length) \
          (HDRBIT_PACK32((msg_ptr), attribute_id, 48, (uint16)(8 * attribute_id_length)))

/*-- Used when the Session Close message MoreInfo Reason is 0x04 --*/
#define HDRSMP_NUM_ATTRIB_RECORDS_SIZE    8
#define HDRSMP_NUM_ATTRIB_RECORDS_OFFSET 48

/* We only report 1 offending record */
#define HDRSMP_PACK_SC_NUM_ATTRIB_RECORDS( msg_ptr ) \
          (HDRBIT_PACK8( msg_ptr, 0x01, HDRSMP_NUM_ATTRIB_RECORDS_OFFSET, \
           HDRSMP_NUM_ATTRIB_RECORDS_SIZE))

#define HDRSMP_PACK_SC_OFFENDING_ATTRIB_RECORD( msg_ptr, attrib_data, size ) \
          (dsm_pushdown_tail((msg_ptr), (attrib_data), (size), \
           DSM_DS_SMALL_ITEM_POOL))

/*-- Configuration Request Message --*/
#define HDRSMP_PACK_CON_REQ_MSG_ID( msg_ptr, message_id) \
          (HDRBIT_PACK8((msg_ptr), message_id, 0, 8))
#define HDRSMP_PACK_CON_REQ_TRANS_ID( msg_ptr, transaction_id) \
          (HDRBIT_PACK8((msg_ptr), transaction_id, 8, 8))
#define HDRSMP_PACK_CON_REQ_LENGTH( msg_ptr, length, offset) \
          (HDRBIT_PACK8((msg_ptr), length, offset, 8))
#define HDRSMP_PACK_CON_REQ_ATTRIBUTE_ID( msg_ptr, attribute_id, aoffset) \
          (HDRBIT_PACK8((msg_ptr), attribute_id, ((aoffset) + 8), 8))
#define HDRSMP_PACK_CON_REQ_ATTRIBUTE_VALUE( msg_ptr, attribute_value, aoffset, voffset) \
          (HDRBIT_PACK16((msg_ptr), attribute_value, ((aoffset) + (voffset)), 16))

/*-- Configuration Response Message --*/
#define HDRSMP_PACK_CON_RSP_MSG_ID( msg_ptr, val) \
          (HDRBIT_PACK8((msg_ptr), (val), 0, 8))
#define HDRSMP_PACK_CON_RSP_TRANS_ID( msg_ptr, val) \
          (HDRBIT_PACK8((msg_ptr), (val), 8, 8))
#define HDRSMP_PACK_CON_RSP_LENGTH( msg_ptr, val, aoffset) \
          (HDRBIT_PACK8((msg_ptr), (val), ((aoffset) + 0), 8))
#define HDRSMP_PACK_CON_RSP_ATTRIBUTE_ID( msg_ptr, val, aoffset) \
          (HDRBIT_PACK8((msg_ptr), (val), ((aoffset) + 8), 8))
#define HDRSMP_PACK_CON_RSP_ATTRIBUTE_VALUE( msg_ptr, val, aoffset)\
          (HDRBIT_PACK16((msg_ptr), (val), ((aoffset) + 16), 16))

/*-- Keep Alive Request and Response Messages --*/
#define HDRSMP_PACK_KA_MSG_ID( msg_ptr, message_id) \
          (HDRBIT_PACK8((msg_ptr), message_id, 0, 8))
#define HDRSMP_PACK_KA_TRANS_ID( msg_ptr, transaction_id) \
          (HDRBIT_PACK8((msg_ptr), transaction_id, 8, 8))

/***************************************************************************
** Unpack routines.  All take a pointer to a DSM item and a reference to
** a variable of the appropriate type
***************************************************************************/
#define HDRSMP_UNPACK_MSG_ID( msg_ptr, msg_id) \
          (HDRBIT_UNPACK8((msg_ptr), (msg_id), 0, 8))
#define HDRSMP_UNPACK_TRANS_ID( msg_ptr, trans_id) \
          (HDRBIT_UNPACK8((msg_ptr), (trans_id), 8, 8))

/*-- Session Close Message --*/
#define HDRSMP_UNPACK_SC_CLOSE_REASON( msg_ptr, close_reason) \
          (HDRBIT_UNPACK8((msg_ptr), close_reason, 8, 8))
#define HDRSMP_UNPACK_SC_MORE_INFO_LEN( msg_ptr, more_info_len) \
          (HDRBIT_UNPACK8((msg_ptr), (more_info_len), 16, 8))
#define HDRSMP_UNPACK_SC_TYPE( msg_ptr, type) \
          (HDRBIT_UNPACK8((msg_ptr), (type), 24, 8))
#define HDRSMP_UNPACK_SC_SUBTYPE( msg_ptr, subtype) \
          (HDRBIT_UNPACK16((msg_ptr), (subtype), 32, 16))
#define HDRSMP_UNPACK_SC_ATTRIBUTE_ID( msg_ptr, attr_id) \
          (HDRBIT_UNPACK32((msg_ptr), (attr_id), 48, 8))

/*-- Configuration Request Message --*/
#define HDRSMP_UNPACK_CON_REQ_LEN( msg_ptr, val_ptr, aoffset) \
          (HDRBIT_UNPACK8((msg_ptr), (val_ptr), ((aoffset) + 0), 8))
#define HDRSMP_UNPACK_CON_REQ_ATTRIBUTE_ID( msg_ptr, val_ptr, aoffset) \
          (HDRBIT_UNPACK8((msg_ptr), (val_ptr), ((aoffset) + 8), 8))

/* For keep alive timeout value */
#define HDRSMP_UNPACK_CON_REQ_ATTRIBUTE_VALUE( msg_ptr, val_ptr, aoffset, voffset) \
          (HDRBIT_UNPACK16((msg_ptr), (val_ptr), ((aoffset) + (voffset) + 0), 16))

/*-- Configuration Response Message --*/
#define HDRSMP_UNPACK_CON_RSP_LEN( msg_ptr, length, aoffset) \
          (HDRBIT_UNPACK16((msg_ptr), (length), ((aoffset) + 0), 8))
#define HDRSMP_UNPACK_CON_RSP_ATTRIBUTE_ID( msg_ptr, attr_id, aoffset) \
          (HDRBIT_UNPACK8((msg_ptr), (attr_id), ((aoffset) + 8), 8))
#define HDRSMP_UNPACK_CON_RSP_ATTRIBUTE_VALUE( msg_ptr, attr_value, aoffset) \
          (HDRBIT_UNPACK16((msg_ptr), (attr_value), ((aoffset) + 16), 16))

/*-- headers for the outcome_cb functions used as pointer in hmp_send_msg --*/
LOCAL void hdrsmp_ka_tx_outcome_cb
(
  void * data_ptr,
  hdrerrno_enum_type err_val
);

LOCAL void hdrsmp_sc_tx_outcome_cb
(
  void * data_ptr,
  hdrerrno_enum_type err_val
);

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRSMP_SESSION_IS_INACTIVE

DESCRIPTION
  This boolean accessor function tells the caller whether the session is
  inactive or not.  It is intended to be used after initialization is completed.

DEPENDENCIES
  None.

PARAMETERS
  sender - The protocol giving this command (only for software trace)

RETURN VALUE
  TRUE - the session is inactive
  FALSE - the session is not inactive

SIDE EFFECTS
  None.

===========================================================================*/
boolean hdrsmp_session_is_inactive
(
  hdrhai_protocol_name_enum_type sender
)
{
  boolean ret_val; /* return value */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  ret_val = (hdrsmps_get_state() == HDRSMPS_INACTIVE);

  HDR_MSG_PROT_2(MSG_LEGACY_LOW, "Session is inactive %d (sender: 0x%x)", 
                 ret_val, sender);

  return ret_val;
} /* hdrsmp_session_is_inactive */


/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRSMP_SESSION_IS_OPEN

DESCRIPTION
  This boolean accessor function tells the caller whether the session is
  open or not.  It is intended to be used after initialization is completed.

DEPENDENCIES
  None.

PARAMETERS
  sender - The protocol giving this command (only for software trace)

RETURN VALUE
  TRUE - the session is open
  FALSE - the session is not open

SIDE EFFECTS
  None.

===========================================================================*/
boolean hdrsmp_session_is_open
(
  hdrhai_protocol_name_enum_type sender
)
{
  boolean ret_val; /* return value */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  ret_val = (hdrsmps_get_state() == HDRSMPS_OPEN);

  if (sender != HDRHAI_RADIO_LINK_PROTOCOL)
  {
    HDRTRACE_HDRSCP_SESSION_IS_OPEN(sender, ret_val);
  }

  return ret_val;
} /* hdrsmp_session_is_open */

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRSMP_SESSION_IS_VALID

DESCRIPTION
  This boolean accessor function tells the caller whether the session is
  valid or not.
 
  Normally hdrsmp_session_is_open is used to check whether session is open or
  not. Sometimes, this is not enough. E.g., in a scenario where AT enters
  HDR network. AT has HRPD session but the eHPRD is allowed. AT does not closes
  session until OVHD is updated. Before that, session is open but not valid
  since we will close the session for sure.

DEPENDENCIES
  None.

PARAMETERS
  sender - The protocol giving this command (only for software trace)

RETURN VALUE
  TRUE - the session is valid
  FALSE - the session is not valid

SIDE EFFECTS
  None.

===========================================================================*/
boolean hdrsmp_session_is_valid
(
  hdrhai_protocol_name_enum_type sender
)
{
  if ((hdrsmp.session_closing == FALSE) && /* if it is TRUE, then current session is going to be closed */
  	   hdrsmp_session_is_open(HDRHAI_SESSION_MANAGEMENT_PROTOCOL) &&
       hdrstream_stream_is_assigned(HDRHAI_APP_PACKET_APP_ANY) )
  {
#ifdef FEATURE_EHRPD
    if ( !hdrscp_ehrpd_status_is_valid() )
    {
      return FALSE;
    }
    else
#endif /* FEATURE_EHRPD */
    {
      return TRUE;
    }
  }
  else
  {
    return FALSE;
  }

} /* hdrsmp_session_is_valid */

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRSMP_SESSION_IS_CLOSING

DESCRIPTION
  This boolean accessor function tells the caller whether the session is
  in the process of closing or not.  
  
  It is intended to be used after initialization is completed.

DEPENDENCIES
  None.

PARAMETERS
  sender - The protocol giving this command (only for software trace)

RETURN VALUE
  TRUE - the session is closing
  FALSE - the session is not closing

SIDE EFFECTS
  None.

===========================================================================*/
boolean hdrsmp_session_is_closing
(
  hdrhai_protocol_name_enum_type sender
)
{
  boolean ret_val; /* return value */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  ret_val = (hdrsmps_get_state() == HDRSMPS_CLOSING);

  HDR_MSG_PROT_2(MSG_LEGACY_LOW,"Session is closing %d (sender: 0x%x)",
                 ret_val, sender);

  return ret_val;
} /* hdrsmp_session_is_closing */


#if defined (FEATURE_DEDICATED_C2K_MODE) || defined (FEATURE_IRAT_PILOT_MEAS)
/* <EJECT> */
/*===========================================================================

FUNCTION HDRSMP_SEND_RPT_TO_CM

DESCRIPTION
  This function sends a report to the CM task.

DEPENDENCIES
  None.

PARAMETERS

cmd_ptr - pointer to command to send to cm

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

LOCAL void hdrsmp_send_rpt_to_cm
(
  cm_mc_rpt_type *cmd_ptr
    /* pointer to command to send to cm */
)
{
  cmd_ptr->hdr.task_ptr = NULL;

  HDR_MSG_PROT_1( MSG_LEGACY_LOW, "Send HDRSMP rpt %d",cmd_ptr->hdr.cmd );
  cm_mc_rpt(cmd_ptr);

} /* hdrsmp_send_rpt_to_cm() */


/* <EJECT> */
/*===========================================================================

FUNCTION HDRSMP_SEND_SESSION_CLOSED_TO_CM

DESCRIPTION
  This function sends a DO session closed report to CM.

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
LOCAL void hdrsmp_send_session_closed_to_cm( void )
{

  cm_mc_rpt_type *cmd_ptr;    /* Pointer to mc buffer */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  #ifdef FEATURE_DEDICATED_C2K_MODE
  /* Mark that we no longer need to maintain the EVDO session */
  hdrsmp.session_maint.maintain_evdo_session = FALSE;
  #endif

  #ifdef FEATURE_IRAT_PILOT_MEAS
  if (hdrsmp.meas_mode_is_on)
  {
    hdrsmp.session_maint.maintain_evdo_session = FALSE;
  }
  #endif

  cmd_ptr = cm_mc_rpt_get_buf_else_err_fatal();

  cmd_ptr->hdr.cmd = CM_HDR_SESSION_CLOSED;

  HDR_MSG_PROT (MSG_LEGACY_MED,  "DCM: Sent CM_HDR_SESSION_CLOSED");
  hdrsmp_send_rpt_to_cm( cmd_ptr );

}/* hdrsmp_send_session_closed_to_cm() */



/* <EJECT> */
/*===========================================================================

FUNCTION HDRSMP_SEND_SESSION_OPEN_FAILED_TO_CM

DESCRIPTION
  This function sends a DO session open failed report to CM.

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void hdrsmp_send_session_open_failed_to_cm( void )
{

  cm_mc_rpt_type *cmd_ptr;    /* Pointer to mc buffer */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Mark that we no longer need to maintain the EVDO session */
  hdrsmp.session_maint.maintain_evdo_session = FALSE;

  cmd_ptr = cm_mc_rpt_get_buf_else_err_fatal();

  cmd_ptr->hdr.cmd = CM_HDR_SESSION_OPEN_FAILED;

  HDR_MSG_PROT (MSG_LEGACY_MED,  "DCM: Send CM_HDR_SESSION_OPEN_FAILED");
  hdrsmp_send_rpt_to_cm( cmd_ptr );

}/* hdrsmp_send_session_open_failed_to_cm() */

#endif /* FEATURE_DEDICATED_C2K_MODE || FEATURE_IRAT_PILOT_MEAS */

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRSMP_SEND_KEEP_ALIVE_REQUEST_MSG

DESCRIPTION
  This helper function sends the Keep Alive Request message.

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
LOCAL void hdrsmp_send_keep_alive_request_msg ( void )
{
  dsm_item_type * send_msg_ptr; /* DSM item pointer to store packed message */
  /* structure holding the data for the Keep Alive Request Msg */
  hdrsmp_keep_alive_msg_type keep_alive_msg;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  hdrsmp.keep_alive_req_msg_info.sending_msg = TRUE;

  /* if the system is acquired, send the Keep Alive Request Message.  If
     not, it will be sent when the system is acquired.
    (see OVHD.InfoUpdated Indication processing) */
  if (hdralmp_system_is_acquired(HDRHAI_SESSION_MANAGEMENT_PROTOCOL) &&
      hdrovhd_info_is_updated(HDRHAI_SESSION_CONFIGURATION_PROTOCOL))
  {
    /*-- store data --*/
    keep_alive_msg.message_id = HDRSMP_KEEP_ALIVE_REQ_MSG;
    keep_alive_msg.transaction_id =
      ++hdrsmp.keep_alive_req_msg_info.trans_id;

    /*-- get new buffer --*/
    send_msg_ptr = hdrutil_new_msg_buffer();

    /*-- pack fields --*/
    HDRSMP_PACK_KA_MSG_ID(send_msg_ptr, keep_alive_msg.message_id);
    HDRSMP_PACK_KA_TRANS_ID(send_msg_ptr, keep_alive_msg.transaction_id);

    /*-- send keep alive request message --*/
    hdrhmp_send_msg( HDRHAI_SESSION_MANAGEMENT_PROTOCOL,
                     HDRHAI_AC,
                     FALSE,
                     HDRSMP_MESSAGE_PRIORITY,
                     HDRHAI_IN_USE, /* Instance of protocol sending message */
                     send_msg_ptr,
                     hdrsmp_ka_tx_outcome_cb,
                     NULL);
  }

} /* hdrsmp_send_keep_alive_request_msg */

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRSMP_SEND_SESSION_CLOSE_MSG

DESCRIPTION
  This helper function sends the previously formatted session close message

DEPENDENCIES
  None.

PARAMETERS
  session_close_msg_ptr - pointer to a formatted session close message

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

LOCAL void hdrsmp_send_session_close_msg
(
  hdrsmp_session_close_msg_type * session_close_msg_ptr
)
{
  dsm_item_type * send_msg_ptr; /* dsm buffer for sent message */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-- get new buffer --*/
  send_msg_ptr = hdrutil_new_msg_buffer();

  /*-- pack common fields --*/
  HDRSMP_PACK_SC_MSG_ID(send_msg_ptr,session_close_msg_ptr->message_id);
  HDRSMP_PACK_SC_CLOSE_REASON(send_msg_ptr,session_close_msg_ptr->close_reason);
  HDRSMP_PACK_SC_MORE_INFO_LEN(send_msg_ptr,session_close_msg_ptr->more_info_len);

  /* different close reasons have different "more info" data */
  switch((*session_close_msg_ptr).close_reason)
  {
    case HDRSMP_CLOSE_NORMAL:
    case HDRSMP_CLOSE_REPLY:
    case HDRSMP_CLOSE_ERROR:
    case HDRSMP_CLOSE_SESSION_CON_FAIL:
    case HDRSMP_CLOSE_SESSION_LOST:
    case HDRSMP_CLOSE_SESSION_UNREACHABLE:
    case HDRSMP_CLOSE_RESOURCES_BUSY:
    /* no extra info here, ignore */
      break;

    case HDRSMP_CLOSE_PROT_CON_FAIL:
      /*-- pack type and subtype --*/
      HDRSMP_PACK_SC_TYPE(send_msg_ptr, session_close_msg_ptr->type);
      HDRSMP_PACK_SC_SUBTYPE(send_msg_ptr, session_close_msg_ptr->subtype);
      break;

    case HDRSMP_CLOSE_PROT_NEG_FAIL:
      /*-- pack type, subtype, and attribute ID --*/
      HDRSMP_PACK_SC_TYPE(send_msg_ptr, session_close_msg_ptr->type);
      HDRSMP_PACK_SC_SUBTYPE(send_msg_ptr, session_close_msg_ptr->subtype);
      HDRSMP_PACK_SC_ATTRIBUTE_ID(send_msg_ptr,
        session_close_msg_ptr->attribute_id,
        session_close_msg_ptr->attribute_id_length);
      break;

  } /* switch */

  /*-- Send Session Close Message --*/

  /* Transition to Closing state and send the Session Close message */
  hdrsmps_change_state( HDRSMPS_CLOSING );

  /* Session Close Message is sent on RTC if available, otherwise AC.  However,
     it is always sent Best Effort, regardless of the channel used.  */
  hdrhmp_send_msg(HDRHAI_SESSION_MANAGEMENT_PROTOCOL,
                  HDRHAI_AC_OR_RTC,
                  FALSE,
                  HDRSMP_MESSAGE_PRIORITY,
                   HDRHAI_IN_USE, /* Instance of protocol sending message */
                  send_msg_ptr,
                  hdrsmp_sc_tx_outcome_cb,
                  NULL);

} /* hdrsmp_send_session_close_msg */

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRSMP_SET_CONFIG_TO_DEFAULT

DESCRIPTION
  This function initializes SMP's configuration parameters to default.

DEPENDENCIES
  None.

PARAMETERS
  current_config - Pointer to current configuration structure of SMP in SCMDB.   

RETURN VALUE
  None.

SIDE EFFECTS
  None 

===========================================================================*/
void hdrsmp_set_config_to_default( hdrsmp_config_params_type *current_config )
{
  current_config->keep_alive_expiration = 
                  HDRSMPKA_DEFAULT_KEEP_ALIVE_EXPIRATION_MINS;             
} /* hdrsmp_set_config_to_default() */

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRSMP_PROCESS_SESSION_CHANGED

DESCRIPTION
  This function processes SCP.SessionChanged Indication.

DEPENDENCIES
  None.

PARAMETERS
   None

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
void hdrsmp_process_session_changed( void )
{
  /* Update the local copy of the current configuration from SCMDB */
  hdrscmdb_get_inuse_config(HDRHAI_SESSION_MANAGEMENT_PROTOCOL,
                            &hdrsmp.session_params.current);

  if (hdrsmps_get_state() == HDRSMPS_OPEN)
  {
    /* Since the keep alive functionality is being restarted, make sure we
       don't send a Keep Alive Request Message */
    hdrsmp.keep_alive_req_msg_info.sending_msg = FALSE;

    /*-- clear the keep alive timer if set and restart keep alive --*/
    (void)rex_clr_sigs(rex_self(), HDRMC_SMP_KA_TIMER_SIG);
    hdrsmpka_set_timer(hdrsmp.session_params.current.keep_alive_expiration);
  }

}/* hdrsmp_process_session_changed() */

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRSMP_CLOSE_SESSION

DESCRIPTION
  This helper function performs the session close actions (excluding
    sending the Session Close Message)

DEPENDENCIES
  None.

PARAMETERS
  close_reason - reason why SCP is being deacitvated

RETURN VALUE
  None.

SIDE EFFECTS
  Deactivates AMP and SCP, changes state to inactive

===========================================================================*/
LOCAL void hdrsmp_close_session
(
  hdrhai_session_close_reason_enum_type close_reason
)
{
  /* Make sure that flag is reset by now. */
  hdrsmp.session_close.connection_closing = FALSE;

  /*-- deactivate SCP and AMP individually --*/
  hdrscp_deactivate(close_reason, HDRHAI_SESSION_MANAGEMENT_PROTOCOL);
  hdramp_deactivate(HDRHAI_SESSION_MANAGEMENT_PROTOCOL);

  /* make sure Keep Alive Message won't be set */
  hdrsmp.keep_alive_req_msg_info.sending_msg = FALSE;

#if defined (FEATURE_DEDICATED_C2K_MODE) || defined (FEATURE_IRAT_PILOT_MEAS)
  /* Report to CM that session is closed */
  if (hdrsmp.session_close.cm_sess_close_pending)
  {
    hdrsmp.session_close.cm_sess_close_pending = FALSE;
    hdrsmp_send_session_closed_to_cm();
  }
#endif

  hdrsmps_change_state(HDRSMPS_INACTIVE);

} /* hdrsmp_close_session */

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRSMP_SEND_SESSION_CLOSE_IND

DESCRIPTION
  This helper function sends a session close indication

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void hdrsmp_send_session_close_ind( void )
{

  hdrind_ind_data_union_type ind_data;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  HDRTRACE_HDRSMP_SESSION_CLOSED(hdrsmp.session_close.close_reason);
  ind_data.close_reason = hdrsmp.session_close.close_reason;
  
  /* Update SCMDB for Session Close and give SessionChanged indication to all
     protocols to update it's local copy of configuration parameters */
  hdrscmdb_process_session_closed( hdrsmp.session_close.close_reason );

  /* Return SessionClosed Indication. Protocols that have to do some additional
     processing on Session close register for this Indication */ 
  hdrind_give_ind(HDRIND_SMP_SESSION_CLOSED, (void*)&ind_data);

} /* hdrsmp_send_session_close_ind() */

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRSMP_RESTART_KA_TIMER

DESCRIPTION
   This function restarts TSMPClose timer 

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  The Keep Alive Interval Timer will be restarted

===========================================================================*/
void hdrsmp_restart_ka_timer ( void )
{
  /* Restart KA timer is only related to the Keep Alive
     functionality, which is only valid in HDRSMPS_OPEN State and when we
     are not in connected state */
  if (hdrsmps_get_state() != HDRSMPS_OPEN ||
      hdralmp_connection_is_open(HDRHAI_SESSION_MANAGEMENT_PROTOCOL) )
  {
#ifndef FEATURE_HDR_RESET_KA_TIMER_ON_ACACK_ONLY
    HDRTRACE_IND_IGNORED_STATE( HDRHAI_SESSION_MANAGEMENT_PROTOCOL,
                        HDRIND_CCMAC_MOBILE_DIRECTED_PKT, hdrsmps_get_state());
#endif
    return;
  }

  /* if we receive a mobile directed packet, the link is intact.  The keep
     alive request message should not be resent.  The keep alive mechanism
     will be restarted. */
  hdrsmp.keep_alive_req_msg_info.sending_msg = FALSE;

  /*-- clear the keep alive timer if set and restart keep alive --*/
  (void)rex_clr_sigs(rex_self(), HDRMC_SMP_KA_TIMER_SIG);
  hdrsmpka_set_timer(hdrsmp.session_params.current.keep_alive_expiration);

} /* hdrsmp_restart_ka_timer */

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRSMP_PROCESS_AMP_FAILED_REQUEST_TIMEOUT

DESCRIPTION
  This function processes the AMP.Failed_Request_Timeout Indication

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  The session will end

===========================================================================*/
LOCAL void hdrsmp_process_amp_failed_request_timeout ( void )
{
  /* AMP.FailedRequestTimeout Indication is only valid in HDRSMPS_AMP_SETUP
     and HDRSMPS_OPEN states */
  if (hdrsmps_get_state() == HDRSMPS_INACTIVE)
  {
    HDRTRACE_IND_IGNORED_STATE(HDRHAI_SESSION_MANAGEMENT_PROTOCOL,
                      HDRIND_AMP_FAILED_REQUEST_TIMEOUT, hdrsmps_get_state());

    return;
  }

  /* set close reason here, it will be passed to SCP and HMP
     when the session is closed */
  hdrsmp.session_close.close_reason = HDRHAI_CLOSE_UATI_FAIL;

  /* Since this indication is processed as a result of the AT moving to a
     different subnet, the AN that would receive a SessionClose msg sent here
     may not be able to bind the ATI attached in the msg to the AT's session.
     Hence we close the session directly without sending the SessionClose msg.
  */
  hdrsmp_close_session(hdrsmp.session_close.close_reason);

} /* hdrsmp_process_amp_failed_request_timeout */

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRSMP_PROCESS_AMP_FAILED_NEW_NETWORK

DESCRIPTION
  This function processes the AMP.Failed_New_Network Indication

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  The session will end

===========================================================================*/
LOCAL void hdrsmp_process_amp_failed_new_network ( void )
{
  /* AMP.FailedNewNetwork Indication is only valid in HDRSMPS_OPEN State */
  if (hdrsmps_get_state() != HDRSMPS_OPEN)
  {
    HDRTRACE_IND_IGNORED_STATE(HDRHAI_SESSION_MANAGEMENT_PROTOCOL,
                         HDRIND_AMP_FAILED_NEW_NETWORK, hdrsmps_get_state());

    return;
  }

  /* set close reason here, it will be passed to SCP and HMP
     when the session is closed */
  if ( hdramp_is_session_close_nai_pending() )
  { 
    /* Don't do restore prior session if session is going to be closed
     * due to NAI update. */
    hdrsmp.session_close.close_reason = HDRHAI_CLOSE_NEW_NAI;
  }
  else
  {
    hdrsmp.session_close.close_reason = HDRHAI_CLOSE_NEW_NETWORK;
  }

  /* Since this indication is processed as a result of the AT moving to a
     different subnet, the AN that would receive a SessionClose msg sent here
     may not be able to bind the ATI attached in the msg to the AT's session.
     Hence we close the session directly without sending the SessionClose msg.
  */  
  hdrsmp_close_session(hdrsmp.session_close.close_reason);

  if ( hdramp_is_session_close_nai_pending() )
  { 
    hdramp_set_session_close_nai_pending(FALSE);
  }
} /* hdrsmp_process_amp_failed_new_network */


/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRSMP_PROCESS_AMP_FAILED_NOT_MAINTAINING_UATI

DESCRIPTION
  This function processes the AMP.Failed_Not_Maintaining_UATI Indication

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  The session will end

===========================================================================*/
LOCAL void hdrsmp_process_amp_failed_not_maint_uati ( void )
{
#ifdef FEATURE_HDR_MIN_UATI_MODE
  boolean close_is_silent = TRUE;
    /* By default not send Session Close message.*/
#endif

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

#ifdef FEATURE_HDR_MIN_UATI_MODE
  /* set close reason here, it will be passed to SCP and HMP
     when the session is closed */
  hdrsmp.session_close.close_reason = HDRHAI_CLOSE_NOT_MAINT_UATI;

  /* Based on the ECS (explicitly close session) feature,
   * determine whether to send SessionClose message or not. */
  #ifdef FEATURE_HDR_MIN_UATI_MODE_ECS
  close_is_silent = FALSE;
  #endif

  if ( close_is_silent )
  {
    HDR_MSG_PROT ( MSG_LEGACY_HIGH, 
                   " MUM: Silently close session, not send SessionClose msg");
  }
  else
  {
    HDR_MSG_PROT ( MSG_LEGACY_HIGH, 
                   " MUM: Explicitly close session, send SessionClose msg");
  }

  hdrsmp_deactivate ( HDRHAI_SESSION_MANAGEMENT_PROTOCOL, 
                      close_is_silent );
#else
  /* If feature is not defined, we should never get to this function */
  HDR_MSG_PROT ( MSG_LEGACY_ERROR, 
                 " MUM: feature undef, shouldn't recv not_maint_UATI ind!");
#endif /* FEATURE_HDR_MIN_UATI_MODE */

} /* hdrsmp_process_amp_failed_not_maint_uati */

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRSMP_PROCESS_AMP_FAILED_NEW_ESN

DESCRIPTION
  This function processes the AMP.Failed_New_ESN Indication

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  The session will end

===========================================================================*/
LOCAL void hdrsmp_process_amp_failed_new_esn ( void )
{
  /* AMP.FailedNewESN Indication is only valid in Setup and Open State.
   */
  if (hdrsmps_get_state() == HDRSMPS_INACTIVE)
  {
    HDRTRACE_IND_IGNORED_STATE(HDRHAI_SESSION_MANAGEMENT_PROTOCOL,
                         HDRIND_AMP_FAILED_NEW_ESN, hdrsmps_get_state());

    return;
  }

  /* set close reason here, it will be passed to SCP and HMP
     when the session is closed */
  hdrsmp.session_close.close_reason = HDRHAI_CLOSE_NEW_ESN;

  /* We close the session directly without sending the SessionClose msg.
  */
  hdrsmp_close_session(hdrsmp.session_close.close_reason);

} /* hdrsmp_process_amp_failed_new_esn() */

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRSMP_PROCESS_AMP_FAILED_NEW_NAI

DESCRIPTION
  This function processes the AMP.Failed_New_NAI Indication

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  The session will end

===========================================================================*/
LOCAL void hdrsmp_process_amp_failed_new_nai ( void )
{
  /* PAC.FailedNewNAI Indication is only valid in Setup and Open State.
   */
  if (hdrsmps_get_state() == HDRSMPS_INACTIVE)
  {
    HDRTRACE_IND_IGNORED_STATE(HDRHAI_SESSION_MANAGEMENT_PROTOCOL,
                         HDRIND_AMP_FAILED_NEW_NAI, hdrsmps_get_state());

    return;
  }

  /* set close reason here, it will be passed to SCP and HMP
     when the session is closed */
  hdrsmp.session_close.close_reason = HDRHAI_CLOSE_NEW_NAI;

  /* We close the session directly without sending the SessionClose msg.
  */
  hdrsmp_close_session(hdrsmp.session_close.close_reason);

} /* hdrsmp_process_amp_failed_new_nai() */

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRSMP_PROCESS_SESSION_CFG_FAIL_RELATED_IND

DESCRIPTION
  This function processes indications related to session configuration failures
  - SCP.SCP_Failed
  - IDLE.prot_neg_failed
  - STREAM.prot_neg_failed
  - KEP.prot_neg_failed

DEPENDENCIES
  None.

PARAMETERS
  ind_name - Session Fail related indication name
  ind_data_ptr - Pointer to data from the indication.  This will indicate
                 one of several possible causes of failure during negotiation.

RETURN VALUE
  None.

SIDE EFFECTS
  The session will end and a Session Close Message sent.

===========================================================================*/
LOCAL void hdrsmp_process_session_cfg_fail_related_ind
(
  hdrind_ind_name_enum_type  ind_name,
  hdrind_ind_data_union_type * ind_data_ptr
)
{
  /* structure holding the data for the Session Close Message */
  hdrsmp_session_close_msg_type session_close_msg;
  /* structure holding the data from the indication data */
  hdrscp_scp_fail_ind_type *scp_fail_data_ptr;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Session fail related Indications are only valid in HDRSMPS_OPEN State */
  if (hdrsmps_get_state() != HDRSMPS_OPEN ||
      (hdrsmp.session_close.connection_closing) )
  {
    HDRTRACE_IND_IGNORED_STATE(HDRHAI_SESSION_MANAGEMENT_PROTOCOL,
                               ind_name,hdrsmps_get_state());
    return;
  }

  /* Initialize local variable - session close message */
  memset ( &session_close_msg, 0, sizeof(hdrsmp_session_close_msg_type) ); 

  /* indication data must be pointing to a valid address */
  ASSERT(ind_data_ptr != NULL);

  /* extract the  data pointer from the indication data */
  scp_fail_data_ptr = (hdrscp_scp_fail_ind_type*)(ind_data_ptr->scp_fail_ptr);

  /* SCP.Failed data pointer must be pointing to a valid address */
  ASSERT(scp_fail_data_ptr != NULL);

  /* fill out common fields of session close message structure */
  session_close_msg.message_id = HDRSMP_SESSION_CLOSE_MSG;

  /*-- fill out reason-specific fields of session close message structure --*/
  switch(scp_fail_data_ptr->reason)
  {
    /*-- Session Failed due to an internal protocol error --*/
    case HDRHAI_CLOSE_CONN_OPEN_FAIL:
    case HDRHAI_CLOSE_CONN_OPEN_DENY:
    case HDRHAI_CLOSE_QUICK_FAILURE:
    case HDRHAI_CLOSE_AN_INIT_EXP:
      session_close_msg.close_reason = HDRSMP_CLOSE_ERROR;
      session_close_msg.more_info_len = 0;
      break;

    /*-- Session Failed due to invalid data from AN --*/
    case HDRHAI_CLOSE_PERSONALITY_INDEX_INVALID:
      session_close_msg.close_reason = HDRSMP_CLOSE_SESSION_CON_FAIL;
      session_close_msg.more_info_len = 0;
      break;

    /*-- Session Failed due to a problem in protocol negotiation --*/
    case HDRHAI_CLOSE_PROT_NEG_FAIL:
    case HDRHAI_CLOSE_CFG_MSG_FAIL:
    case HDRHAI_CLOSE_CFG_RSP_EXP:
    case HDRHAI_CLOSE_AN_GAUP_FAIL:
      /* Give reason 0x03 instead of 0x04; reason 0x04 should be reserved for
         malformed ConfigRequest messages from the AN. However, reason 0x04 
         will not be given; following the standard, we discard malformed 
         messages from the AN */
      session_close_msg.close_reason = HDRSMP_CLOSE_PROT_CON_FAIL;
      session_close_msg.more_info_len = sizeof(session_close_msg.type) +
                                        sizeof(session_close_msg.subtype);
      session_close_msg.type = scp_fail_data_ptr->type;
      session_close_msg.subtype = scp_fail_data_ptr->subtype;

      break;

    default:
      ERR("unknown session config failure reason %d",scp_fail_data_ptr->reason,
           0,0);
      break;
  }

  hdrsmp_send_session_close_msg(&session_close_msg);

  /* set close reason here, it will be passed to SCP as part of deactivation
     when the session is closed (SLP callback of Session Close Msg) */
  hdrsmp.session_close.close_reason = scp_fail_data_ptr->reason;

} /* hdrsmp_process_session_fail_cfg_related_ind() */


#ifdef FEATURE_LTE_TO_HDR_OH
/*===========================================================================

FUNCTION HDRSMP_PROCESS_IRAT_CAPABILITY_CHANGED_IND

DESCRIPTION
  This function closes the session whenever irat capability changes.

DEPENDENCIES
  None.

PARAMETERS

RETURN VALUE
  None.

SIDE EFFECTS
  The session will end and a Session Close Message will be sent.

===========================================================================*/
LOCAL void hdrsmp_process_irat_capability_changed_ind( void )
{
  /* structure holding the data for the Session Close Message */
  hdrsmp_session_close_msg_type session_close_msg;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Session fail related Indications are only valid in HDRSMPS_OPEN State */
  if ( hdrsmps_get_state() != HDRSMPS_OPEN ||
       (hdrsmp.session_close.connection_closing) )
  {
    HDRTRACE_IND_IGNORED_STATE( HDRIND_SCP_EHRPD_AVAILABILITY_CHANGED, 
                                HDRHAI_SESSION_MANAGEMENT_PROTOCOL, 
                                hdrsmps_get_state() );
    return;
  }

  memset ( &session_close_msg, 0, sizeof(hdrsmp_session_close_msg_type) ); 

  /* Fill out the fields of Session Close message */
  session_close_msg.message_id = HDRSMP_SESSION_CLOSE_MSG;
  session_close_msg.close_reason = HDRSMP_CLOSE_SESSION_CON_FAIL;
  session_close_msg.more_info_len = 0;

  hdrsmp_send_session_close_msg(&session_close_msg);

  /* Set close reason here. It will be passed to SCP as part of deactivation
     when the session is closed (SLP callback of Session Close Msg) */
  hdrsmp.session_close.close_reason = HDRHAI_CLOSE_IRAT_CHANGED;

} /* hdrsmp_process_irat_capability_changed_ind() */
#endif /* FEATURE_LTE_TO_HDR_OH */

/* <EJECT> */ 

#ifdef FEATURE_EHRPD
/*===========================================================================

FUNCTION HDRSMP_PROCESS_EHRPD_AVAILABILITY_CHANGED

DESCRIPTION
  This function closes the session whenever eHRPD availability is changed.

DEPENDENCIES
  None.

PARAMETERS

RETURN VALUE
  None.

SIDE EFFECTS
  The session will end and a Session Close Message will be sent.

===========================================================================*/
LOCAL void hdrsmp_process_ehrpd_availability_changed( void )
{
  /* structure holding the data for the Session Close Message */
  hdrsmp_session_close_msg_type session_close_msg;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Session fail related Indications are only valid in HDRSMPS_OPEN State */
  if ( hdrsmps_get_state() != HDRSMPS_OPEN ||
       (hdrsmp.session_close.connection_closing) )
  {
    HDRTRACE_IND_IGNORED_STATE( HDRIND_SCP_EHRPD_AVAILABILITY_CHANGED, 
                                HDRHAI_SESSION_MANAGEMENT_PROTOCOL, 
                                hdrsmps_get_state() );
    return;
  }

  memset ( &session_close_msg, 0, sizeof(hdrsmp_session_close_msg_type) ); 

  /* Fill out the fields of Session Close message */
  session_close_msg.message_id = HDRSMP_SESSION_CLOSE_MSG;
  session_close_msg.close_reason = HDRSMP_CLOSE_SESSION_CON_FAIL;
  session_close_msg.more_info_len = 0;

  hdrsmp_send_session_close_msg(&session_close_msg);

  /* Set close reason here. It will be passed to SCP as part of deactivation
     when the session is closed (SLP callback of Session Close Msg) */
  hdrsmp.session_close.close_reason =  HDRHAI_CLOSE_EHRPD_AVAILABILITY_CHANGED;

} /* hdrsmp_process_ehrpd_availability_changed() */
#endif /* FEATURE_EHRPD */

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRSMP_PROCESS_AMP_OPENED

DESCRIPTION
  This function processes the AMP.Opened Indication

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
LOCAL void hdrsmp_process_amp_opened ( void )
{
  /* AMP.Opened Indication is only valid in HDRSMPS_AMP_SETUP State */
  if (hdrsmps_get_state() != HDRSMPS_AMP_SETUP)
  {
    HDRTRACE_IND_IGNORED_STATE(HDRHAI_SESSION_MANAGEMENT_PROTOCOL,
                                HDRIND_AMP_OPENED, hdrsmps_get_state());

    return;
  }

  /* make sure we will not accidentally send a Keep Alive Request Message */
  hdrsmp.keep_alive_req_msg_info.sending_msg = FALSE;

  /*-- clear the keep alive timer if set and restart keep alive --*/
  (void)rex_clr_sigs(rex_self(), HDRMC_SMP_KA_TIMER_SIG);
  hdrsmpka_set_timer(hdrsmp.session_params.current.keep_alive_expiration);

  hdrsmps_change_state(HDRSMPS_OPEN);

} /* hdrsmp_process_amp_opened */

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRSMP_PROCESS_IDLE_CONNECTION_OPENED

DESCRIPTION
  This function processes the IDLE.ConnectionOpened Indication

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  The Keep Alive Interval Timer and Keep Alive Expiration Timer will
    be stopped.

===========================================================================*/
LOCAL void hdrsmp_process_idle_connection_opened ( void )
{

  /* IDLE.ConnectionOpened Indication is only valid in HDRSMPS_OPEN State */
  if (hdrsmps_get_state() != HDRSMPS_OPEN)
  {
    HDRTRACE_IND_IGNORED_STATE(HDRHAI_SESSION_MANAGEMENT_PROTOCOL,
                          HDRIND_IDLE_CONNECTION_OPENED, hdrsmps_get_state());
  }
  else
  {
    /* When the Connection is opened, the keep alive functionality is
       suspended.  It is restarted when the Connection is closed.  This
       shortcut may be assumed since the rate of receiving mobile directed
       data is much higher, and after a relatively short period of inactivity
       the connection is closed.  */
    hdrsmpka_clr_timer();
    (void)rex_clr_sigs(rex_self(), HDRMC_SMP_KA_TIMER_SIG);
  }

} /* hdrsmp_process_idle_connection_opened */

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRSMP_PROCESS_KEEP_ALIVE_TIMER_EXP

DESCRIPTION
  This function processes the Keep Alive Timer Expiration

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  The session may be closed.

===========================================================================*/
LOCAL void hdrsmp_process_keep_alive_timer_exp ( void )
{

  HDRTRACE_HDRMC_KA_TIMER_SIG( hdrsmps_get_state() );

  /* the timer should never expire if the keep alive functionality was
     negotiated to be disabled */
  ASSERT(hdrsmp.session_params.current.keep_alive_expiration !=
         HDRSMPKA_KEEP_ALIVE_DISABLED);

  /* attempt to restart the keep alive timer.  If successful, send a
     Keep Alive Request Message.  If this timer expiration passed the
     session expiration time, close the session */
  if (hdrsmpka_update_timer(HDRSMPKA_TIMER_SIGNAL_RXED))
  {
    if (!hdrmc_feature_is_enabled(
          HDRMC_FEATURE_HDR_KA_REQUEST_DISABLED))
    {
      if (
#ifdef FEATURE_LTE_TO_HDR_OH
           /* Don't send KeepAliveRequest if PreRegistration is not allowed. */
           !( hdrsap_tunnel_mode_is_enabled( HDRHAI_SESSION_MANAGEMENT_PROTOCOL ) &&
              hdrscp_get_current_subtype( HDRHAI_SIGNALING_ADAPTATION_PROTOCOL )
                == HDRSCP_IRAT_SAP &&
              !hdrovhd_prereg_is_allowed( HDRHAI_SESSION_MANAGEMENT_PROTOCOL ) ) &&
#endif /* FEATURE_LTE_TO_HDR_OH */

           /* If maintain EVDO session is FALSE, don't send KeepAliveRequest. */
           hdrsmp_is_to_maintain_evdo_session()
         )
      {
        hdrsmp_send_keep_alive_request_msg();
      }
      else
      {
        HDR_MSG_PROT ( MSG_LEGACY_HIGH, 
                       " MUM: KA timer expired but don't send KA req message");
      }
    }
  }
  else
  {
    /* set close reason here, it will be passed to SCP and HMP
       when the session is closed */
    hdrsmp.session_close.close_reason = HDRHAI_CLOSE_KA_EXP;

    hdrsmp_close_session( hdrsmp.session_close.close_reason );
  }
} /* hdrsmp_process_keep_alive_timer_exp */

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRSMP_FIRST_ACQ_INIT

DESCRIPTION
  This function initializes SMP upon our first HDR acquisition.

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
LOCAL void hdrsmp_first_acq_init ( void )
{
  boolean session_is_open;
  boolean ka_running;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Initialize */
  session_is_open = (hdrsmps_get_state() == HDRSMPS_OPEN);
  ka_running      = hdrsmpka_initialize_timer(
                      session_is_open,
                      hdrsmp.session_params.current.keep_alive_expiration );

  if ( session_is_open )
  {
    /* First, make sure that our ka timer is not disabled */
    if (hdrsmp.session_params.current.keep_alive_expiration !=
           HDRSMPKA_KEEP_ALIVE_DISABLED)
    {
      /* Check to see if our timer has expired while we were away
         (we now have a valid system time) */

      if ( !ka_running )
      {
        /* Because we just acquired the system, the time source used for
           the keep alive timer must come from the SYNC message, not system
           time */
        hdrsmp_process_keep_alive_timer_exp();
      }
      else
      {
        HDR_MSG_PROT(MSG_LEGACY_HIGH,"KA timer already running");
      }
    }
  }
} /* hdrsmp_first_acq_init() */

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRSMP_PROCESS_ALMP_CONNECTION_CLOSED

DESCRIPTION
  This function processes the ALMP.ConnectionClosed Indication

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
LOCAL void hdrsmp_process_almp_connection_closed ( void )
{
  /* SMP just closed this connection while in the process of closing its session.
   * Session closing was interrupted until the connection close was completed.
   * It can be resumed now that ALMP is done moving out of connected state. */
  if ( hdrsmp.session_close.connection_closing )
  {
    hdrsmp.session_close.connection_closing = FALSE;
    hdrsmp_close_session(hdrsmp.session_close.close_reason);
  }

#if defined (FEATURE_DEDICATED_C2K_MODE) || defined (FEATURE_IRAT_PILOT_MEAS)
  /* report to CM that session is closed (fake close) */
  else if (hdrsmp.session_close.cm_sess_close_pending)
  {
    hdrsmp.session_close.cm_sess_close_pending = FALSE;
    hdrsmp_send_session_closed_to_cm();
  }
#endif /* FEATURE_DEDICATED_C2K_MODE || FEATURE_IRAT_PILOT_MEAS */
} /* hdrsmp_process_almp_connection_closed() */

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRSMP_PROCESS_NETWORK_ACQUIRED

DESCRIPTION
  This function processes the Init.NetworkAcquired Indication

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
LOCAL void hdrsmp_process_network_acquired ( void )
{
  /* If this is our first acquisition after power up */
  static boolean first_acquisition = TRUE;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* If this is our first acquisition after power up, we need to do
     initialization for the session layer that can only occur when
     a system is acquired. */
  if (first_acquisition)
  {
    /* SimuCommonReceiver or DiversityComb setting could
     * be changed after power-cycle. Since AT cannot
     * report this change to AN, close the session. 
     * Send Session Close to AN to invalid the open session. 
     */
    if ( ( hdrscps_get_state() == HDRSCPS_OPEN ) &&
         hdrscmgmmcdp_rf_setting_changed() )
    {
      hdrsmp_deactivate( HDRHAI_SESSION_MANAGEMENT_PROTOCOL, FALSE );
    }

    /* Keep Alive */
    hdrsmp_first_acq_init();

    /* Dual Address Timer */
    hdramp_first_acq_init();
    first_acquisition = FALSE;
  }
}

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRSMP_PROCESS_OVHD_INFO_UPDATED

DESCRIPTION
  This function processes the OVHD.InfoUpdated Indication

  It is possible that we were attempting to send a Keep Alive Request
  message when acquisition was lost, in which case we would need to
  resend the message when the network is reacquired.  Logically we would
  use INIT.NetworkAcquired indication to send the message, but we don't
  know for sure that we reacquired the same subnet until we get the
  overhead information and AMP is able to verify that it is the same
  subnet.

DEPENDENCIES
  SMP must process this indication after AMP.

PARAMETERS
  ind_data_ptr - pointer to the indication data

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
LOCAL void hdrsmp_process_ovhd_info_updated
(
  hdrind_ind_data_union_type * ind_data_ptr
)
{
  /* Since the OVHD.InfoUpdated Indication is only used to determine if
     the Keep Alive Request Message needs to be resent, SMP only uses
     it in the HDRSMPS_OPEN State */
  if (hdrsmps_get_state() != HDRSMPS_OPEN)
  {
    HDRTRACE_IND_IGNORED_STATE(HDRHAI_SESSION_MANAGEMENT_PROTOCOL,
                          HDRIND_OVHD_INFO_UPDATED, hdrsmps_get_state());
    return;
  }

  if (!hdrmc_feature_is_enabled(
        HDRMC_FEATURE_HDR_KA_REQUEST_DISABLED))
  {
    /* If this is the first time OVHD has updated its info since the AT
       reacquired the system, this is the first opportunity we have to
       resend a Keep Alive Request Message, if we were previous attempting to
       send it when acquisition was lost.  */
    if (ind_data_ptr->ovhd_updated_is_first_after_acq &&
        hdrsmp.keep_alive_req_msg_info.sending_msg)
    {
      hdrsmp_send_keep_alive_request_msg();
    }
  }
} /* hdrsmp_process_ovhd_info_updated */

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRSMP_PROCESS_CON_CONNECTION_CLOSED

DESCRIPTION
  This function processes the CON.ConnectionClosed Indication

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  The Keep Alive Inverval Timer is started.  If necessary, the Keep Alive
    Start Time is written to NV.

===========================================================================*/
LOCAL void hdrsmp_process_con_connection_closed( void )
{
  /* The CON.ConnectionClosed Indication is used for the Keep Alive
     functionality but also , SMP only uses it in the HDRSMPS_OPEN State */
  if (hdrsmps_get_state() != HDRSMPS_OPEN)
  {
    HDRTRACE_IND_IGNORED_STATE(HDRHAI_SESSION_MANAGEMENT_PROTOCOL,
                     HDRIND_CONNECTED_CONNECTION_CLOSED, hdrsmps_get_state());

    return;
  }

  /* Since the keep alive functionality is being restarted, make sure we
     don't send a Keep Alive Request Message */
  hdrsmp.keep_alive_req_msg_info.sending_msg = FALSE;

  /*-- clear the keep alive timer if set and restart keep alive --*/
  (void)rex_clr_sigs(rex_self(), HDRMC_SMP_KA_TIMER_SIG);
  hdrsmpka_set_timer(hdrsmp.session_params.current.keep_alive_expiration);

} /* hdrsmp_process_con_connection_closed */

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRSMP_PROCESS_CONFIG_REQ_KA_ATTRIB

DESCRIPTION
  This function processes the Keep Alive Attribute of the Configuration
    Request Message

DEPENDENCIES
  None.

  item_ptr - Pointer to the DSM item chain holding the message
  attrib_len - Length, in bits, of the attribute, including the length field
  attrib_offset - Bit offset of the attribute from the beginning of the message
  value_ptr - Pointer to location to write value of accepted attribute

RETURN VALUE
  TRUE if a new, acceptable value for the attribute was found, FALSE otherwise.
  If return value is TRUE, value is returned through value_ptr

SIDE EFFECTS
  The Configuration Response Message is sent, data may be stored into
    session_params.negotiated.

===========================================================================*/
LOCAL boolean hdrsmp_process_config_req_ka_attrib
(
  dsm_item_type * item_ptr,
  uint16 attrib_len,
  uint16 attrib_offset,
  uint16 *value_ptr
)
{
  /* Bit offset of current attribute value from the beginning of the
   * current attribute */
  uint16 attrib_val_offset;
  boolean val_is_acceptable = FALSE;  /* If we accept the attribute */
  uint16 cur_value = 0; /* current value being extracted */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* The first attribute value is after the attribute length and ID fields */
  attrib_val_offset = HDRSMP_ATTRIB_LEN_SIZE +
                      HDRSMP_ATTRIB_ID_SIZE;

  while ((attrib_val_offset + HDRSMP_KA_ATTRIB_VAL_SIZE) <= attrib_len)
  {
    if (HDRSMP_UNPACK_CON_REQ_ATTRIBUTE_VALUE(item_ptr, &cur_value,
        attrib_offset, attrib_val_offset) != E_SUCCESS)
    {
      ERR("Error unpacking Configuration Request message, ignoring",0,0,0);
      break;
    }

    /* currently any value is acceptable for keep alive timeout */
    val_is_acceptable = TRUE;
    break;

    /* If we ever want to look for more than one value... 
    attrib_val_offset += HDRSMP_KA_ATTRIB_VAL_SIZE;    */
  }

  *value_ptr = cur_value;
  return (val_is_acceptable);
} /* hdrsmp_process_config_req_ka_attrib */

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRSMP_PROCESS_CONFIG_REQ_MSG

DESCRIPTION
  This function processes the Configuration Request Message

DEPENDENCIES
  Message ID is not checked.  It must be checked before the function is called.

PARAMETERS
  item_ptr - Pointer to the DSM item containing the Message

RETURN VALUE
  None.

SIDE EFFECTS
  The Configuration Response Message is sent, data may be stored into
    session_params.negotiated.

===========================================================================*/
LOCAL void hdrsmp_process_config_req_msg
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
    uint16 value;
  } ka_config_attrib;

  dsm_item_type * send_msg_ptr; /* dsm buffer for sent message */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  total_bits_remaining = (dsm_length_packet(item_ptr) *
                          HDRSMP_BITS_PER_BYTE);

  /* Extract the Transaction ID */
  if ((HDRSMP_UNPACK_TRANS_ID(item_ptr, &creq_transaction_id)) !=
       E_SUCCESS)
  {
  ERR("Error extracting TransactionID from ConfigReq Msg",0,0,0);
  /* Without the transaction ID, it makes no sense to send a response */
    return;
  }

  /* Initialize all "is_accepted" flags that are inside structures */
  ka_config_attrib.is_accepted = FALSE;

  /* Initialize attrib_len so the initial attrib_offset will be
   * immediately after the transaction ID of the config request.
   */
  attrib_len = HDRSMP_MSG_ID_SIZE + HDRSMP_TRANSACTION_ID_SIZE;

  /* Unpack the configuration attributes */

  /* While there are at least enough bits for the smallest attribute... */
  while ((total_bits_remaining - attrib_len) >= HDRSMP_MIN_ATTRIB_LEN)
  {
    /* Move to the next attribute record */
    attrib_offset += attrib_len;
    total_bits_remaining -= attrib_len;

    /* Get the length of the new attribute */
    if (HDRSMP_UNPACK_CON_REQ_LEN(item_ptr, &attrib_len,
        attrib_offset) != E_SUCCESS)
    {
      ERR("Error unpacking Configuration Request message, ignoring",0,0,0);
      /* Without the length we can't continue to parse the message. */
      send_response = FALSE;
      break;
    }

    /* The AttributeLength field in the message is the attribute length in
     * bytes, excluding the length of the length field.  For our purposes,
     * we want attrib_len to be the total length of the attribute in bits,
     * including the length field.
     */
    attrib_len *= HDRSMP_BITS_PER_BYTE;
    attrib_len += HDRSMP_ATTRIB_LEN_SIZE;

    /* Any attribute record must have at least an attribute ID */
    if ((attrib_len < HDRSMP_MIN_ATTRIB_LEN) ||
         attrib_len > total_bits_remaining)
    {
      /* If the length is something unexpected, we can't be sure of what
       * the AN intended and it is safest to not send a response
       */
      ERR("Error, invalid total len vs bits in message, ignoring",0,0,0);
      send_response = FALSE;
      break;
    }

    /* There is at least one more attribute record in the message */

    if (HDRSMP_UNPACK_CON_REQ_ATTRIBUTE_ID(item_ptr, &(attrib_id),
        attrib_offset) != E_SUCCESS)
    {
      ERR("Error unpacking Configuration Request message, ignoring",0,0,0);
      /* If we can't get the attribute_id, don't send a response */
      send_response = FALSE;
      break;
    }

    switch (attrib_id)
    {
      case HDRSMP_KEEP_ALIVE_ATTRIBUTE_ID:
        if (attrib_len >= HDRSMP_KA_MIN_LEN)
        {
          ka_config_attrib.is_accepted =
            hdrsmp_process_config_req_ka_attrib( item_ptr,
                                      attrib_len,
                                      attrib_offset,
                                      &ka_config_attrib.value);
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
        ERR("Unknown attribute ID %d",attrib_id,0,0);
        break;
    } /* switch */

    if (!send_response)
    {
      break;
    }
  } /* while */

  /* Form and send the Configuration Response message. */

  if (send_response)
  {
    /* get new buffer */
    send_msg_ptr = hdrutil_new_msg_buffer();

    /* pack fields */
    HDRSMP_PACK_CON_RSP_MSG_ID( send_msg_ptr, HDRSMP_CONFIG_RSP_MSG);
    HDRSMP_PACK_CON_RSP_TRANS_ID( send_msg_ptr, creq_transaction_id);

    /* Now pack in the attribute ID and value of all attributes we have
     * flagged that we want to accept.
     */

    /* attrib_offset is now offset for response message */
    attrib_offset = HDRSMP_MSG_ID_SIZE + HDRSMP_TRANSACTION_ID_SIZE;

    if (ka_config_attrib.is_accepted)
    {
      HDRSMP_PACK_CON_RSP_LENGTH(send_msg_ptr,
        HDRSMP_CRSP_ATTRIB_LEN_BYTES,
        attrib_offset);

      HDRSMP_PACK_CON_RSP_ATTRIBUTE_ID(send_msg_ptr,
        HDRSMP_KEEP_ALIVE_ATTRIBUTE_ID, attrib_offset);

      HDRSMP_PACK_CON_RSP_ATTRIBUTE_VALUE(send_msg_ptr,
        ka_config_attrib.value, attrib_offset);

      attrib_offset += HDRSMP_CRSP_ATTRIB_LEN;

      hdrsmp.session_params.negotiated.keep_alive_expiration =
        ka_config_attrib.value;
    }

    /* if (accept_some_other_attribute) ... */

    hdrhmp_send_msg( HDRHAI_SESSION_MANAGEMENT_PROTOCOL,
                     HDRHAI_RTC,
                     TRUE, /* Reliable */
                     HDRSMP_MESSAGE_PRIORITY,
                     HDRHAI_IN_CONFIG,/* Instance of protocol sending message */
                     send_msg_ptr,
                     NULL,
                     NULL);
  } /* end if (send_response) */
} /* hdrsmp_process_config_req_msg */

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRSMP_PROCESS_KEEP_ALIVE_REQ_MSG

DESCRIPTION
  This function processes the Keep Alive Request Message

DEPENDENCIES
  Message ID is not checked.  It must be checked before the function is called.

PARAMETERS
  item_ptr - Pointer to the DSM item containing the Message

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
LOCAL void hdrsmp_process_keep_alive_req_msg
(
  dsm_item_type * item_ptr
)
{
  /* structure where the Keep Alive response data is kept until sending */
  hdrsmp_keep_alive_msg_type keep_alive_rsp_msg;
  dsm_item_type * send_msg_ptr; /* DSM item pointer to store packed message */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (hdrsmps_get_state() != HDRSMPS_OPEN)
  {
    HDRTRACE_RXMSG_IGNORED_STATE(HDRHAI_SESSION_MANAGEMENT_PROTOCOL,
       HDRSMP_KEEP_ALIVE_REQ_MSG, hdrsmps_get_state() ); //lint !e506

    return;
  }

  if (HDRSMP_UNPACK_TRANS_ID(item_ptr, &(keep_alive_rsp_msg.transaction_id))
      != E_SUCCESS)
  {
    ERR( "Error unpacking Keep Alive Request message, ignoring", 0, 0, 0);

    return;
  }

  /* If maintain EVDO session is FALSE, AT shouldn't send the 
   * Keep Alive Response message to AN. */
  if ( !hdrsmp_is_to_maintain_evdo_session() )
  {
    HDR_MSG_PROT(MSG_LEGACY_HIGH, 
                 " MUM: Ignore KA Request message");
    return;
  }

  keep_alive_rsp_msg.message_id = HDRSMP_KEEP_ALIVE_RSP_MSG;

  send_msg_ptr = hdrutil_new_msg_buffer();

  HDRSMP_PACK_KA_MSG_ID(send_msg_ptr, keep_alive_rsp_msg.message_id);
  HDRSMP_PACK_KA_TRANS_ID(send_msg_ptr, keep_alive_rsp_msg.transaction_id);

  /* Send Keep Alive Response Message. */
  hdrhmp_send_msg( HDRHAI_SESSION_MANAGEMENT_PROTOCOL,
                   HDRHAI_AC_OR_RTC,
                   FALSE,
                   HDRSMP_MESSAGE_PRIORITY,
                   HDRHAI_IN_USE, /* Instance of protocol sending message */
                   send_msg_ptr,
                   NULL,
                   NULL);

} /* hdrsmp_process_keep_alive_req_msg */

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRSMP_PROCESS_KEEP_ALIVE_RSP_MSG

DESCRIPTION
  This function processes the Keep Alive Response Message

DEPENDENCIES
  Message ID is not checked.  It must be checked before the function is called.

PARAMETERS
  item_ptr - Pointer to the DSM item containing the Message

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
LOCAL void hdrsmp_process_keep_alive_rsp_msg
(
  dsm_item_type * item_ptr
)
{
  uint8 transaction_id; /* transaction ID in keep alive response message */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (hdrsmps_get_state() != HDRSMPS_OPEN)
  {
    HDRTRACE_RXMSG_IGNORED_STATE(HDRHAI_SESSION_MANAGEMENT_PROTOCOL,
       HDRSMP_KEEP_ALIVE_RSP_MSG, hdrsmps_get_state() ); //lint !e506

    return;
  }

  if (HDRSMP_UNPACK_TRANS_ID(item_ptr, &(transaction_id)) != E_SUCCESS)
  {
    ERR( "Error unpacking Keep Alive Response Message, ignoring",0,0,0);
    return;
  }

  if (transaction_id != hdrsmp.keep_alive_req_msg_info.trans_id)
  {
    HDRTRACE_RXMSG_IGNORED_SEQ(HDRHAI_SESSION_MANAGEMENT_PROTOCOL,
       HDRSMP_KEEP_ALIVE_RSP_MSG, hdrsmps_get_state()); //lint !e506
    return;
  }

#ifdef FEATURE_LTE_TO_HDR_OH
  if ( hdrsap_tunnel_mode_is_enabled( HDRHAI_SESSION_MANAGEMENT_PROTOCOL ) )
  {
    /* In the tunnel mode, we will not receive either of the following:
     * - HDRIND_ACMAC_TRANSMISSION_SUCCESSFUL,
     * - HDRIND_CCMAC_MOBILE_DIRECTED_PKT,
     * so need to restart KA timer after receiving KeepAliveResponse from AN.
     */
    HDR_MSG_PROT( MSG_LEGACY_HIGH, "Rxed KA Rsp, restart KA timer" );
    hdrsmp_restart_ka_timer();
  }
#endif /* FEATURE_LTE_TO_HDR_OH */

  HDRTRACE_HDRSMP_IGNORE_KEEP_ALIVE_RSP_MSG();

} /* hdrsmp_process_keep_alive_rsp_msg */

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRSMP_PROCESS_SESSION_CLOSE_MSG

DESCRIPTION
  This function processes the Session Close Message

DEPENDENCIES
  Message ID is not checked.  It must be checked before the function is called.

PARAMETERS
  item_ptr - Pointer to the DSM item containing the Message

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
LOCAL void hdrsmp_process_session_close_msg
(
  dsm_item_type * item_ptr
)
{
  /* structure to hold data from received Session Close Message after unpack */
  hdrsmp_session_close_msg_type rec_session_close_msg;
  /* structure to hold sent Session Close Message before pack */
  hdrsmp_session_close_msg_type send_session_close_msg;
  uint8 ret_val = 0; /* counter to store the return value of unpack macros */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if ((hdrsmps_get_state() == HDRSMPS_CLOSING) ||
      (hdrsmps_get_state() == HDRSMPS_INACTIVE))
  {
    HDRTRACE_RXMSG_IGNORED_STATE(HDRHAI_SESSION_MANAGEMENT_PROTOCOL,
       HDRSMP_SESSION_CLOSE_MSG, hdrsmps_get_state() ); //lint !e506

    return;
  }

  ret_val |= (uint8)HDRSMP_UNPACK_SC_CLOSE_REASON(item_ptr,
                            (uint8*)(&(rec_session_close_msg.close_reason)));
  ret_val |= (uint8)HDRSMP_UNPACK_SC_MORE_INFO_LEN(item_ptr,
                                 &(rec_session_close_msg.more_info_len));
  /* if there is a problem unpacking the received session close message,
     assume the close reason is ERROR with 0 more_info_len.  We can recover
     from this error.  */
  if (ret_val != (uint8)E_SUCCESS)
  {
    ERR( "Error unpacking Session Close message", 0, 0, 0);
    rec_session_close_msg.close_reason = HDRSMP_CLOSE_ERROR;
    rec_session_close_msg.more_info_len = 0;
  }

  /* different close reasons have different more_info */
  switch(rec_session_close_msg.close_reason)
  {
    case HDRSMP_CLOSE_NORMAL:
    case HDRSMP_CLOSE_ERROR:
    case HDRSMP_CLOSE_SESSION_CON_FAIL:
    case HDRSMP_CLOSE_SESSION_LOST:
    case HDRSMP_CLOSE_SESSION_UNREACHABLE:
    case HDRSMP_CLOSE_RESOURCES_BUSY:
      /* expect 0 more_info_len */
      if (rec_session_close_msg.more_info_len != 0)
      {
        ERR("bad data for session close msg, session close", 0, 0, 0);
      }
      break;

    case HDRSMP_CLOSE_REPLY:
      /* should never get reply in Open State or AMP Setup State*/
      ERR("session close reply in wrong state, session close", 0, 0, 0);
      break;

    case HDRSMP_CLOSE_PROT_CON_FAIL:
      /* Protocol Configuration Failed. */
      if (rec_session_close_msg.more_info_len != HDRSMP_CON_FAIL_MORE_INFO_LEN)
      {
        ERR("bad data for session close msg, session close", 0, 0, 0);
      }
      else
      {
        /* unpack type and subtype */
        ret_val |= (uint8)HDRSMP_UNPACK_SC_TYPE(item_ptr,
                               &(rec_session_close_msg.type));
        ret_val |= (uint8)HDRSMP_UNPACK_SC_SUBTYPE(item_ptr,
                               &(rec_session_close_msg.subtype));
      }
      break;

    case HDRSMP_CLOSE_PROT_NEG_FAIL:
      /* Protocol Negotiation Failed. Type (1byte), subtype (2bytes),
         and attribute ID (up to 4 bytes) */
      if (rec_session_close_msg.more_info_len > 0)
      {
        /* unpack type and subtype */
        ret_val |= (uint8)HDRSMP_UNPACK_SC_TYPE(item_ptr,
                               &(rec_session_close_msg.type));
        ret_val |= (uint8)HDRSMP_UNPACK_SC_SUBTYPE(item_ptr,
                               &(rec_session_close_msg.subtype));
        ret_val |= (uint8)HDRSMP_UNPACK_SC_ATTRIBUTE_ID(item_ptr,
                               &(rec_session_close_msg.attribute_id));
      }
      else
      {
        ERR("bad data length for session close message, session close",0,0,0);
      }
      break;

    default:
      ERR("Unknown session close reason, session close", 0, 0, 0);
      break;
  }

  if (ret_val != (uint8)E_SUCCESS)
  {
    ERR( "Error unpacking Session Close message", 0, 0, 0);
  }

#ifdef FEATURE_LTE_TO_HDR_OH
  if ( hdrsap_lte_to_hdr_active_ho_is_in_progress() )
  {
    /* When HDR is waiting for TCA (mobility), other msgs are received
     * (e.g. SessionClose), abort Active Handoff immediately and set
     * tunnel mode properly. */
    HDR_MSG_PROT( MSG_LEGACY_HIGH, 
                  "Active HO in progress, SessionClose rx'ed. Abort!" );
    hdridle_process_tunnel_conn_timeout();
  }
#endif /* FEATURE_LTE_TO_HDR_OH */

  /* fill out fields of sent session close message */
  send_session_close_msg.message_id = HDRSMP_SESSION_CLOSE_MSG;
  send_session_close_msg.close_reason = HDRSMP_CLOSE_REPLY;
  send_session_close_msg.more_info_len = 0;

  hdrsmp_send_session_close_msg(&send_session_close_msg);

  /* The close reason and supplementary information may be used
    as part of determining action related to the session being closed,
    such as whether to reopen the session immediately with the same sector,
    open a session with a different sector, or wait a while before opening a
    session.  This may become part of the session closed indication data,
    in which case that indication may be removed from the state machine. */

  /* set close reason here, it will be passed to SCP as part of deactivation
     when the session is closed (SLP callback of Session Close Msg).  It will
     also be passed to HMP after the session is successfully closed */
  hdrsmp.session_close.close_reason = HDRHAI_CLOSE_REPLY;

} /* hdrsmp_process_session_close_msg */

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRSMP_PROCESS_DEACTIVATE

DESCRIPTION
  This function processes the Deactivate Command

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  Changes State.

===========================================================================*/
LOCAL void hdrsmp_process_deactivate
(
  hdrsmp_cmd_type * smp_cmd_ptr
)
{
  /* structure to hold sent session close message */
  hdrsmp_session_close_msg_type session_close_msg;
  hdrsmps_state_enum_type smp_state;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  smp_state = hdrsmps_get_state();

  #if defined (FEATURE_DEDICATED_C2K_MODE) || defined (FEATURE_IRAT_PILOT_MEAS)
  if ((smp_cmd_ptr->sender == HDRHAI_CM) ||
      (smp_cmd_ptr->sender == HDRHAI_PACKET_APPLICATION_CONTROLLER))
  {
    HDR_MSG_PROT_1( MSG_LEGACY_MED, 
                  "DCM: Recv. SMP_DEACT from CM or DS SMP state: %d", 
                  smp_state);
    hdrsmp_dcm_deactivate();
    return;
  }

  /* hdrsmp_dcm_deactivate() may call hdrsmp_deactivate() again
   * if the session needs to be fully closed (vs. fake close), 
   * in this case, the sender == SMP, and cm_sess_close_pending
   * is set */
  if (hdrsmp.session_close.cm_sess_close_pending)
  {
    HDR_MSG_PROT( MSG_LEGACY_MED, 
                  "DCM: smp_deactivate called by dcm_deactivate");
  }
  else
  #endif
  /* non-DCM related session close should behave as before */
  if (smp_state != HDRSMPS_OPEN)
  {
    HDRTRACE_CMD_IGNORED_STATE( HDRHAI_SESSION_MANAGEMENT_PROTOCOL,
      smp_cmd_ptr->name, smp_cmd_ptr->sender, smp_state );
    return;
  }

  /* Silently close the session */
  if ( smp_cmd_ptr->params.deactivate.close_is_silent )
  {
    hdrsmp.session_close.close_reason = HDRHAI_CLOSE_SILENT_DEACTIVATE;
    hdrsmp_close_session(hdrsmp.session_close.close_reason);
  }

  /* Explicitely close the session by sending a SessionClose message to the
   * AN first */
  else
  {
    /* fill out fields of sent session close message */
    session_close_msg.message_id = HDRSMP_SESSION_CLOSE_MSG;
    session_close_msg.close_reason = HDRSMP_CLOSE_NORMAL;
    session_close_msg.more_info_len = 0;

    hdrsmp_send_session_close_msg(&session_close_msg);

    /* set close reason here, it will be passed to SCP as part of deactivation
       when the session is closed (SLP callback of Session Close Msg) */
    hdrsmp.session_close.close_reason = HDRHAI_CLOSE_DEACTIVATE;
  }


} /* hdrsmp_process_deactivate */

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRSMP_PROCESS_ACTIVATE

DESCRIPTION
  This function processes the Activate command

DEPENDENCIES
  None.

PARAMETERS
  smp_cmd_ptr - pointer to the SMP command block

RETURN VALUE
  None.

SIDE EFFECTS
  Changes state.

===========================================================================*/
LOCAL void hdrsmp_process_activate
(
  hdrsmp_cmd_type * smp_cmd_ptr
)
{
  switch(hdrsmps_get_state())
  {
    case HDRSMPS_INACTIVE:
     hdrsmps_change_state(HDRSMPS_AMP_SETUP);
    break;

    case HDRSMPS_AMP_SETUP:
      hdramp_activate(HDRHAI_SESSION_MANAGEMENT_PROTOCOL);
    break;

    case HDRSMPS_OPEN:
    case HDRSMPS_CLOSING:
      HDRTRACE_CMD_IGNORED_STATE(HDRHAI_SESSION_MANAGEMENT_PROTOCOL,
        smp_cmd_ptr->name, smp_cmd_ptr->sender, hdrsmps_get_state() );
        
      #ifdef FEATURE_HDR_SLP_WAIT_FOR_UATI_ASSIGNMENT
      HDR_MSG_PROT( MSG_LEGACY_HIGH, 
                    "SLPQH: Closing the session; allow access immediately." );
      hdramp_done_waiting_for_uati_assignment(
          HDRAMP_CLEAR_SLP_HOLDOFF_SESSION_CLOSED);
      #endif /* FEATURE_HDR_SLP_WAIT_FOR_UATI_ASSIGNMENT */

    break;

    default:
      ERR("unknown state in SMP Activate: %d", hdrsmps_get_state(),0,0);
    break;
  }
} /* hdrsmp_process_activate */

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRSMP_PROCESS_SC_COMPLETE

DESCRIPTION
  This function processes the SC_COMPLETE Command

DEPENDENCIES
  None

PARAMETERS
  smp_cmd_ptr - pointer to the SMP command buffer

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
LOCAL void hdrsmp_process_sc_complete
(
  hdrsmp_cmd_type * smp_cmd_ptr
)
{
  if (hdrsmps_get_state() == HDRSMPS_INACTIVE)
  {
    HDRTRACE_CMD_IGNORED_STATE(HDRHAI_SESSION_MANAGEMENT_PROTOCOL,
      smp_cmd_ptr->name, smp_cmd_ptr->sender, hdrsmps_get_state() );

    if( hdralmp_connection_is_open(HDRHAI_SESSION_MANAGEMENT_PROTOCOL) )
    {
       ERR_FATAL( "Connection is open", 0, 0, 0 );
    }

  #if defined (FEATURE_DEDICATED_C2K_MODE) || defined (FEATURE_IRAT_PILOT_MEAS)
    if (hdrsmp.session_close.cm_sess_close_pending)
    {
      hdrsmp.session_close.cm_sess_close_pending = FALSE;
      hdrsmp_send_session_closed_to_cm();
    }
  #endif

    return;
  }

  /* If we are in connected state, we need to close the connection before we
   * can finish closing the session. Session closing will resume after the
   * we are done tearing down the connection */
  if ( (hdralmp_connection_is_open(HDRHAI_SESSION_MANAGEMENT_PROTOCOL)
#ifdef FEATURE_LTE_TO_HDR_OH
        && !hdrsap_tunnel_mode_is_enabled(HDRHAI_HDR_MESSAGING_PROTOCOL)) ||
        (hdrsap_tunnel_mode_is_enabled(HDRHAI_HDR_MESSAGING_PROTOCOL) &&
         hdrsaps_get_state() == HDRSAPS_OPEN
#endif /* FEATURE_LTE_TO_HDR_OH */
       ) )
  {
    /* Remember that we still need to finish closing our session after
     * we complete the connection close */
    hdrsmp.session_close.connection_closing = TRUE;

    hdralmp_close_connection(HDRHAI_ABORT_NORMAL_CLOSE,
                             HDRHAI_SESSION_MANAGEMENT_PROTOCOL);
  }
  /* We are not connected, we can finish closing the session right away */
  else
  {
    hdrsmp_close_session(hdrsmp.session_close.close_reason);
  }
} /* hdrsmp_process_sc_complete */

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRSMP_PROCESS_KA_OUTCOME

DESCRIPTION
  This function processes the KA_OUTCOME Command

DEPENDENCIES
  None

PARAMETERS
  smp_cmd_ptr - pointer to a SMP command buffer

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
LOCAL void hdrsmp_process_ka_outcome
(
  hdrsmp_cmd_type * smp_cmd_ptr
)
{
  if (smp_cmd_ptr->params.ka_outcome.err_val == E_SUCCESS)
  {
    hdrsmp.keep_alive_req_msg_info.sending_msg = FALSE;
    if ( !hdrsmpka_update_timer(HDRSMPKA_ACK_TO_KEEP_ALIVE_REQUEST) )
    {
      hdrsmp_close_session(HDRHAI_CLOSE_KA_EXP);
    }
  }
} /* hdrsmp_process_ka_outcome */

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRSMP_MSG_CB

DESCRIPTION
  This function is registered to be called whenever a protocol gives a
  message to HDRSMP.

DEPENDENCIES
  None

PARAMETERS
  msg_ptr - Pointer to the DSM item chain holding the message

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void hdrsmp_msg_cb
(
  dsm_item_type * msg_ptr
)
{
  hdrmc_queue_msg( HDRHAI_SESSION_MANAGEMENT_PROTOCOL,
    HDRHAI_IN_USE, msg_ptr );
} /* hdrsmp_msg_cb */

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRSMP_CONFIG_MSG_CB

DESCRIPTION
  This function is registered to be called whenever a protocol gives a
  message to HDRSMP in-config instance.

DEPENDENCIES
  None

PARAMETERS
  msg_ptr - Pointer to the DSM item chain holding the message

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void hdrsmp_config_msg_cb
(
  dsm_item_type * msg_ptr
)
{
  hdrmc_queue_msg( HDRHAI_SESSION_MANAGEMENT_PROTOCOL,
    HDRHAI_IN_CONFIG, msg_ptr );
} /* hdrsmp_config_msg_cb */

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRSMP_IND_CB

DESCRIPTION
  This function is registered to be called whenever a protocol gives an
  indication of interest to HDRSMP.

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

void hdrsmp_ind_cb
(
  hdrind_ind_name_enum_type ind_name,
  void * ind_data_ptr
)
{
  if (rex_self() == HDRSMP_TASK_PTR)
  {
    hdrsmp_process_ind(ind_name, (hdrind_ind_data_union_type *)ind_data_ptr);
  }
  else
  {
    hdrmc_queue_ind( HDRHAI_SESSION_MANAGEMENT_PROTOCOL, ind_name,
                     ind_data_ptr );
  }
} /* hdrsmp_ind_cb */



/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRSMP_POWERUP_INIT

DESCRIPTION
  This function commands SMP to Initialize.

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void hdrsmp_powerup_init ( void )
{
  boolean session_status = FALSE;
/*-------------------------------------------------------------------------*/

  if ( hdrmc_is_first_init() )
  {
    /* Define timer only during powerup initialization. */
    hdrsmpka_def_timer(HDRSMP_TASK_PTR, HDRMC_SMP_KA_TIMER_SIG);
  }

  /* set variables */
  hdrsmp.session_close.connection_closing = FALSE;

  hdrsmp.keep_alive_req_msg_info.sending_msg = FALSE;
  hdrsmp.keep_alive_req_msg_info.trans_id = 0;
  hdrsmp.session_closing = FALSE;
  #ifdef FEATURE_HDR_MIN_UATI_MODE
  if ( hdrmc_is_first_init() )
  {
    /* define the maintain EVDO session timer */
    rex_def_timer_ex( &hdrsmp.session_maint.maintain_session_timer,
                      NULL, /* No callback necessary */
                      HDRSMP_MAINTAIN_EVDO_SESSION_TIMER );
  }

  /* Read the min UATI mode NV item. */
  if ( hdrutil_read_nv_item ( NV_HDR_MIN_UATI_MODE_I, 
         (nv_item_type *)&hdrsmp.session_maint.mum_is_enabled )
       != NV_DONE_S )
  {
    /* If this item is inactive, default to disable min UATI mode */
    hdrsmp.session_maint.mum_is_enabled = FALSE;
    HDR_MSG_PROT( MSG_LEGACY_HIGH, 
                  " MUM: NV item not active, defalut to disable");
  }
  else
  {
    if ( hdrsmp.session_maint.mum_is_enabled )
    {
      HDR_MSG_PROT( MSG_LEGACY_HIGH, " MUM: is enabled" );
    }
    else
    {
      HDR_MSG_PROT( MSG_LEGACY_HIGH, " MUM: is disabled" );
    }
  }
  #else /* FEATURE_HDR_MIN_UATI_MODE */
  /* By default, AT should always maintain EVDO session */
  hdrsmp.session_maint.mum_is_enabled = FALSE;
  HDR_MSG_PROT(MSG_LEGACY_HIGH, 
               " MUM: feature not defined, default to disable");
  #endif /* FEATURE_HDR_MIN_UATI_MODE */

  #ifdef FEATURE_IRAT_PILOT_MEAS
  hdrsmp.session_maint.maintain_evdo_session = TRUE;
  hdrsmp.meas_mode_is_on = FALSE;
  #endif /* FEATURE_IRAT_PILOT_MEAS */

  #if defined (FEATURE_DEDICATED_C2K_MODE) && !defined (FEATURE_IRAT_PILOT_MEAS)
  /* DCM: Enable MUM by default, even though the MUM NV item maybe
   * disabled */
  hdrsmp.session_maint.mum_is_enabled = TRUE;
  hdrsmp.session_close.cm_sess_close_pending = FALSE;
  
  /* DCM: Don't maintain EVDO session at power up. */
  hdrsmp.session_maint.maintain_evdo_session = FALSE;
  HDR_MSG_PROT( MSG_LEGACY_HIGH, 
                "DCM: powerup, not maintain EVDO ession" );
  #endif /* FEATURE_DEDICATED_C2K_MODE || FEATURE_IRAT_PILOT_MEAS */

  /* initialize SCP.  return indicates if session is open or not */
  session_status = hdrscp_powerup_init();

  /* Get current configuration parameters from SCMDB */
  hdrscmdb_get_inuse_config(HDRHAI_SESSION_MANAGEMENT_PROTOCOL,
                            &hdrsmp.session_params.current);
 
  if (session_status)
  {
    /* Assume that the session hasn't expired until we get system time */
    /* (watch for an init_network_acquired indication) */
    hdrsmps_change_state(HDRSMPS_OPEN);

    /* Initialize AMP and SMPKA */
    hdramp_powerup_init(TRUE);
    hdrsmpka_powerup_init();
  }
  else /* SCP initialized with closed session */
  {
    hdramp_powerup_init(FALSE);
  }
} /* hdrsmp_powerup_init */

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRSMP_ACTIVATE

DESCRIPTION
  This function commands SMP to activate.

DEPENDENCIES
  None

PARAMETERS
  sender - The protocol giving this command (only for software trace)

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void hdrsmp_activate
(
  hdrhai_protocol_name_enum_type sender
)

{
  hdrsmp_cmd_type hdrsmp_cmd; /* Build command into local struct */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  HDR_MSG_PROT_1 (MSG_LEGACY_MED, "HDRSMP_ACTIVATE_CMD received %d",
                  hdrsmps_get_state());

  if (hdrsmp.dup_act_detect == HDRSMP_DUP_ACT_DETECT_TRIGGERED)
  {
    /* duplicate activation detected during the processing of a single hdrmc event! */
    HDR_MSG_PROT(MSG_LEGACY_HIGH, "Ignore SMP_ACTIVATE while dup activate detected");
    return;
  }
  else if (hdrsmp.dup_act_detect == HDRSMP_DUP_ACT_DETECT_ENABLED)
  {
     /* This is the first activation since the detection was enabled.  
        Activate now but ignore future activations. */
    HDR_MSG_PROT(MSG_LEGACY_HIGH, "Recv. SMP_ACTIVATE while dup detect enabled");
    hdrsmp_set_dup_act_detect(HDRSMP_DUP_ACT_DETECT_TRIGGERED);
  }

  /* Fill in all members of the hdrsmp_cmd_type for the Activate command */
  hdrsmp_cmd.name = HDRSMP_ACTIVATE_CMD;
  hdrsmp_cmd.sender = sender;

  if (rex_self() != HDRSMP_TASK_PTR)
  {
    ERR("Activate Command not expected from other task",0,0,0);
  }

  /* SMP.Activate command is always queued to prevent session-opening
     activities during HMP's processing of the SMP.SessionClosed Indication */
  /* This function copies the hdrsmp_cmd into a buffer before it returns */
  hdrmc_queue_cmd( HDRHAI_SESSION_MANAGEMENT_PROTOCOL, &hdrsmp_cmd,
                   sizeof (hdrsmp_cmd) );
} /* hdrsmp_activate */

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRSMP_DEACTIVATE

DESCRIPTION
  This function commands HDRSMP to deactivate

DEPENDENCIES
  None

PARAMETERS
  sender - The protocol giving this command (only for software trace)
  close_is_silent - TRUE if the session should be closed silently
                    FALSE if the session should be closed explicitely
                 (i.e. by sending a SessionClose message to the AN first)

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void hdrsmp_deactivate
(
  hdrhai_protocol_name_enum_type sender,
  boolean close_is_silent
)
{
  hdrsmp_cmd_type hdrsmp_cmd; /* Build command into local struct */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  HDR_MSG_PROT_2 (MSG_LEGACY_MED, "HDRSMP_DEACTIVATE_CMD rxed from %d (%d)",
                  sender, hdrsmps_get_state());

  /* Fill in all members of the hdrsmp_cmd_type for the Activate command */
  hdrsmp_cmd.name = HDRSMP_DEACTIVATE_CMD;
  hdrsmp_cmd.sender = sender;
  hdrsmp_cmd.params.deactivate.close_is_silent = close_is_silent;

  if (rex_self() == HDRSMP_TASK_PTR)
  {
    hdrsmp_process_cmd( &hdrsmp_cmd );
  }
  else
  {
    /* This function copies the hdrsmp_cmd into a buffer before it returns */
    hdrmc_queue_cmd( HDRHAI_SESSION_MANAGEMENT_PROTOCOL, &hdrsmp_cmd,
                     sizeof (hdrsmp_cmd) );
  }
} /* hdrsmp_deactivate */

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRSMP_SC_TX_OUTCOME_CB

DESCRIPTION
  This function tells SMP the SLP status of the Session Close message.

DEPENDENCIES
  None

PARAMETERS
  data_ptr - data pointer to data given when message was sent
  err_val - SLP result value

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
LOCAL void hdrsmp_sc_tx_outcome_cb
(
  void * data_ptr,
  hdrerrno_enum_type err_val
)
{

  hdrsmp_cmd_type hdrsmp_cmd; /* Build command into local struct */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  switch (err_val)
  {
    case E_SUCCESS:
    HDRTRACE_TXMSG_ACKED( HDRHAI_SESSION_MANAGEMENT_PROTOCOL,
      HDRSMP_SESSION_CLOSE_MSG, hdrsmps_get_state() ); //lint !e506
    break;

    case E_FAILURE:
    HDRTRACE_TXMSG_FAILED( HDRHAI_SESSION_MANAGEMENT_PROTOCOL,
      HDRSMP_SESSION_CLOSE_MSG, hdrsmps_get_state() ); //lint !e506
    break;

    default:
    break;
  }

  hdrsmp_cmd.name = HDRSMP_SC_COMPLETE_CMD;
  hdrsmp_cmd.params.sc_complete.err_val = err_val;
  hdrsmp_cmd.sender = HDRHAI_SESSION_MANAGEMENT_PROTOCOL;

  /* This cmd should always be queued to avoid circular dependencies */
  hdrmc_queue_cmd( HDRHAI_SESSION_MANAGEMENT_PROTOCOL, &hdrsmp_cmd,
                   sizeof(hdrsmp_cmd) );

} /* hdrsmp_sc_tx_outcome_cb */ //lint !e715 data_ptr not used

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRSMP_KA_TX_OUTCOME_CB

DESCRIPTION
  This function tells SMP the SLP status of the keep alive request message

DEPENDENCIES
  None

PARAMETERS
  data_ptr - data pointer to data given when message was sent
  err_val - SLP result value

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
LOCAL void hdrsmp_ka_tx_outcome_cb
(
  void * data_ptr,
  hdrerrno_enum_type err_val
)
{
  hdrsmp_cmd_type hdrsmp_cmd; /* Build command into local struct */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  HDRTRACE_TXMSG_ACKED( HDRHAI_SESSION_MANAGEMENT_PROTOCOL,
    HDRSMP_KEEP_ALIVE_REQ_MSG, hdrsmps_get_state() ); //lint !e506

  hdrsmp_cmd.name = HDRSMP_KA_OUTCOME_CMD;
  hdrsmp_cmd.sender = HDRHAI_SESSION_MANAGEMENT_PROTOCOL;
  hdrsmp_cmd.params.ka_outcome.err_val = err_val;

  /* This cmd should always be queued to avoid circular dependencies */
  hdrmc_queue_cmd( HDRHAI_SESSION_MANAGEMENT_PROTOCOL, &hdrsmp_cmd,
                   sizeof(hdrsmp_cmd) );
} /* hdrsmp_ka_tx_outcome_cb */ //lint !e715 data_ptr not used

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRSMP_PROCESS_CMD

DESCRIPTION
  This function processes commands given to SMP.

DEPENDENCIES
  None

PARAMETERS
  smp_cmd_ptr - Pointer to the command for SMP to process

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void hdrsmp_process_cmd
(
  hdrsmp_cmd_type * smp_cmd_ptr
)
{
  HDRTRACE_CMD( HDRHAI_SESSION_MANAGEMENT_PROTOCOL, smp_cmd_ptr->name,
    smp_cmd_ptr->sender, hdrsmps_get_state() );
#ifdef FEATURE_HDRMC_DEBUG
  #error code not present
#endif

  switch ( smp_cmd_ptr->name )
  {
    case HDRSMP_ACTIVATE_CMD:        /* Begin Opening a Session*/
      HDR_MSG_PROT (MSG_LEGACY_LOW, "Process Activate Command");
      hdrsmp_process_activate( smp_cmd_ptr);
      break;

    case HDRSMP_KA_OUTCOME_CMD:      /* Keep Alive Request send outcome */
      HDR_MSG_PROT (MSG_LEGACY_LOW,
                    "Process Keep Alive Request Outcome Command");
      hdrsmp_process_ka_outcome(smp_cmd_ptr);
      break;

    case HDRSMP_SC_COMPLETE_CMD:      /* Session Close send outcome */
      HDR_MSG_PROT (MSG_LEGACY_LOW,
                    "Process Session Close Complete Command");
      hdrsmp_process_sc_complete(smp_cmd_ptr);
      break;

    case HDRSMP_DEACTIVATE_CMD:      /* Deactivate the protocol */
      HDR_MSG_PROT (MSG_LEGACY_LOW, "Process Deactivate Command");
      hdrsmp_process_deactivate(smp_cmd_ptr);
      break;

    default:
      break;
  }
#ifdef FEATURE_HDRMC_DEBUG
  #error code not present
#endif
} /* hdrsmp_process_cmd */

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRSMP_PROCESS_AMP_OPENED_PREV_SESSION

DESCRIPTION
  This function processes the AMP.Opened_Prev_Session Indication. It first
  sets SMP and AMP to OPEN state and tell SCP to restore prior session. 

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  The session will end

===========================================================================*/
static void hdrsmp_process_amp_opened_prev_session ( void )
{
  hdrsmps_silent_restore_prior_session();
  hdramp_silent_restore_prior_session();
  hdrscp_silent_restore_prior_session();
} /* hdrsmp_process_amp_opened_prev_session */


#ifdef FEATURE_LTE_TO_HDR_OH
/*===========================================================================
FUNCTION HDRSMP_PROCESS_PREREG_INFO_UPDATED_IND

DESCRIPTION
  This function processes HDRIND_OVHD_PREREG_INFO_UPDATED indication from OMP.

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  The session will end.
===========================================================================*/
static void hdrsmp_process_prereg_info_updated_ind
(
  hdrind_ind_data_union_type *ind_data_ptr 
)
{
  if ( ind_data_ptr->prereg_is_allowed )
  {
    hdrsmp_process_network_acquired();
  }
  else
  {
    /* preReg is not allowed. It's similar to HDR hasn't acquired yet. */
  }

} /* hdrsmp_process_prereg_info_updated_ind() */
#endif /* FEATURE_LTE_TO_HDR_OH */


/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRSMP_PROCESS_IND

DESCRIPTION
  This function processes indications given to SMP.

DEPENDENCIES
  None

PARAMETERS
  ind_name     - Name of the indication for SMP to process
  ind_data_ptr - Any data associated with the indication

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void hdrsmp_process_ind
(
  hdrind_ind_name_enum_type ind_name,
  hdrind_ind_data_union_type * ind_data_ptr
)
{

  HDRTRACE_IND( HDRHAI_SESSION_MANAGEMENT_PROTOCOL, ind_name,
    hdrsmps_get_state() );
#ifdef FEATURE_HDRMC_DEBUG
  #error code not present
#endif
  switch ( ind_name )
  {
#ifdef FEATURE_HDR_RESET_KA_TIMER_ON_ACACK_ONLY
    case HDRIND_ACMAC_TRANSMISSION_SUCCESSFUL:
      HDR_MSG_PROT (MSG_LEGACY_LOW,
                    "ACMAC Transmission Sucessful Indication received");
      hdrsmp_restart_ka_timer ();
      break;
#endif

    case HDRIND_CCMAC_MOBILE_DIRECTED_PKT:
      HDR_MSG_PROT (MSG_LEGACY_LOW,
                    "CCMAC Mobile Directed Packet Indication received");
#ifndef FEATURE_HDR_RESET_KA_TIMER_ON_ACACK_ONLY
      hdrsmp_restart_ka_timer ();
#endif
      break;

    case HDRIND_AMP_FAILED_REQUEST_TIMEOUT:
      HDR_MSG_PROT (MSG_LEGACY_LOW,
                    "AMP Failed Request Timeout Indication received");
      hdrsmp_process_amp_failed_request_timeout ();
      break;

    case HDRIND_AMP_FAILED_NEW_NETWORK:
      HDR_MSG_PROT (MSG_LEGACY_LOW,
                    "AMP Failed New Network Indication received");
      hdrsmp_process_amp_failed_new_network ();
      break;

    case HDRIND_AMP_FAILED_NOT_MAINTAINING_UATI:
      HDR_MSG_PROT (MSG_LEGACY_MED,
                    "AMP Failed Not Maintaining UATI Indication received");
      hdrsmp_process_amp_failed_not_maint_uati ();
      break;

    case HDRIND_AMP_FAILED_NEW_ESN:
      HDR_MSG_PROT (MSG_LEGACY_LOW,
                    "AMP Failed New ESN Indication received");
      hdrsmp_process_amp_failed_new_esn();
      break;

    case HDRIND_AMP_FAILED_NEW_NAI:
      HDR_MSG_PROT(MSG_LEGACY_LOW,
                   "AMP Failed New NAI Indication received");
      hdrsmp_process_amp_failed_new_nai();
      break;

    case HDRIND_SCM_SESSION_CHANGED:
      HDR_MSG_PROT (MSG_LEGACY_LOW, "SCM Session Changed Indication received");
      hdrsmp_process_session_changed();
      break;

    case HDRIND_AMP_OPENED:
      HDR_MSG_PROT (MSG_LEGACY_LOW, "AMP Opened Indication received");
      hdrsmp_process_amp_opened ();
      break;

    case HDRIND_IDLE_CONNECTION_OPENED:
      HDR_MSG_PROT (MSG_LEGACY_LOW,
                    "IDLE Connection Opened Indication received");
      hdrsmp_process_idle_connection_opened ();
      break;

    case HDRIND_CONNECTED_CONNECTION_CLOSED:
      HDR_MSG_PROT (MSG_LEGACY_LOW,
                    "CON Connection Closed Indication received");
      hdrsmp_process_con_connection_closed ();
      break;

    case HDRIND_ALMP_CONNECTION_CLOSED:
      HDR_MSG_PROT (MSG_LEGACY_LOW,
                    "ALMP Connection Closed Indication received");
      hdrsmp_process_almp_connection_closed();
      break;

   /* = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =
   * Note that these indications have indication data, specifying the failure
   * reason, as well as the type and attempted subtype of the failed protocol
   * if applicable */
    case HDRIND_SCP_FAILED:
    case HDRIND_IDLE_PROT_NEG_FAILED:
    case HDRIND_STREAM_PROT_NEG_FAILED:
    case HDRIND_KEP_PROT_NEG_FAILED:
    case HDRIND_GMMCD_PROT_NEG_FAILED:
    case HDRIND_SCM_PROT_NEG_FAILED:
    case HDRIND_RUP_PROT_NEG_FAILED:
    case HDRIND_RMAC_PROT_NEG_FAILED:
    case HDRIND_SCP_AN_GAUP_FAILED:
    case HDRIND_IDLE_AN_GAUP_FAILED:
    case HDRIND_RUP_AN_GAUP_FAILED:
    case HDRIND_PACC_AN_GAUP_FAILED:
    case HDRIND_SCM_AN_GAUP_FAILED:
    case HDRIND_CSNA_PROT_NEG_FAILED:
    case HDRIND_AMAC_PROT_NEG_FAILED:
    case HDRIND_FMAC_PROT_NEG_FAILED:
    case HDRIND_PHY_PROT_NEG_FAILED:
      hdrsmp_process_session_cfg_fail_related_ind(ind_name, ind_data_ptr);
      break;
  /* = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = */

    case HDRIND_OVHD_INFO_UPDATED:
      HDR_MSG_PROT (MSG_LEGACY_LOW, "OVHD Info Updated Indication received");
      hdrsmp_process_ovhd_info_updated (ind_data_ptr);
      break;

#ifdef FEATURE_LTE_TO_HDR_OH
    case HDRIND_OVHD_PREREG_INFO_UPDATED:
      hdrsmp_process_prereg_info_updated_ind( ind_data_ptr );
      break;

    case HDRIND_SCP_IRAT_CAPABILITY_CHANGED:
      hdrsmp_process_irat_capability_changed_ind();
      break;
#endif /* FEATURE_LTE_TO_HDR_OH */

    case HDRIND_INIT_NETWORK_ACQUIRED:
      HDR_MSG_PROT (MSG_LEGACY_LOW,
                    "INIT Network Acquired Indication received");
      hdrsmp_process_network_acquired ();
      break;

    case HDRIND_AMP_OPENED_PREV_SESSION:
      HDR_MSG_PROT (MSG_LEGACY_LOW,
                    "AMP Prev Session Ind. received");      
      hdrsmp_process_amp_opened_prev_session();
      break;

    case HDRIND_ALMP_EXIT_HDR_MODE:
      #if defined (FEATURE_DEDICATED_C2K_MODE) || defined (FEATURE_IRAT_PILOT_MEAS)
      /* Exiting HDR. Deactivate SMP also so we can inform CM about DO 
       * session close and let the control goes to 1x. */
      #ifdef FEATURE_IRAT_PILOT_MEAS
      if (!hdrsmp.meas_mode_is_on)
      {
        HDR_MSG_PROT( MSG_LEGACY_HIGH, 
                      "IRAT: ALMP Exit HDR Indication ignored");
      }
      else
      #endif 
      {
        HDR_MSG_PROT( MSG_LEGACY_HIGH, 
                      "DCM: ALMP Exit HDR Indication received");
        hdrsmp_dcm_deactivate();
      }
      #endif /* FEATURE_DEDICATED_C2K_MODE || FEATURE_IRAT_PILOT_MEAS */
	  /* If SMP is in CLOSING state and EXITING HDR MODE, then cleanup SMP */
      if (hdrsmps_get_state() == HDRSMPS_CLOSING)
      {
	      HDR_MSG_PROT_1( MSG_LEGACY_HIGH, 
                      "SMP: SMP CLOSE SESSION with reason %d",hdrsmp.session_close.close_reason);
          hdrsmp_close_session(hdrsmp.session_close.close_reason);	            
      }
      break;

#ifdef FEATURE_EHRPD
    case HDRIND_SCP_EHRPD_AVAILABILITY_CHANGED:
      hdrsmp_process_ehrpd_availability_changed();
      break;
#endif /* FEATURE_EHRPD */

    default:
      ERR ("Unrecognized Indication %d", ind_name, 0, 0);
      break;
  }
#ifdef FEATURE_HDRMC_DEBUG
  #error code not present
#endif
} /* hdrsmp_process_ind */

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRSMP_PROCESS_MSG

DESCRIPTION
  This function processes messages for SMP.

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
void hdrsmp_process_msg
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
  if (HDRSMP_UNPACK_MSG_ID(item_ptr, &(msg_id)) != E_SUCCESS)
  {
    ERR( "Could not extract message_id", 0, 0, 0 );
    dsm_free_packet( &item_ptr );
    return;
  }

  HDRTRACE_MSG( HDRHAI_SESSION_MANAGEMENT_PROTOCOL, msg_id,
    hdrsmps_get_state() );
#ifdef FEATURE_HDRMC_DEBUG
  #error code not present
#endif
  if (protocol_instance == HDRHAI_IN_USE)
  {
    switch ( msg_id )
    {
      case HDRSMP_KEEP_ALIVE_REQ_MSG:
        HDR_MSG_PROT (MSG_LEGACY_LOW, "Keep Alive Request Msg received");
        hdrsmp_process_keep_alive_req_msg(item_ptr);
        break;

      case HDRSMP_KEEP_ALIVE_RSP_MSG:
        HDR_MSG_PROT (MSG_LEGACY_LOW, "Keep Alive Response Msg received");
        hdrsmp_process_keep_alive_rsp_msg(item_ptr);
        break;

      case HDRSMP_SESSION_CLOSE_MSG:
        HDR_MSG_PROT (MSG_LEGACY_LOW, "Session Close Msg received");
        hdrsmp_process_session_close_msg(item_ptr);
        break;

      default:
        ERR ("HDRSMP unknown in use message %d", msg_id, 0, 0);
        break;
    }
  }
  else /* (protocol_instance == HDRHAI_IN_CONFIG) */
  {
    switch ( msg_id )
    {
      case HDRSMP_CONFIG_REQ_MSG:
        HDR_MSG_PROT (MSG_LEGACY_LOW, "Config Request Msg received");
        hdrsmp_process_config_req_msg(item_ptr);
        break;

      default:
        ERR ("HDRSMP unknown in config message %d", msg_id, 0, 0);
        break;
    }
  }
  dsm_free_packet( &item_ptr );
#ifdef FEATURE_HDRMC_DEBUG
  #error code not present
#endif
} /* hdrsmp_process_msg */

/* <EJECT> */ 
/*=============================================================================
FUNCTION HDRSMP_PROCESS_SIG

DESCRIPTION
  This function processes signals for SMP.

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
void hdrsmp_process_sig
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
    case HDRMC_SMP_KA_TIMER_SIG:
      /*-----------------------------------------------------------------------
      Keep Alive timer expired:
      -----------------------------------------------------------------------*/
      HDRTRACE_HDRMC_KA_TIMER_SIG( hdrsmps_get_state() );
      hdrsmp_process_keep_alive_timer_exp();
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
} /* hdrsmp_process_sig() */

/*===========================================================================

FUNCTION HDRSMP_GET_INCONFIG_HANDLE

DESCRIPTION
  This function returns a pointer to inconfig instance of SMP

DEPENDENCIES
  None

PARAMETERS
  None
                     
RETURN VALUE
  Pointer to inconfig instance of SMP

SIDE EFFECTS
  None

===========================================================================*/
void* hdrsmp_get_inconfig_handle(void)
{
  return (void*) &hdrsmp.session_params.negotiated;
} /* hdrsmp_get_inconfig_handle() */

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRSMP_IS_TO_MAINTAIN_EVDO_SESSION

DESCRIPTION
  This function returns if AT should maintain the EVDO session or not.
  
  Maintain EVDO session is true if any of the followings are TRUE:
  - Minimal UATI Mode (MUM) is off
  - The user has attempted an EVDO data call within the last 3 minutes
  - PPP is established with an active or dormant data session

DEPENDENCIES
  None

PARAMETERS
  None
                     
RETURN VALUE
  TRUE - Yes, AT should maintain EVDO session
  FALSE - No, AT shouldn't maintain EVDO session

SIDE EFFECTS
  None

===========================================================================*/
boolean hdrsmp_is_to_maintain_evdo_session(void)
{
  boolean is_to_maintain_evdo_session = TRUE;
    /* If MUM feature is not defined, always maintain EVDO session */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  #ifdef FEATURE_HDR_MIN_UATI_MODE

  #if defined (FEATURE_DEDICATED_C2K_MODE) || defined (FEATURE_IRAT_PILOT_MEAS)
  is_to_maintain_evdo_session = hdrsmp.session_maint.maintain_evdo_session;

  if (is_to_maintain_evdo_session)
  {
    HDR_MSG_PROT ( MSG_LEGACY_LOW, "DCM: maintain EVDO session" );
  }
  else
  {
    HDR_MSG_PROT ( MSG_LEGACY_LOW, "DCM: not maintain EVDO session" );
  }

  #ifndef FEATURE_IRAT_PILOT_MEAS
  #ifdef FEATURE_HDR_HYBRID
  if (!sd_misc_is_hybr_opr())
  {
    is_to_maintain_evdo_session = TRUE;
    HDR_MSG_PROT ( MSG_LEGACY_MED, "DCM: maintain EVDO if not hybrid mode" );
  }
  #else
  is_to_maintain_evdo_session = TRUE;
  HDR_MSG_PROT ( MSG_LEGACY_MED, "DCM: maintain EVDO if not hybrid mode" );
  #endif /* FEATURE_HDR_HYBRID */
  #endif /* FEATURE_IRAT_PILOT_MEAS */

  #else
  /* Maintain EVDO session is FALSE only if all of the followings
   * are TRUE:
   *   - Minimal UATI Mode (MUM) is ON
   *   - Maintain EVDO session timer has expired.
   *   - There is no PPP session (either active or dormant)
   * This DS707 function checks if there is a packet data session 
   * that is either active or dormant on an HDR system */
  if (hdrsmp.session_maint.mum_is_enabled &&
      (rex_get_timer(&hdrsmp.session_maint.maintain_session_timer)==0) &&
      !hdridle_is_pkt_data_sess_on_hdr()
     )
  {
    is_to_maintain_evdo_session = FALSE;
    HDR_MSG_PROT ( MSG_LEGACY_HIGH, " MUM: not maintain EVDO session" );
  }
  else
  {
    HDR_MSG_PROT_2 ( MSG_LEGACY_MED, 
                " Maintain EVDO session. MUM %d, timer remaining %d(ms)",
                hdrsmp.session_maint.mum_is_enabled,
                rex_get_timer(&hdrsmp.session_maint.maintain_session_timer) );
  }
  #endif /* FEATURE_DEDICATED_C2K_MODE || FEATURE_IRAT_PILOT_MEAS */

  #endif /* FEATURE_HDR_MIN_UATI_MODE */

  return is_to_maintain_evdo_session;

} /* hdrsmp_is_to_maintain_evdo_session */


/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRSMP_MUM_IS_ENABLED

DESCRIPTION
  This function checks if MUM mode is enabled through NV
  
DEPENDENCIES
  None

PARAMETERS
  None
                     
RETURN VALUE
  TRUE - If MUM mode is enabled through NV
  FALSE - Otherwise

SIDE EFFECTS
  None

===========================================================================*/
boolean hdrsmp_mum_is_enabled(void)
{
  return hdrsmp.session_maint.mum_is_enabled;

} /* hdrsmp_mum_is_enabled() */



/*===========================================================================

FUNCTION HDRSMP_PROCESS_CALL_ORIG

DESCRIPTION
  This function does the necessary processing for data call origination.
  It sets the maintain EVDO session timer, which implicitly enables the
  maintaining EVDO session.
  
DEPENDENCIES
  None

PARAMETERS
  None
                     
RETURN VALUE
  None
  
SIDE EFFECTS
  None

===========================================================================*/
void hdrsmp_process_call_orig(void)
{
  #ifdef FEATURE_HDR_MIN_UATI_MODE
  #if defined (FEATURE_DEDICATED_C2K_MODE) || defined (FEATURE_IRAT_PILOT_MEAS)

  /* if AT just came out of DCM mode, force AT to process SP MSG */
  if (!hdrsmp.session_maint.maintain_evdo_session)
  { 
    hdrsmp.session_maint.maintain_evdo_session = TRUE;

    /* Don: I didn't go with the ovhd_invalidate_ovhd_and_set_reacq() 
       approach because I realized we need to set reacq flag even if
       OVHD_INFO is not updated. */

    if (hdrovhd_info_is_updated(HDRHAI_SESSION_MANAGEMENT_PROTOCOL))
    {
      /* if UATI matches current subnet, no need to do anything,
         otherwise, need to force AMP to process OVHD_INFO_UPDATED
         with first_acq flag set. */
      HDR_MSG_PROT( MSG_LEGACY_MED, "DCM: proc call_orig: info updated");
      if (!hdramp_uati_match_subnet())
      {
        if (hdramp_get_dcm_reacq_diff_subnet())
        {
          HDR_MSG_PROT( MSG_LEGACY_MED, "DCM: proc call_orig: reacq-ed during DCM");
          hdrovhd_dcm_reacq_diff_subnet();
        }
        HDR_MSG_PROT( MSG_LEGACY_MED, "DCM: proc call_orig: uati mismatch");
        hdrovhd_reset_info_state();
      }
    }
    else if (hdramp_get_dcm_reacq_diff_subnet())
    {
      /* if OVHD_INFO is not current, AMP will process OVHD_INF_UPDATED
         when it becomes current. Need to force first_reacq if AT has
         reacq-ed on a different subnet during DCM */
      HDR_MSG_PROT( MSG_LEGACY_MED, "DCM: proc call_orig: reacq-ed during DCM");
      hdrovhd_dcm_reacq_diff_subnet();
    }
  }
  HDR_MSG_PROT( MSG_LEGACY_MED, "DCM: turning off DCM");
  #else /* FEATURE_DEDICATED_C2K_MODE || FEATURE_IRAT_PILOT_MEAS */
  /* Starts the timer for 3 minutes */
  (void) rex_set_timer(&hdrsmp.session_maint.maintain_session_timer,
                       HDRSMP_MAINTAIN_EVDO_SESSION_TIMER_MS);
  HDR_MSG_PROT_1 ( MSG_LEGACY_HIGH, "Data call, maintain session timer %d sec",
                   (HDRSMP_MAINTAIN_EVDO_SESSION_TIMER_MS / 1000));
  #endif /* FEATURE_DEDICATED_C2K_MODE || FEATURE_IRAT_PILOT_MEAS */
  #endif /* FEATURE_HDR_MIN_UATI_MODE */
  /* If feature is not defined, do nothing */

} /* hdrsmp_process_call_orig */

#if defined (FEATURE_DEDICATED_C2K_MODE) || defined (FEATURE_IRAT_PILOT_MEAS)
/*===========================================================================

FUNCTION HDRSMP_DCM_DEACTIVATE

DESCRIPTION
  This function commands HDRSMP to deactivate for DCM

DEPENDENCIES
  None

PARAMETERS
  None
                     
RETURN VALUE
  None
  
SIDE EFFECTS
  None

===========================================================================*/
void hdrsmp_dcm_deactivate(void)
{
  hdrsmps_state_enum_type smp_state = hdrsmps_get_state();
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  HDR_MSG_PROT_1(MSG_LEGACY_MED, 
                 "DCM: Recv. deact in SMP state %d", smp_state);

#ifndef FEATURE_IRAT_PILOT_MEAS
  if (!sd_misc_is_hybr_opr())
  {
    HDR_MSG_PROT(MSG_LEGACY_HIGH, 
                 "DCM: ignore deact while hybrid mode off");
    hdrsmp.session_close.cm_sess_close_pending = FALSE;
    hdrsmp_send_session_closed_to_cm();
    return;
  }
#endif /* FEATURE_IRAT_PILOT_MEAS */

  if (hdrsmp.session_maint.maintain_evdo_session == FALSE)
  {
    HDR_MSG_PROT(MSG_LEGACY_MED, 
                 "DCM: Recv. deact. while DCM already on");
    hdrsmp.session_close.cm_sess_close_pending = FALSE;
    hdrsmp_send_session_closed_to_cm();
    return;
  }
  else if (hdrsmp.session_close.cm_sess_close_pending)
  {
    HDR_MSG_PROT_1(MSG_LEGACY_HIGH, 
                   "DCM: Ignore dcm_deact. while deactivating, smp: %d",
                   hdrsmps_get_state());
    return;
  }
  else if (smp_state == HDRSMPS_INACTIVE)
  {
    hdrsmp.session_close.cm_sess_close_pending = FALSE;
    hdrsmp_send_session_closed_to_cm();
  }
  /* if SMP is in AMP_SETUP state, truly close the session */
  else if (smp_state == HDRSMPS_AMP_SETUP)
  {
    hdrsmp.session_close.cm_sess_close_pending = TRUE;
    hdrsmp_deactivate(HDRHAI_SESSION_MANAGEMENT_PROTOCOL,TRUE);
  }
  else if (hdrsmps_get_state() == HDRSMPS_CLOSING)
  {
    hdrsmp.session_close.cm_sess_close_pending = TRUE;
  }
  /* if AT is in the process of opening a session while 
     smp_deactivate is received, we should close the 
     session, so that when next CALL_ORIG comes, AT
     repeat the session opening process from the start. */
  else if ((hdrscps_get_state() != HDRSCPS_OPEN ) ||
       !hdrstream_stream_is_assigned(HDRHAI_APP_PACKET_APP_ANY))
  {
    HDR_MSG_PROT(MSG_LEGACY_MED, 
                 "DCM: Recv. deact. while configuring session.");
    hdrsmp.session_close.cm_sess_close_pending = TRUE;
    hdrsmp_deactivate(HDRHAI_SESSION_MANAGEMENT_PROTOCOL,FALSE);
  }
  else if (hdralmp_connection_is_open(HDRHAI_SESSION_MANAGEMENT_PROTOCOL))
  {
    HDR_MSG_PROT(MSG_LEGACY_MED, 
                 "DCM: Recv. deact. while connected");
    hdrsmp.session_close.cm_sess_close_pending = TRUE;
    hdralmp_close_connection(HDRHAI_ABORT_NORMAL_CLOSE,
                             HDRHAI_SESSION_MANAGEMENT_PROTOCOL);
  }
  else 
  {
    /* IDLE or connection set up before receiving ACK for ConReq */
    if (hdridle_is_connecting_before_ack())      
    {
      HDR_MSG_PROT(MSG_LEGACY_MED,
                   "DCM: Recv. deact. while connecting and before ACK");
      /* fully shut down and restart amac */
      hdramac_deactivate(HDRHAI_SESSION_MANAGEMENT_PROTOCOL, FALSE);
      hdramac_activate(HDRHAI_SESSION_MANAGEMENT_PROTOCOL);
    }
    else if (hdridle_is_connecting())
    {
      HDR_MSG_PROT(MSG_LEGACY_MED,
                   "DCM: Recv. deact. while connecting and before ACK");
    }
    else
    {
      HDR_MSG_PROT(MSG_LEGACY_MED,
                   "DCM: Recv. deact. while idle");
    }
    hdrsmp.session_close.cm_sess_close_pending = FALSE;
    hdrsmp_send_session_closed_to_cm();
  }
  return;
}
#endif

#ifdef FEATURE_IRAT_PILOT_MEAS
/*===========================================================================

FUNCTION HDRSMP_SET_MEAS_MODE_IS_ON_FLAG

DESCRIPTION
  This function sets the measurement mode is on flag to TRUE or FALSE.

DEPENDENCIES
  None

PARAMETERS
  meas_mode_is_on - whether to turn measeurement mode ON or OFF.

RETURN VALUE
  None
  
SIDE EFFECTS
  None

===========================================================================*/
void hdrsmp_set_meas_mode_is_on_flag
(
  boolean meas_mode_is_on
)
{
  hdrsmp.meas_mode_is_on = meas_mode_is_on;
}
#endif

/*===========================================================================

FUNCTION HDRSMP_SET_DUP_ACT_DETECT

DESCRIPTION
  This function commands HDRSMP to deactivate for DCM

DEPENDENCIES
  None

PARAMETERS
  dup_act - duplicate SMP activate detection state
                     
RETURN VALUE
  None
  
SIDE EFFECTS
  None

===========================================================================*/
void hdrsmp_set_dup_act_detect
(
  hdrsmp_dup_act_detect_enum_type dup_act_detect
)
{  
  HDR_MSG_PROT_2(MSG_LEGACY_MED, 
                 "Set SMP DupActDetect: %d->%d",
                 hdrsmp.dup_act_detect,
                 dup_act_detect);
  hdrsmp.dup_act_detect = dup_act_detect;
} /* hdrsmp_set_dupactdetect */

/*===========================================================================

FUNCTION HDRSMP_GET_CLOSE_REASON

DESCRIPTION
  This function gets the internal close reason.

DEPENDENCIES
  None

PARAMETERS
  None
                     
RETURN VALUE
  Internal close reason.
  
SIDE EFFECTS
  None

===========================================================================*/
hdrhai_session_close_reason_enum_type hdrsmp_get_close_reason( void )
{  
  return hdrsmp.session_close.close_reason;
} /* hdrsmp_get_close_reason */

/*===========================================================================

FUNCTION HDRSMP_SET_SESSION_CLOSING

DESCRIPTION
  This function sets the session closing flag, when SMP is about to enter INACTIVE.

DEPENDENCIES
  None

PARAMETERS
  boolean value to indicate whether to set to TRUE/FALSE.
  
RETURN VALUE
 None.
  
SIDE EFFECTS
  None

===========================================================================*/
void hdrsmp_set_session_closing( boolean value )
{  
  hdrsmp.session_closing = value;
} /* hdrsmp_set_session_closing */

/*===========================================================================

FUNCTION HDRSMP_GET_SESSION_CLOSING

DESCRIPTION
  This function gets the current value of session closing flag.

DEPENDENCIES
  None

PARAMETERS
  None.
  
RETURN VALUE
  TRUE - if it is SET
  FALSE - if it is not SET

SIDE EFFECTS
  None

===========================================================================*/
boolean hdrsmp_get_session_closing( void )
{  
  return hdrsmp.session_closing;
} /* hdrsmp_get_session_closing */