/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                           SNDCP Logging Facilities

GENERAL DESCRIPTION
   This module contains functions for logging SNDCP packets.

EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2002-2013 Qualcomm Technologies, Inc.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/gsndcp/src/gsndcplog.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/13/02   ADG     Initial Revision.
===========================================================================*/

#include "geran_variation.h"
#include "customer.h"        /* Include before using any FEATUREs */


/* Debug F3 messages will be emitted if the following line is uncommented */
/* #define SNDCP_LOG_DEBUG */

#if defined(SNDCP_LOG_DEBUG)
#define DEBUGMSG(a,b,c,d)       MSG_HIGH( (a),(b),(c),(d) )
#else
#define DEBUGMSG(a,b,c,d)       /* */
#endif /* defined(SNDCP_LOG_DEBUG) */

#define FEATURE_DSM_WM_CB


/*===========================================================================
                     INCLUDE FILES FOR MODULE
===========================================================================*/
#include <string.h>          /* For memcpy()                               */
#include "log.h"             /* For log_commit(), log_alloc(), log codes   */
#include "event.h"           /* For logging RAR event and event codes      */
#include "log_codes_gsm_v.h"   /* For GSM/GPRS log codes                     */
#ifdef DEBUG_USE_OLD_DIAG
#error code not present
#else
#include "gsmdiag_v.h"
#endif

#include "amssassert.h"
#include "gprs_pdu.h"

#include "gsndcp.h"          /* For structure definitions                  */
#include "gsndef.h"
#include "gsndcplog.h"       /* This module's header file                  */

/*===========================================================================
              TYPE DEFINITIONS FOR EVENTS & LOG PACKETS
===========================================================================*/

/*lint -esym(754, LOG_GPRS_SNDCP_ACTIVE_PDP_CONTEXTS_C_tag::hdr)*/
LOG_RECORD_DEFINE(LOG_GPRS_SNDCP_ACTIVE_PDP_CONTEXTS_C)
  uint16  active_nsapis;
LOG_RECORD_END

/*lint -esym(754, LOG_GPRS_SNDCP_PDP_CONTEXT_INFO_C_tag::hdr)*/
LOG_RECORD_DEFINE(LOG_GPRS_SNDCP_PDP_CONTEXT_INFO_C)
  uint8   nsapi_addr;
  uint8   sapi_addr;
  uint16  n201_u;
  uint16  n201_i;
  uint8   reliability_class;
  uint8   peak_throughput;
  uint8   radio_priority;
  boolean ciphering;
  boolean data_comp;
  uint8   data_comp_ent;
  uint8   data_comp_alg;
  boolean protocol_comp;
  uint8   protocol_comp_ent;
  uint8   protocol_comp_alg;
LOG_RECORD_END

/*lint -esym(754, LOG_GPRS_SNDCP_STATES_C_tag::hdr)*/
LOG_RECORD_DEFINE(LOG_GPRS_SNDCP_STATES_C)
  uint8   nsapi_addr;
  uint8   nsapi_ul_state;
  uint8   nsapi_dl_state;
  uint8   sapi_state;
LOG_RECORD_END

/*lint -esym(754, LOG_GPRS_SNDCP_PDU_STATS_C_tag::hdr)*/
LOG_RECORD_DEFINE(LOG_GPRS_SNDCP_PDU_STATS_C)
  uint8   nsapi_addr;
  uint32  npdu_tx_cnt;
  uint32  npdu_octet_tx_cnt;
  uint32  npdu_retx_cnt;
  uint32  npdu_octet_retx_cnt;
  uint32  npdu_rx_cnt;
  uint32  npdu_octet_rx_cnt;
  uint32  snpdu_tx_cnt;
  uint32  snpdu_octet_tx_cnt;
  uint32  snpdu_retx_cnt;
  uint32  snpdu_octet_retx_cnt;
  uint32  snpdu_rx_cnt;
  uint32  snpdu_octet_rx_cnt;
  uint32  gen_purpose_a_cnt;
  uint32  gen_purpose_b_cnt;
  uint32  gen_purpose_c_cnt;
LOG_RECORD_END

/*lint -esym(754, LOG_GPRS_SNDCP_UL_TCP_HDR_C_tag::hdr)*/
LOG_RECORD_DEFINE(LOG_GPRS_SNDCP_UL_TCP_HDR_C)
  uint16     src_prt;
  uint16     dest_prt;
  uint32     seq;
  uint32     ack;
  uint16     win;
  uint16     data_len;
LOG_RECORD_END

/*lint -esym(754, LOG_GPRS_SNDCP_DL_TCP_HDR_C_tag::hdr)*/
LOG_RECORD_DEFINE(LOG_GPRS_SNDCP_DL_TCP_HDR_C)
  uint16     src_prt;
  uint16     dest_prt;
  uint32     seq;
  uint32     ack;
  uint16     win;
  uint16     data_len;
