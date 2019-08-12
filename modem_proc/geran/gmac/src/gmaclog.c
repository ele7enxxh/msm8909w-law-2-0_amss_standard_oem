/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                            MAC Logging Facilities

GENERAL DESCRIPTION
   This module contains functions for logging MAC packets.

EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2002-2013 Qualcomm Technologies, Inc.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/gmac/src/gmaclog.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
29/10/04   ZYU     Correction for length allocation in ul/dl acknack logging functions
20/08/04   ZYU     Added log packet for EGPRS
08/09/02   ADG     Initial Revision.

===========================================================================*/

#include "geran_variation.h"
#include "customer.h"   /* Include before using any FEATUREs */
#include "gsmdiag_v.h"      /* For gsmdiag_log_alloc() and gsmdiag_log_commit() */


/* Debug F3 messages will be emitted if the following line is uncommented */
/* #define MAC_LOG_DEBUG */

#if defined(MAC_LOG_DEBUG)
#define DEBUGMSG(a,b,c,d)       MSG_HIGH( (a),(b),(c),(d) )
#else
#define DEBUGMSG(a,b,c,d)       /* */
#endif /* defined(MAC_LOG_DEBUG) */

/*===========================================================================
                     INCLUDE FILES FOR MODULE
===========================================================================*/

#include <string.h>          /* For memcpy()                               */
#include "gmaclog.h"         /* This module's header file                  */
#include "log.h"             /* For log_commit(), log_alloc(), log codes   */
#include "event.h"           /* For logging RAR event and event codes      */
#include "log_codes_gsm.h"
#include "log_codes_gsm_v.h"   /* For GSM/GPRS log codes                     */

#include "gmacdef.h"         /* For MAC mode                               */
#include "gmacnull.h"        /* For NULL state machine substates           */
#include "gmacidle.h"        /* For IDLE state machine substates           */
#include "gmactransfer_if.h"    /* For TRANSFER state machine substates       */
#include "amssassert.h"
#include "csn.h"                /* For egprs_ack_nack_des_t type used in mag_log_ul_acknak.    */
#include "gmacproc.h"
#include "stringl/stringl.h"
#include "geran_multi_sim.h"

/*===========================================================================
                        TYPE DEFINITIONS FOR EVENTS & LOG PACKETS
===========================================================================*/

/*lint -esym(754,LOG_GPRS_MAC_STATE_C_tag::hdr)*/
LOG_RECORD_DEFINE(LOG_GPRS_MAC_STATE_C)
  byte mac_mode;
  byte mac_null_fsm_state;
  byte mac_idle_fsm_state;
  byte mac_transfer_fsm_state;
LOG_RECORD_END

/*lint -esym(754,LOG_GPRS_MAC_SIGNALLING_MESSAGE_C_tag::hdr)*/
LOG_RECORD_DEFINE(LOG_GPRS_MAC_SIGNALLING_MESSAGE_C)
  byte channel_type;
  byte message_type;
  byte message_length;
  byte message[1];
LOG_RECORD_END

//EGPRS 2.1:  four fields added for EGPRS - 0x522A  MAC Downlink TBF Establishment  GSM\GPRS/EGPRS\MAC
/*lint -esym(754,LOG_GPRS_MAC_DL_TBF_ESTABLISH_C_tag::hdr)*/
LOG_RECORD_DEFINE(LOG_GPRS_MAC_DL_TBF_ESTABLISH_C)
    byte  dl_tfi;
    byte  rlc_mode;
    byte  dl_ts_bmap;
    // EGPRS 2.1: following four fields added for EGPRS
    byte  egprs_tbf;             // 0 = GPRS, 1 = EGPRS
    uint16  egprs_window_size;   // Decoded window size value in Table 6 of 3GPP 04.60 section 9.1.9
    byte  egprs_link_qual_mode;
    byte  bep_period2;
LOG_RECORD_END

//EGPRS 2.2:  four fields added for EGPRS - 0x5228  MAC Uplink TBF Establishment    GSM\GPRS/EGPRS\MAC
/*lint -esym(754,LOG_GPRS_MAC_UL_TBF_ESTABLISH_C_tag::hdr)*/
LOG_RECORD_DEFINE(LOG_GPRS_MAC_UL_TBF_ESTABLISH_C)
    byte  tbf_request_cause;
    byte  access_granted;
    byte  radio_priority;
    byte  peak_throughput;
    byte  ul_tfi;
    byte  rlc_mode;
    byte  ul_ts_bmap;
    // EGPRS 2.2: following four fields added for EGPRS
    byte  egprs_tbf;             // 0 = GPRS, 1 = EGPRS
    uint16  egprs_window_size;   // Decoded window size value in Table 6 of 3GPP 04.60 section 9.1.9
    byte  resegment;
    byte  bep_period2;
