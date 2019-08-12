/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                            LLC Logging Facilities

GENERAL DESCRIPTION
   This module contains functions for logging LLC packets.

EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2002-2013 Qualcomm Technologies, Inc.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/gllc/src/gllclog.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/05/02   ADG     Initial Revision.
===========================================================================*/

#include "geran_variation.h"
#include "customer.h"        /* Include before using any FEATUREs */


/* Debug F3 messages will be emitted if the following line is uncommented */
/* #define LLC_LOG_DEBUG */

#if defined(LLC_LOG_DEBUG)
#define DEBUGMSG(a,b,c,d)       MSG_HIGH( (a),(b),(c),(d) )
#else
#define DEBUGMSG(a,b,c,d)       /* */
#endif /* defined(LLC_LOG_DEBUG) */


/*===========================================================================
                     INCLUDE FILES FOR MODULE
===========================================================================*/
#include <memory.h>
#include <string.h>          /* For memcpy()                               */
#include "stringl.h"         /* For memscpy(), memsmove()                  */
#include "log.h"             /* For log_commit(), log_alloc(), log codes   */
#include "event.h"           /* For logging RAR event and event codes      */
#include "log_codes_gsm.h"
#include "log_codes_gsm_v.h"   /* For GSM/GPRS log codes                     */
#ifdef DEBUG_USE_OLD_DIAG
#error code not present
#else
#include "gsmdiag.h"
#include "gsmdiag_v.h"
#endif
#include "amssassert.h"

#include "gllc.h"
#include "gllc_v.h"
#include "gllc_v_g.h"            /* For structure definitions                  */
#include "gllci.h"
#include "gllclog.h"         /* This module's header file                  */
#include "naslog.h"
#include "geran_multi_sim.h"

/*===========================================================================
              TYPE DEFINITIONS FOR EVENTS & LOG PACKETS
===========================================================================*/

LOG_RECORD_DEFINE(LOG_GPRS_LLC_ME_INFO_C)
  uint8  llme_state;
  uint8  llme_xid_version;
  uint32 llme_new_tlli;
  uint32 llme_old_tlli;
  uint8  llme_encrypt_alg;
  uint32 llme_encrypt_key[2];
  uint32 llme_input_offset_iov_ui;  /* Check this */
LOG_RECORD_END

LOG_RECORD_DEFINE(LOG_GPRS_LLC_SAPI_STATES_C)
  uint8  gllc_sapi;
  uint8  gllc_lle_state;
  uint8  gllc_lle_substate;
LOG_RECORD_END

LOG_RECORD_DEFINE(LOG_GPRS_LLC_XID_INFO_C)
  uint8  gllc_sapi;
  uint16 pending_xid_bitmap;
  uint32 current_iov_i;
  uint32 current_t200;
  uint32 preferred_t200;
  uint8  current_n200;
  uint8  preferred_n200;
  uint16 current_n201_u;
  uint16 preferred_n201_u;
  uint16 current_n201_i;
  uint16 preferred_n201_i;
  uint16 current_md;
  uint16 preferred_md;
  uint16 current_mu;
  uint16 preferred_mu;
  uint8  current_kd;
  uint8  preferred_kd;
  uint8  current_ku;
  uint8  preferred_ku;
LOG_RECORD_END

LOG_RECORD_DEFINE(LOG_GPRS_LLC_PDU_STATS_C)
  uint8  lle_sapi_addr;
  uint32 ack_l3pdu_tx_cnt;
  uint32 ack_octet_tx_cnt;
  uint32 ack_l3pdu_retx_cnt;
  uint32 ack_octet_retx_cnt;
  uint32 ack_l3pdu_rx_cnt;
  uint32 ack_octet_rx_cnt;
  uint32 unack_l3pdu_tx_cnt;
  uint32 unack_octet_tx_cnt;
  uint32 unack_l3pdu_rx_cnt;
  uint32 unack_octet_rx_cnt;
  uint32 llpdu_tx_cnt;
  uint32 llpdu_rx_cnt;
  uint32 llpdu_fcs_err_cnt;
  uint32 llpdu_frm_rej_cnt;
  uint32 llpdu_tlli_err_cnt;
  uint32 llpdu_addr_err_cnt;
  uint32 llpdu_short_err_cnt;
  uint32 ack_l3pdu_rerx_cnt;
  uint32 ack_octet_rerx_cnt;
LOG_RECORD_END

LOG_RECORD_DEFINE(LOG_GPRS_LLC_PERIODIC_STATS_C)
  uint8  lle_sapi_addr;
  uint16 unack_snd_seq_var_v_u;
  uint16 unack_rcv_seq_var_v_ur;
  uint32 unack_encipher_oc;
  uint32 unack_decipher_oc;
  uint16 ack_snd_seq_var_v_s;
  uint16 ack_rcv_seq_var_v_r;
  uint16 ack_seq_var_v_a;
  uint32 ack_encipher_oc;
  uint32 ack_decipher_oc;
  uint32 reestablishment_cnt;
  uint32 gen_purpose_a_cnt;
  uint32 gen_purpose_b_cnt;
  uint32 gen_purpose_c_cnt;
LOG_RECORD_END

LOG_RECORD_DEFINE(LOG_GPRS_SM_GMM_OTA_MESSAGE_C)
  uint8  direction;
  uint8  message_type;
  uint16 message_length;
  uint8  message[LOG_MAX_NAS_OTA_MESSAGE_SIZE];
