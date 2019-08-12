/*===========================================================================

                                  R R C S E N D

GENERAL DESCRIPTION
 The RRCSEND module consists of details of SEND CHAIN. This module encodes the
 OTA message using ASN1 and converts the same into DSN item. Then enqueue into
 watermark queue.

EXTERNALIZED FUNCTIONS

  rrcsend_send_ul_sdu()
    This function will encode the uplink OTA messages using ASN1. Then converts
    the encoded message into DSM Item and places the DSM iterm in the watermark
    queue pointed by the Logical Channel ID.

  rrcsend_send_rrc_status_message()
    This function sends an RRC Status Message. This function is typically called
    when the ASN.1 decoder fails or when there is a semantic error in a message. 

Copyright (c) 2000-2009 Qualcomm Technologies, Inc.  All Rights Reserved.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government. Diversion contrary to U.S. law prohibited.
===========================================================================*/


/*===========================================================================
                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/RRC/vcs/rrcsend.c_v   1.20   12 Jun 2002 06:57:34   rjeenaga  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/wcdma/rrc/src/rrcsend.c#1 $ $DateTime: 2016/12/13 08:00:12 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
12/08/15   nr      Made changes to get EFACH DRX status from config ptr in use
01/21/15   sa      Made changes for QTF with CRM
11/13/14   sp      Made changes to save procedure substates in crash debug info
11/11/14   sp      Made changes to increase the L2 ack timer value to 850 msec if EFACH_DRX is configured.
10/22/14   sp      Made changes to save UL OTAs to crash debug global even if qxdm logging is disabled.
08/04/14   bc      Made changes to fix compiler warnings.
07/23/14   sp      Made changes to remove F3s as part of FR19999
06/18/14   as      Made changes to delay reselection while waiting for L2 ACK for PCH transition
05/16/14   vi      Made changes to support MOB framework
05/06/14   geg     Use safer versions of memcpy() and memmove()
18/04/14   ad      Made changes to adaptively calcualte T304 for GCF TCs
04/02/14   vg      Made changes to Systematically log all the internal and external primitive handled in RRC and corresponding top level action.
11/27/13   sp      Made changes to add nv support and to save time ticks for 
                   rr to/from rrc mm to/from rrc and to add msg_type for crash debug ota changes
10/27/13   as      Made changes to backup OTAs for crash debugging
08/14/13   vg      Made changes to implement a new diag event EVENT_WCDMA_RRC_FAILURE
03/22/13   mp      Removed tmc.h include file
02/04/13   ad      Made changes to ignore the return type of 
                   rrcsmc_append_uplink_integrity_check_info function
01/03/13   ad      Made changes to reduce the redundant F3s
11/08/12   as      Made chagnes to prioritize the non meas report OTAs
10/23/12   pm      Made changes to assign pointer to NULL after de-allocation
08/23/12   ad      Fixed a bug related to logging in 
                   rrcsend_send_rrc_status_message function
08/03/12  md       Fixed compilation warning for the CR-383320 fix.
08/13/12   md      Made changes to remove padded zeros from encoded CCCH message length 
                   to compare against TM SDU size
07/24/12   geg     Upmerge WCDMA.MPSS.1.0 to WCDMA.MPSS.2.0
04/06/12   ad      Changes for HS-RACH feature
12/30/11   ad      Fixed compilation warnings
12/09/11   ad      Added code to log WCDMA_RRC_PROTOCOL_ERRORS packet
06/07/11   rl      Merged with VU_MODEM_WCDMA_RRC.01.99.50
06/02/11   ad      Added MMAL changes to return the UL trans id in OTA
06/02/11   ad      Added MMAL changes to return the trch type for the OTA
04/26/11   rl      Merged with VU_MODEM_WCDMA_RRC.01.96.50
04/21/11   ad      Fixed compilation warnings
04/19/11   ad      Changes to add callback function for MMAL support
03/29/11   rl      Merged with VU_MODEM_WCDMA_RRC.01.94.50
03/28/11   sks     Made changes to trim RRC connection request msg if its size exceeds
                   allowed TM SDU size.
02/23/11   rl      Removed setting the buffer context as it is done in the 
                   rrcasn1_free_buf() instead
02/23/11   rl      Setting the buffer pointer before encoding ASN pdu
01/28/11   rl      Merged with VU_MODEM_WCDMA_RRC.01.89.57
01/12/11   ad      Added changes for RRC SW decoupling
12/31/10   rl      Merged with VU_MODEM_WCDMA_RRC.01.86.50
10/30/10   rl      Objective systems migration changes check in 
                   on code base VU_MODEM_WCDMA_RRC.01.80.00
06/18/10   rm      Fixed compiler warnings
05/08/09   ss      Updated Copyright Information
04/29/09   ss      Made changes to compiler warnings on gcc compilers.
10/18/07   vm      Added support to hit EF on encoding failure.
07/17/07   rm      Added support for FEATURE_WCDMA_INTER_FREQ_RACH_REPORTING feature.
06/13/07   kp      Added support for trimming CU message if it exceed UL TM RLC size.
02/08/06   bd      Suppressed "symbol not accessed" lint errors for ul_dcch_msg_debug_ptr
                   and ul_ccch_msg_debug_ptr since they are used only for debugging.
02/06/06   vr      Changes for RVDS compiler
11/11/05   vr      Even if L2 Ack is not requested for a UL SDU by the procedure,
                   register for L2 Ack with RLC.This is done for debugging purposes
                   The L2 Ack will eventually be dropped by dispatcher. 
04/06/05   vr      Include rrclogging.h unconditionally for logging on all platforms
21/10/04   sm      To correct logging of integrity check info for CCCH messages 
05/13/04   kc      Initialized bit-mask for rrcstatus message to 0 to indicate that
                   non-critical extensions are not supported for the message.
04/22/04   sk      Added rrc_ prefix to two structure types encode_info_type and decode_info_type
                   and corresponding variables
04/21/04   sk      Added two variables encode_info and decode_info of type
                   encode_info_type and decode_info_type structures to get 
                   ASN.1 encoding/decoding failure information.
                   Error_fatal in different lines based on encoding failure
                   cause.
08/28/03   sk      Added two global pointers to hold the latest ul_ccch_message
                   and ul_dcch_messge that is being encoded. These pointers can 
                   be used for debugging in case of ASN.1 encoding failure
07/22/03   jh      Removed coupling with QXDM logging for RRC signaling message
                   event.
05/22/03   rj      Modified code to log the UL message after MACI is appended
                   to the message.
03/20/03   rj      Modified code to declare ERR_FATAL if ASN.1 encdoing fails.
02/05/03   sk      March2002Rel specific changes 
                   Changed  rrc_signallingConnectionReleaseRequest_chosen
                   to rrc_signallingConnectionReleaseIndication_chosen
01/13/02   ram     Linted file and cleared all existing errors.
12/12/02   jh      Added rrc_get_ul_message_type() function.  Added QXDM
                   event logging for uplink signalling messages sent.
10/24/02   rj      Updated to use rrc_malloc and rrc_free to use dual heaps 
                   in place of mem_malloc and mem_free to avoid memory
                   fragmentation
10/12/02   rj      Added function rrcsend_send_ul_sdu_with_mui to implement
                   L2 ACK for Initial Direct Transfer and Uplink Direct
                   Transfer messages for Inter-RAT handover.
09/27/02   kc      Fixed CR24884.  Added additional check to not include Int.
                   Check Info for CCCH message RRC Connection Request.
06/11/02   rj      Updated code to send MUI to RLC for every Uplink  SDU
                   Transmission to avoid discontinuity in MUIs.
04/20/02   rj      Fixed a potential memory leak if invalid RLC Size is
                   received from the RRC LLC.
04/15/02   rj      Updated function rrcsend_send_ul_sdu to forward length of 
                   SDU in bits instead of in bytes to Integrity Engine to
                   append Integrity Check Info.
04/10/02   rj      Updated function rrcsend_send_ul_sdu to allocate
                   memory for Message Authentication Code.
04/09/02   rj      Updated code for the modified interface  to function
                   rrcsmc_append_uplink_integrity_check_info.
03/29/02   rj      Added code include Integrity Check Info if Integrity
                   Protection is started.
12/06/01   kc      Fix for CR19362.  dl_logchan_type was being used 
                   instead of ul_logchan_type in log_ul_sig_message
10/24/01   rj      Initialized uninitialized vars to remove compiler warnings
09/13/01   rj      Added code de-allocate memory for unencoded PDU even though
                   SEND CHAIN fails to send the Uplink message and cleaned
                   some F3 messages. Modified function 
                   rrcsend_send_rrc_status_message to declare ERR_FATAL if
                   memory allocation fails.
08/30/01   rj      Moved function to send RRC STATUS message from 
                   RRCASN1UTIL module. Removed Feature FEATURE_QXDM_LOGGING.
                   Renamed rrc_send_rrc_status_message to
                   rrcsend_send_rrc_status_message.
07/30/01   rj      Renamed feature FEATURE_RRC_DISK_LOGGING to T_WINNT.
06/18/01   jrs     Featurized on-target signalling message logging with 
                   FEATURE_QXDM_LOGGING.
06/18/01   jrs     Changed rrcsend_log_ul_sig_message() to take a pdu_buf_type.
06/11/01   rj      Added a new function rrcsend_log_ul_sig_message to perform
                   QXDM logging for Uplink Signalling messages.
03/30/01   rj      Changed dl_sdu_num to uint32 from int to be compatible
                   with latest LED based rrcasn1.h
03/22/01   rj      Fixed a bug while enqueuing DSM items and Added code to
                   pad zeros for Transparent mode SDU.
02/26/01   rj      Replaced rrc_DL_CCCH_Message_integrityCheckInfo_present with
                   rrc_UL_DCCH_Message_integrityCheckInfo_present
02/02/01   rj      Added additional parameters like RRC procedure
                   and L2 Ack status to the rrcsend_send_ul_sdu function.
                   Fixed a bug in the send chain code and Implemented
                   Acknowledged mode transmission                    
01/30/01   rj      RRCLCM_CCCH and RRCLCM_DCCH are replaced with 
                   UE_LOGCHAN_CCCH and UE_LOGCHAN_DCCH respectively. And
                   rrclcm_channel_type_e_type is replaced with 
                   uecomdef_logchan_e_type.
12/22/00   rj      Fixed a bug in converting SDU to DSM Item and updated the
                   rrcsend_to_ul_sdu with real memory freeing routines.
                   be compatible with rrcasn1util module.
12/12/00   rj      Created file.

===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "wcdma_variation.h"
#include "rrcsend.h"
#include "rrclcm.h"
#include "rrcasn1.h"
#include "rrcasn1util.h"
#include "dsm.h"
#include "dsmutil.h"
#include "err.h"
#include "rex.h"
#include "event.h"
#include "rrcllc.h"
#include "rrcsmc.h"
#include "rrclogging.h"
#ifdef FEATURE_WCDMA_HS_RACH
#include "rrcllcoc.h"
#endif
#ifdef T_WINNT
#error code not present
#endif
#include "rrccu.h"
#include "rrcrce.h"
#include "rrcscmgr.h"
#include "rrctmr.h"
#include "stringl.h"

#ifdef TEST_FRAMEWORK
#error code not present
#endif

/*===========================================================================

                DEFINITIONS AND DECLARATIONS FOR MODULE

===========================================================================*/