LOG_RECORD_END

/*===========================================================================
                         DATA DECLARATIONS
===========================================================================*/
boolean sndcp_log_initialized[ NUM_GERAN_DATA_SPACES ] = {FALSE};

/* Used by the reporting callbacks to determine whether the data supplied  */
/* originates from the required SAPI and packet. Zero means always accept. */
typedef enum _SNDCP_PACKET_TO_LOG
{
    SNDCP_PACKET_LOG_ALL,
    SNDCP_PACKET_ACTIVE_PDP_CONTEXTS,
    SNDCP_PACKET_PDP_CONTEXT,
    SNDCP_PACKET_NSAPI_STATE
} SNDCP_PACKET_TO_LOG;

SNDCP_PACKET_TO_LOG gsn_packet_wanted[ NUM_GERAN_DATA_SPACES ] = { SNDCP_PACKET_LOG_ALL };

#define SNDCP_NSAPI_LOG_ALL 0
uint8 nsapi_wanted[ NUM_GERAN_DATA_SPACES ] = { SNDCP_NSAPI_LOG_ALL };


/*===========================================================================
                         FUNCTION DECLARATIONS
===========================================================================*/
/* Log-on-demand callback to be registered with DIAG */
log_on_demand_status_enum_type sndcp_log_on_demand(log_code_type log_code);

/* Log-packet-request callback to be registered with DIAG */
void sndcp_log_packet_request(uint16 log_code,
                              uint8  zero_stats,
                              uint8  additional_info);

/* Reporting callbacks to be registered with other parts of SNDCP */
void sndcp_log_active_nsapis_cb(const gsndcp_diag_actv_nsapis_rpt_t *rpt_ptr);
void sndcp_log_nsapi_pdp_context_cb(const gsndcp_diag_nsapi_cntx_rpt_t *rpt_ptr);
void sndcp_log_nsapi_state_cb(const gsndcp_diag_nsapi_state_rpt_t *rpt_ptr);


/*===========================================================================

FUNCTION  sndcp_log_init

DESCRIPTION
  This function initializes the log_on_demand packets.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void sndcp_log_init(void)
{
  gas_id_t gas_id = GERAN_ACCESS_STRATUM_ID_1;

  if (sndcp_log_initialized[ gas_id ])
  {
      DEBUGMSG( "sndcp_log_init> already initialised", 0,0,0 );
      return;
  }
  else
  {
      /* Call this only once */
      sndcp_log_initialized[ gas_id ] = TRUE;
      gsn_packet_wanted[ gas_id ]     = SNDCP_PACKET_LOG_ALL;
      nsapi_wanted[ gas_id ]          = SNDCP_NSAPI_LOG_ALL;

      DEBUGMSG( "sndcp_log_init> register log-on-demand", 0,0,0 );
      (void)log_on_demand_register(
        LOG_GPRS_SNDCP_ACTIVE_PDP_CONTEXTS_C, sndcp_log_on_demand);

      DEBUGMSG( "sndcp_log_init> register log-packet-request", 0,0,0 );
      (void)diagpkt_gsm_gprs_lpr_register(
        LOG_GPRS_SNDCP_ACTIVE_PDP_CONTEXTS_C, sndcp_log_packet_request);
      (void)diagpkt_gsm_gprs_lpr_register(
        LOG_GPRS_SNDCP_PDP_CONTEXT_INFO_C, sndcp_log_packet_request);
      (void)diagpkt_gsm_gprs_lpr_register(
        LOG_GPRS_SNDCP_STATES_C, sndcp_log_packet_request);
      (void)diagpkt_gsm_gprs_lpr_register(
        LOG_GPRS_SNDCP_PDU_STATS_C, sndcp_log_packet_request);
      (void)diagpkt_gsm_gprs_lpr_register(
        LOG_GPRS_SNDCP_UL_TCP_HDR_C, sndcp_log_packet_request);
      (void)diagpkt_gsm_gprs_lpr_register(
        LOG_GPRS_SNDCP_DL_TCP_HDR_C, sndcp_log_packet_request);

      DEBUGMSG( "sndcp_log_init> register callbacks", 0,0,0 );
      gsndcp_diag_register(sndcp_log_active_nsapis_cb,
                           sndcp_log_nsapi_pdp_context_cb,
                           sndcp_log_nsapi_state_cb);

      DEBUGMSG( "sndcp_log_init> finished", 0,0,0 );
  }
}