LOG_RECORD_END

LOG_RECORD_DEFINE(LOG_GPRS_LLC_READY_TIMER_STATUS_C)
  uint8  ready_timer_status;
LOG_RECORD_END

LOG_RECORD_DEFINE(LOG_GPRS_LLC_SEND_UI_FRAME_C)
  uint16  seq_num;
  uint8   sapi;
  uint16  pdu_len;
  uint8   cipher_mode;
LOG_RECORD_END

LOG_RECORD_DEFINE(LOG_GPRS_LLC_RECV_UI_FRAME_C)
  uint16  seq_num;
  uint8   sapi;
  uint16  pdu_len;
  uint8   cipher_mode;
LOG_RECORD_END

LOG_RECORD_DEFINE(LOG_GPRS_LLC_READY_TIMER_STATUS_VER2_C)
  uint32 ready_timer_value;
  uint8  ready_timer_status;
LOG_RECORD_END

uint8 gllc_gs_queue[ NUM_GERAN_DATA_SPACES ] =
          { INITIAL_VALUE_DIFFERENT(GS_QUEUE_LLC,
                                    GS_QUEUE_LLC_2,
                                    GS_QUEUE_LLC_3) };

/*===========================================================================
                         DATA DECLARATIONS
===========================================================================*/
boolean llc_log_initialized[NUM_GERAN_DATA_SPACES] = {FALSE};

/* Used by the reporting callbacks to determine whether the data supplied  */
/* originates from the required SAPI and packet. Zero means always accept. */
typedef enum _LLC_PACKET_TO_LOG
{
    LLC_PACKET_LOG_ALL,
    LLC_PACKET_ME_INFO,
    LLC_PACKET_SAPI_STATES,
    LLC_PACKET_XID_INFO
} LLC_PACKET_TO_LOG;

LLC_PACKET_TO_LOG packet_wanted[NUM_GERAN_DATA_SPACES] = {LLC_PACKET_LOG_ALL};

#define LLC_SAPI_LOG_ALL    0
uint8 sapi_wanted[NUM_GERAN_DATA_SPACES] = {LLC_SAPI_LOG_ALL};

/*===========================================================================
                         FUNCTION DECLARATIONS
===========================================================================*/
/* Log-on-demand callback to be registered with DIAG */
log_on_demand_status_enum_type llc_log_on_demand(log_code_type log_code);

/* Log-packet-request callback to be registered with DIAG */
void llc_log_packet_request(uint16 log_code,
                            uint8  zero_stats,
                            uint8  additional_info);

/* Reporting callbacks to be registered with other parts of LLC */
void llc_log_llme_state_cb(const gllc_diag_llme_state_rpt_t *rpt_ptr);
void llc_log_lle_state_cb(const gllc_diag_lle_state_rpt_t *rpt_ptr);
void llc_log_lle_xid_cb(const gllc_diag_lle_xid_rpt_t *rpt_ptr);


/*===========================================================================

FUNCTION  llc_log_init

DESCRIPTION
  This function initializes the log_on_demand packets.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void llc_log_init(gas_id_t gas_id)
{
  if (llc_log_initialized[gas_id])
  {
      DEBUGMSG( "llc_log_init> already initialised", 0,0,0 );
      return;
  }
  else
  {
      /* Call this only once */
      llc_log_initialized[gas_id] = TRUE;
      packet_wanted[gas_id]       = LLC_PACKET_LOG_ALL;
      sapi_wanted[gas_id]         = LLC_SAPI_LOG_ALL;

      DEBUGMSG( "llc_log_init> register log-on-demand", 0,0,0 );
      (void)log_on_demand_register(LOG_GPRS_LLC_ME_INFO_C, llc_log_on_demand);

      DEBUGMSG( "llc_log_init> register log-packet-request", 0,0,0 );
      (void)diagpkt_gsm_gprs_lpr_register(
        LOG_GPRS_LLC_ME_INFO_C, llc_log_packet_request);
      (void)diagpkt_gsm_gprs_lpr_register(
        LOG_GPRS_LLC_SAPI_STATES_C, llc_log_packet_request);
      (void)diagpkt_gsm_gprs_lpr_register(
        LOG_GPRS_LLC_XID_INFO_C, llc_log_packet_request);
      (void)diagpkt_gsm_gprs_lpr_register(
        LOG_GPRS_LLC_PDU_STATS_C,llc_log_packet_request);
      (void)diagpkt_gsm_gprs_lpr_register(
        LOG_GPRS_LLC_PERIODIC_STATS_C, llc_log_packet_request);
      (void)diagpkt_gsm_gprs_lpr_register(
        LOG_GPRS_LLC_SEND_UI_FRAME_C, llc_log_packet_request);
      (void)diagpkt_gsm_gprs_lpr_register(
        LOG_GPRS_LLC_RECV_UI_FRAME_C, llc_log_packet_request);

      DEBUGMSG( "llc_log_init> register callbacks", 0,0,0 );
      gllc_diag_register( gas_id,
                          llc_log_llme_state_cb,
                          llc_log_lle_state_cb,
                          llc_log_lle_xid_cb
                        );

      DEBUGMSG( "llc_log_init> finished", 0,0,0 );
  }
}