LOG_RECORD_END

/*EGPRS 2.5: New one for EGPRS - 0x522D  EGPRS Downlink AckNack          GSM\GPRS/EGPRS\MAC
For GPRS Ref. 3GPP TS 04.60 section 10.3.1 and 11.2.6, and EGPRS 10.3.1 and 11.2.6a
*/
//variable length implementation
/*lint -esym(754,LOG_EGPRS_MAC_DL_ACKNACK_C_tag::hdr)*/
LOG_RECORD_DEFINE(LOG_EGPRS_MAC_DL_ACKNACK_C)
    byte  type;                                // type: 0=GPRS, 1=EGPRS
    byte  dl_acknak[1];
LOG_RECORD_END
/*EGPRS 2.6: New one for EGPRS - 0x522C  EGPRS Uplink AckNack            GSM\GPRS/EGPRS\MAC
For GPRS and EGPRS Ref. 3GPP TS 04.60 section 10.3.2 and 11.2.28.
NW sends to UE this control message which can be either GPRS or EGPRS, depends on CSN decoding.
Therefore no type field is needed.
*/

//variable length implementation
/*lint -esym(754,LOG_EGPRS_MAC_UL_ACKNACK_C_tag::hdr)*/
LOG_RECORD_DEFINE(LOG_EGPRS_MAC_UL_ACKNACK_C)
    //uint16 msg_bit_len;         //use the packet len in the header instead of this one
    byte  ul_acknak[1];           // Raw control data for UE, upto 22 bytes for now
LOG_RECORD_END

/*lint -esym(754,LOG_GPRS_MAC_UL_TBF_RELEASE_C_tag::hdr)*/
LOG_RECORD_DEFINE(LOG_GPRS_MAC_UL_TBF_RELEASE_C)
    byte  ul_tfi;
    byte  failure_cause;
LOG_RECORD_END

/*lint -esym(754,LOG_GPRS_MAC_DL_TBF_RELEASE_C_tag::hdr)*/
LOG_RECORD_DEFINE(LOG_GPRS_MAC_DL_TBF_RELEASE_C)
    byte  dl_tfi;
    byte  failure_cause;
LOG_RECORD_END

/*===========================================================================
                         DATA DECLARATIONS
===========================================================================*/
static boolean mac_log_initialized = FALSE;

/*===========================================================================
                         FUNCTION DECLARATIONS
===========================================================================*/
log_on_demand_status_enum_type mac_log_on_demand(log_code_type log_code);

/*===========================================================================

FUNCTION  mac_log_packet_init

DESCRIPTION
  Initialises a gsmdiag_pkt_info structure ready for use with logging functions

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
static void mac_log_packet_init(gsmdiag_pkt_info_t *info_ptr, log_code_type pkt_id, unsigned int pkt_size, gas_id_t gas_id)
{
  if (info_ptr != NULL)
  {
    info_ptr->pkt_id   = pkt_id;
    info_ptr->pkt_size = pkt_size;
#ifdef FEATURE_DUAL_SIM
    info_ptr->dual_sim_active = TRUE;
    info_ptr->active_sub_id   = 1 + GERAN_MAP_GAS_ID_TO_DATA_SPACE_INDEX(gas_id);
    info_ptr->pkt_version_id  = GSMDIAG_PKT_VERSION_DEFAULT;
#endif /*FEATURE_DUAL_SIM*/
  }

  return;
}

/*===========================================================================

FUNCTION  mac_log_init

DESCRIPTION
  This function initializes the log_on_demand packets.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void mac_log_init(gas_id_t gas_id)
{
  if (mac_log_initialized)
  {
      DEBUGMSG( "mac_log_init> already initialised", 0,0,0 );
      return;
  }
  else
  {
    /* Call this only once */
    mac_log_initialized = TRUE;

    DEBUGMSG( "mac_log_init> register log-on-demand", 0,0,0 );
    (void)log_on_demand_register(LOG_GPRS_MAC_STATE_C, mac_log_on_demand);

    DEBUGMSG( "mac_log_init> finished", 0,0,0 );
  }
}



/*===========================================================================

FUNCTION  mac_log_on_demand

DESCRIPTION
  This function calls relevant log packet routine when the DM demands it.

RETURN VALUE
  Status of the call (should always be good)

SIDE EFFECTS
  None

===========================================================================*/
log_on_demand_status_enum_type mac_log_on_demand(log_code_type log_code)
{
  boolean retVal = FALSE;
  gas_id_t gas_id = GERAN_ACCESS_STRATUM_ID_1; /*TODO DSDA needs API*/

  switch (log_code)
  {
    case LOG_GPRS_MAC_STATE_C:
      retVal = mac_log_state(gas_id);
      break;

    default:
      /* Prevent Lint warning about no default */
      break;
  }/* end switch */

  return (retVal ? LOG_ON_DEMAND_SENT_S : LOG_ON_DEMAND_FAILED_ATTEMPT_S);

} /* mac_log_on_demand */