/* This defines an invalid PDU number and used to initialize the local
   variable */
#define RRCSEND_INVALID_PDU_NUM 9999
#define MAC_LENGTH 32

/* The following two pointers hold the latest uplink message that was encoded 
   These pointers can be used for debugging purpose in case encoding fails */
/*lint -esym(552,ul_dcch_msg_debug_ptr,ul_ccch_msg_debug_ptr) */   
rrc_UL_DCCH_Message *ul_dcch_msg_debug_ptr=NULL;  /* Pointer to Uplink DCCH message */
rrc_UL_CCCH_Message *ul_ccch_msg_debug_ptr=NULL;  /* Pointer to Uplink CCCH message */

/* Following two variables are used to get the ASN.1 decoding/encoding status when
there is any decoding or encoding failure */
rrc_encode_info_type rrc_encode_info;
rrc_decode_info_type rrc_decode_info;

extern void rrc_get_ul_log_message( pdu_buf_type encoded_sdu,uint8 channel_type,uint8 message_type,uint8 rb_id,void *msg_ptr,uecomdef_logchan_e_type rrclog_ul_lc);

extern uint16 ueci_message_length;
/*===========================================================================

FUNCTION    rrc_get_ul_message_type

DESCRIPTION
  This function determines the type of the message received and returns this
  information as a byte suitable for reporting in a QXDM event.  This value
  is *not* the same as the ASN.1 based message type values returned by other
  similar functions.  See the rrclogging.h file for details.
  
DEPENDENCIES
  None

RETURN VALUE
  uint8 : Returns the message type if successful. Otherwise it returns 0xff.
  
SIDE EFFECTS
  None

===========================================================================*/
static uint8 rrc_get_ul_message_type(
  uecomdef_logchan_e_type ul_lc,    /* logical channel type */
  pdu_buf_type encoded_sdu          /* encoded SDU */
)
{
  uint8 asn1_msg_type;
  uint8 msg_type = 0xff;

  switch(ul_lc)
  {
    case UE_LOGCHAN_DCCH:
      asn1_msg_type = 
        rrc_get_ul_dcch_message_type(encoded_sdu.value,
                                     (uint32) encoded_sdu.length);
      switch(asn1_msg_type)
      {
        /* The following needs to be updated as and when new RRC
           procedures are implemented */
        case T_rrc_UL_DCCH_MessageType_activeSetUpdateComplete:
          msg_type = RRCLOG_ACTIVE_SET_UPDATE_COMPLETE_MSG;
          break;
        case T_rrc_UL_DCCH_MessageType_activeSetUpdateFailure:
          msg_type = RRCLOG_ACTIVE_SET_UPDATE_FAILURE_MSG;
          break;
        case T_rrc_UL_DCCH_MessageType_cellChangeOrderFromUTRANFailure:
          msg_type = RRCLOG_CELL_CHANGE_ORDER_FROM_UTRAN_FAILURE_MSG;
          break;
        case T_rrc_UL_DCCH_MessageType_counterCheckResponse:
          msg_type = RRCLOG_COUNTER_CHECK_RESPONSE_MSG;
          break;
        case T_rrc_UL_DCCH_MessageType_handoverToUTRANComplete:
          msg_type = RRCLOG_HANDOVER_TO_UTRAN_COMPLETE_MSG;
          break;
        case T_rrc_UL_DCCH_MessageType_initialDirectTransfer:
          msg_type = RRCLOG_INITIAL_DIRECT_TRANSFER_MSG;
          break;
        case T_rrc_UL_DCCH_MessageType_handoverFromUTRANFailure:
          msg_type = RRCLOG_HANDOVER_FROM_UTRAN_FAILURE_MSG;
          break;
        case T_rrc_UL_DCCH_MessageType_measurementControlFailure:
          msg_type = RRCLOG_MEASUREMENT_CONTROL_FAILURE_MSG;
          break;
        case T_rrc_UL_DCCH_MessageType_measurementReport:
          msg_type = RRCLOG_MEASUREMENT_REPORT_MSG;
          break;
        case T_rrc_UL_DCCH_MessageType_physicalChannelReconfigurationComplete:
          msg_type = RRCLOG_PHYSICAL_CHANNEL_RECONFIGURATION_COMPLETE_MSG;
          break;
        case T_rrc_UL_DCCH_MessageType_physicalChannelReconfigurationFailure:
          msg_type = RRCLOG_PHYSICAL_CHANNEL_RECONFIGURATION_FAILURE_MSG;
          break;
        case T_rrc_UL_DCCH_MessageType_radioBearerReconfigurationComplete:
          msg_type = RRCLOG_RADIO_BEARER_RECONFIGURATION_COMPLETE_MSG;
          break;
        case T_rrc_UL_DCCH_MessageType_radioBearerReconfigurationFailure:
          msg_type = RRCLOG_RADIO_BEARER_RECONFIGURATION_FAILURE_MSG;
          break;
        case T_rrc_UL_DCCH_MessageType_radioBearerReleaseComplete:
          msg_type = RRCLOG_RADIO_BEARER_RELEASE_COMPLETE_MSG;
          break;
        case T_rrc_UL_DCCH_MessageType_radioBearerReleaseFailure:
          msg_type = RRCLOG_RADIO_BEARER_RELEASE_FAILURE_MSG;
          break;
        case T_rrc_UL_DCCH_MessageType_radioBearerSetupComplete:
          msg_type = RRCLOG_RADIO_BEARER_SETUP_COMPLETE_MSG;
          break;
        case T_rrc_UL_DCCH_MessageType_radioBearerSetupFailure:
              msg_type = RRCLOG_RADIO_BEARER_SETUP_FAILURE_MSG;
              break;
        case T_rrc_UL_DCCH_MessageType_rrcConnectionReleaseComplete:
          msg_type = RRCLOG_RRC_CONNECTION_RELEASE_COMPLETE_MSG;
          break;
        case T_rrc_UL_DCCH_MessageType_rrcConnectionSetupComplete:
          msg_type = RRCLOG_RRC_CONNECTION_SETUP_COMPLETE_MSG;
          break;
        case T_rrc_UL_DCCH_MessageType_rrcStatus:
          msg_type = RRCLOG_RRC_STATUS_MSG;
          break;
        case T_rrc_UL_DCCH_MessageType_securityModeComplete:
          msg_type = RRCLOG_SECURITY_MODE_COMPLETE_MSG;
          break;
        case T_rrc_UL_DCCH_MessageType_securityModeFailure:
          msg_type = RRCLOG_SECURITY_MODE_FAILURE_MSG;
          break;
        case  T_rrc_UL_DCCH_MessageType_signallingConnectionReleaseIndication: 
          msg_type = RRCLOG_SIGNALLING_CONNECTION_RELEASE_INDICATION_MSG;
          break;
        case T_rrc_UL_DCCH_MessageType_transportChannelReconfigurationComplete:
          msg_type = RRCLOG_TRANSPORT_CHANNEL_RECONFIGURATION_COMPLETE_MSG;
          break;
        case T_rrc_UL_DCCH_MessageType_transportChannelReconfigurationFailure:
          msg_type = RRCLOG_TRANSPORT_CHANNEL_RECONFIGURATION_FAILURE_MSG;
          break;
        case T_rrc_UL_DCCH_MessageType_transportFormatCombinationControlFailure:
          msg_type = RRCLOG_TRANSPORT_FORMAT_COMBINATION_CONTROL_FAILURE_MSG;
          break;
        case T_rrc_UL_DCCH_MessageType_ueCapabilityInformation:
          msg_type = RRCLOG_UE_CAPABILITY_INFORMATION_MSG;
          break;
        case T_rrc_UL_DCCH_MessageType_uplinkDirectTransfer:
          msg_type = RRCLOG_UPLINK_DIRECT_TRANSFER_MSG;
          break;
        case T_rrc_UL_DCCH_MessageType_utranMobilityInformationConfirm:
          msg_type = RRCLOG_UTRAN_MOBILITY_INFORMATION_CONFIRM_MSG;
          break;
        case T_rrc_UL_DCCH_MessageType_utranMobilityInformationFailure:
          msg_type = RRCLOG_UTRAN_MOBILITY_INFORMATION_FAILURE_MSG;
          break;
        default:
          WRRC_MSG1_HIGH("The msg type %d is not supported",
                    msg_type );
          break;
      }  
      break;

    case UE_LOGCHAN_CCCH:
      asn1_msg_type = 
        rrc_get_ul_ccch_message_type(encoded_sdu.value,
                                     (uint32) encoded_sdu.length);
      switch(asn1_msg_type)
      {
        /* The following needs to be updated as and when new RRC
           procedures are implemented */
        case T_rrc_UL_CCCH_MessageType_cellUpdate:
          msg_type = RRCLOG_CELL_UPDATE_MSG;
          break;
        case T_rrc_UL_CCCH_MessageType_rrcConnectionRequest:
          msg_type = RRCLOG_RRC_CONNECTION_REQUEST_MSG;
          break;
        case T_rrc_UL_CCCH_MessageType_uraUpdate:
          msg_type = RRCLOG_URA_UPDATE_MSG;
          break;
        default:
          WRRC_MSG1_HIGH("The msg type %d is not supported",
                    msg_type);
          break;
      }
      break;

    default:
      WRRC_MSG1_HIGH("The SDU type %d is not supported",
                ul_lc);
      break;
  }

  return msg_type;
} /* end function rrc_get_ul_message_type */