/*===========================================================================

FUNCTION  sndcp_log_on_demand

DESCRIPTION
  This function calls relevant log packet routine when the DM demands it.

RETURN VALUE
  Status of the call (should always be good)

SIDE EFFECTS
  None

===========================================================================*/
log_on_demand_status_enum_type sndcp_log_on_demand(log_code_type log_code)
{
  boolean retVal = TRUE;

  switch (log_code)
  {
    case LOG_GPRS_SNDCP_ACTIVE_PDP_CONTEXTS_C:
      sndcp_log_active_nsapis();
      break;

    default:
      retVal = FALSE;
      break;
  }/* end switch */

  return (retVal ? LOG_ON_DEMAND_SENT_S : LOG_ON_DEMAND_FAILED_ATTEMPT_S);

} /* sndcp_log_on_demand */



/*===========================================================================

FUNCTION  sndcp_log_packet_request

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
void sndcp_log_packet_request(uint16 log_code,
                              uint8  zero_stats,
                              uint8  additional_info)
{
  switch (log_code)
  {
    case LOG_GPRS_SNDCP_ACTIVE_PDP_CONTEXTS_C:
        sndcp_log_active_nsapis();
        break;

    case LOG_GPRS_SNDCP_PDP_CONTEXT_INFO_C:
        /* additional_info is the NSAPI */
        sndcp_log_nsapi_pdp_context(additional_info);
        break;

    case LOG_GPRS_SNDCP_STATES_C:
        /* additional_info is the NSAPI */
        sndcp_log_nsapi_state(additional_info);
        break;

    case LOG_GPRS_SNDCP_PDU_STATS_C:
        /* additional_info is the NSAPI */
        sndcp_log_pdu_stats(additional_info, zero_stats);
        break;

    default:
        /* Prevent Lint warning about no default */
        break;
  } /* end switch */
} /* sndcp_log_packet_request*/



/*===========================================================================

FUNCTION  sndcp_log_active_nsapis_cb

DESCRIPTION
  Callback registered internally to SNDCP, to be called when the set of
  active NSAPIs changes.
  It will send a log packet to the DM.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void sndcp_log_active_nsapis_cb(const gsndcp_diag_actv_nsapis_rpt_t *rpt_ptr)
{
  LOG_GPRS_SNDCP_ACTIVE_PDP_CONTEXTS_C_type *log_ptr; /* pointer to log buffer */
  gas_id_t gas_id = GERAN_ACCESS_STRATUM_ID_1;

  /* Do we want this packet? */
  if ( (gsn_packet_wanted[ gas_id ] != SNDCP_PACKET_LOG_ALL) &&
       (gsn_packet_wanted[ gas_id ] != SNDCP_PACKET_ACTIVE_PDP_CONTEXTS) )
  {
    DEBUGMSG( "sndcp_log_active_nsapis_cb> Packet not wanted", 0,0,0 );
    return;
  }

  /* Try to allocate memory for the log packet -             */
  /* this will fail if the log mask is not set appropriately */
  log_ptr = (LOG_GPRS_SNDCP_ACTIVE_PDP_CONTEXTS_C_type *)log_alloc(LOG_GPRS_SNDCP_ACTIVE_PDP_CONTEXTS_C,
                      sizeof(LOG_GPRS_SNDCP_ACTIVE_PDP_CONTEXTS_C_type));
  if (log_ptr == NULL)
  {
    DEBUGMSG( "sndcp_log_active_nsapis_cb> log_alloc failed", 0,0,0 );
  }
  else
  {
    /* Fill packet and commit it */
    log_ptr->active_nsapis = *rpt_ptr;

    log_commit(log_ptr);

    DEBUGMSG( "sndcp_log_active_nsapis_cb> packet logged", 0,0,0 );
  }
} /* sndcp_log_active_nsapis_cb */