/*===========================================================================

FUNCTION  llc_log_packet_init

DESCRIPTION
  Initialises a gsmdiag_pkt_info structure ready for use with logging functions

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
static void llc_log_packet_init( gas_id_t            gas_id,
                                 gsmdiag_pkt_info_t *info_ptr,
                                 log_code_type       pkt_id,
                                 unsigned int        pkt_size
                               )
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

FUNCTION  llc_log_on_demand

DESCRIPTION
  This function calls relevant log packet routine when the DM demands it.

RETURN VALUE
  Status of the call (should always be good)

SIDE EFFECTS
  None

===========================================================================*/
log_on_demand_status_enum_type llc_log_on_demand(log_code_type log_code)
{
  boolean retVal = TRUE;

  switch (log_code)
  {
    case LOG_GPRS_LLC_ME_INFO_C:
      llc_log_me_info();
      break;

    case LOG_GPRS_LLC_READY_TIMER_STATUS_VER2_C:
      llc_log_ready_timer_status(GERAN_ACCESS_STRATUM_ID_1); //temporary default value for gas_id parameter
      break;

    default:
      retVal = FALSE;
      break;
  }/* end switch */

  return (retVal ? LOG_ON_DEMAND_SENT_S : LOG_ON_DEMAND_FAILED_ATTEMPT_S);

} /* llc_log_on_demand */



/*===========================================================================

FUNCTION  llc_log_packet_request

DESCRIPTION
  This function calls relevant log packet routine when the DM demands it.
  log-packet-request works like an extended version log-on-demand,
  with extra parameters to clear statistics and to supply additional
  information according to packet type.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void llc_log_packet_request(uint16 log_code,
                            uint8  zero_stats,
                            uint8  additional_info)
{
  switch (log_code)
  {
    case LOG_GPRS_LLC_ME_INFO_C:
        llc_log_me_info();
        break;

    case LOG_GPRS_LLC_SAPI_STATES_C:
        /* additional_info is the SAPI */
        llc_log_sapi_states(additional_info);
        break;

    case LOG_GPRS_LLC_XID_INFO_C:
        /* additional_info is the SAPI */
        llc_log_xid_info(additional_info);
        break;

    case LOG_GPRS_LLC_PDU_STATS_C:
        /* additional_info is the SAPI */
        llc_log_pdu_stats(additional_info, zero_stats);
        break;

    case LOG_GPRS_LLC_PERIODIC_STATS_C:
        /* additional_info is the SAPI */
        llc_log_periodic_stats(additional_info, zero_stats);
        break;

    default:
        /* Prevent Lint warning about no default */
        break;
  } /* end switch */
} /* llc_log_packet_request*/



/*===========================================================================

FUNCTION  llc_log_llme_state_cb

DESCRIPTION
  Callback registered internally to LLC, to be called when LLC ME
  state info changes.
  It will send a log packet to the DM.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void llc_log_llme_state_cb(const gllc_diag_llme_state_rpt_t *rpt_ptr)
{
  LOG_GPRS_LLC_ME_INFO_C_type *log_ptr;    /* pointer to log buffer */

  gas_id_t gas_id = GERAN_ACCESS_STRATUM_ID_1;

  /* Do we want this packet? */
  if ( (packet_wanted[ gas_id ] != LLC_PACKET_LOG_ALL) &&
       (packet_wanted[ gas_id ] != LLC_PACKET_ME_INFO) )
  {
    DEBUGMSG( "llc_log_llme_state_cb> Packet not wanted", 0,0,0 );
    return;
  }

  /* Try to allocate memory for the log packet -             */
  /* this will fail if the log mask is not set appropriately */
  log_ptr = (LOG_GPRS_LLC_ME_INFO_C_type *)
    log_alloc(LOG_GPRS_LLC_ME_INFO_C, sizeof(LOG_GPRS_LLC_ME_INFO_C_type));
  if (log_ptr == NULL)
  {
    DEBUGMSG( "llc_log_llme_state_cb> log_alloc failed", 0,0,0 );
  }
  else
  {
    /* Fill packet and commit it */
    log_ptr->llme_state               = rpt_ptr->llme_state;
    log_ptr->llme_xid_version         = rpt_ptr->llme_xid_version;
    log_ptr->llme_new_tlli            = rpt_ptr->llme_new_tlli;
    log_ptr->llme_old_tlli            = rpt_ptr->llme_old_tlli;
    log_ptr->llme_encrypt_alg         = rpt_ptr->llme_encrypt_alg;
    log_ptr->llme_encrypt_key[0]      = rpt_ptr->llme_encrypt_key[0];
    log_ptr->llme_encrypt_key[1]      = rpt_ptr->llme_encrypt_key[1];
    log_ptr->llme_input_offset_iov_ui = rpt_ptr->llme_input_offset_iov_ui;

    log_commit(log_ptr);

    DEBUGMSG( "llc_log_llme_state_cb> packet logged", 0,0,0 );
  }
} /* llc_log_llme_state_cb */