/*===========================================================================

FUNCTION    rrc_get_ul_asn1_message_type

DESCRIPTION
  This function determines the type of the message received and returns this
  information as a byte suitable for reporting in a QXDM event.  This value
  is *not* the same as the ASN.1 based message type values returned by other
  similar functions.  See the rrclogging.h file for details.
  
DEPENDENCIES
  None

RETURN VALUE
  uint8 : Returns the message type if successful. Otherwise it returns 0xff.
  
SIDE EFFECTS
  None

===========================================================================*/
static uint8 rrc_get_ul_asn1_message_type(
  uecomdef_logchan_e_type ul_lc,    /* logical channel type */
  pdu_buf_type encoded_sdu          /* encoded SDU */
)
{
  uint8 asn1_msg_type=1;

  switch(ul_lc)
  {
    case UE_LOGCHAN_DCCH:
      asn1_msg_type = 
        rrc_get_ul_dcch_message_type(encoded_sdu.value,
                                     (uint32) encoded_sdu.length);
      
      break;

    case UE_LOGCHAN_CCCH:
      asn1_msg_type = 
        rrc_get_ul_ccch_message_type(encoded_sdu.value,
                                     (uint32) encoded_sdu.length);
      break;

    default:
      WRRC_MSG1_HIGH("The SDU type %d is not supported",ul_lc);
      break;
  }

  return (asn1_msg_type-1);
} /* end function rrc_get_ul_asn1_message_type */

/*===========================================================================

FUNCTION rrcsend_log_ul_sig_message

DESCRIPTION
  This function will log the uplink encoded OTA messages into QXDM.

DEPENDENCIES


RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static void rrcsend_log_ul_sig_message
(   
  pdu_buf_type encoded_sdu,                /* encoded SDU */
  rlc_lc_id_type   lc_id,                   /* Logical Channel Id */
  void *msg_ptr,
  rrc_proc_e_type rrc_procedure
)
{
#ifdef T_WINNT
#error code not present
#else /* QXDM logging */
  uint32 log_record_length;                 /* Local variable to store Log 
                                               Record Length */
  WCDMA_SIGNALLING_MESSAGE_type *log_record_ptr;
                                            /* Local variable to store Log
                                               record */
  uecomdef_logchan_e_type rrclog_ul_lc;     /* Variable required for logging*/
  rrclog_sig_msg_event_type sig_msg_event;  /* Variable for message rx event */
  uint8 rb_id;                              /* Variable for radio bearer ID */
  uint8 channel_type;                       /* Variable for channel type */
  uint8 asn1_msg_type;
  uint8 msg_choice;
  rrc_crash_debug_ota_pdu_type sdu_type = RRC_SDU_TYPE_NONE; 

#endif /* T_WINNT */ 

#ifdef T_WINNT
  #error code not present
#else /* QXDM logging */

  /*Get the Logical Channel Type from LCM */
  (void) rrclcm_get_ul_lc_type(lc_id, &rrclog_ul_lc);

  /* Determine and store RB and channel type info */
  rb_id = (uint8)rrclcm_get_ul_rb_id(lc_id);
  switch(rrclog_ul_lc)
  {
    case UE_LOGCHAN_CCCH:         /* Common Control Channel.*/
      channel_type = RRCLOG_SIG_UL_CCCH;
      break;

    case UE_LOGCHAN_DCCH:         /* Dedicated Control Channel.*/
      channel_type = RRCLOG_SIG_UL_DCCH;
      break;

    default:
      channel_type = 0xFF;
      WRRC_MSG1_ERROR("Sig Msg is received on invalid Logical Channel Type %d",
           rrclog_ul_lc);
      break;
  }

  /* Construct and send an event. */
  sig_msg_event.channel_type = channel_type;
  sig_msg_event.message_type = rrc_get_ul_message_type(rrclog_ul_lc,
                                                       encoded_sdu);
  asn1_msg_type = rrc_get_ul_asn1_message_type(rrclog_ul_lc,encoded_sdu);	
  rrc_get_ul_log_message(encoded_sdu,channel_type,asn1_msg_type,rb_id,msg_ptr,rrclog_ul_lc);	
  event_report_payload(EVENT_RRC_MESSAGE_SENT, sizeof(sig_msg_event),
                       (void *)&sig_msg_event);

  /*Calculate the record length */
  log_record_length = 
    FPOS(WCDMA_SIGNALLING_MESSAGE_type, signalling_message) + 
    (uint32) encoded_sdu.length;

  /*Allocate memory for Log Record. */
  log_record_ptr = (WCDMA_SIGNALLING_MESSAGE_type *)log_alloc( WCDMA_SIGNALLING_MESSAGE, log_record_length);

  /* Make sure memory allocation is successful before to proceed with 
     logging */
  if(log_record_ptr != NULL)
  {
    /* Update RB Id.*/
    log_record_ptr->rb_id = rb_id;

    /*Update Logical Channel Type */
    log_record_ptr->channel_type = channel_type;

    /* Update length of the signalling message */
    log_record_ptr->length = (uint16) encoded_sdu.length;

    /* Copy the Signalling mesaage payload to the log record */
    WCDMA_MEMCPY((void *)log_record_ptr->signalling_message, 
                 (log_record_length - FPOS(WCDMA_SIGNALLING_MESSAGE_type, signalling_message)),
                 (uint8 *) encoded_sdu.value, 
                 (uint32) encoded_sdu.length);

    /*commit the log record */
    log_commit(log_record_ptr);

  }
  else
  {
    /* The QXDM is not turn ON. Hence Do not log the OTA messages */
  }  
  
    /* Saving all UL messages here */
    sdu_type = (rrclog_ul_lc == UE_LOGCHAN_DCCH)?RRC_DCCH_UL_SDU : RRC_CCCH_UL_SDU;
  if(sdu_type == RRC_CCCH_UL_SDU)
    {
      msg_choice = rrc_get_ul_ccch_message_type(encoded_sdu.value,
                                     (uint32) encoded_sdu.length);
    }
    else
    {
      msg_choice = rrc_get_ul_dcch_message_type(encoded_sdu.value,
                                     (uint32) encoded_sdu.length);
    }
    rrc_crash_debug_save_ota((uint8 *) encoded_sdu.value,encoded_sdu.length,sdu_type,msg_choice,rrc_procedure);

#endif /*T_WINNT */
} /* rrcsend_log_sig_message */
/*===========================================================================

FUNCTION rrcsend_send_ul_sdu

DESCRIPTION
  This function will encode the uplink OTA messages using ASN1. Then converts
  the encoded message into DSM Item and places the DSM iterm in the watermark
  queue pointed by the Logical Channel ID.

DEPENDENCIES
  The l2ack_status  should not be set RRCSEND_L2ACK_REQUIRED for Transparent
  and Unacknowleded mode transmission.

RETURN VALUE
  rrcsend_status_e_type : If it is successful, then it returns RRCSEND_SUCCESS. 
  Otherwise it returns one  of the causes for failure.

SIDE EFFECTS
  None
===========================================================================*/