/*===========================================================================

FUNCTION  mac_log_state

DESCRIPTION
  This function should be called when MAC state info changes.
  It will send a log packet to the DM.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
boolean mac_log_state(gas_id_t gas_id)
{
  LOG_GPRS_MAC_STATE_C_type *log_ptr;    /* pointer to log buffer */
  gmac_global_data_t *global_data_ptr = gmac_get_global_data_ptr(gas_id);
  gsmdiag_pkt_info_t  pkt_info;

  /* Ensure that logging is initialised */
  mac_log_init(gas_id);

  mac_log_packet_init(&pkt_info, LOG_GPRS_MAC_STATE_C, sizeof(LOG_GPRS_MAC_STATE_C_type), gas_id);

  /* Try to allocate memory for the log packet -             */
  /* this will fail if the log mask is not set appropriately */
  log_ptr = (LOG_GPRS_MAC_STATE_C_type *)gsmdiag_log_alloc(&pkt_info);

  if (log_ptr == NULL)
  {
    DEBUGMSG( "mac_log_state> log_alloc failed", 0,0,0 );
  }
  else
  {
    log_ptr->mac_mode               = (byte) global_data_ptr->mac_mode;
    log_ptr->mac_null_fsm_state     = (byte) gmac_null_get_null_state(gas_id);
    log_ptr->mac_idle_fsm_state     = (byte) gmac_idle_get_idle_state(gas_id);
    log_ptr->mac_transfer_fsm_state = (byte) gmac_tran_get_transfer_state(gas_id);

    gsmdiag_log_commit(&pkt_info);

    DEBUGMSG( "mac_log_state> packet logged", 0,0,0 );
  }

  return (log_ptr != NULL);
} /* mac_log_state */



/*===========================================================================

FUNCTION  mac_log_signalling_message

DESCRIPTION
  This function should be called when MAC state info changes.
  It will send a log packet to the DM.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
boolean mac_log_signalling_message(byte        channel_type,
                                   byte        message_type,
                                   byte        message_length,
                                   const byte *message_ptr,
                                   gas_id_t    gas_id)
{
  LOG_GPRS_MAC_SIGNALLING_MESSAGE_C_type *log_ptr;    /* pointer to log buffer */
  gsmdiag_pkt_info_t  pkt_info;

  /* Ensure that logging is initialised */
  mac_log_init(gas_id);

  /* Determine whether or not there is any message payload */
  if (message_length == 0)
    message_ptr = NULL;
  else if (message_ptr == NULL)
    message_length = 0;

  /* Try to allocate memory for the log packet -             */
  /* this will fail if the log mask is not set appropriately */
  if (message_ptr == NULL)
  {
    /* Message has no payload - only a message type */
    DEBUGMSG( "mac_log_sig_msg> no payload", 0,0,0 );

    mac_log_packet_init(&pkt_info, LOG_GPRS_MAC_SIGNALLING_MESSAGE_C, sizeof(LOG_GPRS_MAC_SIGNALLING_MESSAGE_C_type) - 1, gas_id);

    log_ptr = (LOG_GPRS_MAC_SIGNALLING_MESSAGE_C_type *)gsmdiag_log_alloc(&pkt_info);
  }
  else
  {
    /* Log an event if we have a payload */
      byte payload[4];

      payload[0] = geran_get_diag_sub_id(gas_id); /* Subscription Id */
      payload[1] = message_type;              /* Message type */
      payload[2] = 0;                         /* Protocol discriminator */
      payload[3] = channel_type;              /* Channel type */

      if((channel_type & 0x80) != 0)
      {
        geran_diag_event(EVENT_GPRS_MAC_MSG_RECEIVED, EVENTS_DS_GPRS_MAC_MSG_RECEIVED, sizeof(payload), payload);
      }
      else
      {
        geran_diag_event(EVENT_GPRS_MAC_MSG_SENT, EVENTS_DS_GPRS_MAC_MSG_SENT, sizeof(payload), payload);
      }

      DEBUGMSG( "mac_log_sig_msg> event logged", 0,0,0 );


    /* Message has a payload */
    DEBUGMSG( "mac_log_sig_msg> %d byte payload", message_length, 0,0 );

    mac_log_packet_init(&pkt_info, LOG_GPRS_MAC_SIGNALLING_MESSAGE_C, (sizeof(LOG_GPRS_MAC_SIGNALLING_MESSAGE_C_type) - 1) + message_length, gas_id);

    log_ptr = (LOG_GPRS_MAC_SIGNALLING_MESSAGE_C_type *)gsmdiag_log_alloc(&pkt_info);
  }

  if (log_ptr == NULL)
  {
    DEBUGMSG( "mac_log_sig_msg> log_alloc failed", 0,0,0 );
  }
  else
  {

    /* Fill the log packet and commit it */
    log_ptr->channel_type   = channel_type;
    log_ptr->message_type   = message_type;
    log_ptr->message_length = message_length;

    /*Suppressed lint error because lint checks gmacidleutil.c which  */
    /*calls mac_log_signalling_message() before gmaclog.c so is blind */
    /*to definitions in LOG_GPRS_MAC_SIGNALLING_MESSAGE_C macro       */

    if (message_ptr != NULL)
        memscpy((void *) (log_ptr->message), message_length, 
                 message_ptr, message_length); /*lint !e419 !e420*/

    gsmdiag_log_commit(&pkt_info);

    DEBUGMSG( "mac_log_sig_msg> packet logged", 0,0,0 );
  }

  return (log_ptr != NULL);
} /* mac_log_signalling_message */