/*===========================================================================

FUNCTION  llc_log_lle_state_cb

DESCRIPTION
  Callback registered internally to LLC, to be called when LLE
  state info changes.
  It will send a log packet to the DM.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void llc_log_lle_state_cb(const gllc_diag_lle_state_rpt_t *rpt_ptr)
{
  LOG_GPRS_LLC_SAPI_STATES_C_type *log_ptr;    /* pointer to log buffer */
  gas_id_t gas_id = GERAN_ACCESS_STRATUM_ID_1;

  /* Do we want this packet? */
  if ( (packet_wanted[ gas_id ] != LLC_PACKET_LOG_ALL) &&
       (packet_wanted[ gas_id ] != LLC_PACKET_SAPI_STATES) )
  {
    DEBUGMSG( "llc_log_lle_state_cb> Packet not wanted", 0,0,0 );
    return;
  }

  /* Do we want the state for this SAPI? */
  if (sapi_wanted[ gas_id ] != LLC_SAPI_LOG_ALL)
  {
    if (rpt_ptr->lle_sapi_addr != sapi_wanted[ gas_id ])
    {
      DEBUGMSG( "llc_log_lle_state_cb> Discard SAPI %d (%d)",
                rpt_ptr->lle_sapi_addr, sapi_wanted[ gas_id ], 0 );
      return;
    }
  }

  /* Try to allocate memory for the log packet -             */
  /* this will fail if the log mask is not set appropriately */
  log_ptr = (LOG_GPRS_LLC_SAPI_STATES_C_type *)
    log_alloc(LOG_GPRS_LLC_SAPI_STATES_C, sizeof(LOG_GPRS_LLC_SAPI_STATES_C_type));
  if (log_ptr == NULL)
  {
    DEBUGMSG( "llc_log_lle_state_cb> log_alloc failed", 0,0,0 );
  }
  else
  {
    /* Fill packet and commit it */
    log_ptr->gllc_sapi         = rpt_ptr->lle_sapi_addr;
    log_ptr->gllc_lle_state    = rpt_ptr->lle_state;
    log_ptr->gllc_lle_substate = rpt_ptr->lle_substate;

    log_commit(log_ptr);

    DEBUGMSG( "llc_log_lle_state_cb> packet logged", 0,0,0 );
  }
} /* llc_log_lle_state_cb */



/*===========================================================================

FUNCTION  llc_log_lle_xid_cb

DESCRIPTION
  Callback registered internally to LLC, to be called when XID
  info changes.
  It will send a log packet to the DM.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void llc_log_lle_xid_cb(const gllc_diag_lle_xid_rpt_t *rpt_ptr)
{
  LOG_GPRS_LLC_XID_INFO_C_type *log_ptr;    /* pointer to log buffer */
  gas_id_t gas_id = GERAN_ACCESS_STRATUM_ID_1;

  /* Do we want this packet? */
  if ( (packet_wanted[ gas_id ] != LLC_PACKET_LOG_ALL) &&
       (packet_wanted[ gas_id ] != LLC_PACKET_XID_INFO) )
  {
    DEBUGMSG( "llc_log_lle_xid_cb> Packet not wanted", 0,0,0 );
    return;
  }

  /* Do we want the XID information for this SAPI? */
  if (sapi_wanted[ gas_id ] != LLC_SAPI_LOG_ALL)
  {
    if (rpt_ptr->lle_sapi_addr != sapi_wanted[ gas_id ])
    {
      DEBUGMSG( "llc_log_lle_xid_cb> Discard SAPI %d (%d)",
                rpt_ptr->lle_sapi_addr, sapi_wanted[ gas_id ], 0 );
      return;
    }
  }

  /* Try to allocate memory for the log packet -             */
  /* this will fail if the log mask is not set appropriately */
  log_ptr = (LOG_GPRS_LLC_XID_INFO_C_type *)
    log_alloc(LOG_GPRS_LLC_XID_INFO_C, sizeof(LOG_GPRS_LLC_XID_INFO_C_type));
  if (log_ptr == NULL)
  {
    DEBUGMSG( "llc_log_lle_xid_cb> log_alloc failed", 0,0,0 );
  }
  else
  {
    /* Fill packet and commit it */
    log_ptr->gllc_sapi          = rpt_ptr->lle_sapi_addr;
    log_ptr->pending_xid_bitmap = rpt_ptr->pending_xid_bitmap;
    log_ptr->current_iov_i      = rpt_ptr->current_iov_i;
    log_ptr->current_t200       = rpt_ptr->current_t200;
    log_ptr->preferred_t200     = rpt_ptr->preferred_t200;
    log_ptr->current_n200       = rpt_ptr->current_n200;
    log_ptr->preferred_n200     = rpt_ptr->preferred_n200;
    log_ptr->current_n201_u     = rpt_ptr->current_n201_u;
    log_ptr->preferred_n201_u   = rpt_ptr->preferred_n201_u;
    log_ptr->current_n201_i     = rpt_ptr->current_n201_i;
    log_ptr->preferred_n201_i   = rpt_ptr->preferred_n201_i;
    log_ptr->current_md         = rpt_ptr->current_md;
    log_ptr->preferred_md       = rpt_ptr->preferred_md;
    log_ptr->current_mu         = rpt_ptr->current_mu;
    log_ptr->preferred_mu       = rpt_ptr->preferred_mu;
    log_ptr->current_kd         = rpt_ptr->current_kd;
    log_ptr->preferred_kd       = rpt_ptr->preferred_kd;
    log_ptr->current_ku         = rpt_ptr->current_ku;
    log_ptr->preferred_ku       = rpt_ptr->preferred_ku;

    log_commit(log_ptr);

    DEBUGMSG( "llc_log_lle_xid_cb> packet logged", 0,0,0 );
  }
} /* llc_log_lle_xid_cb */