rrcsend_status_e_type rrcsend_send_ul_sdu
(    
  rrc_proc_e_type  rrc_procedure,          /* RRC procedure */
  void             *msg_ptr,               /* Pointer to the Uplink OTA 
                                              message */
  rlc_lc_id_type   lc_id,                  /* Logical Channel Id */
  rrcsend_l2ack_status_e_type l2ack_status /* Indicates whether L2 Ack 
                                              required or not */
)
{  
  pdu_buf_type encoded_sdu;                /* encoded SDU */
  uecomdef_logchan_e_type ul_lc_ch;        /* Logical Channel Type */
  uint32 pdu_num = RRCSEND_INVALID_PDU_NUM;
                                           /* PDU type a.k.a. SDU type*/
  dsm_item_type *dsm_item=NULL;                 /* Pointer to DSM Item */
  rrc_UL_DCCH_Message *ul_dcch_msg=NULL;        /* Pointer to Uplink DCCH
                                              message */
  rrc_UL_CCCH_Message *ul_ccch_msg=NULL;        /* Pointer to Uplink CCCH
                                              message */
  rrclcm_mui_type mui;                     /* Local variable to store 
                                              MUI */
  rrc_RB_Identity rb_id;                   /* Local variable to store
                                              RB Id. */  
  uint32 rrc_tm_sdu_size_bits;             /* The SDU size in bits for
                                              tranaparent mode */
  uint32 rrc_tm_sdu_size_bytes;            /* The SDU size in bytes for
                                              tranaparent mode */
  uint8 *rrc_tm_sdu_ptr;                   /* The pointer to the SDU
                                              in Transparent */
  uint32 index;                            /* Local parameter for 
                                              indexing. */
  uint16 l2_ack_timer_val = 0;
                                              

#ifdef FEATURE_WCDMA_HS_FACH_DRX
    ordered_config_type *config_ptr = rrcllc_get_config_ptr_in_use();
#endif

  rrcsend_status_e_type status = RRCSEND_SUCCESS;

  uint32 last_octet_pad_len =0;            /* Local parameter for CCCH pad bits */
  rrc_proc_e_type proc_waiting_for_l2_ack;

  /*initialize the global debug pointers before any assignment */
  ul_dcch_msg_debug_ptr = NULL;
  ul_ccch_msg_debug_ptr = NULL;

  /* Gets the logical Channel Type and identifies the PDU number */
  if(RRCLCM_SUCCESS == rrclcm_get_ul_lc_type(lc_id, &ul_lc_ch) )
  {
    switch (ul_lc_ch)
    {
      case UE_LOGCHAN_CCCH:
        pdu_num = rrc_UL_CCCH_Message_PDU;
        ul_ccch_msg = (rrc_UL_CCCH_Message *) msg_ptr;
        ul_ccch_msg_debug_ptr = (rrc_UL_CCCH_Message *) msg_ptr;

        /* Check if Integrity Protection started */
        if(RRCSMC_IP_NOT_STARTED == rrcsmc_check_integrity_status() ||
           ul_ccch_msg->message.t == T_rrc_UL_CCCH_MessageType_rrcConnectionRequest )
        {
          RRC_RESET_MSG_IE_PRESENT_PTR(ul_ccch_msg);     /*  Integrity Check is disabled */
        }
        else
        {
          RRC_MSG_COMMON_SET_BITMASK_IE_PTR(ul_ccch_msg,        /*  Integrity Check is enabled */
            rrc_UL_CCCH_Message,integrityCheckInfo); 

          /* Set the MAC value to Radio Bearer Id  0 for CCCH Message*/
          ul_ccch_msg->integrityCheckInfo.messageAuthenticationCode.numbits = MAC_LENGTH;
          *(ul_ccch_msg->integrityCheckInfo.messageAuthenticationCode.data) = 0;
          *(ul_ccch_msg->integrityCheckInfo.messageAuthenticationCode.data + 1) = 0;
          *(ul_ccch_msg->integrityCheckInfo.messageAuthenticationCode.data + 2) = 0;
          *(ul_ccch_msg->integrityCheckInfo.messageAuthenticationCode.data + 3) = 0;
          
          /* Set the RRC Sequence Number to 0 */
          ul_ccch_msg->integrityCheckInfo.rrc_MessageSequenceNumber = 0;
                                        
        }
        break;
      case UE_LOGCHAN_DCCH:
        pdu_num = rrc_UL_DCCH_Message_PDU;
        ul_dcch_msg = (rrc_UL_DCCH_Message *) msg_ptr;
        ul_dcch_msg_debug_ptr = (rrc_UL_DCCH_Message *) msg_ptr;
        
        /* Check if Integrity Protection started */
        if(RRCSMC_IP_NOT_STARTED == rrcsmc_check_integrity_status() )
        {
          RRC_RESET_MSG_IE_PRESENT_PTR(ul_dcch_msg);   /*  Integrity Check is disabled */
        }
        else
        {
         /*lint -e570 loss of sign is ok here since RHS operand is constant > 0 */
          RRC_MSG_COMMON_SET_BITMASK_IE_PTR(ul_dcch_msg,       /*  Integrity Check is enabled */
             rrc_UL_DCCH_Message,integrityCheckInfo); 
          /*lint +e570 end we're ok with the loss of sign */

          /* Set the MAC value to Radio Bearer Id */
          ul_dcch_msg->integrityCheckInfo.messageAuthenticationCode.numbits= 
            MAC_LENGTH;
         
          *(ul_dcch_msg->integrityCheckInfo.messageAuthenticationCode.data) = 0;
          *(ul_dcch_msg->integrityCheckInfo.messageAuthenticationCode.data + 1) = 0;
          *(ul_dcch_msg->integrityCheckInfo.messageAuthenticationCode.data + 2) = 0;
          *(ul_dcch_msg->integrityCheckInfo.messageAuthenticationCode.data + 3) = 
            (uint8)rrclcm_get_ul_rb_id(lc_id);
          
          /* Set the RRC Sequence Number to 0 */
          ul_dcch_msg->integrityCheckInfo.rrc_MessageSequenceNumber = 0;  
        }                
        break;

      default:
        WRRC_MSG1_ERROR("ERROR:Invalid Logical Channel Type: %d", ul_lc_ch);
        status = RRCSEND_LOGICAL_CHANNEL_NOT_SUPPORTED;
        break;
    }
    if(status != RRCSEND_SUCCESS)
    {
      /* The memory freeing for msg_ptr should be done here even though SEND
         CHAIN fails. This function should free memory allocated for msg_ptr and as 
         well as linked lists in the message if they are any allocated.
      */
      if(rrcasn1_free_pdu((int )pdu_num,msg_ptr)) 
      {
        /* This is a memory leak and it should never happen. */
        WRRC_MSG0_ERROR("Memory freeing is unsuccessful");
      }
      msg_ptr = NULL;
      return(status);
    }
  }
  else
  {

    /* Here we do not know the PDU Number of the unencoded message. Hence we can
       not de-allocate memory. Normally this should never occur since validity of
       the RLC Id is checked by the procedure before calling SEND CHAIN. */

    ERR_FATAL("Logical channel is not setup %d",lc_id,0,0);
  }
  /* Encodes the OTA message using ASN1. The memory for msg_ptr is allocated
     by the RRC procedure and the memory for encoded_sdu will be allocated
     by the following function */
  encoded_sdu = rrcasn1_encode_pdu(msg_ptr, (int) pdu_num);

  if(encoded_sdu.value !=NULL)              /* Checks here whether encoding is
                                               successful */
  {
    rb_id = rrclcm_get_ul_rb_id (lc_id);  /* Gets the RB Id from RLC LC Id. */

    if (rb_id == CCCH_RADIO_BEARER_ID)
    {
      /* Gets the lowest Transport Format from the LLC to enqueue TM SDU */
      rrc_tm_sdu_size_bits = rrcllc_calculate_ul_tm_ccch_rlc_size();
      /*Gets last octet pad bits*/
      last_octet_pad_len = PU_GETPADBITS(&enc_ctxt);
#ifdef FEATURE_WCDMA_HS_RACH
      /*Dont trim CCCH messages when HS-RACH is on*/
      if(TRUE == rrchsrach_camped_cell_supports_hsrach())
      {
        rrc_tm_sdu_size_bits = encoded_sdu.length * 8;
      }
#endif
      /* The TM data length is not constrained to be a multiple of 8 bits
         So extra padded zeros should be deducted from ASN encoded SDU lenth
         to get the accurate length in bits.         
       */
      if ((rrc_tm_sdu_size_bits != RRCLLC_INVALID_RLC_SIZE) &&
        ((uint32)((encoded_sdu.length * 8) - last_octet_pad_len) <= rrc_tm_sdu_size_bits))
      {
        /* Convert the received size in bits to the size in bytes */
        rrc_tm_sdu_size_bytes = rrc_tm_sdu_size_bits / 8;

        rrc_tm_sdu_size_bytes += (rrc_tm_sdu_size_bits % 8) ? 1: 0;
        /* Allocate memory for the size in bytes */
        rrc_tm_sdu_ptr =  (uint8 *) rrc_malloc (rrc_tm_sdu_size_bytes); 

      
        /* Copy the encoded byte stream */
        for(index = 0; index < (uint32) encoded_sdu.length; index++)
        {
          *(rrc_tm_sdu_ptr + index) = *(encoded_sdu.value + index);
        }
        /* Pad the addional bytes with zeros */
        for(index = (uint32) encoded_sdu.length; index < rrc_tm_sdu_size_bytes;
            index++)
        {
          *(rrc_tm_sdu_ptr + index) = 0;
        }

        if( ul_ccch_msg != NULL )
        {
          /* Update Integrity Check Info  if Integrity Protection started */
          if(RRCSMC_IP_STARTED == rrcsmc_check_integrity_status() && 
             (ul_ccch_msg->message.t != T_rrc_UL_CCCH_MessageType_rrcConnectionRequest))
          {

            (void)rrcsmc_append_uplink_integrity_check_info( rrc_tm_sdu_ptr,
               rrc_tm_sdu_size_bits, CCCH_RADIO_BEARER_ID);

            /* Fill 'encoded_sdu' with integrity check info if integrity information is added, 
                This is done for appropriate logging of CCCH messages.
                First 37 bits of RRC message contains integrity check info, hence copying first
                five bytes of RRC message.
             */
            WCDMA_MEMCPY(encoded_sdu.value,
                         encoded_sdu.length,
                         rrc_tm_sdu_ptr, 
                         5);
          }
        }
        dsm_item = dsm_sdu_to_pkt_chain_bit(rrc_tm_sdu_ptr, 
                                           (unsigned short) rrc_tm_sdu_size_bits );
                                             /* Converts the encoded SDU into 
                                                DSM item */
        /* Free the allocated for the TM SDU */  
        if (rrc_tm_sdu_ptr != NULL)
        {
          rrc_free( rrc_tm_sdu_ptr );
        }
      }
      else if(rrc_tm_sdu_size_bits != RRCLLC_INVALID_RLC_SIZE)
      {
        if( ul_ccch_msg != NULL )
        {
          if(ul_ccch_msg->message.t == T_rrc_UL_CCCH_MessageType_cellUpdate )
          {
            /*Special treatment for CU message here. Need to trim this 
             message & see if we can construct a CU OTA message which 
             will fit UL TM TB size*/
    
            /*Variable to keep track of whether CU message trimming is done. 
              This will be set to TRUE when CU message can fit in UL RLC TM TB 
              size or no more trimming possible */
            boolean is_trim_cu_done = FALSE; 
            rrccu_reset_trim_bit_mask();
            WRRC_MSG0_HIGH("CU size big so kick in CU trimming opt....");
            do
            {
              /*Start CU trimming here*/
              if(rrccu_trim_cu_msg_ul_tm_tb_size(msg_ptr,(uint32)encoded_sdu.length) == TRUE)
              {
                /*Lets discard this encoded PDU*/
                rrcasn1_free_buf(encoded_sdu.value);

                /*Lets re-encoded this PDU*/
                encoded_sdu = rrcasn1_encode_pdu(msg_ptr, (int) pdu_num);

                if(encoded_sdu.value !=NULL)
                {
                  /* Gets the lowest Transport Format from the LLC to enqueue TM SDU */
                  rrc_tm_sdu_size_bits = rrcllc_calculate_ul_tm_ccch_rlc_size();
                  /*Gets last octet pad bits*/
                  last_octet_pad_len = PU_GETPADBITS(&enc_ctxt);
                  /* The TM data length is not constrained to be a multiple of 8 bits
                    So extra padded zeros should be deducted from ASN encoded SDU lenth
                    to get the accurate length in bits.         
                  */                  
                  /*Check whether CU OTA message can be sent on UL*/
                  if ((uint32)((encoded_sdu.length * 8) - last_octet_pad_len)<= rrc_tm_sdu_size_bits)
                  {
                    /* Convert the received size in bits to the size in bytes */
                    rrc_tm_sdu_size_bytes = rrc_tm_sdu_size_bits / 8;
            
                    rrc_tm_sdu_size_bytes += (rrc_tm_sdu_size_bits % 8) ? 1: 0;
                   
                    /* Allocate memory for the size in bytes */
                    rrc_tm_sdu_ptr =  (uint8 *) rrc_malloc (rrc_tm_sdu_size_bytes); 
            
                  
                    /* Copy the encoded byte stream */
                    for(index = 0; index < (uint32) encoded_sdu.length; index++)
                    {
                      *(rrc_tm_sdu_ptr + index) = *(encoded_sdu.value + index);
                    }
                    /* Pad the addional bytes with zeros */
                    for(index = (uint32) encoded_sdu.length; index < rrc_tm_sdu_size_bytes;
                        index++)
                    {
                      *(rrc_tm_sdu_ptr + index) = 0;
                    }
            
                    if( ul_ccch_msg != NULL )
                    {
                      /* Update Integrity Check Info  if Integrity Protection started */
                      if(RRCSMC_IP_STARTED == rrcsmc_check_integrity_status() && 
                         (ul_ccch_msg->message.t != T_rrc_UL_CCCH_MessageType_rrcConnectionRequest))
                      {
                          (void)rrcsmc_append_uplink_integrity_check_info( rrc_tm_sdu_ptr,
                           rrc_tm_sdu_size_bits, CCCH_RADIO_BEARER_ID);
            
                        /* Fill 'encoded_sdu' with integrity check info if integrity information is added, 
                            This is done for appropriate logging of CCCH messages.
                            First 37 bits of RRC message contains integrity check info, hence copying first
                            five bytes of RRC message.
                         */
                        WCDMA_MEMCPY(encoded_sdu.value, 
                                     encoded_sdu.length,
                                     rrc_tm_sdu_ptr, 
                                     5);
                      }
                    }
                    dsm_item = dsm_sdu_to_pkt_chain_bit(rrc_tm_sdu_ptr, 
                                                       (unsigned short) rrc_tm_sdu_size_bits );
                                                         /* Converts the encoded SDU into 
                                                            DSM item */
                    /* Free the allocated for the TM SDU */  
                    if (rrc_tm_sdu_ptr != NULL)
                    {
                      rrc_free( rrc_tm_sdu_ptr );
                    }
                    is_trim_cu_done = TRUE;
                    rrccu_reset_trim_bit_mask();
                  }
                }
                else
                {/*ASN.1 encoding failed*/
                  status = RRCSEND_FAILED_TO_ENCODE;
                  is_trim_cu_done = TRUE;
                  rrccu_reset_trim_bit_mask();

                  ERR_FATAL("ASN1 encoding failed reason %d [8=out of memory, 17 = asn1 constraint violation , every else others]  PDU no. %d",
                                      rrc_encode_info.encode_status,
                                      rrc_encode_info.pdu_num, 0);
                      
                }
              }
              else
              {
                /* Set the status to invalid RLC Size. This implies that RRC LLC does not
                 have a suitable Tranport Format for the encoded SDU */
                WRRC_MSG2_HIGH("Invalid RLC Size is revd: %d, encoded_sdu.length = %d. Need to add more cases in CU trim options",
                                                                                 rrc_tm_sdu_size_bits,
                                                                                 (encoded_sdu.length * 8));
                status = RRCSEND_INVALID_TM_RLC_SIZE_RECEIVED;
                is_trim_cu_done = TRUE;
                rrccu_reset_trim_bit_mask();
              }
            }while(is_trim_cu_done == FALSE);
          }
          else if(ul_ccch_msg->message.t == T_rrc_UL_CCCH_MessageType_rrcConnectionRequest)
          {
            /*Special treatment for connection request message here. Need to trim this 
             message & see if we can construct a connection request OTA message which 
             will fit UL TM TB size*/
    
            WRRC_MSG0_HIGH("encoded size big so kick in connection request trimming opt....");

            /*Start RRC connection request message trimming here*/
            rrcrce_trim_conn_request_msg_ul_tm_tb_size(msg_ptr,(uint32)encoded_sdu.length);

            /*Lets discard this encoded PDU*/
            rrcasn1_free_buf(encoded_sdu.value);
            /*Lets re-encoded this PDU*/
            encoded_sdu = rrcasn1_encode_pdu(msg_ptr, (int) pdu_num);
  
            if(encoded_sdu.value !=NULL)
            {
              /* Gets the lowest Transport Format from the LLC to enqueue TM SDU */
              rrc_tm_sdu_size_bits = rrcllc_calculate_ul_tm_ccch_rlc_size();
              /* Gets last octet pad bits*/
              last_octet_pad_len = PU_GETPADBITS(&enc_ctxt);
              /* The TM data length is not constrained to be a multiple of 8 bits
                 So extra padded zeros should be deducted from ASN encoded SDU lenth
                  to get the accurate length in bits.         
              */
              /*Check whether connection request OTA message can be sent on UL*/
              if ((uint32)((encoded_sdu.length * 8) - last_octet_pad_len )<= rrc_tm_sdu_size_bits)
              {
                /* Convert the received size in bits to the size in bytes */
                rrc_tm_sdu_size_bytes = rrc_tm_sdu_size_bits / 8;
        
                rrc_tm_sdu_size_bytes += (rrc_tm_sdu_size_bits % 8) ? 1: 0;
               
                /* Allocate memory for the size in bytes */
                rrc_tm_sdu_ptr =  (uint8 *) rrc_malloc (rrc_tm_sdu_size_bytes); 
        
              
                /* Copy the encoded byte stream */
                for(index = 0; index < (uint32) encoded_sdu.length; index++)
                {
                  *(rrc_tm_sdu_ptr + index) = *(encoded_sdu.value + index);
                }
                /* Pad the addional bytes with zeros */
                for(index = (uint32) encoded_sdu.length; index < rrc_tm_sdu_size_bytes;
                    index++)
                {
                  *(rrc_tm_sdu_ptr + index) = 0;
                }
        
                dsm_item = dsm_sdu_to_pkt_chain_bit(rrc_tm_sdu_ptr, 
                                                   (unsigned short) rrc_tm_sdu_size_bits );
                                                     /* Converts the encoded SDU into 
                                                        DSM item */
                /* Free the allocated for the TM SDU */  
                if (rrc_tm_sdu_ptr != NULL)
                {
                  rrc_free( rrc_tm_sdu_ptr );
                }
              }
              else
              {
                ERR_FATAL("Even after trimming, encoded msg length:%d  exceeds:%d",
                          (encoded_sdu.length * 8),
                          rrc_tm_sdu_size_bits, 0);
              }
            }
            else
            {/*ASN.1 encoding failed*/
              status = RRCSEND_FAILED_TO_ENCODE;

              ERR_FATAL("ASN1 encoding failed reason %d [8=out of memory, 17 = asn1 constraint violation , every else others]  PDU no. %d",
                                  rrc_encode_info.encode_status,
                                  rrc_encode_info.pdu_num, 0);
                  
            }
          }
          else
          {
            /* Set the status to invalid RLC Size. This implies that RRC LLC does not
               have a suitable Tranport Format for the encoded SDU */
            status = RRCSEND_INVALID_TM_RLC_SIZE_RECEIVED;
          }
        }
        else
        {
          /* Set the status to invalid RLC Size. This implies that RRC LLC does not
             have a suitable Tranport Format for the encoded SDU */
          status = RRCSEND_INVALID_TM_RLC_SIZE_RECEIVED;
        }
      }
      else        
      {
        /* Set the status to invalid RLC Size. This implies that RRC LLC does not
           have a suitable Tranport Format for the encoded SDU */
        status = RRCSEND_INVALID_TM_RLC_SIZE_RECEIVED;
      }
    }
    else
    {
      /* Update Integrity Check Info  if Integrity Protection started */
        if(RRCSMC_IP_STARTED == rrcsmc_check_integrity_status() )
        {
           if(rrcsmc_append_uplink_integrity_check_info( 
             (byte *) encoded_sdu.value, 
             (uint32) encoded_sdu.length * 8,
             rb_id))
           {
             ERR_FATAL("Integrity append failed", 0, 0, 0);
           }
        }   
      /* rrcsmc_append_uplink_integrity_check_info */
      dsm_item = dsm_sdu_to_pkt_chain_byte(encoded_sdu.value, 
                                         (unsigned short)encoded_sdu.length);
                                           /* Converts the encoded SDU into 
                                              DSM item */       
       /*Store the size of the UECI message sent out*/                                    
      if((ul_dcch_msg != NULL) && (ul_dcch_msg->message.t == T_rrc_UL_DCCH_MessageType_ueCapabilityInformation))
      {
        ueci_message_length = encoded_sdu.length;
        WRRC_MSG1_HIGH("T304_DEBUG Stored UECI length : %d",ueci_message_length);
      }							 
#ifdef FEATURE_WCDMA_SRB_PRIO_OVER_MEAS 
      /*To prioritize non measreport messages while sending OTAs to NW*/
      if((dsm_item != NULL) && (ul_dcch_msg != NULL))
      {
        dsm_item->priority = DSM_HIGHEST;
        if (ul_dcch_msg->message.t == T_rrc_UL_DCCH_MessageType_measurementReport)
        {
          dsm_item->priority = DSM_NORMAL;
        }
      }
 #endif

    }   
    if(status == RRCSEND_INVALID_TM_RLC_SIZE_RECEIVED)
    {
      WRRC_MSG1_HIGH("Invalid RLC Size is revd: %d",rrc_tm_sdu_size_bits);
    }
    /* log the encoded uplink signalling message */
    rrcsend_log_ul_sig_message(encoded_sdu, lc_id,msg_ptr,rrc_procedure); 

    /* memory freeing for encoded_sdu should be done here using OSS compiler
       functions.  It doesn't matter whether SDU is converted successfully 
       into DSM item or not */
    
    rrcasn1_free_buf(encoded_sdu.value);
  
    if(status != RRCSEND_SUCCESS)
    {
      if(rrcasn1_free_pdu((int )pdu_num,msg_ptr)) 
      {
        /* This is a memory leak and it should never happen.*/
        WRRC_MSG0_ERROR("Memory freeing is unsuccessful");
      }
      msg_ptr = NULL;
      return(status);
    }

    if(dsm_item != NULL)                   /* If conversion is successful, then
                                              enqueues the same into the 
                                              watermark queue */
    {      
      /* Switch on Radio Bearer in order to know the mode of transmission */
   
      switch(rb_id)
      {
        case CCCH_RADIO_BEARER_ID:
        case DCCH_UM_RADIO_BEARER_ID:
          if(l2ack_status == RRCSEND_L2ACK_REQUIRED)
          {
            WRRC_MSG1_HIGH("Incomptible L2 Req from the proc : rrc_proc_e_type_value%d",
                      rrc_procedure);
            status = RRCSEND_INCOMPATIBLE_L2ACK_REQUEST;
          }
          break;

        case DCCH_AM_RADIO_BEARER_ID:
        case DCCH_DT_HIGH_PRI_RADIO_BEARER_ID:
        case DCCH_DT_LOW_PRI_RADIO_BEARER_ID:
          if(l2ack_status == RRCSEND_L2ACK_REQUIRED)
          {
            dsm_set_sdu_cnf (dsm_item, TRUE); /* Informs RLC L2 ACK for this
                                                 SDU is required */
            /* Get MUI from the Logical Channel Manager */
            if(RRCLCM_SUCCESS == rrclcm_get_mui(rrc_procedure,
                                                rb_id, TRUE, &mui))
            {
              dsm_write_sdu_mui(dsm_item,mui);
              WRRC_MSG2_HIGH("The MUI: %d for RB Id: %d", mui, rb_id);
#ifdef TEST_FRAMEWORK
                #error code not present
#endif
            }
            else
            {
              WRRC_MSG0_ERROR("The LCM is unable to allocate MUI");
            }
          }
          else
          {
            dsm_set_sdu_cnf (dsm_item, TRUE); /* Informs RLC L2 ACK for this
                                                 SDU is required.This is done
                                                 for debugging purposes.The L2 Ack
                                                 will get dropped in the dispatcher */
            /* Get MUI from the Logical Channel Manager */
            if(RRCLCM_SUCCESS == rrclcm_get_mui(rrc_procedure,
                                                rb_id, FALSE, &mui))
            {
              dsm_write_sdu_mui(dsm_item,mui);
              WRRC_MSG2_HIGH("The MUI: %d for RB Id: %d", mui, rb_id);
#ifdef TEST_FRAMEWORK
                            #error code not present
#endif
            }
            else
            {
              WRRC_MSG0_ERROR("The LCM is unable to allocate MUI");
            }
          }
          
          break;

        default:
          WRRC_MSG1_HIGH("Invalid Radio Bearer ID: %d",rb_id);
          status = RRCSEND_INVALID_RB_ID;
          break;
      }        
      if(status == RRCSEND_SUCCESS)
      {
        dsm_enqueue(rrclcm_get_ul_watermark_ptr(lc_id), &dsm_item);
      } 
      else
      {
        WRRC_MSG0_HIGH("Failed to enqueue UL SDU");
      }     
    }
    else /* Failed convert the SDU into DSM Items */
    {
      WRRC_MSG0_ERROR("Failed to convert SDU to DSM Item");
      status = RRCSEND_FAILED_CONVERT_SDU_TO_PACKECT;
    } 
  }
  else /* ASN1 Encoding is failed */
  {
    status = RRCSEND_FAILED_TO_ENCODE;

      ERR_FATAL("ASN1 encoding failed reason %d [8=out of memory, 17 = asn1 constraint violation , every else others]  PDU no. %d",
                        rrc_encode_info.encode_status,
                        rrc_encode_info.pdu_num, 0);
      
  }

  /* The memory freeing for msg_ptr should be done here. using OSS compiler
     functions. This function should free memory allocated for msg_ptr and as 
     well as linked lists in the message if they are any allocated.*/
 #ifdef TEST_FRAMEWORK
  #error code not present
#endif   
  if(rrcasn1_free_pdu((int )pdu_num,msg_ptr)) 
  {
    /* This is a memory leak and it should never happen.*/
    WRRC_MSG0_ERROR("Memory freeing is unsuccessful");
  }
  if(status == RRCSEND_SUCCESS)
  {
    if((rrc_check_if_any_proc_waiting_for_l2_ack_for_pch_transition(&proc_waiting_for_l2_ack)) &&
        (proc_waiting_for_l2_ack  == rrc_procedure) &&
        (proc_waiting_for_l2_ack != RRC_PROCEDURE_CU))
    {
 #ifdef FEATURE_WCDMA_HS_FACH_DRX
      if(TRUE == rrcenhstate_get_e_fach_drx_status(&(config_ptr->l1_hsdpa_info)))
      {
         /* Set timer value as 850 ms for EDRX*/
         l2_ack_timer_val = RRC_WAIT_TIME_FOR_L2_ACK_FOR_PCH_TRANSITION_FOR_EDRX;
      }
      else
 #endif
      {
         l2_ack_timer_val = RRC_WAIT_TIME_FOR_L2_ACK_FOR_PCH_TRANSITION;
      }
      rrctmr_start_timer(RRCTMR_WAIT_FOR_L2_ACK_FOR_PCH_TRANSITION_TIMER, l2_ack_timer_val);
    }
  }

  return (status);
}

