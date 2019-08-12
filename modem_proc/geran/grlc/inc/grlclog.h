#ifndef RLC_LOG_H
#define RLC_LOG_H

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                            RLC Logging Facilities

GENERAL DESCRIPTION
   This module contains functions for logging RLC packets.

Copyright (c) 2002-2013 Qualcomm Technologies, Inc.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/grlc/inc/grlclog.h#1 $

when       who      what, where, why
--------   ---      ----------------------------------------------------------
09/09/02   ADG     Initial Revision.
===========================================================================*/

#include "geran_variation.h"
#include "comdef.h" /* for types */
#include "grlcmac.h"    /* for rlc_log_epuan(..) */
#include "gmacrlc.h"    /* for rlc_log_epdan(..) */

typedef enum
{
  RLC_LOG_UL_MAC_RELEASE_IND,
  RLC_LOG_UL_EXPIRED_T3182_IN_ACK_RELEASE,
  RLC_LOG_UL_EXPIRED_T3182_IN_ACK_TRANSFER,
  RLC_LOG_UL_EXPIRED_N3102_IN_ACK_TRANSFER,
  RLC_LOG_UL_EXPIRED_T3182_IN_UNACK_RELEASE,
  RLC_LOG_UL_EXPIRED_T3182_IN_UNACK_TRANSFER,
  RLC_LOG_UL_EXPIRED_N3102_IN_UNACK_TRANSFER
}rlc_log_ul_release_t;

typedef enum
{
  RLC_LOG_DL_MAC_RELEASE_IND,
  RLC_LOG_DL_EXPIRED_T3192_IN_ACK_TRANSFER,
  RLC_LOG_DL_EXPIRED_T3190_IN_ACK_TRANSFER,
  RLC_LOG_DL_EXPIRED_T3192_IN_UNACK_TRANSFER,
  RLC_LOG_DL_EXPIRED_T3190_IN_UNACK_TRANSFER
}rlc_log_dl_release_t;

typedef enum 
{ 
  GPRS_HEADER_TYPE, 
  EGPRS_HEADER_TYPE_1,  
  EGPRS_HEADER_TYPE_2, 
  EGPRS_HEADER_TYPE_3
}EGPRS_hdr_type;

typedef struct
{
  uint32  dl_rx;
  uint32  ul_tx;
  uint32  ul_retx;
}rlc_log_rfblks_cnts_t;

typedef struct
{
  uint32  dl_rx;
  uint32  dl_reprx;
}rlc_eng_mode_dl_cnts_t;


/* Log 3 Bytes of E/GPPRS Payload
*/
#define LOG_MAX_LI_E  3 


/* INITIALISATION */
extern void rlc_log_init( gas_id_t gas_id );

/* LOG PACKETS */
extern boolean rlc_log_ul_statistics(gas_id_t gas_id, boolean zero_stats);
extern boolean rlc_log_dl_statistics(gas_id_t gas_id, boolean zero_stats);
extern void rlc_log_ul_acknack_params( gas_id_t gas_id );

extern void rlc_log_dl_acknack_params
(
 gas_id_t gas_id,
 uint8  dl_tfi,
 uint8  fai, 
 uint8  ssn,
 uint8  vq,
 uint32 hu32,
 uint32 lu32,
 uint8  cs
);

extern void rlc_log_ul_release_ind( gas_id_t gas_id, uint8 ul_tfi, rlc_log_ul_release_t release_cause);
extern void rlc_log_dl_release_ind( gas_id_t gas_id, uint8 dl_tfi, rlc_log_dl_release_t release_cause);
extern rlc_log_rfblks_cnts_t* rlc_log_rfblks_cnts( gas_id_t gas_id );
extern rlc_eng_mode_dl_cnts_t* rlc_log_dl_blks_cnts( gas_id_t gas_id );


extern void rlc_log_dl_header
(
 gas_id_t    gas_id, 
 byte        type,
 const byte* dl_header,
 byte        crc_status,
 byte        time_slot,
 uint32      frame_number,
 const byte* dl_payload
);

extern void rlc_log_ul_header
(
 gas_id_t    gas_id, 
 byte        type,   
 const byte* ul_header
);

extern void rlc_log_epuan(gas_id_t gas_id, const mr_ul_pkt_acknack_ind_t* puan_msg);
extern void rlc_log_epdan(gas_id_t gas_id, const rm_dl_pkt_acknack_req_t* pdan_msg);

/*===========================================================================

FUNCTION  grlc_log_timer_event

DESCRIPTION
  Called when a GRLC timer expires - used for both Uplink and Downlink tasks

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void grlc_log_timer_event(gas_id_t gas_id, const rm_timer_t timer_t);

/* EVENTS */
/* extern void rlc_log_some_event(args); */
extern void rlc_log_ul_config(mr_ul_alloc_cnf_t *config_ptr, const gas_id_t gas_id);
extern void rlc_log_dl_config(mr_dl_ass_ind_t *config_ptr, const gas_id_t gas_id);
extern void rlc_log_ul_statistics_ext(const gas_id_t gas_id);
extern void rlc_log_dl_statistics_ext(const gas_id_t gas_id);

#endif /* RLC_LOG_H */