/*===========================================================================

FUNCTION  mac_log_camped_on_cell

DESCRIPTION
  This function should be called when MAC receives GRR_MAC_CELL_RESELECT_IND.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void mac_log_camped_on_cell(gas_id_t gas_id)
{
  byte payload = geran_get_diag_sub_id(gas_id);
  geran_diag_event(EVENT_GPRS_MAC_CAMPED_ON_CELL, EVENTS_DS_GPRS_MAC_CAMPED_ON_CELL, sizeof(payload), &payload);
} // mac_log_camped_on_cell



/*===========================================================================

    FUNCTION  mac_log_ul_tbf_establish

    DESCRIPTION
    This function should be called when MAC receives UL resources network
    to establish UL TBF.

    RETURN VALUE
    None

   SIDE EFFECTS
   None

===========================================================================*/
void mac_log_ul_tbf_establish
(
 byte ul_tfi,
 byte ul_ts_bmap,
 gas_id_t gas_id
 )
{
#define GPRS_TBF 0 //for keeping GPRS one compatable with both old and new.
    LOG_GPRS_MAC_UL_TBF_ESTABLISH_C_type *log_ptr;    /* pointer to log buffer */
    gmac_global_data_t *global_data_ptr = gmac_get_global_data_ptr(gas_id);
    gmac_idle_data_t *idle_data_ptr = gmac_get_idle_data_ptr(gas_id);
    gsmdiag_pkt_info_t  pkt_info;

    mac_log_packet_init(&pkt_info, LOG_GPRS_MAC_UL_TBF_ESTABLISH_C, sizeof(LOG_GPRS_MAC_UL_TBF_ESTABLISH_C_type), gas_id);

    log_ptr = (LOG_GPRS_MAC_UL_TBF_ESTABLISH_C_type *)gsmdiag_log_alloc(&pkt_info);

    if (NULL != log_ptr)
    {
        log_ptr->tbf_request_cause    = (uint8)global_data_ptr->access_requested;
        log_ptr->access_granted       = (uint8)idle_data_ptr->access_granted;
        log_ptr->radio_priority       = global_data_ptr->gmac_local_stored_ul_chan_req.radio_priority;
        log_ptr->peak_throughput      = global_data_ptr->gmac_local_stored_ul_chan_req.peak_thr_class;
        log_ptr->ul_tfi               = ul_tfi;
        log_ptr->rlc_mode             = global_data_ptr->gmac_local_stored_ul_chan_req.rlc_mode;
        log_ptr->ul_ts_bmap           = ul_ts_bmap;
        log_ptr->egprs_tbf            = GPRS_TBF; //for keeping GPRS one compatable with both old and new.

        gsmdiag_log_commit(&pkt_info);
    }

} // mac_log_ul_tbf_etsblish for GPRS only