/*===========================================================================

FUNCTION rrcsend_send_ul_sdu_with_mui

DESCRIPTION
  This function will encode the uplink OTA messages using ASN1. Then converts
  the encoded message into DSM Item and places the DSM iterm in the watermark
  queue pointed by the Logical Channel ID.

DEPENDENCIES
  The l2ack_status  should not be set RRCSEND_L2ACK_REQUIRED for Transparent
  and Unacknowleded mode transmission.

RETURN VALUE
  rrcsend_status_e_type : If it is successful, then it returns RRCSEND_SUCCESS. 
  Otherwise it returns one  of the causes for failure.

SIDE EFFECTS
  None
===========================================================================*/

rrcsend_status_e_type rrcsend_send_ul_sdu_with_mui
(    
  rrc_proc_e_type  rrc_procedure,          /* RRC procedure */
  void             *msg_ptr,               /* Pointer to the Uplink OTA 
                                              message */
  rlc_lc_id_type   lc_id,                  /* Logical Channel Id */
  rrcsend_l2ack_status_e_type l2ack_status,
                                           /* Indicates whether L2 Ack 
                                              required or not */
  uint8 *msg_mui                           /* MUI Tagged to the message
                                             transmitted */
)
{  
  pdu_buf_type encoded_sdu;                /* encoded SDU */
  uecomdef_logchan_e_type ul_lc_ch;        /* Logical Channel Type */
  uint32 pdu_num = RRCSEND_INVALID_PDU_NUM;
                                           /* PDU type a.k.a. SDU type*/
  dsm_item_type *dsm_item=NULL;                 /* Pointer to DSM Item */
  rrc_UL_DCCH_Message *ul_dcch_msg;        /* Pointer to Uplink DCCH
                                              message */
  rrc_UL_CCCH_Message *ul_ccch_msg;        /* Pointer to Uplink CCCH
                                              message */
  rrclcm_mui_type mui;                     /* Local variable to store 
                                              MUI */
  rrc_RB_Identity rb_id;                   /* Local variable to store
                                              RB Id. */  
  uint32 rrc_tm_sdu_size_bits;             /* The SDU size in bits for
                                              tranaparent mode */
  uint32 rrc_tm_sdu_size_bytes;            /* The SDU size in bytes for
                                              tranaparent mode */
  uint8 *rrc_tm_sdu_ptr;                   /* The pointer to the SDU
                                              in Transparent */
  uint32 index;                            /* Local parameter for 
                                              indexing. */
                                              

  rrcsend_status_e_type status = RRCSEND_SUCCESS;
  
  uint32 last_octet_pad_len =0; 		   /* Local parameter for CCCH pad bits */

  /*initialize the global debug pointers before any assignment */
  ul_dcch_msg_debug_ptr = NULL;
  ul_ccch_msg_debug_ptr = NULL;

  /* Gets the logical Channel Type and identifies the PDU number */
  if(RRCLCM_SUCCESS == rrclcm_get_ul_lc_type(lc_id, &ul_lc_ch) )
  {
    switch (ul_lc_ch)
    {
      case UE_LOGCHAN_CCCH:
        pdu_num = rrc_UL_CCCH_Message_PDU;
        ul_ccch_msg = (rrc_UL_CCCH_Message *) msg_ptr;
        ul_ccch_msg_debug_ptr = (rrc_UL_CCCH_Message *) msg_ptr;

        /* Check if Integrity Protection started */
        if(RRCSMC_IP_NOT_STARTED == rrcsmc_check_integrity_status() ||
           ul_ccch_msg->message.t == T_rrc_UL_CCCH_MessageType_rrcConnectionRequest )
        {
          RRC_RESET_MSG_IE_PRESENT_PTR(ul_ccch_msg);     /*  Integrity Check is disabled */
        }
        else
        {
          /*lint -e570 we're ok with the loss of sign */
          RRC_MSG_COMMON_SET_BITMASK_IE_PTR(ul_ccch_msg,        /*  Integrity Check is enabled */
            rrc_UL_CCCH_Message,integrityCheckInfo); 
          /*lint +e570 we're ok with the loss of sign */

          /* Set the MAC value to Radio Bearer Id  0 for CCCH Message*/
          ul_ccch_msg->integrityCheckInfo.messageAuthenticationCode.numbits= 
            MAC_LENGTH;
         
          *(ul_ccch_msg->integrityCheckInfo.messageAuthenticationCode.data) = 0;
          *(ul_ccch_msg->integrityCheckInfo.messageAuthenticationCode.data + 1) = 0;
          *(ul_ccch_msg->integrityCheckInfo.messageAuthenticationCode.data + 2) = 0;
          *(ul_ccch_msg->integrityCheckInfo.messageAuthenticationCode.data + 3) = 0;
          
          /* Set the RRC Sequence Number to 0 */
          ul_ccch_msg->integrityCheckInfo.rrc_MessageSequenceNumber = 0;
                                        
        }
        break;
      case UE_LOGCHAN_DCCH:
        pdu_num = rrc_UL_DCCH_Message_PDU;
        ul_dcch_msg = (rrc_UL_DCCH_Message *) msg_ptr;
        ul_dcch_msg_debug_ptr = (rrc_UL_DCCH_Message *) msg_ptr;
        
        /* Check if Integrity Protection started */
        if(RRCSMC_IP_NOT_STARTED == rrcsmc_check_integrity_status() )
        {
          RRC_RESET_MSG_IE_PRESENT_PTR(ul_dcch_msg);   /*  Integrity Check is disabled */
        }
        else
        {
          /*lint -e570 we're ok with the loss of sign */
         RRC_MSG_COMMON_SET_BITMASK_IE_PTR(ul_dcch_msg,        /*  Integrity Check is enabled */
            rrc_UL_DCCH_Message,integrityCheckInfo); 
          /*lint +e570 we're ok with the loss of sign */

          /* Set the MAC value to Radio Bearer Id */
          ul_dcch_msg->integrityCheckInfo.messageAuthenticationCode.numbits= 
            MAC_LENGTH;
          
          *(ul_dcch_msg->integrityCheckInfo.messageAuthenticationCode.data) = 0;
          *(ul_dcch_msg->integrityCheckInfo.messageAuthenticationCode.data + 1) = 0;
          *(ul_dcch_msg->integrityCheckInfo.messageAuthenticationCode.data + 2) = 0;
          *(ul_dcch_msg->integrityCheckInfo.messageAuthenticationCode.data + 3) = 
            (uint8)rrclcm_get_ul_rb_id(lc_id);
          
          /* Set the RRC Sequence Number to 0 */
          ul_dcch_msg->integrityCheckInfo.rrc_MessageSequenceNumber = 0;  
        }                
        break;

      default:
        WRRC_MSG1_ERROR("ERROR:Invalid Logical Channel Type: %d", ul_lc_ch);
        status = RRCSEND_LOGICAL_CHANNEL_NOT_SUPPORTED;
        break;
    }
    if(status != RRCSEND_SUCCESS)
    {
      /* The memory freeing for msg_ptr should be done here even though SEND
         CHAIN fails. This function should free memory allocated for msg_ptr and as 
         well as linked lists in the message if they are any allocated. */
      if(rrcasn1_free_pdu((int )pdu_num,msg_ptr)) 
      {
        /* This is a memory leak and it should never happen.*/
        ERR_FATAL("Memory freeing is unsuccessful",0,0,0);
      }
      msg_ptr = NULL;
      return(status);
    }
  }
  else
  {

    /* Here we do not know the PDU Number of the unencoded message. Hence we can
       not de-allocate memory. Normally this should never occur since validity of
       the RLC Id is checked by the procedure before calling SEND CHAIN. */

    ERR_FATAL("Logical channel is not setup %d",lc_id,0,0);
  }
  /* Encodes the OTA message using ASN1. The memory for msg_ptr is allocated
     by the RRC procedure and the memory for encoded_sdu will be allocated
     by the following function */
  encoded_sdu = rrcasn1_encode_pdu(msg_ptr, (int) pdu_num);

  if(encoded_sdu.value !=NULL)              /* Checks here whether encoding is
                                               successful */
  {
    rb_id = rrclcm_get_ul_rb_id (lc_id);  /* Gets the RB Id from RLC LC Id. */

    if (rb_id == CCCH_RADIO_BEARER_ID)
    {
      /* Gets the lowest Transport Format from the LLC to enqueue TM SDU */
      rrc_tm_sdu_size_bits = rrcllc_calculate_ul_tm_ccch_rlc_size();
      /*Gets last octet pad bits*/
      last_octet_pad_len = PU_GETPADBITS(&enc_ctxt);
#ifdef FEATURE_WCDMA_HS_RACH
      /*Dont trim CCCH messages when HS-RACH is on*/
      if(TRUE == rrchsrach_camped_cell_supports_hsrach())
      {
        rrc_tm_sdu_size_bits = encoded_sdu.length * 8;
      }
#endif
       /* The TM data length is not constrained to be a multiple of 8 bits
         So extra padded zeros should be deducted from ASN encoded SDU lenth
         to get the accurate length in bits.         
       */
      if ((rrc_tm_sdu_size_bits != RRCLLC_INVALID_RLC_SIZE)&&
        ((uint32)((encoded_sdu.length * 8) - last_octet_pad_len ) <= rrc_tm_sdu_size_bits))
      {
        /* Convert the received size in bits to the size in bytes */
        rrc_tm_sdu_size_bytes = rrc_tm_sdu_size_bits / 8;

        rrc_tm_sdu_size_bytes += (rrc_tm_sdu_size_bits % 8) ? 1: 0;
       
        /* Allocate memory for the size in bytes */
        rrc_tm_sdu_ptr =  (uint8 *) rrc_malloc (rrc_tm_sdu_size_bytes); 

        /* Copy the encoded byte stream */
        for(index = 0; index < (uint32) encoded_sdu.length; index++)
        {
          *(rrc_tm_sdu_ptr + index) = *(encoded_sdu.value + index);
        }
        /* Pad the addional bytes with zeros */
        for(index = (uint32) encoded_sdu.length; index < rrc_tm_sdu_size_bytes;
            index++)
        {
          *(rrc_tm_sdu_ptr + index) = 0;
        }
               
        /* Update Integrity Check Info  if Integrity Protection started */
        if(RRCSMC_IP_STARTED == rrcsmc_check_integrity_status() )
        {
          (void)rrcsmc_append_uplink_integrity_check_info( rrc_tm_sdu_ptr,
              rrc_tm_sdu_size_bits, CCCH_RADIO_BEARER_ID);
              
        }                          
        dsm_item = dsm_sdu_to_pkt_chain_bit(rrc_tm_sdu_ptr, 
                                           (unsigned short) rrc_tm_sdu_size_bits );
                                             /* Converts the encoded SDU into 
                                                DSM item */
        /* Free the allocated for the TM SDU */  
        if (rrc_tm_sdu_ptr != NULL)
        {
          rrc_free( rrc_tm_sdu_ptr );
        }
      }
      else
      {
        /* Set the status to invalid RLC Size. This implies that RRC LLC does not
           have a suitable Tranport Format for the encoded SDU */
        WRRC_MSG1_HIGH("Invalid RLC Size is revd: %d",rrc_tm_sdu_size_bits);

        status = RRCSEND_INVALID_TM_RLC_SIZE_RECEIVED;
      }

    }
    else
    {
      /* Update Integrity Check Info  if Integrity Protection started */
        if(RRCSMC_IP_STARTED == rrcsmc_check_integrity_status() )
        {
           if(rrcsmc_append_uplink_integrity_check_info( 
             (byte *) encoded_sdu.value, 
             (uint32) encoded_sdu.length * 8,
             rb_id))
           {
             ERR_FATAL("Integrity append failed", 0, 0, 0);
           }
        }   
      /* rrcsmc_append_uplink_integrity_check_info */
      dsm_item = dsm_sdu_to_pkt_chain_byte(encoded_sdu.value, 
                                         (unsigned short)encoded_sdu.length);
                                           /* Converts the encoded SDU into 
                                              DSM item */       
#ifdef FEATURE_WCDMA_SRB_PRIO_OVER_MEAS 
      /*To prioritize non measreport messages while sending OTAs to NW*/
      if((dsm_item != NULL) && (ul_dcch_msg != NULL))
      {
        dsm_item->priority = DSM_HIGHEST;
        if (ul_dcch_msg->message.t == T_rrc_UL_DCCH_MessageType_measurementReport)
        {
          dsm_item->priority = DSM_NORMAL;
        }
      }
#endif

    }     

    /* log the encoded uplink signalling message */
    rrcsend_log_ul_sig_message(encoded_sdu, lc_id,msg_ptr,rrc_procedure ); 

    /* memory freeing for encoded_sdu should be done here using OSS compiler
       functions.  It doesn't matter whether SDU is converted successfully 
       into DSM item or not */
    
    rrcasn1_free_buf(encoded_sdu.value);
  
    if(status != RRCSEND_SUCCESS)
    {
      if(rrcasn1_free_pdu((int )pdu_num,msg_ptr)) 
      {
        /* This is a memory leak and it should never happen.*/
        WRRC_MSG0_ERROR("Memory freeing is unsuccessful");
      }
      msg_ptr = NULL;
      return(status);
    }

#ifdef TEST_FRAMEWORK
		#error code not present
#endif


    if(dsm_item != NULL)                   /* If conversion is successful, then
                                              enqueues the same into the 
                                              watermark queue */
    {      
      /* Switch on Radio Bearer in order to know the mode of transmission */
   
      switch(rb_id)
      {
        case CCCH_RADIO_BEARER_ID:
        case DCCH_UM_RADIO_BEARER_ID:
          if(l2ack_status == RRCSEND_L2ACK_REQUIRED)
          {
            WRRC_MSG1_HIGH("Incomptible L2 Req from the proc : rrc_proc_e_type_value%d",
                      rrc_procedure);
            status = RRCSEND_INCOMPATIBLE_L2ACK_REQUEST;
          }
          break;

        case DCCH_AM_RADIO_BEARER_ID:
        case DCCH_DT_HIGH_PRI_RADIO_BEARER_ID:
        case DCCH_DT_LOW_PRI_RADIO_BEARER_ID:
          if(l2ack_status == RRCSEND_L2ACK_REQUIRED)
          {
            dsm_set_sdu_cnf (dsm_item, TRUE); /* Informs RLC L2 ACK for this
                                                 SDU is required */
            /* Get MUI from the Logical Channel Manager */
            if(RRCLCM_SUCCESS == rrclcm_get_mui(rrc_procedure,
                                                rb_id, TRUE, &mui))
            {
              dsm_write_sdu_mui(dsm_item,mui);
              WRRC_MSG2_HIGH("The MUI: %d for RB Id: %d", mui, rb_id);
              *msg_mui = mui;
              
#ifdef TEST_FRAMEWORK
                                          #error code not present
#endif
            }
            else
            {
              WRRC_MSG0_ERROR("The LCM is unable to allocate MUI");
            }
          }
          else
          {
            dsm_set_sdu_cnf (dsm_item, FALSE); /* Informs RLC L2 ACK for this
                                                 SDU is not required */
            /* Get MUI from the Logical Channel Manager */
            if(RRCLCM_SUCCESS == rrclcm_get_mui(rrc_procedure,
                                                rb_id, FALSE, &mui))
            {
              dsm_write_sdu_mui(dsm_item,mui);
              WRRC_MSG2_HIGH("The MUI: %d for RB Id: %d", mui, rb_id);
              *msg_mui = mui;
              
#ifdef TEST_FRAMEWORK
                                          #error code not present
#endif
            }
            else
            {
              WRRC_MSG0_ERROR("The LCM is unable to allocate MUI");
            }
          }
          
          break;

        default:
          WRRC_MSG1_HIGH("Invalid Radio Bearer ID: %d",rb_id);
          status = RRCSEND_INVALID_RB_ID;
          break;
      }        
      if(status == RRCSEND_SUCCESS)
      {
        dsm_enqueue(rrclcm_get_ul_watermark_ptr(lc_id), &dsm_item);
      } 
      else
      {
        WRRC_MSG0_HIGH("Failed to enqueue UL SDU");
      }     
    }
    else /* Failed convert the SDU into DSM Items */
    {
      WRRC_MSG0_ERROR("Failed to convert SDU to DSM Item");
      status = RRCSEND_FAILED_CONVERT_SDU_TO_PACKECT;
    } 
  }
  else /* ASN1 Encoding is failed */
  {
    status = RRCSEND_FAILED_TO_ENCODE;

    ERR_FATAL("ASN1 encoding failed reason %d [8=out of memory, 17 = asn1 constraint violation , every else others]  PDU no. %d",
                        rrc_encode_info.encode_status,
                        rrc_encode_info.pdu_num, 0);
      
  }

  /* The memory freeing for msg_ptr should be done here. using OSS compiler
     functions. This function should free memory allocated for msg_ptr and as 
     well as linked lists in the message if they are any allocated.*/
  if(rrcasn1_free_pdu((int )pdu_num,msg_ptr)) 
  {
    /* This is a memory leak and it should never happen.*/
    WRRC_MSG0_ERROR("Memory freeing is unsuccessful");
  }

  return (status);
}