/*===========================================================================

FUNCTION  llc_log_me_info

DESCRIPTION
  This function is called by the log-on-demand handler, to send a
  LLME Info log packet to the DM.
  The function uses a documented side effect of gllc_diag_register()
  to cause the callback llc_log_llme_state_cb() to be invoked.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void llc_log_me_info(void)
{
  gas_id_t gas_id = GERAN_ACCESS_STRATUM_ID_1;

  /* Ensure logging is initialised */
  llc_log_init( gas_id );

  /* Select the packet type for the callback */
  packet_wanted[ gas_id ] = LLC_PACKET_ME_INFO;

  /* First de-register the callback for this packet */
  DEBUGMSG( "llc_log_me_info> about to deregister", 0,0,0 );
  gllc_diag_register(gas_id,
                     NULL,
                     llc_log_lle_state_cb,
                     llc_log_lle_xid_cb);

  /* Re-registering the callback will cause it to be invoked immediately */
  DEBUGMSG( "llc_log_me_info> about to reregister", 0,0,0 );
  gllc_diag_register(gas_id,
                     llc_log_llme_state_cb,
                     llc_log_lle_state_cb,
                     llc_log_lle_xid_cb);

  /* Restore the packet type for the callback */
  packet_wanted[ gas_id ] = LLC_PACKET_LOG_ALL;

  DEBUGMSG( "llc_log_me_info> finished", 0,0,0 );
} /* llc_log_me_info */



/*===========================================================================

FUNCTION  llc_log_sapi_states

DESCRIPTION
  This function is called by the log-packet-request handler, to send a
  SAPI States log packet to the DM.
  The function uses a documented side effect of gllc_diag_register()
  to cause the callback llc_log_lle_state_cb() to be invoked.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void llc_log_sapi_states(uint8 sapi)
{

  gas_id_t gas_id = GERAN_ACCESS_STRATUM_ID_1;

  /* Ensure logging is initialised */
  llc_log_init( gas_id );

  /* Set up the required SAPI and packet type for the callback */
  packet_wanted[ gas_id ] = LLC_PACKET_SAPI_STATES;
  sapi_wanted[ gas_id ]   = sapi;

  /* First de-register the callback for this packet */
  DEBUGMSG( "llc_log_sapi_states> about to deregister", 0,0,0 );
  gllc_diag_register(gas_id,
                     llc_log_llme_state_cb,
                     NULL,
                     llc_log_lle_xid_cb);

  /* Re-registering the callback will cause it to be invoked immediately */
  DEBUGMSG( "llc_log_sapi_states> about to reregister", 0,0,0 );
  gllc_diag_register(gas_id,
                     llc_log_llme_state_cb,
                     llc_log_lle_state_cb,
                     llc_log_lle_xid_cb);

  /* Restore the original SAPI and packet type */
  packet_wanted[ gas_id ] = LLC_PACKET_LOG_ALL;
  sapi_wanted[ gas_id ]   = LLC_SAPI_LOG_ALL;

  DEBUGMSG( "llc_log_sapi_states> finished", 0,0,0 );
} /* llc_log_sapi_states */



/*===========================================================================

FUNCTION  llc_log_xid_info

DESCRIPTION
  This function is called by the log-packet-request handler, to send a
  XID Info log packet to the DM.
  The function uses a documented side effect of gllc_diag_register()
  to cause the callback llc_log_lle_xid_cb() to be invoked.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void llc_log_xid_info(uint8 sapi)
{

  gas_id_t gas_id = GERAN_ACCESS_STRATUM_ID_1;

  /* Ensure logging is initialised */
  llc_log_init( gas_id );

  /* Set up the required SAPI and packet type for the callback */
  packet_wanted[ gas_id ] = LLC_PACKET_XID_INFO;
  sapi_wanted[ gas_id ]   = sapi;

  /* First de-register the callback for this packet */
  DEBUGMSG( "llc_log_xid_info> about to deregister", 0,0,0 );
  gllc_diag_register(gas_id,
                     llc_log_llme_state_cb,
                     llc_log_lle_state_cb,
                     NULL);

  /* Re-registering the callback will cause it to be invoked immediately */
  DEBUGMSG( "llc_log_xid_info> about to reregister", 0,0,0 );
  gllc_diag_register(gas_id,
                     llc_log_llme_state_cb,
                     llc_log_lle_state_cb,
                     llc_log_lle_xid_cb);

  /* Restore the original SAPI and packet type */
  packet_wanted[ gas_id ] = LLC_PACKET_LOG_ALL;
  sapi_wanted[ gas_id ]   = LLC_SAPI_LOG_ALL;

  DEBUGMSG( "llc_log_xid_info> finished", 0,0,0 );
} /* llc_log_xid_info */



