#ifndef RR_MAC_SEND_H
#define RR_MAC_SEND_H
/*==============================================================================

            R R  M A C  S E N D   ---   H E A D E R   F I L E

GENERAL DESCRIPTION
  This module provides functions for sending primitives to MAC.  Each primitive
  has a corresponding function.

EXTERNALIZED FUNCTIONS
  

INITIALIZATION AND SEQUENCING REQUIREMENTS
  rr_mac_send_reset_last_cell_info should be called at task initialisation.

Copyright (c) 2001-2013 Qualcomm Technologies, Inc.
==============================================================================*/

/*==============================================================================

                       EDIT HISTORY FOR MODULE

 * $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/grr/src/rr_mac_send.h#1 $

 * $History: rr_mac_send.h $
 * 
 * *****************  Version 2  *****************
 * User: Damon        Date: 28/08/02   Time: 17:42
 * Updated in $/GPRS_Integration_1_0/gprs/grr
 * Remove compiler warning
 * 
 * *****************  Version 1  *****************
 * User: Damon        Date: 6/08/02    Time: 15:21
 * Created in $/GPRS_Integration_1_0/gprs/grr
 * GPRS RR
 * 
 * *****************  Version 3  *****************
 * User: Mel          Date: 5/08/02    Time: 16:57
 * Updated in $/GPRS_RR_Host/gsm/grr
 * Adding 51 Packet Access functionality
 * 
 * *****************  Version 2  *****************
 * User: Nick         Date: 26/07/02   Time: 11:43
 * Updated in $/GPRS_RR_Host/gsm/grr
 * Added the packet_measurement_report_t to the send packet measurement
 * report function.
 * 
 * *****************  Version 1  *****************
 * User: Damon        Date: 8/07/02    Time: 11:18
 * Created in $/GPRS_RR_Host/gsm/grr
 * Compatible with GSM_RR.02.01.04
 * 
 * *****************  Version 1  *****************
 * User: Damon        Date: 3/07/02    Time: 18:51
 * Created in $/GSM_GRR_VU
 * Initial port from 'PerlUTF archive'
 * 
 * *****************  Version 2  *****************
 * User: Damon        Date: 16/01/02   Time: 11:55a
 * Updated in $/PerlUTF/gsm/rr
 * Revised version with many of the parameters added
 * 
 * *****************  Version 1  *****************
 * User: Damon        Date: 6/12/01    Time: 8:11p
 * Created in $/PerlUTF/gsm/rr
 * Initial version. Functions to send all GRR->MAC signals are
 * defined, but most of the parameters are missing at the moment

=============================================================================*/

#include "geran_variation.h"
#include "customer.h"
#include "comdef.h"
#include "gprsdef.h"
#include "gprsdef_g.h"
#include "gmacgrrsig.h"
#include "csn.h"
#include "rr_l2.h"
#include "geran_multi_sim.h"

/*==============================================================================

                PUBLIC DEFINITIONS AND DECLARATIONS FOR MODULE

==============================================================================*/

/*==============================================================================
  Typedefs
==============================================================================*/

/*==============================================================================
  Constants and Macros
==============================================================================*/

/*==============================================================================
  Variables
==============================================================================*/

/*==============================================================================
                    PUBLIC FUNCTION DECLARATIONS FOR MODULE
==============================================================================*/

extern void rr_mac_send_reset_last_cell_info(
  const gas_id_t gas_id
);

extern void rr_send_grr_mac_ps_access_req(
  boolean purge_gmm_signalling,
  const gas_id_t gas_id
);

extern void rr_send_grr_mac_no_ps_access_ind(
  delete_pdu_cause_t,
  const gas_id_t gas_id
);

extern void rr_send_grr_mac_send_meas_rpt(
  pmr_or_pemr_e,
  pmr_or_pemr_u *,
  uint8,
  uint8,
  const gas_id_t gas_id
);

extern void rr_send_grr_mac_send_pccf(
  packet_cell_change_failure_t,
  const gas_id_t gas_id
);

extern void rr_send_grr_mac_ul_sb_est(
  packet_channel_description_t *,
  ia_rest_octets_t *,
  frequency_list_T *,
  byte,
  const gas_id_t gas_id
);