/*===========================================================================

FUNCTION RRCSEND_SEND_RRC_STATUS_MESSAGE

DESCRIPTION
  This function sends an RRC Status Message. This function is typically called
  when the ASN.1 decoder fails or when there is a semantic error in a message.

DEPENDENCIES
  An uplink DCCH must be set up.

RETURN VALUE
  SUCCESS if the message was successfully sent, otherwise FAILURE.

SIDE EFFECTS
  None.

===========================================================================*/

uecomdef_status_e_type rrcsend_send_rrc_status_message
(
  uint32 protocol_error_choice, /* The protocol error cause to be placed in
                                   the message*/
  rrc_RRC_TransactionIdentifier transaction_id,
                                /* Transaction Id received */

  rrc_ReceivedMessageType received_msg_id, 
                                 /* Identification of the Recvd message.*/
  rrc_proc_e_type rrc_procedure,           /* RRC procedure */
  rlc_lc_id_type   lc_id,                  /* Logical Channel Id */
  rrcsend_l2ack_status_e_type l2ack_status /* Indicates whether L2 Ack 
                                              required or not */
)
{
  rrc_UL_DCCH_Message *msg;     /* Pointer to the RRC Status Message */

  /* Allocate a buffer for an uplink DCCH message. This buffer is released
   * by rrcsend_send_ul_sdu().
   */
  msg = (rrc_UL_DCCH_Message *)rrc_malloc( 
                                          sizeof (struct rrc_UL_DCCH_Message));

  /* Identify the message as an RRC Status Message */
  msg->message.t = T_rrc_UL_DCCH_MessageType_rrcStatus;

  /* First set bit mask to 0 */
  RRC_RESET_MSG_IE_PRESENT(msg->message.u.rrcStatus);

  /* Set the diagnostic type to Type 1 */
  msg->message.u.rrcStatus.protocolErrorInformation.diagnosticsType.t = 
    T_rrc_ProtocolErrorMoreInformation_diagnosticsType_type1;

  
  msg->message.u.rrcStatus.protocolErrorInformation.diagnosticsType.u.type1 
   = rtxMemAllocTypeZ (&enc_ctxt, rrc_ProtocolErrorMoreInformation_type1);
  /* Set the protocol error cause IE to be what was passed to this function */
  msg->message.u.rrcStatus.protocolErrorInformation.diagnosticsType.u.type1->
    t = protocol_error_choice;

  switch(protocol_error_choice)
  {
    case T_rrc_ProtocolErrorMoreInformation_type1_asn1_ViolationOrEncodingError:
    case T_rrc_ProtocolErrorMoreInformation_type1_messageTypeNonexistent:
      /*Do nothing */
      break;

    case T_rrc_ProtocolErrorMoreInformation_type1_messageNotCompatibleWithReceiverState:

      msg->message.u.rrcStatus.protocolErrorInformation.diagnosticsType.u.type1->
        u.messageNotCompatibleWithReceiverState
        = rtxMemAllocTypeZ (&enc_ctxt, rrc_IdentificationOfReceivedMessage);
      
      msg->message.u.rrcStatus.protocolErrorInformation.diagnosticsType.u.type1->
        u.messageNotCompatibleWithReceiverState->rrc_TransactionIdentifier = 
        transaction_id;
      
      
      msg->message.u.rrcStatus.protocolErrorInformation.diagnosticsType.u.type1->
        u.messageNotCompatibleWithReceiverState->receivedMessageType =
        received_msg_id;
      break;

    case T_rrc_ProtocolErrorMoreInformation_type1_ie_ValueNotComprehended:
        msg->message.u.rrcStatus.protocolErrorInformation.diagnosticsType.u.type1->
        u.ie_ValueNotComprehended
        = rtxMemAllocTypeZ (&enc_ctxt, rrc_IdentificationOfReceivedMessage);
         
      msg->message.u.rrcStatus.protocolErrorInformation.diagnosticsType.u.type1->
        u.ie_ValueNotComprehended->rrc_TransactionIdentifier = 
        transaction_id;

      msg->message.u.rrcStatus.protocolErrorInformation.diagnosticsType.u.type1->
        u.ie_ValueNotComprehended->receivedMessageType =
        received_msg_id;
      break;

    case T_rrc_ProtocolErrorMoreInformation_type1_conditionalInformationElementError:

      msg->message.u.rrcStatus.protocolErrorInformation.diagnosticsType.u.type1->
        u.conditionalInformationElementError
        = rtxMemAllocTypeZ (&enc_ctxt, rrc_IdentificationOfReceivedMessage);
       
      msg->message.u.rrcStatus.protocolErrorInformation.diagnosticsType.u.type1->
        u.conditionalInformationElementError->rrc_TransactionIdentifier = 
        transaction_id;

      msg->message.u.rrcStatus.protocolErrorInformation.diagnosticsType.u.type1->
        u.conditionalInformationElementError->receivedMessageType =
        received_msg_id;
      break;

    case T_rrc_ProtocolErrorMoreInformation_type1_messageExtensionNotComprehended:
      msg->message.u.rrcStatus.protocolErrorInformation.diagnosticsType.u.type1->
        u.messageExtensionNotComprehended
        = rtxMemAllocTypeZ (&enc_ctxt, rrc_IdentificationOfReceivedMessage);
       
      msg->message.u.rrcStatus.protocolErrorInformation.diagnosticsType.u.type1->
        u.messageExtensionNotComprehended->rrc_TransactionIdentifier = 
        transaction_id;

      msg->message.u.rrcStatus.protocolErrorInformation.diagnosticsType.u.type1->
        u.messageExtensionNotComprehended->receivedMessageType =
        received_msg_id;
      break;

    case T_rrc_ProtocolErrorMoreInformation_type1_spare1:
    case T_rrc_ProtocolErrorMoreInformation_type1_spare2:
      /*Do Nothing */
      break;

    default:
      WRRC_MSG1_HIGH("Invalid protocol Error t: %d",protocol_error_choice);
      break;
  } 
    /*log the protocol error*/
    rrc_log_protocol_error( (uint8) (rrc_get_state()),
                            (uint8) rrc_procedure,
                            (uint8) T_rrc_FailureCauseWithProtErr_protocolError,
                            (uint8) protocol_error_choice - 1                           
                          );
  /* Send the signalling message */
  if (rrcsend_send_ul_sdu(rrc_procedure,msg,lc_id,
                          l2ack_status) == RRCSEND_SUCCESS)
  {
    return SUCCESS;
  }
  else
  {
    return FAILURE;
  }
} /* rrcsend_send_rrc_status_message */