/*===========================================================================

FUNCTION  llc_log_pdu_stats

DESCRIPTION
  This function is called by the log-packet-request handler, to send a
  PDU Statistics log packet to the DM.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void llc_log_pdu_stats(uint8 sapi, uint8 zero_stats)
{
  const gllc_diag_pdu_stat_rsp_t *rsp_ptr; /* Pointer to stats inside LLC */
  LOG_GPRS_LLC_PDU_STATS_C_type  *log_ptr; /* pointer to log buffer */
  gsmdiag_pkt_info_t              pkt_info;
  gas_id_t                        gas_id = GERAN_ACCESS_STRATUM_ID_1;

  /* Ensure logging is initialised */
  llc_log_init( gas_id );

  llc_log_packet_init( gas_id,
                       &pkt_info,
                       LOG_GPRS_LLC_PDU_STATS_C,
                       sizeof(LOG_GPRS_LLC_PDU_STATS_C_type)
                     );

  /* First clear ou the statistics if required */
  if (zero_stats)
  {
    DEBUGMSG( "llc_log_pdu_stats> clear statistics", 0,0,0 );
    gllc_diag_reset_pdu_stats(gas_id, sapi);
  }

  /* Obtain the latest statistics for this SAPI */
  rsp_ptr = gllc_diag_get_pdu_stats(gas_id, sapi);
  if (rsp_ptr == NULL)
  {
    DEBUGMSG( "llc_log_pdu_stats> unable to get statistics", 0,0,0 );
    return;
  }

  /* Try to allocate memory for the log packet -             */
  /* this will fail if the log mask is not set appropriately */
  log_ptr = (LOG_GPRS_LLC_PDU_STATS_C_type *)gsmdiag_log_alloc(&pkt_info);


  if (log_ptr == NULL)
  {
    DEBUGMSG( "llc_log_pdu_stats> log_alloc failed", 0,0,0 );
  }
  else
  {
    /* Fill packet and commit it */
    log_ptr->lle_sapi_addr       = rsp_ptr->lle_sapi_addr;
    log_ptr->ack_l3pdu_tx_cnt    = rsp_ptr->ack_l3pdu_tx_cnt;
    log_ptr->ack_octet_tx_cnt    = rsp_ptr->ack_octet_tx_cnt;
    log_ptr->ack_l3pdu_retx_cnt  = rsp_ptr->ack_l3pdu_retx_cnt;
    log_ptr->ack_octet_retx_cnt  = rsp_ptr->ack_octet_retx_cnt;
    log_ptr->ack_l3pdu_rx_cnt    = rsp_ptr->ack_l3pdu_rx_cnt;
    log_ptr->ack_octet_rx_cnt    = rsp_ptr->ack_octet_rx_cnt;
    log_ptr->ack_l3pdu_rerx_cnt  = rsp_ptr->ack_l3pdu_rerx_cnt;
    log_ptr->ack_octet_rerx_cnt  = rsp_ptr->ack_octet_rerx_cnt;
    log_ptr->unack_l3pdu_tx_cnt  = rsp_ptr->unack_l3pdu_tx_cnt;
    log_ptr->unack_octet_tx_cnt  = rsp_ptr->unack_octet_tx_cnt;
    log_ptr->unack_l3pdu_rx_cnt  = rsp_ptr->unack_l3pdu_rx_cnt;
    log_ptr->unack_octet_rx_cnt  = rsp_ptr->unack_octet_rx_cnt;
    log_ptr->llpdu_tx_cnt        = rsp_ptr->llpdu_tx_cnt;
    log_ptr->llpdu_rx_cnt        = rsp_ptr->llpdu_rx_cnt;
    log_ptr->llpdu_fcs_err_cnt   = rsp_ptr->llpdu_fcs_err_cnt;
    log_ptr->llpdu_frm_rej_cnt   = rsp_ptr->llpdu_frm_rej_cnt;
    log_ptr->llpdu_tlli_err_cnt  = rsp_ptr->llpdu_tlli_err_cnt;
    log_ptr->llpdu_addr_err_cnt  = rsp_ptr->llpdu_addr_err_cnt;
    log_ptr->llpdu_short_err_cnt = rsp_ptr->llpdu_short_err_cnt;

    gsmdiag_log_commit(&pkt_info);

    DEBUGMSG( "llc_log_pdu_stats> packet logged", 0,0,0 );
  }
} /* llc_log_pdu_stats */