/*===========================================================================

    FUNCTION  mac_log_ul_etbf_establish

        DESCRIPTION
    This function should be called when MAC receives UL resources network
    to establish UL TBF.

    For EGPRS set the EGPRS_TBF = 1, and in mac_log_ul_tbf_establish for GPRS
    EGPRS_TBF = 0.

    RETURN VALUE
    None

    SIDE EFFECTS
    None

===========================================================================*/
void mac_log_ul_etbf_establish
(
 byte    ul_tfi,
 byte    ul_ts_bmap,
 // EGPRS 3.2: following four fields added for EGPRS
 byte    egprs_tbf,           // a flag, not TBF value, 0 for GPRS, 1 for EGPRS
 uint16  egprs_window_size,   // Real value, not coded one. See Ref 4.60 table 9.1.9.2.1, the coding column.
 byte    resegment,           // see ICD 5.7.5
 byte    bep_period2,         // valid range 0..15, see ICD 5.7.5
 gas_id_t gas_id
 )
{
  LOG_GPRS_MAC_UL_TBF_ESTABLISH_C_type *log_ptr;    /* pointer to log buffer */
  gmac_global_data_t *global_data_ptr = gmac_get_global_data_ptr(gas_id);
  gmac_idle_data_t *idle_data_ptr = gmac_get_idle_data_ptr(gas_id);
  gsmdiag_pkt_info_t  pkt_info;

  mac_log_packet_init(&pkt_info, LOG_GPRS_MAC_UL_TBF_ESTABLISH_C, sizeof(LOG_GPRS_MAC_UL_TBF_ESTABLISH_C_type), gas_id);

  log_ptr = (LOG_GPRS_MAC_UL_TBF_ESTABLISH_C_type *)gsmdiag_log_alloc(&pkt_info);

  if (NULL != log_ptr)
  {
    log_ptr->tbf_request_cause    = (uint8)global_data_ptr->access_requested;
    log_ptr->access_granted       = (uint8)idle_data_ptr->access_granted;
    log_ptr->radio_priority       = global_data_ptr->gmac_local_stored_ul_chan_req.radio_priority;
    log_ptr->peak_throughput      = global_data_ptr->gmac_local_stored_ul_chan_req.peak_thr_class;
    log_ptr->ul_tfi               = ul_tfi;
    log_ptr->rlc_mode             = global_data_ptr->gmac_local_stored_ul_chan_req.rlc_mode;
    log_ptr->ul_ts_bmap           = ul_ts_bmap;
    log_ptr->egprs_tbf            = egprs_tbf;
    log_ptr->egprs_window_size    = egprs_window_size;
    log_ptr->resegment            = resegment;
    log_ptr->bep_period2          = bep_period2;

    gsmdiag_log_commit(&pkt_info);
  }

} // mac_log_ul_etbf_etsblish for both GPRS and EGPRS




/*===========================================================================

FUNCTION  mac_log_ul_tbf_release

DESCRIPTION
  This function should be called when MAC has failed to support current
    UL TBF.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void mac_log_ul_tbf_release
(
 mac_log_ul_tbf_release_t  failure_reason,
 gas_id_t                  gas_id
)
{
    LOG_GPRS_MAC_UL_TBF_RELEASE_C_type *log_ptr;    /* pointer to log buffer */
    gmac_global_data_t *global_data_ptr = gmac_get_global_data_ptr(gas_id);
    gsmdiag_pkt_info_t  pkt_info;

    mac_log_packet_init(&pkt_info, LOG_GPRS_MAC_UL_TBF_RELEASE_C, sizeof(LOG_GPRS_MAC_UL_TBF_RELEASE_C_type), gas_id);

    log_ptr = (LOG_GPRS_MAC_UL_TBF_RELEASE_C_type *)gsmdiag_log_alloc(&pkt_info);

    if (NULL != log_ptr)
    {
      log_ptr->ul_tfi = global_data_ptr->gmac_local_tfi_store.ul_tfi;
      log_ptr->failure_cause = (byte)failure_reason;
      gsmdiag_log_commit(&pkt_info);
    }

} // mac_log_ul_tbf_release

/*===========================================================================

FUNCTION  mac_log_dl_tbf_establish

DESCRIPTION
  This function should be called when MAC receives Packet DL Assignment
    from the network to establish DL TBF.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void mac_log_dl_tbf_establish
(
 byte dl_tfi,
 byte dl_ts_bmap,
 byte rlc_mode,
 gas_id_t gas_id
)
{
    LOG_GPRS_MAC_DL_TBF_ESTABLISH_C_type *log_ptr;    /* pointer to log buffer */
    gsmdiag_pkt_info_t  pkt_info;

    mac_log_packet_init(&pkt_info, LOG_GPRS_MAC_DL_TBF_ESTABLISH_C, sizeof(LOG_GPRS_MAC_DL_TBF_ESTABLISH_C_type), gas_id);

    log_ptr = (LOG_GPRS_MAC_DL_TBF_ESTABLISH_C_type *)gsmdiag_log_alloc(&pkt_info);

    if (NULL != log_ptr)
    {
      log_ptr->dl_tfi = dl_tfi;
      log_ptr->rlc_mode = rlc_mode;
      log_ptr->dl_ts_bmap = dl_ts_bmap;
      // EGPRS 3.1: following four fields added for EGPRS
      log_ptr->egprs_tbf            = 0;
      log_ptr->egprs_window_size    = 0;
      log_ptr->egprs_link_qual_mode = 0;
      log_ptr->bep_period2          = 0;

      gsmdiag_log_commit(&pkt_info);
    }

} // mac_log_dl_tbf_etsblish for GPRS only