/*===========================================================================

FUNCTION  sndcp_log_nsapi_pdp_context_cb

DESCRIPTION
  Callback registered internally to SNDCP, to be called when a NSAPI's
  PDP context information changes.
  It will send a log packet to the DM.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void sndcp_log_nsapi_pdp_context_cb(const gsndcp_diag_nsapi_cntx_rpt_t *rpt_ptr)
{
  LOG_GPRS_SNDCP_PDP_CONTEXT_INFO_C_type *log_ptr; /* pointer to log buffer */
  gas_id_t gas_id = GERAN_ACCESS_STRATUM_ID_1;

  /* Do we want this packet? */
  if ( (gsn_packet_wanted[ gas_id ] != SNDCP_PACKET_LOG_ALL) &&
       (gsn_packet_wanted[ gas_id ] != SNDCP_PACKET_PDP_CONTEXT) )
  {
    DEBUGMSG( "sndcp_log_nsapi_pdp_context_cb> Packet not wanted", 0,0,0 );
    return;
  }

  /* Do we want the state for this NSAPI? */
  if (nsapi_wanted[ gas_id ] != SNDCP_NSAPI_LOG_ALL)
  {
    if (rpt_ptr->nsapi_addr != nsapi_wanted[ gas_id ])
    {
      DEBUGMSG( "sndcp_log_nsapi_pdp_context_cb> Discard NSAPI %d (%d)",
                rpt_ptr->nsapi_addr, nsapi_wanted[ gas_id ], 0 );
      return;
    }
  }

  /* Try to allocate memory for the log packet -             */
  /* this will fail if the log mask is not set appropriately */
  log_ptr = (LOG_GPRS_SNDCP_PDP_CONTEXT_INFO_C_type *)log_alloc(LOG_GPRS_SNDCP_PDP_CONTEXT_INFO_C,
                      sizeof(LOG_GPRS_SNDCP_PDP_CONTEXT_INFO_C_type));
  if (log_ptr == NULL)
  {
    DEBUGMSG( "sndcp_log_nsapi_pdp_context_cb> log_alloc failed", 0,0,0 );
  }
  else
  {
    /* Fill packet and commit it */
    log_ptr->nsapi_addr        = rpt_ptr->nsapi_addr;
    log_ptr->sapi_addr         = rpt_ptr->sapi_addr;
    log_ptr->n201_u            = rpt_ptr->n201_u;
    log_ptr->n201_i            = rpt_ptr->n201_i;
    log_ptr->reliability_class = rpt_ptr->reliability_class;
    log_ptr->peak_throughput   = rpt_ptr->peak_throughput;
    log_ptr->radio_priority    = rpt_ptr->radio_priority;
    log_ptr->ciphering         = rpt_ptr->ciphering;
    log_ptr->data_comp         = rpt_ptr->data_comp;
    log_ptr->data_comp_ent     = rpt_ptr->data_comp_ent;
    log_ptr->data_comp_alg     = rpt_ptr->data_comp_alg;
    log_ptr->protocol_comp     = rpt_ptr->protocol_comp;
    log_ptr->protocol_comp_ent = rpt_ptr->protocol_comp_ent;
    log_ptr->protocol_comp_alg = rpt_ptr->protocol_comp_alg;

    log_commit(log_ptr);

    DEBUGMSG( "sndcp_log_nsapi_pdp_context_cb> packet logged", 0,0,0 );
  }
} /* sndcp_log_nsapi_pdp_context_cb */



/*===========================================================================

FUNCTION  sndcp_log_nsapi_state_cb

DESCRIPTION
  Callback registered internally to SNDCP, to be called when a NSAPI's
  state changes.
  It will send a log packet to the DM.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void sndcp_log_nsapi_state_cb(const gsndcp_diag_nsapi_state_rpt_t *rpt_ptr)
{
  LOG_GPRS_SNDCP_STATES_C_type *log_ptr; /* pointer to log buffer */
  gas_id_t gas_id = GERAN_ACCESS_STRATUM_ID_1;

  /* Do we want this packet? */
  if ( (gsn_packet_wanted[ gas_id ] != SNDCP_PACKET_LOG_ALL) &&
       (gsn_packet_wanted[ gas_id ] != SNDCP_PACKET_NSAPI_STATE) )
  {
    DEBUGMSG( "sndcp_log_nsapi_state_cb> Packet not wanted", 0,0,0 );
    return;
  }

  /* Do we want the state for this NSAPI? */
  if (nsapi_wanted[ gas_id ] != SNDCP_NSAPI_LOG_ALL)
  {
    if (rpt_ptr->nsapi_addr != nsapi_wanted[ gas_id ])
    {
      DEBUGMSG( "sndcp_log_nsapi_state_cb> Discard NSAPI %d (%d)",
                rpt_ptr->nsapi_addr, nsapi_wanted[ gas_id ], 0 );
      return;
    }
  }

  /* Try to allocate memory for the log packet -             */
  /* this will fail if the log mask is not set appropriately */
  log_ptr = (LOG_GPRS_SNDCP_STATES_C_type *)log_alloc(LOG_GPRS_SNDCP_STATES_C,
                      sizeof(LOG_GPRS_SNDCP_STATES_C_type));
  if (log_ptr == NULL)
  {
    DEBUGMSG( "sndcp_log_nsapi_state_cb> log_alloc failed", 0,0,0 );
  }
  else
  {
    /* Fill packet and commit it */
    log_ptr->nsapi_addr     = rpt_ptr->nsapi_addr;
    log_ptr->nsapi_ul_state = rpt_ptr->nsapi_ul_state;
    log_ptr->nsapi_dl_state = rpt_ptr->nsapi_dl_state;
    log_ptr->sapi_state     = rpt_ptr->sapi_state;

    log_commit(log_ptr);

    DEBUGMSG( "sndcp_log_nsapi_state_cb> packet logged", 0,0,0 );
  }
} /* sndcp_log_nsapi_state_cb */