/*===========================================================================

FUNCTION  llc_log_periodic_stats

DESCRIPTION
  This function is called by the log-packet-request handler, to send a
  Periodic Statistics log packet to the DM.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void llc_log_periodic_stats(uint8 sapi, uint8 zero_stats)
{
  const gllc_diag_state_stat_rsp_t   *rsp_ptr; /* Pointer to stats inside LLC */
  LOG_GPRS_LLC_PERIODIC_STATS_C_type *log_ptr; /* pointer to log buffer */
  gsmdiag_pkt_info_t                  pkt_info;
  gas_id_t                            gas_id = GERAN_ACCESS_STRATUM_ID_1;

  /* Ensure logging is initialised */
  llc_log_init( gas_id );

  llc_log_packet_init( gas_id,
                       &pkt_info,
                       LOG_GPRS_LLC_PERIODIC_STATS_C,
                       sizeof(LOG_GPRS_LLC_PERIODIC_STATS_C_type)
                     );

  /* First clear out the statistics if required */
  if (zero_stats)
  {
    DEBUGMSG( "llc_log_periodic_stats> clear statistics", 0,0,0 );
    gllc_diag_reset_ancilliary_stats( gas_id, sapi);
  }

  /* Obtain the latest statistics for this SAPI */
  rsp_ptr = gllc_diag_get_state_stats( gas_id, sapi);
  if (rsp_ptr == NULL)
  {
    DEBUGMSG( "llc_log_periodic_stats> unable to get statistics", 0,0,0 );
    return;
  }

  /* Try to allocate memory for the log packet -             */
  /* this will fail if the log mask is not set appropriately */
  log_ptr = (LOG_GPRS_LLC_PERIODIC_STATS_C_type *)gsmdiag_log_alloc(&pkt_info);

  if (log_ptr == NULL)
  {
    DEBUGMSG( "llc_log_periodic_stats> log_alloc failed", 0,0,0 );
  }
  else
  {
    /* Fill packet and commit it */
    log_ptr->lle_sapi_addr          = rsp_ptr->lle_sapi_addr;
    log_ptr->unack_snd_seq_var_v_u  = rsp_ptr->unack_snd_seq_var_v_u;
    log_ptr->unack_rcv_seq_var_v_ur = rsp_ptr->unack_rcv_seq_var_v_ur;
    log_ptr->unack_encipher_oc      = rsp_ptr->unack_encipher_oc;
    log_ptr->unack_decipher_oc      = rsp_ptr->unack_decipher_oc;
    log_ptr->ack_snd_seq_var_v_s    = rsp_ptr->ack_snd_seq_var_v_s;
    log_ptr->ack_rcv_seq_var_v_r    = rsp_ptr->ack_rcv_seq_var_v_r;
    log_ptr->ack_seq_var_v_a        = rsp_ptr->ack_seq_var_v_a;
    log_ptr->ack_encipher_oc        = rsp_ptr->ack_encipher_oc;
    log_ptr->ack_decipher_oc        = rsp_ptr->ack_decipher_oc;
    log_ptr->reestablishment_cnt    = rsp_ptr->reestablishment_cnt;
    log_ptr->gen_purpose_a_cnt      = rsp_ptr->gen_purpose_a_cnt;
    log_ptr->gen_purpose_b_cnt      = rsp_ptr->gen_purpose_b_cnt;
    log_ptr->gen_purpose_c_cnt      = rsp_ptr->gen_purpose_c_cnt;

    gsmdiag_log_commit(&pkt_info);

    DEBUGMSG( "llc_log_periodic_stats> packet logged", 0,0,0 );
  }
} /* llc_log_periodic_stats */



/*===========================================================================

FUNCTION  llc_log_ota_message

DESCRIPTION
  This function is called by the LLC code to log a SM or GMM OTA
  signalling message..

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void llc_log_ota_message( gas_id_t     gas_id,
                          boolean      downlink,
                          uint8        message_type,
                          uint16       message_length,
                          const uint8 *message_ptr
                        )
{
  LOG_GPRS_SM_GMM_OTA_MESSAGE_C_type *log_ptr; /* pointer to log buffer */
  gsmdiag_pkt_info_t                  pkt_info;

  /* Ensure logging is initialised */
  llc_log_init( gas_id );

  /* Determine whether or not there is any message payload */
  if (message_length == 0)
    message_ptr = NULL;
  else if (message_ptr == NULL)
    message_length = 0;

  /* Try to allocate memory for the log packet -             */
  /* this will fail if the log mask is not set appropriately */
  if (message_ptr == NULL)
  {

    llc_log_packet_init( gas_id,
                         &pkt_info,
                         LOG_GPRS_SM_GMM_OTA_MESSAGE_C,
                         ( sizeof(LOG_GPRS_SM_GMM_OTA_MESSAGE_C_type) - 1)
                       );

    /* Message has no payload - only a message type */
    log_ptr = (LOG_GPRS_SM_GMM_OTA_MESSAGE_C_type *)gsmdiag_log_alloc(&pkt_info);

  }
  else
  {
    /* Log an event if we have a payload */
    byte payload[4];

    payload[0] = geran_get_diag_sub_id(gas_id);
    payload[1] = message_type;              /* Message type */
    payload[2] = message_ptr[0] & 0x0F;     /* Protocol discriminator */
    payload[3] = 0;                         /* Channel type */

    if(downlink)
    {
      geran_diag_event(EVENT_GPRS_SMGMM_MSG_RECEIVED, EVENTS_DS_GPRS_SMGMM_MSG_RECEIVED, 
        sizeof(payload), payload);
    }
    else
    {
      geran_diag_event(EVENT_GPRS_SMGMM_MSG_SENT, EVENTS_DS_GPRS_SMGMM_MSG_SENT, 
        sizeof(payload), payload);
    }

    DEBUGMSG( "llc_log_ota_message> event logged", 0,0,0 );

    llc_log_packet_init( gas_id,
                         &pkt_info,
                         LOG_GPRS_SM_GMM_OTA_MESSAGE_C,
                         (( sizeof(LOG_GPRS_SM_GMM_OTA_MESSAGE_C_type) - 1) + message_length )
                       );


    /* Message has a payload */
    log_ptr = (LOG_GPRS_SM_GMM_OTA_MESSAGE_C_type *)gsmdiag_log_alloc(&pkt_info);
  }

  if (log_ptr == NULL)
  {
    DEBUGMSG( "llc_log_ota_message> log_alloc failed", 0,0,0 );
  }
  else
  {
    /* Fill the log packet and commit it */
    log_ptr->direction = (downlink ? 1 : 0);
    log_ptr->message_type = message_type;
    log_ptr->message_length = message_length;

    if (message_ptr != NULL)
      memscpy((void *) (log_ptr->message), sizeof(log_ptr->message),message_ptr, message_length);

    gsmdiag_log_commit(&pkt_info);

    MSG_GERAN_MED_2_G(" Message size %d, dest buffer size %d ", message_length, sizeof(log_ptr->message));
    DEBUGMSG( "llc_log_ota_message> packet logged", 0,0,0 );
  }
} /* llc_log_ota_message */