/*===========================================================================

FUNCTION  mac_log_dl_etbf_establish

DESCRIPTION
  This function is a replacement for the GPRS one when FEATURE_GSM_EGPRS enabled. It
  covers both GPRS and EGPRS. Doing it this way intends to reduce the risk of
  break the system when this new log is added.

  It should be called when MAC receives Packet DL Assignment
  from the network to establish DL TBF. Using the new field EGPRS_TBF to indicate
  GPRS or EGPRS. (This will enable future obsolating the old log packet.

  For EGPRS set the EGPRS_TBF = 1, and for GPRS EGPRS_TBF = 0.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void mac_log_dl_etbf_establish
(
 byte dl_tfi,
 byte dl_ts_bmap,
 byte rlc_mode,
 // EGPRS 3.1: following four fields added for EGPRS
 byte  egprs_tbf,           // a flag, not TBF value, 0 for GPRS, 1 for EGPRS
 uint16  egprs_window_size,   // not real value, but coded one. See Ref 4.60 table 9.1.9.2.1, the coding column.
 byte  egprs_link_qual_mode,// see ICD 5.7.5
 byte  bep_period2,          // valid range 0..15, see ICD 5.7.5
 gas_id_t gas_id
 )
{
  LOG_GPRS_MAC_DL_TBF_ESTABLISH_C_type *log_ptr;    /* pointer to log buffer */
  gsmdiag_pkt_info_t  pkt_info;

  mac_log_packet_init(&pkt_info, LOG_GPRS_MAC_DL_TBF_ESTABLISH_C, sizeof(LOG_GPRS_MAC_DL_TBF_ESTABLISH_C_type), gas_id);

  log_ptr = (LOG_GPRS_MAC_DL_TBF_ESTABLISH_C_type *)gsmdiag_log_alloc(&pkt_info);

  if (NULL != log_ptr)
  {
    log_ptr->dl_tfi = dl_tfi;
    log_ptr->rlc_mode = rlc_mode;
    log_ptr->dl_ts_bmap = dl_ts_bmap;
    // EGPRS 3.1: following four fields added for EGPRS
    log_ptr->egprs_tbf            = egprs_tbf;
    log_ptr->egprs_window_size    = egprs_window_size;
    log_ptr->egprs_link_qual_mode = egprs_link_qual_mode;
    log_ptr->bep_period2          = bep_period2;

    gsmdiag_log_commit(&pkt_info);
  }

} // mac_log_dl_tbf_etsblish for both GPRS and EGPRS






/*===========================================================================

FUNCTION  mac_log_dl_tbf_release

DESCRIPTION
  This function should be called when MAC has failed to support current
    dL TBF.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void mac_log_dl_tbf_release
(
 mac_log_dl_tbf_release_t  failure_reason,
 gas_id_t gas_id
 )
{
  LOG_GPRS_MAC_DL_TBF_RELEASE_C_type *log_ptr;    /* pointer to log buffer */
  gmac_global_data_t *global_data_ptr = gmac_get_global_data_ptr(gas_id);
  gsmdiag_pkt_info_t  pkt_info;

  mac_log_packet_init(&pkt_info, LOG_GPRS_MAC_DL_TBF_RELEASE_C, sizeof(LOG_GPRS_MAC_DL_TBF_RELEASE_C_type), gas_id);

  log_ptr = (LOG_GPRS_MAC_DL_TBF_RELEASE_C_type *)gsmdiag_log_alloc(&pkt_info);

  if (NULL != log_ptr)
  {
    log_ptr->dl_tfi = global_data_ptr->gmac_local_tfi_store.dl_tfi;
    log_ptr->failure_cause = (byte)failure_reason;
    gsmdiag_log_commit(&pkt_info);
  }

} // mac_log_dl_tbf_release





/*===========================================================================

FUNCTION  mac_log_dl_acknack

DESCRIPTION
  EGPRS 3.5 0x522D  EGPRS Downlink AckNack, direction MS -> NW
    This function should be called when MAC has created (E)PDAN of current
    DL TBF.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void mac_log_dl_acknack
(
 byte type,  uint8 acknack_byte_len,  uint8* dl_acknak,
 gas_id_t gas_id
 )
{
  LOG_EGPRS_MAC_DL_ACKNACK_C_type *log_ptr;    /* pointer to log buffer */
  gsmdiag_pkt_info_t  pkt_info;

  mac_log_packet_init(&pkt_info, LOG_EGPRS_MAC_DL_ACKNACK_C, sizeof(LOG_EGPRS_MAC_DL_ACKNACK_C_type) + acknack_byte_len-1, gas_id);

  log_ptr = (LOG_EGPRS_MAC_DL_ACKNACK_C_type *)gsmdiag_log_alloc(&pkt_info);

  if (NULL != log_ptr)
  {
    log_ptr->type = type;

    (void)memscpy( (void*)log_ptr->dl_acknak,acknack_byte_len, 
                  (void*)dl_acknak, acknack_byte_len );

    gsmdiag_log_commit(&pkt_info);
  }

} // mac_log_dl_acknack