/*===========================================================================

FUNCTION  sndcp_log_active_nsapis

DESCRIPTION
  This function is called by the log-on-demand handler, to send a
  Active NSAPIs log packet to the DM.
  The function uses a documented side effect of gsndcp_diag_register()
  to cause the callback sndcp_log_active_nsapis_cb() to be invoked.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void sndcp_log_active_nsapis(void)
{
    gas_id_t gas_id = GERAN_ACCESS_STRATUM_ID_1;

    /* Ensure logging is initialised */
    sndcp_log_init();

    /* Select the packet type for the callback */
    gsn_packet_wanted[ gas_id ] = SNDCP_PACKET_ACTIVE_PDP_CONTEXTS;

    /* First de-register the callback for this packet */
    DEBUGMSG( "sndcp_log_active_nsapis> about to deregister", 0,0,0 );
    gsndcp_diag_register(NULL,
                         sndcp_log_nsapi_pdp_context_cb,
                         sndcp_log_nsapi_state_cb);

    /* Re-registering the callback will cause it to be invoked immediately */
    DEBUGMSG( "sndcp_log_active_nsapis> about to reregister", 0,0,0 );
    gsndcp_diag_register(sndcp_log_active_nsapis_cb,
                         sndcp_log_nsapi_pdp_context_cb,
                         sndcp_log_nsapi_state_cb);

    /* Restore the packet type for the callback */
    gsn_packet_wanted[ gas_id ] = SNDCP_PACKET_LOG_ALL;

    DEBUGMSG( "sndcp_log_active_nsapis> finished", 0,0,0 );
} /* sndcp_log_active_nsapis */



/*===========================================================================

FUNCTION  sndcp_log_nsapi_pdp_context

DESCRIPTION
  This function is called by the log-on-demand handler, to send a
  NSAPI PDP Context log packet to the DM.
  The function uses a documented side effect of gsndcp_diag_register()
  to cause the callback sndcp_log_nsapi_pdp_context_cb() to be invoked.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void sndcp_log_nsapi_pdp_context(uint8 nsapi)
{
    gas_id_t gas_id = GERAN_ACCESS_STRATUM_ID_1;

    /* Ensure logging is initialised */
    sndcp_log_init();

    /* Set up the required NSAPI and packet type for the callback */
    gsn_packet_wanted[ gas_id ] = SNDCP_PACKET_PDP_CONTEXT;
    nsapi_wanted[ gas_id ]      = nsapi;

    /* First de-register the callback for this packet */
    DEBUGMSG( "sndcp_log_nsapi_pdp_context> about to deregister", 0,0,0 );
    gsndcp_diag_register(sndcp_log_active_nsapis_cb,
                         NULL,
                         sndcp_log_nsapi_state_cb);

    /* Re-registering the callback will cause it to be invoked immediately */
    DEBUGMSG( "sndcp_log_nsapi_pdp_context> about to reregister", 0,0,0 );
    gsndcp_diag_register(sndcp_log_active_nsapis_cb,
                         sndcp_log_nsapi_pdp_context_cb,
                         sndcp_log_nsapi_state_cb);

    /* Restore the original NSAPI and packet type */
    gsn_packet_wanted[ gas_id ] = SNDCP_PACKET_LOG_ALL;
    nsapi_wanted[ gas_id ]      = SNDCP_NSAPI_LOG_ALL;

    DEBUGMSG( "sndcp_log_nsapi_pdp_context> finished", 0,0,0 );
} /* sndcp_log_nsapi_pdp_context */



/*===========================================================================

FUNCTION  sndcp_log_nsapi_state

DESCRIPTION
  This function is called by the log-on-demand handler, to send a
  NSAPI State log packet to the DM.
  The function uses a documented side effect of gsndcp_diag_register()
  to cause the callback sndcp_log_nsapi_state_cb() to be invoked.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void sndcp_log_nsapi_state(uint8 nsapi)
{
    gas_id_t gas_id = GERAN_ACCESS_STRATUM_ID_1;

    /* Ensure logging is initialised */
    sndcp_log_init();

    /* Set up the required NSAPI and packet type for the callback */
    gsn_packet_wanted[ gas_id ] = SNDCP_PACKET_NSAPI_STATE;
    nsapi_wanted[ gas_id ]      = nsapi;

    /* First de-register the callback for this packet */
    DEBUGMSG( "sndcp_log_nsapi_state> about to deregister", 0,0,0 );
    gsndcp_diag_register(sndcp_log_active_nsapis_cb,
                         sndcp_log_nsapi_pdp_context_cb,
                         NULL);

    /* Re-registering the callback will cause it to be invoked immediately */
    DEBUGMSG( "sndcp_log_nsapi_state> about to reregister", 0,0,0 );
    gsndcp_diag_register(sndcp_log_active_nsapis_cb,
                         sndcp_log_nsapi_pdp_context_cb,
                         sndcp_log_nsapi_state_cb);

    /* Restore the original NSAPI and packet type */
    gsn_packet_wanted[ gas_id ] = SNDCP_PACKET_LOG_ALL;
    nsapi_wanted[ gas_id ]      = SNDCP_NSAPI_LOG_ALL;

    DEBUGMSG( "sndcp_log_nsapi_state> finished", 0,0,0 );
} /* sndcp_log_nsapi_state */