/*===========================================================================

FUNCTION  llc_log_ready_timer_status

DESCRIPTION
  Indicating GMM Ready Timer status
  It will send a log packet to the DM.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void llc_log_ready_timer_status( gas_id_t gas_id )
{
  LOG_GPRS_LLC_READY_TIMER_STATUS_C_type      *log_ptr;    /* pointer to log buffer */
  LOG_GPRS_LLC_READY_TIMER_STATUS_VER2_C_type *log_ver2_ptr;
  gsmdiag_pkt_info_t                           pkt_info;
  byte payload;

  llc_log_init( gas_id );
  payload = geran_get_diag_sub_id(gas_id);

  if (gllc_gmm_ready_timer_active[ gas_id ] == TRUE)
  {
    geran_diag_event(EVENT_GPRS_LLC_READY_TIMER_START, EVENTS_DS_GPRS_LLC_READY_TIMER_START, sizeof(payload), &payload);
  }
  else
  {
    geran_diag_event(EVENT_GPRS_LLC_READY_TIMER_END, EVENTS_DS_GPRS_LLC_READY_TIMER_END, sizeof(payload), &payload);
  }

  llc_log_packet_init(gas_id, &pkt_info, LOG_GPRS_LLC_READY_TIMER_STATUS_VER2_C, sizeof(LOG_GPRS_LLC_READY_TIMER_STATUS_VER2_C_type));

  /* Do we want this packet? */
  /* Try to allocate memory for the log packet -             */
  /* this will fail if the log mask is not set appropriately */
  log_ver2_ptr = (LOG_GPRS_LLC_READY_TIMER_STATUS_VER2_C_type *)gsmdiag_log_alloc(&pkt_info);

  if (log_ver2_ptr != NULL)
  {
    log_ver2_ptr->ready_timer_value  = gllc_gmm_ready_time[ gas_id ];
    log_ver2_ptr->ready_timer_status = gllc_gmm_ready_timer_active[ gas_id ];
    gsmdiag_log_commit(&pkt_info);
  }

  llc_log_packet_init(gas_id, &pkt_info, LOG_GPRS_LLC_READY_TIMER_STATUS_C, sizeof(LOG_GPRS_LLC_READY_TIMER_STATUS_C_type));

  log_ptr = (LOG_GPRS_LLC_READY_TIMER_STATUS_C_type *)gsmdiag_log_alloc(&pkt_info);

  if (log_ptr != NULL)
  {
    log_ptr->ready_timer_status = gllc_gmm_ready_timer_active[ gas_id ];
    gsmdiag_log_commit(&pkt_info);
  }
}

/*===========================================================================

FUNCTION  llc_log_send_ui_frame

DESCRIPTION

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void llc_log_send_ui_frame(gas_id_t gas_id, uint16 send_seq, uint8 sapi, uint16 pdu_len, uint8 cipher_mode)
{

  LOG_GPRS_LLC_SEND_UI_FRAME_C_type *log_ptr;    /* pointer to log buffer */
  gsmdiag_pkt_info_t                 pkt_info;

  llc_log_init( gas_id );

  llc_log_packet_init(gas_id, &pkt_info, LOG_GPRS_LLC_SEND_UI_FRAME_C, sizeof(LOG_GPRS_LLC_SEND_UI_FRAME_C_type));

  /* Do we want this packet? */
  /* Try to allocate memory for the log packet -             */
  /* this will fail if the log mask is not set appropriately */
  log_ptr = (LOG_GPRS_LLC_SEND_UI_FRAME_C_type *)gsmdiag_log_alloc(&pkt_info);

  if (log_ptr != NULL)
  {
    log_ptr->seq_num     = send_seq;
    log_ptr->sapi        = sapi;
    log_ptr->pdu_len     = pdu_len;
    log_ptr->cipher_mode = cipher_mode;

    gsmdiag_log_commit(&pkt_info);
  }

} /*llc_log_send_ui_frame*/


/*===========================================================================

FUNCTION  llc_log_recv_ui_frame

DESCRIPTION

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void llc_log_recv_ui_frame(gas_id_t gas_id, uint16 recv_seq, uint8 sapi, uint16 pdu_len, uint8 cipher_mode)
{

  LOG_GPRS_LLC_RECV_UI_FRAME_C_type *log_ptr;    /* pointer to log buffer */
  gsmdiag_pkt_info_t                 pkt_info;

  llc_log_init( gas_id );

  llc_log_packet_init(gas_id, &pkt_info, LOG_GPRS_LLC_RECV_UI_FRAME_C, sizeof(LOG_GPRS_LLC_RECV_UI_FRAME_C_type));


  /* Do we want this packet? */
  /* Try to allocate memory for the log packet -             */
  /* this will fail if the log mask is not set appropriately */
  log_ptr = (LOG_GPRS_LLC_RECV_UI_FRAME_C_type *)gsmdiag_log_alloc(&pkt_info);

  if (log_ptr != NULL)
  {
    log_ptr->seq_num     = recv_seq;
    log_ptr->sapi        = sapi;
    log_ptr->pdu_len     = pdu_len;
    log_ptr->cipher_mode = cipher_mode;

    gsmdiag_log_commit(&pkt_info);
  }

} /*llc_log_recv_ui_frame*/