/*===========================================================================

FUNCTION  mac_log_ul_acknack_with_byte_len

DESCRIPTION

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void mac_log_ul_acknack_with_byte_len
(
 uint16 acknack_byte_len,   uint8*  ul_acknak,
 gas_id_t gas_id
)
{

  LOG_EGPRS_MAC_UL_ACKNACK_C_type *log_ptr;    /* pointer to log buffer */
  gsmdiag_pkt_info_t  pkt_info;

  mac_log_packet_init(&pkt_info, LOG_EGPRS_MAC_UL_ACKNACK_C, sizeof(LOG_EGPRS_MAC_UL_ACKNACK_C_type) + acknack_byte_len-1, gas_id);

  log_ptr = (LOG_EGPRS_MAC_UL_ACKNACK_C_type *)gsmdiag_log_alloc(&pkt_info);

  if (NULL != log_ptr)
  {

    (void)memscpy( (void*)log_ptr->ul_acknak,acknack_byte_len, 
                   (void*)ul_acknak, acknack_byte_len );

    gsmdiag_log_commit(&pkt_info);
  }


} // mac_log_ul_acknack_with_byte_len


/*===========================================================================

FUNCTION  mac_log_ul_acknack

DESCRIPTION
  EGPRS 3.6 0x522C  EGPRS Uplink AckNack , direction NW -> MS

  This function should be called when MAC has got a (E)PUAN of current
    UL TBF.

    This function take variable length msg.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void mac_log_ul_acknack
(
 uint16 acknack_bit_len,   uint8*  ul_acknak,
 gas_id_t gas_id
)
{
  uint8 acknack_byte_len;

  if (acknack_bit_len <= 176 ) /*22 bytes*/
    acknack_byte_len = 22;
  else if (acknack_bit_len <= 352 )
    acknack_byte_len = 44;
  else {
    acknack_byte_len = 44;
    DEBUGMSG( "mac_EPUAN is longer than 44 bytes. ", 0,0,0 );
      }
  mac_log_ul_acknack_with_byte_len(acknack_byte_len, ul_acknak, gas_id);

} // mac_log_ul_acknack


/*event logging changes*/
/*===========================================================================

FUNCTION  mac_log_grr_events

DESCRIPTION
  This function is called when a RR msg is received in the MAC task.  It will
  send an event in all MAC states when GRR_MAC_CELL_RESEL_IND or GRR_MAC_UL_TBF_EST_REJ
  or GRR_MAC_SEND_PCCF GRR msgs are received.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void mac_log_grr_events( uint8 signal_id, gas_id_t gas_id )
{
  gmac_global_data_t *global_data_ptr = gmac_get_global_data_ptr(gas_id);

  switch ( signal_id )
  {
    case GRR_MAC_SEND_PCCF:

      /* 04.60 v4.13.0 cause 4 is not included, cause 7 and above is reserved*/
      if ( ( global_data_ptr->msg_ptr->grr_mac_sig.msg.gm_send_pccf.pccf.cause != 4 ) &&
           ( global_data_ptr->msg_ptr->grr_mac_sig.msg.gm_send_pccf.pccf.cause <= 6 ) )
      {
        byte payload[2];
        payload[0] = geran_get_diag_sub_id(gas_id);
        payload[1] = global_data_ptr->msg_ptr->grr_mac_sig.msg.gm_send_pccf.pccf.cause;
        geran_diag_event(EVENT_GPRS_CELL_CHANGE_FAILURE, EVENTS_DS_GPRS_CELL_CHANGE_FAILURE, 
          sizeof(payload), payload);
      }
      else
      {
        MSG_GERAN_ERROR_0_G("GMAC EVENT LOGGING PCCF Cause out of range");
      }
    break;

    default: /* do nothing*/
    break;
  }
}