extern void rr_send_grr_mac_ul_tbf_est(
  packet_channel_description_t *,
  ia_rest_octets_t *,
  frequency_list_T *,
  byte,
  byte,
  const gas_id_t gas_id
);

extern void rr_send_grr_mac_dl_sb_est(
  packet_channel_description_t *,
  ia_rest_octets_t *,
  frequency_list_T *,
  byte,
  const gas_id_t gas_id
);

extern void rr_send_grr_mac_dl_tbf_est(
  packet_channel_description_t *,
  ia_rest_octets_t *,
  frequency_list_T *,
  byte,
  const gas_id_t gas_id
);

extern void rr_send_grr_mac_rach_req(
  const gas_id_t gas_id
);


extern void rr_send_grr_mac_send_pccn_req(
  packet_cell_change_notification_t *,
  const gas_id_t gas_id
);

extern void rr_send_grr_mac_nacc_release_req(
  uint8,
  uint8,
  const gas_id_t gas_id
);


void rr_send_grr_mac_send_pss_req(
  packet_si_status_t *,
  const gas_id_t gas_id
);


extern boolean rr_send_grr_mac_reset_req(
  const gas_id_t gas_id
);


#ifdef FEATURE_GSM_DTM

extern void rr_send_grr_mac_dtm_channel_assign_req(
  uint8 cs_timeslot,
  cs_chan_desc_t * cs_chan_desc,
  frequency_information_T * cs_freq_info,
  gprs_l1_cell_opt_t * l1_cell_options,
  rr_packet_uplink_assignment_t * pkt_uplink_assign,
  rr_packet_downlink_assignment_t * pkt_downlink_assign,
  const gas_id_t gas_id
);

extern void rr_send_grr_mac_dtm_channel_release_ind(
  dtm_ps_rel_cause_t ps_rel_cause,
  const gas_id_t gas_id
);

extern void rr_send_grr_mac_dtm_ul_dcch_ind(
  const gas_id_t gas_id
);

extern void rr_send_grr_mac_dtm_ul_dcch_cnf(
  error_code_T,
  const gas_id_t gas_id
);

#ifdef FEATURE_GSM_EDTM
extern void rr_send_grr_mac_edtm_pkt_cs_est_req(
  uint8,
  const gas_id_t gas_id
);

extern void rr_send_grr_mac_edtm_channel_release_ind(
  const gas_id_t gas_id
);
#endif /* FEATURE_GSM_EDTM */

#endif /* FEATURE_GSM_DTM */

#ifdef FEATURE_GPRS_PS_HANDOVER
void rr_send_grr_mac_psho_req(
  psho_type_t,
  const gas_id_t gas_id
);

void rr_send_grr_mac_psho_complete(
  psho_status_t,
  const gas_id_t gas_id
);
#endif /* FEATURE_GPRS_PS_HANDOVER */

/**
  @brief Returns TRUE if PS access is currently enabled in MAC
*/ 
extern boolean rr_mac_ps_access_granted(
  const gas_id_t gas_id
);

/**
  @brief Returns TRUE if PS access is currently blocked in MAC
*/ 
extern boolean rr_mac_ps_access_blocked(
  const gas_id_t gas_id
);

/**
  @brief Returns TRUE if PS access is currently blocked in MAC and all PDUs deleted
*/ 
extern boolean rr_mac_ps_access_blocked_del_all(
  const gas_id_t gas_id
);

/**
  @brief Initialisation function
*/ 
extern void rr_mac_send_init(
  const gas_id_t gas_id
);

/**
  @brief Initialisation function. Should be called once at RR task start
*/ 
extern void rr_mac_send_task_start_init(
  void
);

/**
  @brief Returns TRUE if DL TBForSB EST message is sent to MAC
*/ 
extern boolean rr_mac_send_get_dl_msg_sent(
  const gas_id_t gas_id
);

/**
  @brief Set/Reset the dl_msg_sent flag
*/ 
extern void rr_mac_send_set_dl_msg_sent(
  boolean dl_msg_sent,
  const gas_id_t gas_id
);  

#endif /* #ifndef RR_MAC_SEND_H */

/* EOF */