/*===========================================================================

FUNCTION  sndcp_log_pdu_stats

DESCRIPTION
  This function is called by the log-packet-request handler, to send a
  PDU Statistics log packet to the DM.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void sndcp_log_pdu_stats(uint8 nsapi, uint8 zero_stats)
{
    const gsndcp_diag_pdu_stat_rsp_t *rsp_ptr; /* Pointer to stats inside SNDCP */
    LOG_GPRS_SNDCP_PDU_STATS_C_type  *log_ptr; /* pointer to log buffer */

    /* Ensure logging is initialised */
    sndcp_log_init();

    /* First clear ou the statistics if required */
    if (zero_stats)
    {
        DEBUGMSG( "sndcp_log_pdu_stats> clear statistics", 0,0,0 );
        gsndcp_diag_reset_pdu_stats(nsapi);
    }

    /* Obtain the latest statistics for this SAPI */
    rsp_ptr = gsndcp_diag_get_pdu_stats(nsapi);
    if (rsp_ptr == NULL)
    {
        DEBUGMSG( "sndcp_log_pdu_stats> unable to get statistics", 0,0,0 );
        return;
    }

    /* Try to allocate memory for the log packet -             */
    /* this will fail if the log mask is not set appropriately */
    log_ptr = (LOG_GPRS_SNDCP_PDU_STATS_C_type  *)log_alloc(LOG_GPRS_SNDCP_PDU_STATS_C, sizeof(LOG_GPRS_SNDCP_PDU_STATS_C_type));
    if (log_ptr == NULL)
    {
        DEBUGMSG( "sndcp_log_pdu_stats> log_alloc failed", 0,0,0 );
    }
    else
    {
        /* Fill packet and commit it */
        log_ptr->nsapi_addr           = rsp_ptr->nsapi_addr;
        log_ptr->npdu_tx_cnt          = rsp_ptr->npdu_tx_cnt;
        log_ptr->npdu_octet_tx_cnt    = rsp_ptr->npdu_octet_tx_cnt;
        log_ptr->npdu_retx_cnt        = rsp_ptr->npdu_retx_cnt;
        log_ptr->npdu_octet_retx_cnt  = rsp_ptr->npdu_octet_retx_cnt;
        log_ptr->npdu_rx_cnt          = rsp_ptr->npdu_rx_cnt;
        log_ptr->npdu_octet_rx_cnt    = rsp_ptr->npdu_octet_rx_cnt;
        log_ptr->snpdu_tx_cnt         = rsp_ptr->snpdu_tx_cnt;
        log_ptr->snpdu_octet_tx_cnt   = rsp_ptr->snpdu_octet_tx_cnt;
        log_ptr->snpdu_retx_cnt       = rsp_ptr->snpdu_retx_cnt;
        log_ptr->snpdu_octet_retx_cnt = rsp_ptr->snpdu_octet_retx_cnt;
        log_ptr->snpdu_rx_cnt         = rsp_ptr->snpdu_rx_cnt;
        log_ptr->snpdu_octet_rx_cnt   = rsp_ptr->snpdu_octet_rx_cnt;
        log_ptr->gen_purpose_a_cnt    = rsp_ptr->gen_purpose_a_cnt;
        log_ptr->gen_purpose_b_cnt    = rsp_ptr->gen_purpose_b_cnt;
        log_ptr->gen_purpose_c_cnt    = rsp_ptr->gen_purpose_c_cnt;

        log_commit(log_ptr);

        DEBUGMSG( "sndcp_log_pdu_stats> packet logged", 0,0,0 );
    }
} /* sndcp_log_pdu_stats */