/*===========================================================================

FUNCTION  mac_log_gprs_pua_event

DESCRIPTION
  This function is called when a Packet Uplink Assignment msg is rxd.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void mac_log_gprs_pua_event( uint8 ts_alloc, uint8 cs, gas_id_t gas_id )
{
  uint8 pua_event[3];

  pua_event[0] = geran_get_diag_sub_id(gas_id);
  pua_event[1] = ts_alloc;
  pua_event[2] = cs;

  geran_diag_event(EVENT_GPRS_PACKET_UPLINK_ASSIGNMENT, EVENTS_DS_GPRS_PACKET_UPLINK_ASSIGNMENT, 
    sizeof(pua_event), pua_event);
}

/*===========================================================================

FUNCTION  mac_log_gprs_pda_event

DESCRIPTION
  This function is called when a Packet Downlink Assignment msg is rxd.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void mac_log_gprs_pda_event( uint8 ts_alloc, uint8 tfi, gas_id_t gas_id )
{
  uint8 pda_event[3];

  pda_event[0] = geran_get_diag_sub_id(gas_id);
  pda_event[1] = ts_alloc;
  pda_event[2] = tfi;

  geran_diag_event(EVENT_GPRS_PACKET_DOWNLINK_ASSIGNMENT, EVENTS_DS_GPRS_PACKET_DOWNLINK_ASSIGNMENT, 
      sizeof(pda_event), pda_event);
}

/*===========================================================================

FUNCTION  mac_log_convert_single_ts_to_bitmp

DESCRIPTION
  This function converts a single TS to a bitmapped TS.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
uint8 mac_log_convert_single_ts_to_bitmp( uint8 single_ts, gas_id_t gas_id )
{
  uint8 ts_bitmap = 0;

  ts_bitmap = 0x80 >> single_ts;

  return ts_bitmap;
}



/*===========================================================================

FUNCTION  mac_log_packet_tbf_rel_event

DESCRIPTION
  This function is called when a Packet TBF Release msg is received.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void mac_log_packet_tbf_rel_event( release_tbf_t release_type, gas_id_t gas_id )
{
  byte payload[2];

  payload[0] = geran_get_diag_sub_id(gas_id);
  payload[1] = (byte)release_type;

  geran_diag_event(EVENT_GPRS_TBF_RELEASE, EVENTS_DS_GPRS_TBF_RELEASE, 
        sizeof(payload), payload);
}

/*===========================================================================
===
===  FUNCTION      mac_log_timer_event()
===
===  DESCRIPTION
===    Called from MAC's timer call back function .
===
===  DEPENDENCIES
===
===
===  RETURN VALUE
===
===
===  SIDE EFFECTS
===
===
===========================================================================*/

void mac_log_timer_event(const uint32 timer_id, gas_id_t gas_id)
{
  uint8 ev_rpt_timer_exp = 0;

  /*Map MAC TImer ID onto Event Reporting Timer ID*/
  switch ( timer_id )
  {
    case MAC_TIMER_T3166:
      ev_rpt_timer_exp = 0; /*T3166*/
    break;

    case MAC_TIMER_T3168:
      ev_rpt_timer_exp = 1; /*T3168*/
    break;

    case MAC_TIMER_T3180:
      ev_rpt_timer_exp = 2; /*T3180*/
    break;

    default:
      ev_rpt_timer_exp = 0xff; /* do not send event rpt*/
    break;
  }

  if ( ev_rpt_timer_exp != 0xff )
  {
    byte payload[2];

    payload[0] = geran_get_diag_sub_id(gas_id);
    payload[1] = ev_rpt_timer_exp;

    geran_diag_event(EVENT_GPRS_TIMER_EXPIRY, EVENTS_DS_GPRS_TIMER_EXPIRY, sizeof(payload), payload);    
  }
}


/*===========================================================================
===
===  FUNCTION      mac_log_prr_event()
===
===  DESCRIPTION
===   Called when L1 is requested to send a PRR.
===
===  DEPENDENCIES
===
===
===  RETURN VALUE
===
===
===  SIDE EFFECTS
===
===
===========================================================================*/

void mac_log_prr_event(access_type_e access_req, gas_id_t gas_id)
{
  byte payload[2];
  
  payload[0] = geran_get_diag_sub_id(gas_id);
  payload[1] = (byte)access_req;
  
  geran_diag_event(EVENT_GPRS_PACKET_RESOURCE_REQUEST, EVENTS_DS_GPRS_PACKET_RESOURCE_REQUEST, 
    sizeof(payload), payload);    
}


/*===========================================================================
===
===  FUNCTION      mac_log_ptr_event()
===
===  DESCRIPTION
===   Called when L1 is requested to send a PRR.
===
===  DEPENDENCIES
===
===
===  RETURN VALUE
===
===
===  SIDE EFFECTS
===
===
===========================================================================*/

void mac_log_ptr_event(uint8 ul_ts, uint8 dl_ts, uint8 ul_tfi, uint8 dl_tfi,
                       gas_id_t gas_id)
{
  uint8 ptr_event[5];

  ptr_event[0] = geran_get_diag_sub_id(gas_id);
  ptr_event[1] = ul_ts;
  ptr_event[2] = dl_ts;
  ptr_event[3] = ul_tfi;
  ptr_event[4] = dl_tfi;

  geran_diag_event(EVENT_PACKET_TIMESLOT_RECONFIGURE, EVENTS_DS_PACKET_TIMESLOT_RECONFIGURE, 
    sizeof(ptr_event), (byte*)&ptr_event);    
}