/*===========================================================================

FUNCTION  sndcp_log_ul_tcp_hdr

DESCRIPTION

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void sndcp_log_ul_tcp_hdr(dsm_item_type *npdu)
{
  LOG_GPRS_SNDCP_UL_TCP_HDR_C_type  *log_ptr; /* pointer to log buffer */

  sndcp_log_init();

  /* Try to allocate memory for the log packet -             */
  /* this will fail if the log mask is not set appropriately */

  log_ptr = (LOG_GPRS_SNDCP_UL_TCP_HDR_C_type  *)log_alloc(LOG_GPRS_SNDCP_UL_TCP_HDR_C, sizeof(LOG_GPRS_SNDCP_UL_TCP_HDR_C_type));

  if (log_ptr == NULL)
  {
    DEBUGMSG( "sndcp_log_ul_tcp_hdr> log_alloc failed", 0,0,0 );
  }
  else
  {
    uint8         ip_datagram[50];
    uint16        ip_total_len;
    uint8         ip_prtcol_type;
    uint8         ip_hdr_len, tcp_hdr_len;
    dsm_item_type *npdu_copy;

    /* Duplicate only the top 40 bytes of the N-PDU. These include the 20 bytes
    ** IP header plus the 20 bytes TCP header (without optional fields).
    */
    ip_total_len = GPRS_PDU_LENGTH( npdu );

    if ( ip_total_len > 40 )
      ip_total_len = 40;

    npdu_copy = gprs_pdu_duplicate( npdu, 0, ip_total_len );

    if( npdu_copy != NULL )
    {
      if( !gprs_pdu_strip_head( &npdu_copy, ip_datagram, ip_total_len ) )
      {
        MSG_GERAN_ERROR_0("GSN " "SNDCP logging: gprs_pdu_strip_head() failed");
        log_free(log_ptr);
      }
      else
      {
        if ( *ip_datagram == 0x45 ) /* IPv4 without optional fields */
        {
          ip_hdr_len          = (*ip_datagram & 0x0f ) * 4;
          ip_total_len        = (((uint16) ip_datagram[2] ) << 8 ) | ((uint16) ip_datagram[3] ) ;
          ip_prtcol_type      = (uint8) ip_datagram[9];

          if( ip_prtcol_type == 6 ) /* TCP protocol */
          {
            tcp_hdr_len       = (uint8)(((ip_datagram[ip_hdr_len + 12]  >> 4) & 0x0f) * 4);

            log_ptr->data_len = (uint16) ip_total_len - (ip_hdr_len + tcp_hdr_len);

            log_ptr->src_prt  = ((uint16)ip_datagram[ip_hdr_len     ] <<  8) | ((uint16)ip_datagram[ip_hdr_len +  1]);
            log_ptr->dest_prt = ((uint16)ip_datagram[ip_hdr_len +  2] <<  8) | ((uint16)ip_datagram[ip_hdr_len +  3]);
            log_ptr->seq      = ( ((uint32)ip_datagram[ip_hdr_len +  4] << 24) | ((uint32)ip_datagram[ip_hdr_len +  5] << 16) |
                                  ((uint32)ip_datagram[ip_hdr_len +  6] <<  8) | ((uint32)ip_datagram[ip_hdr_len +  7]      )   );
            log_ptr->ack      = ( ((uint32)ip_datagram[ip_hdr_len +  8] << 24) | ((uint32)ip_datagram[ip_hdr_len +  9] << 16) |
                                  ((uint32)ip_datagram[ip_hdr_len + 10] <<  8) | ((uint32)ip_datagram[ip_hdr_len + 11]      )   );
            log_ptr->win      = ((uint16)ip_datagram[ip_hdr_len + 14] <<  8) | ((uint16)ip_datagram[ip_hdr_len + 15]);
          }
          else if( ip_prtcol_type == 17 ) /* UDP protocol */
          {
            log_ptr->src_prt  = ((uint16)ip_datagram[ip_hdr_len    ] <<  8) | ((uint16)ip_datagram[ip_hdr_len + 1]);
            log_ptr->dest_prt = ((uint16)ip_datagram[ip_hdr_len + 2] <<  8) | ((uint16)ip_datagram[ip_hdr_len + 3]);
            log_ptr->data_len = ((uint16)ip_datagram[ip_hdr_len + 4] <<  8) | ((uint16)ip_datagram[ip_hdr_len + 5]);
            log_ptr->seq      = 0;
            log_ptr->ack      = 0;
            log_ptr->win      = 0;
          }
          else
          {
            log_ptr->data_len = 0;
            log_ptr->src_prt  = 0;
            log_ptr->dest_prt = 0;
            log_ptr->seq      = 0;
            log_ptr->ack      = 0;
            log_ptr->win      = 0;
          }
        }
        else
        {
          log_ptr->data_len = 0;
          log_ptr->src_prt  = 0;
          log_ptr->dest_prt = 0;
          log_ptr->seq      = 0;
          log_ptr->ack      = 0;
          log_ptr->win      = 0;
        }

        log_commit(log_ptr);
        DEBUGMSG( "sndcp_log_ul_tcp_hdr> packet logged", 0,0,0 );
      }
    }
    else
    {
      MSG_GERAN_ERROR_0("GSN " "SNDCP logging: run out of dup dsm items");
      log_free(log_ptr);
    }
  }

} /* sndcp_log_ul_tcp_hdr */


/*===========================================================================

FUNCTION  sndcp_log_dl_tcp_hdr

DESCRIPTION

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void sndcp_log_dl_tcp_hdr(dsm_item_type *npdu)
{
  LOG_GPRS_SNDCP_DL_TCP_HDR_C_type  *log_ptr; /* pointer to log buffer */

  sndcp_log_init();

  /* Try to allocate memory for the log packet -             */
  /* this will fail if the log mask is not set appropriately */

  log_ptr = (LOG_GPRS_SNDCP_DL_TCP_HDR_C_type  *)log_alloc(LOG_GPRS_SNDCP_DL_TCP_HDR_C, sizeof(LOG_GPRS_SNDCP_DL_TCP_HDR_C_type));

  if (log_ptr == NULL)
  {
    DEBUGMSG( "sndcp_log_dl_tcp_hdr> log_alloc failed", 0,0,0 );
  }
  else
  {
    uint8         ip_datagram[50];
    uint16        ip_total_len;
    uint8         ip_prtcol_type;
    uint8         ip_hdr_len, tcp_hdr_len;
    dsm_item_type *npdu_copy;

    /* Duplicate only the top 40 bytes of the N-PDU. These include the 20 bytes
    ** IP header plus the 20 bytes TCP header (without optional fields).
    */
    ip_total_len = GPRS_PDU_LENGTH( npdu );

    if ( ip_total_len > 40 )
      ip_total_len = 40;

    npdu_copy = gprs_pdu_duplicate( npdu, 0, ip_total_len );

    if( npdu_copy != NULL )
    {
      if( !gprs_pdu_strip_head( &npdu_copy, ip_datagram, ip_total_len ) )
      {
        MSG_GERAN_ERROR_0("GSN " "SNDCP logging: gprs_pdu_strip_head() failed");
        log_free(log_ptr);
      }
      else
      {
        if ( *ip_datagram == 0x45 ) /* IPv4 without optional fields */
        {
          ip_hdr_len          = (*ip_datagram & 0x0f ) * 4;
          ip_total_len        = (((uint16) ip_datagram[2] ) << 8 ) | ((uint16) ip_datagram[3] ) ;
          ip_prtcol_type      = (uint8) ip_datagram[9];

          if( ip_prtcol_type == 6 ) /* TCP protocol */
          {
            tcp_hdr_len       = (uint8)(((ip_datagram[ip_hdr_len + 12]  >> 4) & 0x0f) * 4);

            log_ptr->data_len = (uint16) ip_total_len - (ip_hdr_len + tcp_hdr_len);

            log_ptr->src_prt  = ((uint16)ip_datagram[ip_hdr_len     ] <<  8) | ((uint16)ip_datagram[ip_hdr_len +  1]);
            log_ptr->dest_prt = ((uint16)ip_datagram[ip_hdr_len +  2] <<  8) | ((uint16)ip_datagram[ip_hdr_len +  3]);
            log_ptr->seq      = ( ((uint32)ip_datagram[ip_hdr_len +  4] << 24) | ((uint32)ip_datagram[ip_hdr_len +  5] << 16) |
                                  ((uint32)ip_datagram[ip_hdr_len +  6] <<  8) | ((uint32)ip_datagram[ip_hdr_len +  7]      )   );
            log_ptr->ack      = ( ((uint32)ip_datagram[ip_hdr_len +  8] << 24) | ((uint32)ip_datagram[ip_hdr_len +  9] << 16) |
                                  ((uint32)ip_datagram[ip_hdr_len + 10] <<  8) | ((uint32)ip_datagram[ip_hdr_len + 11]      )   );
            log_ptr->win      = ((uint16)ip_datagram[ip_hdr_len + 14] <<  8) | ((uint16)ip_datagram[ip_hdr_len + 15]);
          }
          else if( ip_prtcol_type == 17 ) /* UDP protocol */
          {
            log_ptr->src_prt  = ((uint16)ip_datagram[ip_hdr_len    ] <<  8) | ((uint16)ip_datagram[ip_hdr_len + 1]);
            log_ptr->dest_prt = ((uint16)ip_datagram[ip_hdr_len + 2] <<  8) | ((uint16)ip_datagram[ip_hdr_len + 3]);
            log_ptr->data_len = ((uint16)ip_datagram[ip_hdr_len + 4] <<  8) | ((uint16)ip_datagram[ip_hdr_len + 5]);
            log_ptr->seq      = 0;
            log_ptr->ack      = 0;
            log_ptr->win      = 0;
          }
          else
          {
            log_ptr->data_len = 0;
            log_ptr->src_prt  = 0;
            log_ptr->dest_prt = 0;
            log_ptr->seq      = 0;
            log_ptr->ack      = 0;
            log_ptr->win      = 0;
          }
        }
        else
        {
          log_ptr->data_len = 0;
          log_ptr->src_prt  = 0;
          log_ptr->dest_prt = 0;
          log_ptr->seq      = 0;
          log_ptr->ack      = 0;
          log_ptr->win      = 0;
        }

        log_commit(log_ptr);
        DEBUGMSG( "sndcp_log_dl_tcp_hdr> packet logged", 0,0,0 );
      }
    }
    else
    {
      MSG_GERAN_ERROR_0("GSN " "SNDCP logging: run out of dup dsm items");
      log_free(log_ptr);
    }
  }

} /* sndcp_log_dl_tcp_hdr */


