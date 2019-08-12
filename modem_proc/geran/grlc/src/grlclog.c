/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                            RLC Logging Facilities

GENERAL DESCRIPTION
   This module contains functions for logging RLC packets.

EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2002-2013 Qualcomm Technologies, Inc.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/grlc/src/grlclog.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/09/02   ADG     Initial Revision.
04/07/13    hv     CR461047 - Make safe usage of memcpy() and memmove() in GRLC
===========================================================================*/
#include <stringl.h>

#include "geran_variation.h"
#include "customer.h"        /* Include before using any FEATUREs */


/* Debug F3 messages will be emitted if the following line is uncommented */
/* #define RLC_LOG_DEBUG */

#if defined(RLC_LOG_DEBUG)
#error code not present
#else
#define DEBUGMSG(a,b,c,d)       /* */
#endif /* defined(RLC_LOG_DEBUG) */


/*===========================================================================
                     INCLUDE FILES FOR MODULE
===========================================================================*/
#include <string.h>          /* For memcpy()                               */

#include "comdef.h"          /* For log_commit(), log_alloc(), log codes   */
#include "log.h"             /* For log_commit(), log_alloc(), log codes   */
#include "log_codes_gsm.h"
#include "log_codes_gsm_v.h"   /* For GSM/GPRS log codes                     */
#include "event.h"           /* For logging event codes */
#ifdef DEBUG_USE_OLD_DIAG
#error code not present
#else
#include "gsmdiag.h"
#include "gsmdiag_v.h"
#endif
#ifdef FEATURE_GRLC_STORE_F3_MSG
#include "gmutex.h"
#endif /* FEATURE_GRLC_STORE_F3_MSG */
#include "grlcultypes.h"
#include "grlcdiag.h"        /* Interface to the rest of RLC               */
#include "grlclog.h"
#include "grlcultst.h"
#include "grlcdli.h"
#include "grlcdltst.h"
#include "grlculproc.h"
#include "grlcdli.h"
#include "grlcdl.h"
#include "geran_multi_sim.h"

#ifdef FEATURE_GRLC_STORE_F3_MSG
/*===========================================================================
              TYPE DEFINITIONS FOR F3 MESSAGE STORAGE
===========================================================================*/

/* Mutex to protect multi thread access */
rex_crit_sect_type grlc_f3_msg_crit_sec;

/* Data structure to store F3 message */
typedef struct {
  uint16   f3_msg_id;
  uint8    gas_id;
  uint32   arg1;
  uint32   arg2;
  uint32   arg3;
} grlc_f3_msg_entry_t;

#define GRLC_F3_MSG_BUF_SIZE 500

typedef struct {
  uint16              index;
  grlc_f3_msg_entry_t buf[GRLC_F3_MSG_BUF_SIZE];
} grlc_f3_msg_buf_t;

static grlc_f3_msg_buf_t grlc_f3_msg_buf;
#endif /* FEATURE_GRLC_STORE_F3_MSG */

/*===========================================================================
              TYPE DEFINITIONS FOR EVENTS & LOG PACKETS
===========================================================================*/

/*lint -esym(754, LOG_GPRS_RLC_UL_ABNRML_RLS_COUNTS_C_tag::hdr)*/
LOG_RECORD_DEFINE(LOG_GPRS_RLC_UL_ABNRML_RLS_COUNTS_C)
  uint16 access_reject_cnt;
  uint16 arc_retry_cnt;
  uint16 arc_wo_retry_cnt;
  uint16 arc_sys_info_cnt;
LOG_RECORD_END

/*lint -esym(754, LOG_GPRS_RLC_UL_EVENT_COUNTS_C_tag::hdr)*/
LOG_RECORD_DEFINE(LOG_GPRS_RLC_UL_EVENT_COUNTS_C)
  uint16 llc_event_cnt;
  uint16 mac_event_cnt;
  uint16 pl1_event_cnt;
  uint16 tmr_event_cnt;
LOG_RECORD_END

/*lint -esym(754, LOG_GPRS_RLC_UL_STATS_C_tag::hdr)*/
LOG_RECORD_DEFINE(LOG_GPRS_RLC_UL_STATS_C)
  uint32 grlc_ul_state;
  uint32 grlc_ul_substate;
  uint32 ack_tx_pdu_cnt;
  uint32 ack_tx_oct_cnt;
  uint32 unack_tx_pdu_cnt;
  uint32 unack_tx_oct_cnt;
  uint16 coding_sch_changes;
  uint8  ul_seq_va;
  uint8  ul_seq_vs;
  uint8  ul_seq_nseg;
  uint8  ul_seq_vsf;
  uint8  ul_seq_pack_vsf;
  uint8  ul_seq_nack_vsf;
  int8   ul_n3102;
  int8   ul_n3104;
  uint32 ul_gen_purpose_count_a;
  uint32 ul_gen_purpose_count_b;
  uint32 ul_gen_purpose_count_c;
LOG_RECORD_END

/*lint -esym(754, LOG_GPRS_RLC_DL_STATS_C_tag::hdr)*/
LOG_RECORD_DEFINE(LOG_GPRS_RLC_DL_STATS_C)
  uint32 grlc_dl_state;
  uint32 ack_rx_pdu_cnt;
  uint32 ack_rx_oct_cnt;
  uint32 unack_rx_pdu_cnt;
  uint32 unack_pdu_filled_cnt;
  uint32 unack_rx_oct_cnt;
  uint32 unack_oct_filled_cnt;
  uint32 ack_nack_req_cnt;
  uint8  dl_seq_vq;
  uint8  dl_seq_vr;
  uint8  dl_seq_ssn;
  uint32 dl_gen_purpose_count_a;
  uint32 dl_gen_purpose_count_b;
  uint32 dl_gen_purpose_count_c;
LOG_RECORD_END

/*lint -esym(754, LOG_GPRS_RLC_UL_ACKNACK_PARAMS_VER2_C_tag::hdr)*/
LOG_RECORD_DEFINE(LOG_GPRS_RLC_UL_ACKNACK_PARAMS_VER2_C)
  uint8  ul_tfi;
  uint8  fai;   /* Final Ack Indicator */
  uint8  ssn;   /* Starting Sequence Number */
  uint8  cs;    /* Coding Scheme, 0=CS1, 1=CS2.. */
  uint8  cv;    /* Countdown Value */
  uint8  va;    /* ACK State Var */
  uint8  vs;    /* Send State Var */
  uint8  si;    /* Stall Indicator */
  uint32 hu32; /* Higher 32 bits of RBB field */
  uint32 lu32; /* Lower 32 bits of RBB field */
LOG_RECORD_END

/*lint -esym(754, LOG_GPRS_RLC_DL_ACKNACK_PARAMS_VER2_C_tag::hdr)*/
LOG_RECORD_DEFINE(LOG_GPRS_RLC_DL_ACKNACK_PARAMS_VER2_C)
  uint8  dl_tfi;
  uint8  fai;   /* Final Ack Indicator */
  uint8  ssn;   /* Starting Sequence Number  = V(r) Receive State Var*/
  uint8  vq;    /* Receive Window Var */
  uint32 hu32;  /* Higher 32 bits of RBB field */
  uint32 lu32;  /* Lower 32 bits of RBB field */
  uint8  cs;    /* Coding Scheme, 0=CS1, 1=CS2.. */
LOG_RECORD_END

/*lint -esym(754, LOG_GPRS_RLC_UL_RELEASE_IND_C_tag::hdr)*/
LOG_RECORD_DEFINE(LOG_GPRS_RLC_UL_RELEASE_IND_C)
  uint8 ul_tfi;
  uint8 release_cause;
LOG_RECORD_END

/*lint -esym(754, LOG_GPRS_RLC_DL_RELEASE_IND_C_tag::hdr)*/
LOG_RECORD_DEFINE(LOG_GPRS_RLC_DL_RELEASE_IND_C)
  uint8 dl_tfi;
  uint8 release_cause;
LOG_RECORD_END

/*lint -esym(754, LOG_EGPRS_RLC_DL_HEADER_C_tag::hdr)*/
LOG_RECORD_DEFINE(LOG_EGPRS_RLC_DL_HEADER_C)
    byte  type;
    byte  dl_hdr[5];
    byte      crc_status; /* Bit0:CRC_1, Bit1:CRC_2, Bit2:E_1,
                          ** Bit3:FBI_1, Bit4:E_2, Bit5:FBI_2
                          */
    byte   time_slot;
    uint32 frame_number;
    byte      dl_payload[4*LOG_MAX_LI_E];
LOG_RECORD_END

/*lint -esym(754, LOG_EGPRS_RLC_UL_HEADER_C_tag::hdr)*/
LOG_RECORD_DEFINE(LOG_EGPRS_RLC_UL_HEADER_C)
    byte  type;
    byte  ul_hdr[6];
LOG_RECORD_END


/*lint -esym(754, LOG_EGPRS_RLC_EPUAN_C_tag::hdr)*/
LOG_RECORD_DEFINE(LOG_EGPRS_RLC_EPUAN_C)
      byte      tfi;           /* Temporary Flow Indicator  [0..31]                */
      byte      mcs;           /* Commanded Mod. & Coding Scheme  [0..8]           */
      byte      mask;          /* | d | d | scc | pre | res | eow | bow | fai |    */
                               /*   8   7    6     5     4     3     2     1       */
                                      /* scc - Starting Colour Code [0/1]
                                      ** pre - preemptive tx. [0/1]
                                      ** res - Resegment [1-IR OFF , 0- IR ON]
                                      ** bow - Begin Window of EPUAN
                                      ** eow - End Window of EPUAN
                                      ** fai  - Final Ack Indicator
                                      */
      word      ssn;           /* Starting Sequence Number [0..2047]                */
      byte      crbb_len;      /* Compressed Bitmap Length [Max size 128]           */
      uint32    crbb[4];       /* crbb array holding crbb_len bits                  */
      byte      urbb_len;      /* Uncompressed Bitmap Length [Max Size : 160 bits   */
      uint32    urbb[5];       /* urbb array holding urbb_len bits                  */
LOG_RECORD_END

/*lint -esym(754, LOG_EGPRS_RLC_EPDAN_C_tag::hdr)*/
LOG_RECORD_DEFINE(LOG_EGPRS_RLC_EPDAN_C)
      byte      tfi;        /* Temporary Flow Indicator  [0..31]                     */
      byte      mask;       /* | 8PSK | GMSK | scc | esp2 |esp1  | eow | bow | fai | */
                            /*    8      7      6     5     4       3     2     1    */
                            /* 8PSK: 1: psk_bep is vaild, 0: psk_bpe not valid.
                            ** GMSK: 1: gmsk_bep is valid, 0: gmsk_bep is not valid.
                            ** scc - Starting Colour Code [0/1]
                            ** esp2:esp1 - ES/P
                            **   0      0    -  N/A
                            **   0      1    - First Partial Bitmap
                            **   1      0    - Next Partial Bitmap
                            **   1      1    - Next Partial Bitmap w/ Quality Report
                            ** bow - Begin Window of EPUAN
                            ** eow - End Window of EPUAN
                            ** fai  - Final Ack Indicator
                            */
      word      ssn;        /* Starting Sequence Number [0..2047]                    */
      byte      crbb_len;   /* Compressed Bitmap Length [Max size 128]               */
      uint32    crbb[4];    /* crbb array holding crbb_len bits                      */
      byte      urbb_len;   /* Uncompressed Bitmap Length [Max Size : 160 bits       */
      uint32    urbb[5];    /* urbb array holding urbb_len bits                      */
      byte      gmsk_bep;   /* Serving cell GMSK BEP   mean_BEP = gmsk_bep & 0x1F;
                            ** cv_BEP = gmsk_bep>>5;
                            */
      byte      psk_bep;    /* Serving cell 8PSK BEP    mean_BEP = psk_bep & 0x1F;
                            ** cv_BEP = psk_bep>>5;
                            */
      byte      c_value;    /* useful parameter and make the log packet even number
                            ** bytes.
                            */
LOG_RECORD_END


#define GRLC_LOG_UL_PDU_BUF_MAX     GRLC_DIAG_UL_PDU_BUF_MAX
#define GRLC_LOG_UL_BLOCK_BUF_MAX   GRLC_DIAG_UL_BLOCK_BUF_MAX
#define GRLC_LOG_UL_TX_BUF_MAX      GRLC_DIAG_UL_TX_BUF_MAX
#define GRLC_LOG_DL_PDU_BUF_MAX     GRLC_DIAG_DL_PDU_BUF_MAX
#define GRLC_LOG_DL_RX_BUF_MAX      GRLC_DIAG_DL_RX_BUF_MAX

typedef PACKED struct PACKED_POST
{
  uint16    psn;             /* PDU Sequence Number */
  uint16    len;             /* PDU length */
  uint8     pdu_prio;        /* 0 - LOW, 1 - HIGH */
  uint8     rlc_mode;        /* 0 - ACK, 1 - UNACK */
  uint8     peak_throughput; /* Peak throughput */
  uint8     radio_priority;  /* Radio Priority */
  uint8     cause;           /* 0 - LAY3_DATA, 1 - GMM_SIGNALLING, 2 - LLC_SIGNALLING, 3 - GMM_PAGE_RESPONSE,
                              * 4 - GMM_CELL_UPDATE, 5 - GMM_CELL_NOTIFY, 6 - SM_SIGNALLING */
  uint8     pfc;             /* Packet Flow Control: 0 - False, 1 - True */
  uint8     pfi;             /* Packet Flow Indicator */
} grlc_log_ul_pdu_T;

typedef PACKED struct PACKED_POST
{
  uint16    bsn;       /* Block Sequence Number */
  uint16    first_psn; /* First PSN contained in the BSN */
  uint16    last_psn;  /* Last PSN contained in the BSN */
} grlc_log_ul_block_T;

typedef PACKED struct PACKED_POST
{
  uint8     ack_state; /* 0 - INACTIVE, 1 - UNACK, 2 - NACK, 3 - PTX,
                        * 4 - PENDING_PACK, 5 - PACK, 6 - ACK, 7 - INVALID */
  uint16    bsn;       /* Block Sequence Number */
  uint8     len;       /* Length of BSN */
  uint8     tbc;       /* Total Block Count */
  uint8     abs_bsn;   /* Absolute BSN */
  uint8     cv;        /* Countdown Value */
  uint8     cs;        /* Coding Scheme */
  uint8     cps;       /* Puncturing Scheme */
  uint8     si;        /* Stall Indicator. 0 -   Not_Stalled, 1 - Stalled */
} grlc_log_ul_tx_T;

LOG_RECORD_DEFINE(LOG_GPRS_RLC_UL_STATISTICS_C)
  uint8                  version;               /* Log packet version. Range: 0..255. Current version = 1 */

  uint8                  state;                 /* UL Main State: ul[gas_id].state */
  uint8                  susp_state;            /* Suspend State: ul[gas_id].susp_state */
  uint8                  realloc_state;         /* Realloc State: ul[gas_id].realloc_state */

  uint16                 curr_pdu_psn;          /* curr_pdu_ptr[gas_id]->psn */
  uint16                 next_pdu_psn;          /* next_pdu_ptr[gas_id]->psn */
  uint16                 first_psn_to_free;     /* ul[gas_id].first_psn_to_free */
  uint16                 psn_to_free_inclusive; /* ul[gas_id].psn_to_free_inclusive */

  uint16                 dsm_small_free_cnt;    /* DSM Small Item Free Count */
  uint16                 dsm_dup_free_cnt;      /* DSM DUP Item Free Count */
  uint16                 dsm_large_free_cnt;    /* DSM Large Item Free Count */

  uint16                 seg_ind;               /* Seg Ind: ul[gas_id].seg_ind */
  uint16                 va;                    /* VA: ul[gas_id].va */
  uint16                 vs;                    /* VS: ul[gas_id].vs */
  uint16                 vsf;                   /* VSF: ul[gas_id].vsf */
  uint16                 pack_vsf;              /* PAck VSF: ul[gas_id].pack_vsf */
  uint16                 nack_vsf;              /* NAck VSF: ul[gas_id].nack_vsf */
  uint16                 ptx_cnt;               /* PTX blocks transmitted count: ul[gas_id].ptx_cnt */
  uint16                 pack_cnt;              /* PAck blocks transmitted count: ul[gas_id].pack_cnt */
  uint16                 nack_cnt;              /* NAck blocks transmitted count: ul[gas_id].nack_cnt */
  uint16                 paq_ctr;               /* PAQ Counter: ul[gas_id].paq_ctr */
  uint8                  n3102;                 /* Current N3102 Value: ul[gas_id].n3102 */
  uint8                  n3104;                 /* Current N3104 Value: ul[gas_id].n3104 */
  uint8                  tbc;                   /* Total Block Count: ul[gas_id].tbc */
  uint8                  cv;                    /* CV: ul[gas_id].cv */
  uint8                  ptx_cv;                /* PTK CV: ul[gas_id].ptx_cv */
  uint8                  pak_cv;                /* PAK CV: ul[gas_id].pak_cv */

  uint8                  fifo_nack_ctr;         /* NAck FIFO counter: grlc_ul_fifo[gas_id].nack_ctr */
  uint8                  fifo_ptx_ctr;          /* PTX FIFO counter: grlc_ul_fifo[gas_id].ptx_ctr */
  uint8                  fifo_pack_ctr;         /* PAck FIFO counter: grlc_ul_fifo[gas_id].pack_ctr */

  uint16                 num_llc_pdus;          /* Number of LLC PDUs present in UL queue. ul[gas_id].llc_pdus */
  uint32                 num_llc_octets;        /* Octet count of LLC PDUs in UL queue. ul[gas_id].llc_octets */

  uint8                  pdu_cnt;                                 /* Uplink PDU Buffer Count. Range: 1..50 */
  grlc_log_ul_pdu_T      pdu_buf[GRLC_LOG_UL_PDU_BUF_MAX];        /* Uplink PDU Buffer */

  uint8                  block_cnt;                               /* Uplink Data Block Count. Range: 1..10 */
  grlc_log_ul_block_T    ul_block_buf[GRLC_LOG_UL_BLOCK_BUF_MAX]; /* Uplink Data Block Buffer */

  uint8                  tx_cnt;                                  /* Uplink Tx Block Count. Range: 1..10 */
  grlc_log_ul_tx_T       tx_buf[GRLC_LOG_UL_TX_BUF_MAX];          /* Uplink Tx Block Buffer */
LOG_RECORD_END

typedef PACKED struct PACKED_POST
{
  uint8     time_slot; /* Timeslot on which block is received */
  uint8     crc;       /* 0-FAIL, 1-PASS */
  uint8     cs;        /* Coding Scheme */
  uint16    bsn;       /* Block Seq Number */
  uint8     len;       /* Length of the Block */
  uint8     vn;        /* Receive State: 0 - INVALID, 1 - RECEIVED, 2 - RCVD_ASSEMBLED */
  uint8     sp;        /* S/P */
  uint8     fbi;       /* Final Block Indicator */
} grlc_log_dl_rx_T;

typedef PACKED struct PACKED_POST
{
  uint32    pdu_num;   /* Downlink PDU number */
  uint16    len;       /* Length of the PDU */
  uint16    first_bsn; /* First BSN contained in this PDU */
  uint16    last_bsn;  /* Last BSN contained in this PDU */
} grlc_log_dl_pdu_T;

LOG_RECORD_DEFINE(LOG_GPRS_RLC_DL_STATISTICS_C)
  uint8                version;  /* Log packet version. Range: 0..255. Current version = 1 */

  uint8                state;    /* DL State: dl[gas_id].state */

  uint16               vr;       /* VR: dl[gas_id].vr */
  uint16               vq;       /* VQ: dl[gas_id].vq */
  uint16               ssn;      /* SSN: dl[gas_id].ssn */
  uint16               pbsn;     /* PBSN: dl[gas_id].pbsn */

  uint8                fifo_ctr; /* grlc_dl_fifo[gas_id].ctr */

  uint32               FN;       /* Frame Number of Rx blocks */
  uint8                rx_cnt;                           /* Downlink Rx Block Count. Range: 1..10 */
  grlc_log_dl_rx_T     rx_buf[GRLC_LOG_DL_RX_BUF_MAX];   /* Downlink Rx Block Buffer */

  uint8                pdu_cnt;                          /* Downlink PDU Buffer Count. Range: 1..20 */
  grlc_log_dl_pdu_T    pdu_buf[GRLC_LOG_DL_PDU_BUF_MAX]; /* Downlink PDU Buffer */
LOG_RECORD_END

LOG_RECORD_DEFINE(LOG_GPRS_RLC_UL_CONFIG_C)
  uint8     version;         /* Log packet version. Range: 0..255. Current version = 1 */

  uint8     tbf_mode;        /* 0 - Invalid, 1 - GPRS, 2 - EGPRS */
  uint8     dtm_access;      /* 0 - False, 1 - True */

  uint8     cs;              /* Coding scheme */
  uint8     resegment;       /* EGPRS Resegment */
  uint16    ws;              /* Window size. Range: 64..1024 */

  uint8     access_mode;     /* 0 - Dynamic, 1 - Extended Dynamic */
  uint8     access_phase;    /* 100 - One Phase, 101 - Two Phase, 102 - Short Access 
                              * 103 - Single Block, 104 - No Phase, 105 - EGPRS One Phase
                              * 106 - EGPRS Two Phase, 107 - EGPRS Signalling */

  uint8     ti;              /* TLLI Indicator */
  uint32    tlli;            /* TLLI */
  uint8     tlli_block_cs;   /* 0 - CS1, 1 - Assigned CS */

  uint8     retry;           /* Channel request retry count */
  uint8     ul_tfi;          /* UL TFI */
  uint8     num_timeslots;   /*Number of UL Timeslots */
  uint8     usf_granularity; /* 0 - One Block, 1 - Four Consecutive Blocks */
LOG_RECORD_END

LOG_RECORD_DEFINE(LOG_GPRS_RLC_DL_CONFIG_C)
  uint8     version;    /* Log packet version. Range: 0..255. Current version = 1 */

  uint8     tbf_mode;   /* 0 - Invalid, 1 - GPRS, 2 - EGPRS */
  uint8     rlc_mode;   /* 0 - ACK, 1 - UNACK */
  uint16    ws;         /* Window size. Range: 64..1024 */
  uint32    tlli;       /* TLLI */
  uint8     num_dl_ts;  /* Number of DL Timeslots */
  uint8     ctrl_ack;   /* Control Ack */
LOG_RECORD_END

/*===========================================================================
                         DATA DECLARATIONS
===========================================================================*/
static boolean rlc_log_initialized[NUM_GERAN_DATA_SPACES] = {FALSE};

/*===========================================================================
                         FUNCTION DECLARATIONS
===========================================================================*/
/* Log-on-demand callback to be registered with DIAG */
static log_on_demand_status_enum_type rlc_log_on_demand(log_code_type log_code);

/* Log-packet-request callback to be registered with DIAG */
static void rlc_log_packet_request(/* gas_id_t gas_id, */
                                   uint16 log_code,
                                   uint8  zero_stats,
                                   uint8  additional_info);

static boolean rlc_log_ul_abnrml_rls_counts(gas_id_t gas_id, boolean zero_stats);
static boolean rlc_log_ul_event_counts(gas_id_t gas_id, boolean zero_stats);

/*===========================================================================

FUNCTION  rlc_log_packet_init

DESCRIPTION
  Initialises a gsmdiag_pkt_info structure ready for use with logging functions

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
static void rlc_log_packet_init
(
  gas_id_t            gas_id,
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

FUNCTION  rlc_log_init(gas_id)

DESCRIPTION
  This function initializes the log_on_demand packets.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void rlc_log_init( gas_id_t gas_id )
{
  if (rlc_log_initialized[gas_id])
  {
      DEBUGMSG( "rlc_log_init(gas_id)> already initialised", 0,0,0 );
      return;
  }
  else
  {
      /* Call this only once */
      rlc_log_initialized[gas_id] = TRUE;

      DEBUGMSG( "rlc_log_init register log-on-demand", 0,0,0 );
      (void)log_on_demand_register(LOG_GPRS_RLC_UL_ABNRML_RLS_COUNTS_C,
                                    rlc_log_on_demand);
      (void)log_on_demand_register(LOG_GPRS_RLC_UL_EVENT_COUNTS_C,
                                    rlc_log_on_demand);
      (void)log_on_demand_register(LOG_GPRS_RLC_UL_STATS_C,
                                    rlc_log_on_demand);
      (void)log_on_demand_register(LOG_GPRS_RLC_DL_STATS_C,
                                    rlc_log_on_demand);

      DEBUGMSG( "rlc_log_init register log-packet-request", 0,0,0 );
      (void)diagpkt_gsm_gprs_lpr_register(LOG_GPRS_RLC_UL_ABNRML_RLS_COUNTS_C,
                                          rlc_log_packet_request);
      (void)diagpkt_gsm_gprs_lpr_register(LOG_GPRS_RLC_UL_EVENT_COUNTS_C,
                                          rlc_log_packet_request);
      (void)diagpkt_gsm_gprs_lpr_register(LOG_GPRS_RLC_UL_STATS_C,
                                          rlc_log_packet_request);
      (void)diagpkt_gsm_gprs_lpr_register(LOG_GPRS_RLC_DL_STATS_C,
                                          rlc_log_packet_request);

      DEBUGMSG( "rlc_log_init finished", 0,0,0 );
  }
}


/*===========================================================================

FUNCTION  rlc_log_on_demand

DESCRIPTION
  This function calls relevant log packet routine when the DM demands it.

RETURN VALUE
  Status of the call (should always be good)

SIDE EFFECTS
  None

===========================================================================*/
static log_on_demand_status_enum_type rlc_log_on_demand(log_code_type log_code)
{
  /* HV- temporarily default to using ..ID_1, until Diag Team provides new API
  */
  rlc_log_packet_request(/* GERAN_ACCESS_STRATUM_ID_1, */
                         log_code, FALSE, 0);

  return LOG_ON_DEMAND_SENT_S;

} /* rlc_log_on_demand */


/*===========================================================================

FUNCTION  rlc_log_packet_request

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
static void rlc_log_packet_request(/* gas_id_t gas_id, */
                            uint16 log_code,
                            uint8  zero_stats,
                            uint8  additional_info)
{
  /* Default to ID_1 until the Diag team provides the new API
  */
  gas_id_t  gas_id = GERAN_ACCESS_STRATUM_ID_1;

  NOTUSED(additional_info);

  switch (log_code)
  {
    case LOG_GPRS_RLC_UL_ABNRML_RLS_COUNTS_C:
      (void)rlc_log_ul_abnrml_rls_counts(gas_id, zero_stats);
      break;

    case LOG_GPRS_RLC_UL_EVENT_COUNTS_C:
      (void)rlc_log_ul_event_counts(gas_id, zero_stats);
      break;

    case LOG_GPRS_RLC_UL_STATS_C:
      (void)rlc_log_ul_statistics(gas_id, zero_stats);
      break;

    case LOG_GPRS_RLC_DL_STATS_C:
      (void)rlc_log_dl_statistics(gas_id, zero_stats);
      break;

    default:
        /* Prevent Lint warning about no default */
        break;
  } /* end switch */
} /* rlc_log_packet_request*/

/*===========================================================================

FUNCTION  rlc_log_ul_abnrml_rls_counts

DESCRIPTION
  This function populates and sends a Uplink Abnormal Release Counts
  log packet, optionally clearing the counts in the process.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
static boolean rlc_log_ul_abnrml_rls_counts(gas_id_t gas_id, boolean zero_stats)
{
    const grlc_diag_ul_abnormal_release_counts_t *rpt_ptr;  /* data source */
    gsmdiag_pkt_info_t                           pkt_info;
    LOG_GPRS_RLC_UL_ABNRML_RLS_COUNTS_C_type     *log_ptr;  /* log buffer */

    /* Ensure logging is initialised */
    rlc_log_init(gas_id);

    rlc_log_packet_init(gas_id, &pkt_info, LOG_GPRS_RLC_UL_ABNRML_RLS_COUNTS_C,
                                    sizeof(LOG_GPRS_RLC_UL_ABNRML_RLS_COUNTS_C_type));

    /* Clear the counts if required */
    if (zero_stats)
    {
        DEBUGMSG( "rlc_log_ul_abnrml_rls_counts> clear counts", 0,0,0 );
        grlc_diag_clr_ul_abnormal_release_counts(gas_id);
    }

    /* Obtain the current counts */
    rpt_ptr = grlc_diag_get_ul_abnormal_release_counts(gas_id);
    if (rpt_ptr == NULL)
    {
        DEBUGMSG( "rlc_log_ul_abnrml_rls_counts> failed to get counts", 0,0,0 );
        return FALSE;
    }

    /* Try to allocate memory for the log packet -             */
    /* this will fail if the log mask is not set appropriately */
    log_ptr = (LOG_GPRS_RLC_UL_ABNRML_RLS_COUNTS_C_type *)gsmdiag_log_alloc(&pkt_info);

    if (log_ptr == NULL)
    {
        DEBUGMSG( "rlc_log_ul_abnrml_rls_counts> log_alloc failed", 0,0,0 );
    }
    else
    {
        /* Fill packet and commit it */
        log_ptr->access_reject_cnt = rpt_ptr->access_reject_cnt;
        log_ptr->arc_retry_cnt     = rpt_ptr->arc_retry_cnt;
        log_ptr->arc_wo_retry_cnt  = rpt_ptr->arc_wo_retry_cnt;
        log_ptr->arc_sys_info_cnt  = rpt_ptr->arc_sys_info_cnt;

        gsmdiag_log_commit(&pkt_info);
        DEBUGMSG( "rlc_log_ul_abnrml_rls_counts> packet logged", 0,0,0 );
    }

    return (log_ptr != NULL);
} /* rlc_log_ul_abnrml_rls_counts */



/*===========================================================================

FUNCTION  rlc_log_ul_event_counts

DESCRIPTION
  This function populates and sends a Uplink Event Counts
  log packet, optionally clearing the counts in the process.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
boolean rlc_log_ul_event_counts(gas_id_t gas_id, boolean zero_stats)
{
    const grlc_diag_ul_event_counts_t   *rpt_ptr;           /* data source */
    gsmdiag_pkt_info_t                  pkt_info;
    LOG_GPRS_RLC_UL_EVENT_COUNTS_C_type *log_ptr;           /* log buffer */

    /* Ensure logging is initialised */
    rlc_log_init(gas_id);

    rlc_log_packet_init(gas_id, &pkt_info, LOG_GPRS_RLC_UL_EVENT_COUNTS_C,
                                    sizeof(LOG_GPRS_RLC_UL_EVENT_COUNTS_C_type));

    /* Clear the counts if required */
    if (zero_stats)
    {
        DEBUGMSG( "rlc_log_ul_event_counts> clear counts", 0,0,0 );
        grlc_diag_clr_ul_event_counts(gas_id);
    }

    /* Obtain the current counts */
    rpt_ptr = grlc_diag_get_ul_event_counts(gas_id);
    if (rpt_ptr == NULL)
    {
        DEBUGMSG( "rlc_log_ul_event_counts> failed to get counts", 0,0,0 );
        return FALSE;
    }

    /* Try to allocate memory for the log packet -             */
    /* this will fail if the log mask is not set appropriately */
    log_ptr = (LOG_GPRS_RLC_UL_EVENT_COUNTS_C_type *)gsmdiag_log_alloc(&pkt_info);

    if (log_ptr == NULL)
    {
        DEBUGMSG( "rlc_log_ul_event_counts> log_alloc failed", 0,0,0 );
    }
    else
    {
        /* Fill packet and commit it */
        log_ptr->llc_event_cnt = rpt_ptr->llc_event_cnt;
        log_ptr->mac_event_cnt = rpt_ptr->mac_event_cnt;
        log_ptr->pl1_event_cnt = rpt_ptr->pl1_event_cnt;
        log_ptr->tmr_event_cnt = rpt_ptr->tmr_event_cnt;

        gsmdiag_log_commit(&pkt_info);
        DEBUGMSG( "rlc_log_ul_event_counts> packet logged", 0,0,0 );
    }

    return (log_ptr != NULL);
} /* rlc_log_ul_event_counts */



/*===========================================================================

FUNCTION  rlc_log_ul_statistics

DESCRIPTION
  This function populates and sends a Uplink Statistics
  log packet, optionally clearing the counts in the process.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
boolean rlc_log_ul_statistics(gas_id_t gas_id, boolean zero_stats)
{
    const grlc_diag_ul_statistics_t *rpt_ptr;               /* data source */
    gsmdiag_pkt_info_t              pkt_info;
    LOG_GPRS_RLC_UL_STATS_C_type    *log_ptr;               /* log buffer */

    /* Ensure logging is initialised */
    rlc_log_init(gas_id);

    rlc_log_packet_init(gas_id, &pkt_info, LOG_GPRS_RLC_UL_STATS_C,
                                    sizeof(LOG_GPRS_RLC_UL_STATS_C_type));

    /* Clear the counts if required */
    if (zero_stats)
    {
        DEBUGMSG( "gs%d:rlc_log_ul_statistics> clear counts", GAS_ID_M,0,0 );
        grlc_diag_clr_ul_statistics(gas_id);
    }

    /* Obtain the current counts */
    rpt_ptr = grlc_diag_get_ul_statistics(gas_id);
    if (rpt_ptr == NULL)
    {
        DEBUGMSG( "gs%d:rlc_log_ul_statistics> failed to get counts", GAS_ID_M,0,0 );
        return FALSE;
    }

    /* Try to allocate memory for the log packet -             */
    /* this will fail if the log mask is not set appropriately */
    log_ptr = (LOG_GPRS_RLC_UL_STATS_C_type *)gsmdiag_log_alloc(&pkt_info);

    if (log_ptr == NULL)
    {
        DEBUGMSG( "rlc_log_ul_statistics> log_alloc failed", 0,0,0 );
    }
    else
    {
        /* Fill packet and commit it */
        log_ptr->grlc_ul_state          = rpt_ptr->grlc_ul_state;
        log_ptr->grlc_ul_substate       = rpt_ptr->grlc_ul_substate;
        log_ptr->ack_tx_pdu_cnt         = rpt_ptr->ack_tx_pdu_cnt;
        log_ptr->ack_tx_oct_cnt         = rpt_ptr->ack_tx_oct_cnt;
        log_ptr->unack_tx_pdu_cnt       = rpt_ptr->unack_tx_pdu_cnt;
        log_ptr->unack_tx_oct_cnt       = rpt_ptr->unack_tx_oct_cnt;
        log_ptr->coding_sch_changes     = rpt_ptr->coding_sch_changes;
        log_ptr->ul_seq_va              = rpt_ptr->ul_seq_va;
        log_ptr->ul_seq_vs              = rpt_ptr->ul_seq_vs;
        log_ptr->ul_seq_nseg            = rpt_ptr->ul_seq_nseg;
        log_ptr->ul_seq_vsf             = rpt_ptr->ul_seq_vsf;
        log_ptr->ul_seq_pack_vsf        = rpt_ptr->ul_seq_pack_vsf;
        log_ptr->ul_seq_nack_vsf        = rpt_ptr->ul_seq_nack_vsf;
        log_ptr->ul_n3102               = rpt_ptr->ul_n3102;
        log_ptr->ul_n3104               = rpt_ptr->ul_n3104;
        log_ptr->ul_gen_purpose_count_a = rpt_ptr->ul_gen_purpose_count_a;
        log_ptr->ul_gen_purpose_count_b = rpt_ptr->ul_gen_purpose_count_b;
        log_ptr->ul_gen_purpose_count_c = rpt_ptr->ul_gen_purpose_count_c;

        gsmdiag_log_commit(&pkt_info);
        DEBUGMSG( "rlc_log_ul_statistics> packet logged", 0,0,0 );
    }

    return (log_ptr != NULL);
} /* rlc_log_ul_statistics */



/*===========================================================================

FUNCTION  rlc_log_dl_statistics

DESCRIPTION
  This function populates and sends a Downlink Statistics
  log packet, optionally clearing the counts in the process.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
boolean rlc_log_dl_statistics(gas_id_t gas_id, boolean zero_stats)
{
    const grlc_diag_dl_statistics_t *rpt_ptr;               /* data source */
    gsmdiag_pkt_info_t              pkt_info;
    LOG_GPRS_RLC_DL_STATS_C_type    *log_ptr;               /* log buffer */

    /* Ensure logging is initialised */
    rlc_log_init(gas_id);

    rlc_log_packet_init(gas_id, &pkt_info, LOG_GPRS_RLC_DL_STATS_C,
                                    sizeof(LOG_GPRS_RLC_DL_STATS_C_type));

    /* Clear the counts if required */
    if (zero_stats)
    {
        DEBUGMSG( "rlc_log_dl_statistics> clear counts", 0,0,0 );
        grlc_diag_clr_dl_statistics(gas_id);
    }

    /* Obtain the current counts */
    rpt_ptr = grlc_diag_get_dl_statistics(gas_id);
    if (rpt_ptr == NULL)
    {
        DEBUGMSG( "rlc_log_dl_statistics> failed to get counts", 0,0,0 );
        return FALSE;
    }

    /* Try to allocate memory for the log packet -             */
    /* this will fail if the log mask is not set appropriately */
    log_ptr = (LOG_GPRS_RLC_DL_STATS_C_type *)gsmdiag_log_alloc(&pkt_info);

    if (log_ptr == NULL)
    {
        DEBUGMSG( "rlc_log_dl_statistics> log_alloc failed", 0,0,0 );
    }
    else
    {
        /* Fill packet and commit it */
        log_ptr->grlc_dl_state          = rpt_ptr->grlc_dl_state;
        log_ptr->ack_rx_pdu_cnt         = rpt_ptr->ack_rx_pdu_cnt;
        log_ptr->ack_rx_oct_cnt         = rpt_ptr->ack_rx_oct_cnt;
        log_ptr->unack_rx_pdu_cnt       = rpt_ptr->unack_rx_pdu_cnt;
        log_ptr->unack_pdu_filled_cnt   = rpt_ptr->unack_pdu_filled_cnt;
        log_ptr->unack_rx_oct_cnt       = rpt_ptr->unack_rx_oct_cnt;
        log_ptr->unack_oct_filled_cnt   = rpt_ptr->unack_oct_filled_cnt;
        log_ptr->ack_nack_req_cnt       = rpt_ptr->ack_nack_req_cnt;
        log_ptr->dl_seq_vq              = rpt_ptr->dl_seq_vq;
        log_ptr->dl_seq_vr              = rpt_ptr->dl_seq_vr;
        log_ptr->dl_seq_ssn             = rpt_ptr->dl_seq_ssn;
        log_ptr->dl_gen_purpose_count_a = rpt_ptr->dl_gen_purpose_count_a;
        log_ptr->dl_gen_purpose_count_b = rpt_ptr->dl_gen_purpose_count_b;
        log_ptr->dl_gen_purpose_count_c = rpt_ptr->dl_gen_purpose_count_c;

        gsmdiag_log_commit(&pkt_info);
        DEBUGMSG( "rlc_log_dl_statistics> packet logged", 0,0,0 );
    }

    return (log_ptr != NULL);
} /* rlc_log_dl_statistics */


/*===========================================================================

FUNCTION  rlc_log_ul_acknack_params

DESCRIPTION
  This function populates and sends a uplink acknack description parameters .

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void rlc_log_ul_acknack_params( gas_id_t gas_id )
{
    uint16 u;

    gsmdiag_pkt_info_t                            pkt_info;
    /* log buffer */
    LOG_GPRS_RLC_UL_ACKNACK_PARAMS_VER2_C_type    *log_ptr;

    rlc_log_packet_init(gas_id, &pkt_info, LOG_GPRS_RLC_UL_ACKNACK_PARAMS_VER2_C,
                                    sizeof(LOG_GPRS_RLC_UL_ACKNACK_PARAMS_VER2_C_type));

    /* Try to allocate memory for the log packet -             */
    /* this will fail if the log mask is not set appropriately */

    log_ptr = (LOG_GPRS_RLC_UL_ACKNACK_PARAMS_VER2_C_type *)gsmdiag_log_alloc(&pkt_info);
    if (log_ptr != NULL)
    {
      log_ptr->lu32 = 0L;
      log_ptr->hu32 = 0L;
      log_ptr->ul_tfi = (uint8)ul[gas_id].cur_res.ul_tfi;
      log_ptr->ssn    = ul[gas_id].ul_acknack.ssn;
      log_ptr->fai    = ul[gas_id].ul_acknack.fai;
      log_ptr->cs     = ul[gas_id].cur_res.cs;
      log_ptr->cv     = ul[gas_id].cv;
      log_ptr->va     = (uint8)ul[gas_id].va;
      log_ptr->vs     = (uint8)ul[gas_id].vs;
      log_ptr->si     = ul[gas_id].si;

      for (u=0; u<4; u++)
      {
        /* RBB[4..7] into the lower 32 bits
        */
        log_ptr->lu32 |= (ul[gas_id].ul_acknack.rbb[u+4] << (8*((4-u)-1)) );

        /* RBB[0..3] into the higher 32 bits
        */
        log_ptr->hu32 |= (ul[gas_id].ul_acknack.rbb[u] << (8*((4-u)-1)) );
      }
      gsmdiag_log_commit(&pkt_info);
    }
} /* rlc_log_ul_acknack_params */


/*===========================================================================

FUNCTION  rlc_log_dl_acknack_params

DESCRIPTION
  This function populates and sends a downlink acknack description parameters .

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void rlc_log_dl_acknack_params
(
 gas_id_t  gas_id,
 uint8  dl_tfi,
 uint8  fai,
 uint8  ssn,
 uint8  vq,
 uint32 hu32,
 uint32 lu32,
 uint8  cs
)
{
    gsmdiag_pkt_info_t                          pkt_info;
    LOG_GPRS_RLC_DL_ACKNACK_PARAMS_VER2_C_type  *log_ptr;       /* log buffer */

    rlc_log_packet_init(gas_id, &pkt_info, LOG_GPRS_RLC_DL_ACKNACK_PARAMS_VER2_C,
                                    sizeof(LOG_GPRS_RLC_DL_ACKNACK_PARAMS_VER2_C_type));

    /* Try to allocate memory for the log packet -             */
    /* this will fail if the log mask is not set appropriately */
    log_ptr = (LOG_GPRS_RLC_DL_ACKNACK_PARAMS_VER2_C_type    *)gsmdiag_log_alloc(&pkt_info);

    if (log_ptr != NULL)
    {
      log_ptr->dl_tfi = dl_tfi;
      log_ptr->fai    = fai;
      log_ptr->ssn    = ssn;
      log_ptr->vq     = vq;
      log_ptr->hu32   = hu32;
      log_ptr->lu32   = lu32;
      log_ptr->cs    = cs;

      gsmdiag_log_commit(&pkt_info);
    }
} /* rlc_log_dl_acknack_params */


/*===========================================================================

FUNCTION  rlc_log_ul_release_ind

DESCRIPTION
  This function populates and sends uplink release cause.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void rlc_log_ul_release_ind
(
 gas_id_t              gas_id,
 uint8                 ul_tfi,
 rlc_log_ul_release_t  release_cause
)
{
    gsmdiag_pkt_info_t                  pkt_info;
    LOG_GPRS_RLC_UL_RELEASE_IND_C_type  *log_ptr;               /* log buffer */

    rlc_log_packet_init(gas_id, &pkt_info, LOG_GPRS_RLC_UL_RELEASE_IND_C,
                                    sizeof(LOG_GPRS_RLC_UL_RELEASE_IND_C_type));

    /* Try to allocate memory for the log packet -             */
    /* this will fail if the log mask is not set appropriately */
    log_ptr = (LOG_GPRS_RLC_UL_RELEASE_IND_C_type *)gsmdiag_log_alloc(&pkt_info);

    if (log_ptr != NULL)
    {
      log_ptr->ul_tfi        = ul_tfi;
      log_ptr->release_cause = (byte)release_cause;

      gsmdiag_log_commit(&pkt_info);
    }
} /* rlc_log_ul_release_ind */


/*===========================================================================

FUNCTION  rlc_log_dl_release_ind

DESCRIPTION
  This function populates and sends downlink release cause.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void rlc_log_dl_release_ind
(
 gas_id_t gas_id,
 uint8  dl_tfi,
 rlc_log_dl_release_t release_cause
)
{
    gsmdiag_pkt_info_t                  pkt_info;
    LOG_GPRS_RLC_DL_RELEASE_IND_C_type  *log_ptr;               /* log buffer */

    rlc_log_packet_init(gas_id, &pkt_info, LOG_GPRS_RLC_DL_RELEASE_IND_C,
                                    sizeof(LOG_GPRS_RLC_DL_RELEASE_IND_C_type));

    /* Try to allocate memory for the log packet -             */
    /* this will fail if the log mask is not set appropriately */
    log_ptr = (LOG_GPRS_RLC_DL_RELEASE_IND_C_type *)gsmdiag_log_alloc(&pkt_info);

    if (log_ptr != NULL)
    {
      log_ptr->dl_tfi           = dl_tfi;
      log_ptr->release_cause    = (byte)release_cause;

      gsmdiag_log_commit(&pkt_info);
    }
} /* rlc_log_dl_release_ind */

/*===========================================================================
===
===  FUNCTION      rlc_log_rfblks_cnts()
===
===  DESCRIPTION
===
===    RLC function to fill and return the Uplink Statistics Info
===
===  DEPENDENCIES
===
===
===  RETURN VALUE
===
===    pointer to data structure
===
===  SIDE EFFECTS
===
===    none
===
===========================================================================*/
rlc_log_rfblks_cnts_t* rlc_log_rfblks_cnts( gas_id_t gas_id )
{
  static rlc_log_rfblks_cnts_t  temp_st[NUM_GERAN_DATA_SPACES];

  temp_st[gas_id].dl_rx   = dl[gas_id].rx_blk_cnt;
  temp_st[gas_id].ul_tx   = ul[gas_id].ptx_cnt;
  temp_st[gas_id].ul_retx = ul[gas_id].nack_cnt + ul[gas_id].pack_cnt;

  return &temp_st[gas_id];
}

/*===========================================================================
===
===  FUNCTION      rlc_log_dl_blks_cnts()
===
===  DESCRIPTION
===
===    RLC function to fill and return the Downlink Statistics Info
===
===  DEPENDENCIES
===
===
===  RETURN VALUE
===
===    pointer to data structure
===
===  SIDE EFFECTS
===
===    none
===
===========================================================================*/
rlc_eng_mode_dl_cnts_t* rlc_log_dl_blks_cnts(gas_id_t gas_id)
{
  static rlc_eng_mode_dl_cnts_t  temp_st[NUM_GERAN_DATA_SPACES];

  temp_st[gas_id].dl_rx   = dl[gas_id].eng_mode_rcvd_dl_block_cnt + dl[gas_id].eng_mode_rcvd_dup_dl_block_cnt;
  temp_st[gas_id].dl_reprx  = dl[gas_id].eng_mode_rcvd_dup_dl_block_cnt;
  return &temp_st[gas_id];
}


/*===========================================================================

FUNCTION  rlc_log_dl_header

DESCRIPTION
  EGPRS 3.3 0x5206  EGPRS Downlink Header
  This function should be called when RLC has got a data block of DL TBF.
  For GPRS type = 0, For EGPRS type value is defined as spec says.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void rlc_log_dl_header
(
 gas_id_t    gas_id,
 byte        type,
 const byte* dl_header,
 byte        crc_status,
 byte        time_slot,
 uint32      frame_number,
 const byte* dl_payload
)
{
  gsmdiag_pkt_info_t              pkt_info;
  LOG_EGPRS_RLC_DL_HEADER_C_type *log_ptr;    /* pointer to log buffer */

  rlc_log_packet_init(gas_id, &pkt_info, LOG_EGPRS_RLC_DL_HEADER_C,
                                  sizeof(LOG_EGPRS_RLC_DL_HEADER_C_type));

  log_ptr = (LOG_EGPRS_RLC_DL_HEADER_C_type *)gsmdiag_log_alloc(&pkt_info);

  if (log_ptr != NULL)
  {
    log_ptr->type = type;

    /* Copy 5 Bytes of DL Header */
    (void)memscpy((void*)log_ptr->dl_hdr, sizeof(log_ptr->dl_hdr), (void*)dl_header, 5);

    log_ptr->crc_status  =  crc_status;
    log_ptr->time_slot     =  time_slot;
    log_ptr->frame_number  =  frame_number;

    /* Copy 4*LOG_MAX_LI_E = 12 bytes of DL Payload if Header Type-1, otherwise
       copy 2*LOG_MAX_LI_E = 6 bytes*/
    (void)memscpy((void*)log_ptr->dl_payload,
                  sizeof(log_ptr->dl_payload),
                 (void*)dl_payload,
                 (((type ==(byte) EGPRS_HEADER_TYPE_1)? 4 : 2)*LOG_MAX_LI_E));

    gsmdiag_log_commit(&pkt_info);
  }
} /* rlc_log_dl_header */


/*===========================================================================

FUNCTION  rlc_log_ul_header

DESCRIPTION
  EGPRS 3.4 0x5205  EGPRS Uplink Header
  This function should be called when RLC has got a data block of UL TBF.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void rlc_log_ul_header
(
 gas_id_t    gas_id,
 byte        type,
 const byte* ul_header
)
{
  gsmdiag_pkt_info_t              pkt_info;
  LOG_EGPRS_RLC_UL_HEADER_C_type *log_ptr;    /* pointer to log buffer */

  rlc_log_packet_init(gas_id, &pkt_info, LOG_EGPRS_RLC_UL_HEADER_C,
                                  sizeof(LOG_EGPRS_RLC_UL_HEADER_C_type));

  log_ptr = (LOG_EGPRS_RLC_UL_HEADER_C_type *)gsmdiag_log_alloc(&pkt_info);

  if (log_ptr != NULL)
  {
    log_ptr->type = type;

    /* Copy 6 Bytes of DL Header */
    (void)memscpy((void*)log_ptr->ul_hdr, sizeof(log_ptr->ul_hdr), (void*)ul_header, 6);

    gsmdiag_log_commit(&pkt_info);
  }

} /* rlc_log_ul_header */


/*===========================================================================

FUNCTION  rlc_log_epuan

DESCRIPTION
  EGPRS RLC UL ACK/NACK (0x5209)
  This function should be called when RLC has got it from MAC.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void rlc_log_epuan (gas_id_t gas_id, const mr_ul_pkt_acknack_ind_t* puan_msg)
{

  gsmdiag_pkt_info_t         pkt_info;
  LOG_EGPRS_RLC_EPUAN_C_type *log_ptr;

  rlc_log_packet_init(gas_id, &pkt_info, LOG_EGPRS_RLC_EPUAN_C,
                                  sizeof(LOG_EGPRS_RLC_EPUAN_C_type));

  log_ptr = (LOG_EGPRS_RLC_EPUAN_C_type *)gsmdiag_log_alloc(&pkt_info);

  if (log_ptr != NULL)
  {
    /* Initialize mask */
    log_ptr->mask    = 0;

    log_ptr->tfi      = puan_msg->tfi;
    log_ptr->mcs      = puan_msg->mcs;

    /* log_ptr->mask
    ** | d | d | scc | pre | res | eow | bow | fai |
    **   8   7    6     5     4     3     2     1
    */
    log_ptr->mask     = (uint8)(  ( (byte)puan_msg->egprs_acknack_desc.fai       )  |
                                  ( ((byte)puan_msg->egprs_acknack_desc.bow) << 1)  |
                                  ( ((byte)puan_msg->egprs_acknack_desc.eow) << 2)  |
                                  ( ((byte)puan_msg->resegment) << 3             )  |
                                  ( ((byte)puan_msg->pre_emptive_tx) << 4        )  |
                                  ( ((byte)puan_msg->egprs_acknack_desc.scc) << 5)
                               );

    log_ptr->ssn      = puan_msg->egprs_acknack_desc.ssn;

    if (puan_msg->egprs_acknack_desc.comp_present)
    {
       log_ptr->crbb_len = puan_msg->egprs_acknack_desc.crbb_len;
       (void)memscpy((void*)log_ptr->crbb, sizeof(log_ptr->crbb), (void*)puan_msg->egprs_acknack_desc.crbb, 16);
    }
    else
    {
       log_ptr->crbb_len = 0;
    }

    log_ptr->urbb_len = puan_msg->egprs_acknack_desc.urbb_len;
    if (log_ptr->urbb_len)
    {
      (void)memscpy((void*)log_ptr->urbb, sizeof(log_ptr->urbb), (void*)puan_msg->egprs_acknack_desc.urbb, 20);
    }

    gsmdiag_log_commit(&pkt_info);
  }
} /* rlc_log_epuan */

/*===========================================================================

FUNCTION  rlc_log_epdan

DESCRIPTION
  EGPRS RLC UL ACK/NACK (0x5211)
  This function should be called when RLC has got it from MAC.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void rlc_log_epdan (gas_id_t gas_id, const rm_dl_pkt_acknack_req_t* pdan_msg)
{
  gsmdiag_pkt_info_t         pkt_info;
  LOG_EGPRS_RLC_EPDAN_C_type *log_ptr;

  rlc_log_packet_init(gas_id, &pkt_info, LOG_EGPRS_RLC_EPDAN_C,
                                  sizeof(LOG_EGPRS_RLC_EPDAN_C_type));

  log_ptr = (LOG_EGPRS_RLC_EPDAN_C_type *)gsmdiag_log_alloc(&pkt_info);

  if (log_ptr != NULL)
  {
    /* Initialize mask */
    log_ptr->mask    = 0;

    log_ptr->tfi     = pdan_msg->tfi;

    /* log_ptr->mask
    ** log_ptr->mask
    ** | 8PSK | GMSK | scc | esp2 |esp1  | eow | bow | fai |
    **    8      7      6     5     4       3     2     1
    */
    log_ptr->ssn      = pdan_msg->e_acknack_desc.ssn;

    if (pdan_msg->e_acknack_desc.comp_present)
    {
       log_ptr->crbb_len = pdan_msg->e_acknack_desc.crbb_len;
       (void)memscpy((void*)log_ptr->crbb, sizeof(log_ptr->crbb), (void*)pdan_msg->e_acknack_desc.crbb, 16);

       /* Store Starting Colour Code into MASK */
       log_ptr->mask |= (pdan_msg->e_acknack_desc.scc & 0x01) << 5;

    }
    else
    {
       log_ptr->crbb_len = 0;
    }

    log_ptr->urbb_len = pdan_msg->e_acknack_desc.urbb_len;
    if (log_ptr->urbb_len)
    {
      (void)memscpy((void*)log_ptr->urbb, sizeof(log_ptr->urbb), (void*)pdan_msg->e_acknack_desc.urbb, 20);
    }

    /* Store other fields into MASK */
    log_ptr->mask   |= (pdan_msg->esp                & 0x03) << 3;
    log_ptr->mask   |= (pdan_msg->e_acknack_desc.eow & 0x01) << 2;
    log_ptr->mask   |= (pdan_msg->e_acknack_desc.bow & 0x01) << 1;
    log_ptr->mask   |= (pdan_msg->e_acknack_desc.fai & 0x01) << 0;


    /* get the BEP and C from channel quality report*/
    if( pdan_msg->pl1_qrep_ptr != NULL)
    {
      log_ptr->gmsk_bep = 0;
      log_ptr->psk_bep  = 0;

      log_ptr->mask    |= (pdan_msg->pl1_qrep_ptr->gmsk_mean_bep_present & 0x01) << 6;
      log_ptr->mask    |= (pdan_msg->pl1_qrep_ptr->psk_mean_bep_present  & 0x01) << 7;


      log_ptr->c_value = pdan_msg->pl1_qrep_ptr->c_value;

      log_ptr->gmsk_bep |= (pdan_msg->pl1_qrep_ptr->gmsk_cv_bep & 0x07) << 5;
      log_ptr->gmsk_bep |= (pdan_msg->pl1_qrep_ptr->gmsk_m_bep  & 0x1F) << 0;

      log_ptr->psk_bep  |= (pdan_msg->pl1_qrep_ptr->psk_cv_bep & 0x07)  << 5;
      log_ptr->psk_bep  |= (pdan_msg->pl1_qrep_ptr->psk_m_bep  & 0x1F)  << 0;

    }
    else
    {
      log_ptr->mask    &= 0x3F;      /* set both GMSK and 8PSK BEP Flag invalid */
      log_ptr->c_value = 0xFF;       /* 0xFF indicating C value is out of range.
                                     ** Not 0-63
                                     */
    }

    gsmdiag_log_commit(&pkt_info);
  }
} /* rlc_log_epdan */

/*===========================================================================

FUNCTION  grlc_log_timer_event

DESCRIPTION
  Called when a GRLC timer expires - used for both Uplink and Downlink tasks

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void grlc_log_timer_event(gas_id_t gas_id, const rm_timer_t timer_t)
{

/**********************************************************************
** These defines reflect the contents of the section below of the
** Serial Interface Control Document for QXDM Events
** 2.569 GPRS_TIMER_EXPIRY(ID 1077)
***********************************************************************
*/
#define EVENT_GPRS_TIMER_00   0     /* T3166                         */
#define EVENT_GPRS_TIMER_01   1     /* T3168                         */
#define EVENT_GPRS_TIMER_02   2     /* T3180                         */
#define EVENT_GPRS_TIMER_03   3     /* T3182                         */
#define EVENT_GPRS_TIMER_04   4     /* T3190                         */
#define EVENT_GPRS_TIMER_05   5     /* T3198                         */
#define EVENT_GPRS_TIMER_06   6     /* T3192                         */
#define EVENT_GPRS_TIMER_07   7     /* Await MAC Response            */
#define EVENT_GPRS_TIMER_08   8     /* Extended Uplink TBF Timer     */
#define EVENT_GPRS_TIMER_09   9     /* Await Uplink Assignment Guard */
#define EVENT_GPRS_TIMER_10   10    /* T200                          */
#define EVENT_GPRS_TIMER_11   11    /* T201                          */
#define EVENT_GPRS_TIMER_12   12    /* N3102                         */
#define EVENT_GPRS_TIMER_13   13    /* N3104                         */

  uint8 ev_rpt_timer_exp = EVENT_GPRS_TIMER_00;

  /* Map GRLC Timer ID onto Event Reporting Timer ID
  */
  switch ( timer_t )
  {
    /* RLC DL: wait for the 1st valid downlink data block.
    ** action: start when an assignment is received. Stop when the 1st
    ** valid data block is received. If expired, do abnormal release
    ** with random access
    */
    case RM_TMR_T3190_AWAIT_VALID_DL_DATA:
      ev_rpt_timer_exp = EVENT_GPRS_TIMER_04;   /* T3190 */
      break;

    /* RLC DL: wait for release of downlink tbf
    ** action: start after sending packet acknack (ack mode) with FAI
    ** set or after sending packet control ack in response to final
    ** data block in unack mode. If expired, return to Idle.
    */
    case RM_TMR_T3192_AWAIT_RELEASE_DL_TBF:
      ev_rpt_timer_exp = EVENT_GPRS_TIMER_06;   /* T3192 */
      break;

    /* RLC: wait for ul acknack.
    ** Action: start after sending block with CV = 0 or detecting window stall
    ** condition. Stop timer on receipt of PUAN. When expired, decrement
    ** N3102 and perform abnormal release with random access.
    */
    case RM_TMR_T3182_AWAIT_UL_ACKNACK:
      ev_rpt_timer_exp = EVENT_GPRS_TIMER_03;   /* T3182 */
      break;

    /* RLC UL: timeout waiting for MAC reply.
    ** action: starts/re-starts every time RLC sends MAC message.
    ** expired: prints error message and calls state machine.
    */
    case RM_TMR_AWAIT_MAC_UL_RESP:
      ev_rpt_timer_exp = EVENT_GPRS_TIMER_07;   /* Await MAC Response */
      break;

    /* Guard Timer for Extended Uplink TBF mode (XTimer)
    */
    case RM_TMR_AWAIT_UL_ACKNACK:
      ev_rpt_timer_exp = EVENT_GPRS_TIMER_08;   /* Extended Uplink TBF Timer */
      break;

    case RM_TMR_AWAIT_ASS_GUARD:
      ev_rpt_timer_exp = EVENT_GPRS_TIMER_09;   /* Await Uplink Assignment Guard */
      break;

    default:
      ev_rpt_timer_exp = EVENT_GPRS_TIMER_00; /* ??? T3166 ??? */
      break;
  }

  if ( ev_rpt_timer_exp != 0xff )
  {
    event_report_payload( EVENT_GPRS_TIMER_EXPIRY, 1, (void*)&ev_rpt_timer_exp );
  }
} /* grlc_log_timer_event */

/*===========================================================================

FUNCTION  rlc_log_ul_config

DESCRIPTION
  This function populates and sends a Uplink Configuration log packet.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void rlc_log_ul_config(mr_ul_alloc_cnf_t *config_ptr, const gas_id_t gas_id)
{
  gsmdiag_pkt_info_t             pkt_info;
  LOG_GPRS_RLC_UL_CONFIG_C_type *log_ptr;

  if (config_ptr == NULL)
  {
    return;
  }

  rlc_log_packet_init(gas_id, &pkt_info, LOG_GPRS_RLC_UL_CONFIG_C, sizeof(LOG_GPRS_RLC_UL_CONFIG_C_type));
  log_ptr = (LOG_GPRS_RLC_UL_CONFIG_C_type *) gsmdiag_log_alloc(&pkt_info);

  if (log_ptr != NULL)
  {
    log_ptr->version         = 1;
    
    log_ptr->tbf_mode        = config_ptr->tbf_mode;
    log_ptr->dtm_access      = config_ptr->dtm_access;
    if (config_ptr->tbf_mode == TBF_MODE_EGPRS)
    {
      log_ptr->cs            = config_ptr->mcs;
    }
    else
    {
      log_ptr->cs            = config_ptr->cs;
    }
    log_ptr->resegment       = config_ptr->resegment;
    log_ptr->ws              = config_ptr->ws;
    log_ptr->access_mode     = config_ptr->access_mode;
    log_ptr->access_phase    = config_ptr->access_phase;
    log_ptr->ti              = config_ptr->ti;
    log_ptr->tlli            = config_ptr->tlli;
    log_ptr->tlli_block_cs   = config_ptr->tlli_block_cs;
    log_ptr->retry           = config_ptr->retry;
    log_ptr->ul_tfi          = config_ptr->ul_tfi;
    log_ptr->num_timeslots   = config_ptr->num_timeslots;
    log_ptr->usf_granularity = config_ptr->usf_granularity;

    gsmdiag_log_commit(&pkt_info);
  }
  else
  {
    MSG_GERAN_ERROR_0_G( "RLC Log Packet - UL Config:: gsmdiag_log_alloc failed");
  }
}

/*===========================================================================

FUNCTION  rlc_log_dl_config

DESCRIPTION
  This function populates and sends a Downlink Configuration log packet.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void rlc_log_dl_config(mr_dl_ass_ind_t *config_ptr, const gas_id_t gas_id)
{
  gsmdiag_pkt_info_t             pkt_info;
  LOG_GPRS_RLC_DL_CONFIG_C_type *log_ptr;

  if (config_ptr == NULL)
  {
    return;
  }

  rlc_log_packet_init(gas_id, &pkt_info, LOG_GPRS_RLC_DL_CONFIG_C, sizeof(LOG_GPRS_RLC_DL_CONFIG_C_type));
  log_ptr = (LOG_GPRS_RLC_DL_CONFIG_C_type *) gsmdiag_log_alloc(&pkt_info);

  if (log_ptr != NULL)
  {
    log_ptr->version   = 1;
    
    log_ptr->tbf_mode  = config_ptr->tbf_mode;
    log_ptr->rlc_mode  = config_ptr->rlc_mode;
    log_ptr->ws        = config_ptr->ws;
    log_ptr->tlli      = config_ptr->tlli;
    log_ptr->num_dl_ts = config_ptr->num_dl_ts;
    log_ptr->ctrl_ack  = config_ptr->ctrl_ack;

    gsmdiag_log_commit(&pkt_info);
  }
  else
  {
    MSG_GERAN_ERROR_0_G( "RLC Log Packet - DL Config:: gsmdiag_log_alloc failed");
  }
}

/*===========================================================================

FUNCTION  rlc_log_ul_statistics_ext

DESCRIPTION
  This function populates and sends a Uplink Statistics log packet.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void rlc_log_ul_statistics_ext(const gas_id_t gas_id)
{
  gsmdiag_pkt_info_t                 pkt_info;
  LOG_GPRS_RLC_UL_STATISTICS_C_type *log_ptr;
  
  uint8 *byte_log_ptr;
  uint8  index;
  uint8  cnt;
  uint8  pdu_cnt   = grlc_ul_diag_pdu_stat_get_count(gas_id);
  uint8  block_cnt = grlc_ul_diag_block_stat_get_count(gas_id);
  uint8  tx_cnt    = grlc_ul_diag_tx_stat_get_count(gas_id);
  uint16 size;

  if (pdu_cnt > GRLC_LOG_UL_PDU_BUF_MAX)
  {
    pdu_cnt = GRLC_LOG_UL_PDU_BUF_MAX;
  }
  if (block_cnt > GRLC_LOG_UL_BLOCK_BUF_MAX)
  {
    block_cnt = GRLC_LOG_UL_BLOCK_BUF_MAX;
  }
  if (tx_cnt > GRLC_LOG_UL_TX_BUF_MAX)
  {
    tx_cnt = GRLC_LOG_UL_TX_BUF_MAX;
  }

  size = sizeof(LOG_GPRS_RLC_UL_STATISTICS_C_type)
         - (sizeof(grlc_log_ul_pdu_T)   * (GRLC_LOG_UL_PDU_BUF_MAX - pdu_cnt))
         - (sizeof(grlc_log_ul_block_T) * (GRLC_LOG_UL_BLOCK_BUF_MAX - block_cnt))
         - (sizeof(grlc_log_ul_tx_T)    * (GRLC_LOG_UL_TX_BUF_MAX - tx_cnt));
  
  rlc_log_packet_init(gas_id, &pkt_info, LOG_GPRS_RLC_UL_STATISTICS_C, size);
  log_ptr = (LOG_GPRS_RLC_UL_STATISTICS_C_type *) gsmdiag_log_alloc(&pkt_info);

  if (log_ptr != NULL)
  {
    log_ptr->version = 1;

    log_ptr->state         = ul[gas_id].state;
    log_ptr->susp_state    = ul[gas_id].suspend_state;
    log_ptr->realloc_state = ul[gas_id].realloc_state;
    
    if (cur_pdu_ptr[gas_id])
    {
      log_ptr->curr_pdu_psn = cur_pdu_ptr[gas_id]->psn;
    }
    else
    {
      log_ptr->curr_pdu_psn = 0;
    }

    if (next_pdu_ptr[gas_id])
    {
      log_ptr->next_pdu_psn = next_pdu_ptr[gas_id]->psn;
    }
    else
    {
      log_ptr->next_pdu_psn = 0;
    }
    
    log_ptr->first_psn_to_free     = ul[gas_id].first_psn_to_free;
    log_ptr->psn_to_free_inclusive = ul[gas_id].psn_to_free_inclusive;

    log_ptr->dsm_small_free_cnt = DSM_POOL_FREE_CNT(DSM_DS_SMALL_ITEM_POOL);
    log_ptr->dsm_dup_free_cnt   = DSM_POOL_FREE_CNT(DSM_DUP_ITEM_POOL);
    log_ptr->dsm_large_free_cnt = DSM_POOL_FREE_CNT(DSM_DS_LARGE_ITEM_POOL);

    log_ptr->seg_ind  = ul[gas_id].seg_ind;
    log_ptr->va       = ul[gas_id].va;
    log_ptr->vs       = ul[gas_id].vs;
    log_ptr->vsf      = ul[gas_id].vsf;
    log_ptr->pack_vsf = ul[gas_id].pack_vsf;
    log_ptr->nack_vsf = ul[gas_id].nack_vsf;
    log_ptr->ptx_cnt  = ul[gas_id].ptx_cnt;
    log_ptr->pack_cnt = ul[gas_id].pack_cnt;
    log_ptr->nack_cnt = ul[gas_id].nack_cnt;
    log_ptr->paq_ctr  = ul[gas_id].paq_ctr;
    log_ptr->n3102    = ul[gas_id].n3102;
    log_ptr->n3104    = ul[gas_id].n3104;
    log_ptr->tbc      = ul[gas_id].tbc;
    log_ptr->cv       = ul[gas_id].cv;
    log_ptr->ptx_cv   = ul[gas_id].ptx_cv;
    log_ptr->pak_cv   = ul[gas_id].pak_cv;

    log_ptr->fifo_nack_ctr = grlc_ul_fifo[gas_id].nack_ctr;
    log_ptr->fifo_ptx_ctr  = grlc_ul_fifo[gas_id].ptx_ctr;
    log_ptr->fifo_pack_ctr = grlc_ul_fifo[gas_id].pack_ctr;

    log_ptr->num_llc_pdus   = ul[gas_id].llc_pdus;
    log_ptr->num_llc_octets = ul[gas_id].llc_octets;

    /* Fill UL PDU statistics. 
     * Each PDU contain below elements::
     *  uint16    psn;
     *  uint16    len;
     *  uint8     pdu_prio;
     *  uint8     rlc_mode;
     *  uint8     peak_throughput;
     *  uint8     radio_priority;
     *  uint8     cause;
     *  uint8     pfc;
     *  uint8     pfi; */
    byte_log_ptr = (uint8 *) &log_ptr->pdu_cnt;
    index = 0;
    byte_log_ptr[index++] = pdu_cnt;

    for (cnt = 0; cnt < pdu_cnt; cnt++)
    {
      grlc_diag_ul_pdu_t *pdu_ptr = grlc_ul_diag_pdu_stat_remove_entry(gas_id);

      if (pdu_ptr == NULL)
      {
        MSG_GERAN_ERROR_2_G( "RLC Log Packet - UL Statistics:: NULL PDU ptr (cnt=%d, pdu_cnt=%d)", cnt, pdu_cnt);
        index += (pdu_cnt - cnt) * sizeof(grlc_log_ul_pdu_T);
        break;
      }

      byte_log_ptr[index++] = (uint8) pdu_ptr->psn;
      byte_log_ptr[index++] = (uint8) (pdu_ptr->psn >> 8);
      byte_log_ptr[index++] = (uint8) pdu_ptr->len;
      byte_log_ptr[index++] = (uint8) (pdu_ptr->len >> 8);
      byte_log_ptr[index++] = pdu_ptr->pdu_prio;
      byte_log_ptr[index++] = pdu_ptr->rlc_mode;
      byte_log_ptr[index++] = pdu_ptr->peak_throughput;
      byte_log_ptr[index++] = pdu_ptr->radio_priority;
      byte_log_ptr[index++] = pdu_ptr->cause;
      byte_log_ptr[index++] = pdu_ptr->pfc;
      byte_log_ptr[index++] = pdu_ptr->pfi;
    }

    /* Fill UL Data Block statistics. 
     * Each Block contain below elements::
     *  uint16    bsn;
     *  uint16    first_psn;
     *  uint16    last_psn; */
    byte_log_ptr[index++] = block_cnt;

    for (cnt = 0; cnt < block_cnt; cnt++)
    {
      grlc_diag_ul_block_t *block_ptr = grlc_ul_diag_block_stat_remove_entry(gas_id);

      if (block_ptr == NULL)
      {
        MSG_GERAN_ERROR_2_G( "RLC Log Packet - UL Statistics:: NULL Block ptr (cnt=%d, block_cnt=%d)", cnt, block_cnt);
        index += (block_cnt - cnt) * sizeof(grlc_log_ul_block_T);
        break;
      }

      byte_log_ptr[index++] = (uint8) block_ptr->bsn;
      byte_log_ptr[index++] = (uint8) (block_ptr->bsn >> 8);
      byte_log_ptr[index++] = (uint8) block_ptr->first_psn;
      byte_log_ptr[index++] = (uint8) (block_ptr->first_psn >> 8);
      byte_log_ptr[index++] = (uint8) block_ptr->last_psn;
      byte_log_ptr[index++] = (uint8) (block_ptr->last_psn >> 8);
    }

    /* Fill UL Tx statistics. 
     * Each Tx block contain below elements::
     *  uint8     ack_state;
     *  uint16    bsn;
     *  uint8     len;
     *  uint8     tbc;
     *  uint8     abs_bsn;
     *  uint8     cv;
     *  uint8     cs;
     *  uint8     cps;
     *  uint8     si; */
    byte_log_ptr[index++] = tx_cnt;

    for (cnt = 0; cnt < tx_cnt; cnt++)
    {
      grlc_diag_ul_tx_t *tx_ptr = grlc_ul_diag_tx_stat_remove_entry(gas_id);

      if (tx_ptr == NULL)
      {
        MSG_GERAN_ERROR_2_G( "RLC Log Packet - UL Statistics:: NULL Tx ptr (cnt=%d, tx_cnt=%d)", cnt, tx_cnt);
        index += (tx_cnt - cnt) * sizeof(grlc_log_ul_tx_T);
        break;
      }

      byte_log_ptr[index++] = tx_ptr->ack_state;
      byte_log_ptr[index++] = (uint8) tx_ptr->bsn;
      byte_log_ptr[index++] = (uint8) (tx_ptr->bsn >> 8);
      byte_log_ptr[index++] = tx_ptr->len;
      byte_log_ptr[index++] = tx_ptr->tbc;
      byte_log_ptr[index++] = tx_ptr->abs_bsn;
      byte_log_ptr[index++] = tx_ptr->cv;
      byte_log_ptr[index++] = tx_ptr->cs;
      byte_log_ptr[index++] = tx_ptr->cps;
      byte_log_ptr[index++] = tx_ptr->si;
    }

    gsmdiag_log_commit(&pkt_info);
  }
  else
  {
    MSG_GERAN_ERROR_0_G( "RLC Log Packet - UL Statistics:: gsmdiag_log_alloc failed");
  }
}

/*===========================================================================

FUNCTION  rlc_log_dl_statistics_ext

DESCRIPTION
  This function populates and sends a Downlink Statistics log packet.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void rlc_log_dl_statistics_ext(const gas_id_t gas_id)
{
  gsmdiag_pkt_info_t                 pkt_info;
  LOG_GPRS_RLC_DL_STATISTICS_C_type *log_ptr;
  
  uint8 *byte_log_ptr;
  uint8  index;
  uint8  cnt;
  uint8  rx_cnt    = grlc_dl_diag_rx_stat_get_count(gas_id);
  uint8  pdu_cnt   = grlc_dl_diag_pdu_stat_get_count(gas_id);
  uint16 size;

  if (rx_cnt > GRLC_LOG_DL_RX_BUF_MAX)
  {
    rx_cnt = GRLC_LOG_DL_RX_BUF_MAX;
  }
  if (pdu_cnt > GRLC_LOG_DL_PDU_BUF_MAX)
  {
    pdu_cnt = GRLC_LOG_DL_PDU_BUF_MAX;
  }

  size = sizeof(LOG_GPRS_RLC_DL_STATISTICS_C_type)
         - (sizeof(grlc_log_dl_rx_T)  * (GRLC_LOG_DL_RX_BUF_MAX - rx_cnt))
         - (sizeof(grlc_log_dl_pdu_T) * (GRLC_LOG_DL_PDU_BUF_MAX - pdu_cnt));

  rlc_log_packet_init(gas_id, &pkt_info, LOG_GPRS_RLC_DL_STATISTICS_C, size);
  log_ptr = (LOG_GPRS_RLC_DL_STATISTICS_C_type *) gsmdiag_log_alloc(&pkt_info);

  if (log_ptr != NULL)
  {
    log_ptr->version   = 1;

    log_ptr->state     = dl[gas_id].state;
    log_ptr->vr        = dl[gas_id].vr;
    log_ptr->vq        = dl[gas_id].vq;
    log_ptr->ssn       = dl[gas_id].ssn;
    log_ptr->pbsn      = dl[gas_id].pbsn;

    log_ptr->fifo_ctr  = grlc_dl_fifo[gas_id].ctr;
    log_ptr->FN        = dl[gas_id].frame_number;

    /* Fill DL Rx statistics. 
     * Each Rx block contain below elements::
     *  uint8     time_slot;
     *  uint8     crc;
     *  uint8     cs;
     *  uint16    bsn;
     *  uint8     len;
     *  uint8     vn;
     *  uint8     sp;
     *  uint8     fbi; */
    byte_log_ptr = (uint8 *) &log_ptr->rx_cnt;
    index = 0;
    byte_log_ptr[index++] = rx_cnt;

    for (cnt = 0; cnt < rx_cnt; cnt++)
    {
      grlc_diag_dl_rx_t *rx_ptr = grlc_dl_diag_rx_stat_remove_entry(gas_id);

      if (rx_ptr == NULL)
      {
        MSG_GERAN_ERROR_2_G( "RLC Log Packet - UL Statistics:: NULL Rx ptr (cnt=%d, rx_cnt=%d)", cnt, rx_cnt);
        index += (rx_cnt - cnt) * sizeof(grlc_log_dl_rx_T);
        break;
      }

      byte_log_ptr[index++] = rx_ptr->time_slot;
      byte_log_ptr[index++] = rx_ptr->crc;
      byte_log_ptr[index++] = rx_ptr->cs;
      byte_log_ptr[index++] = (uint8) rx_ptr->bsn;
      byte_log_ptr[index++] = (uint8) (rx_ptr->bsn >> 8);
      byte_log_ptr[index++] = rx_ptr->len;
      byte_log_ptr[index++] = rx_ptr->vn;
      byte_log_ptr[index++] = rx_ptr->sp;
      byte_log_ptr[index++] = rx_ptr->fbi;
    }

    /* Fill DL PDU statistics. 
     * Each PDU contain below elements::
     *  uint32    pdu_num;
     *  uint16    len;
     *  uint16    first_bsn;
     *  uint16    last_bsn; */
    byte_log_ptr[index++] = pdu_cnt;

    for (cnt = 0; cnt < pdu_cnt; cnt++)
    {
      grlc_diag_dl_pdu_t *pdu_ptr = grlc_dl_diag_pdu_stat_remove_entry(gas_id);

      if (pdu_ptr == NULL)
      {
        MSG_GERAN_ERROR_2_G( "RLC Log Packet - UL Statistics:: NULL PDU ptr (cnt=%d, pdu_cnt=%d)", cnt, pdu_cnt);
        index += (pdu_cnt - cnt) * sizeof(grlc_log_dl_pdu_T);
        break;
      }

      byte_log_ptr[index++] = (uint8) pdu_ptr->pdu_num;
      byte_log_ptr[index++] = (uint8) (pdu_ptr->pdu_num >> 8);
      byte_log_ptr[index++] = (uint8) (pdu_ptr->pdu_num >> 16);
      byte_log_ptr[index++] = (uint8) (pdu_ptr->pdu_num >> 24);
      byte_log_ptr[index++] = (uint8) pdu_ptr->len;
      byte_log_ptr[index++] = (uint8) (pdu_ptr->len >> 8);
      byte_log_ptr[index++] = (uint8) pdu_ptr->first_bsn;
      byte_log_ptr[index++] = (uint8) (pdu_ptr->first_bsn >> 8);
      byte_log_ptr[index++] = (uint8) pdu_ptr->last_bsn;
      byte_log_ptr[index++] = (uint8) (pdu_ptr->last_bsn >> 8);
    }

    gsmdiag_log_commit(&pkt_info);
  }
  else
  {
    MSG_GERAN_ERROR_0_G( "RLC Log Packet - DL Statistics:: gsmdiag_log_alloc failed");
  }
}

#ifdef FEATURE_GRLC_STORE_F3_MSG
/*===========================================================================

FUNCTION  grlc_get_f3_msg_buf_ptr

DESCRIPTION
  Returns the address of the buffer to store F3 messages.

RETURN VALUE
  grlc_f3_msg_buf_t *: buffer address

SIDE EFFECTS
  None

===========================================================================*/
static grlc_f3_msg_buf_t * grlc_get_f3_msg_buf_ptr(void)
{
  return &grlc_f3_msg_buf;
}

/*===========================================================================

FUNCTION  grlc_exclude_f3_msg

DESCRIPTION
  Returns if the F3 message need to be excluded from logging into the buffer or not.

RETURN VALUE
  TRUE if F3 msg need to be excluded, FALSE otherwise.

SIDE EFFECTS
  None

===========================================================================*/
static boolean grlc_exclude_f3_msg(const grlc_f3_msg_enum_t id)
{
  /* Certain F3 messages are logged only at the time of error recovery.
   * These messsages will anyway come as F3s in the crash report. So, no need to put in the buffer. */
  if ((id == GRLC_F3_MSG_ENUM_736) || (id >= GRLC_F3_MSG_ENUM_751 && id <= GRLC_F3_MSG_ENUM_776))
  {
    return TRUE;
  }

  return FALSE;
}

/*===========================================================================

FUNCTION  grlc_f3_msg_init

DESCRIPTION
  Initializes buffer to store F3 messages.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void grlc_f3_msg_init(void)
{
  rex_init_crit_sect(&grlc_f3_msg_crit_sec);
  return;
}

/*===========================================================================

FUNCTION  grlc_store_f3_msg

DESCRIPTION
  Called to store F3 msg into a buffer.

RETURN VALUE
  None

SIDE EFFECTS
  gas_id may contain invalid value.

===========================================================================*/
void grlc_store_f3_msg(
  const grlc_f3_msg_enum_t f3_msg_id,
  const uint32             arg1,
  const uint32             arg2,
  const uint32             arg3,
  const gas_id_t           gas_id
)
{
  grlc_f3_msg_buf_t   *grlc_f3_msg_buf_ptr = grlc_get_f3_msg_buf_ptr();
  grlc_f3_msg_entry_t *entry_ptr;

  if (grlc_exclude_f3_msg(f3_msg_id))
  {
    return;
  }

  GRLC_F3_MSG_LOCK();
  entry_ptr = &grlc_f3_msg_buf_ptr->buf[grlc_f3_msg_buf_ptr->index];
  INC_MOD(grlc_f3_msg_buf_ptr->index, GRLC_F3_MSG_BUF_SIZE);
  GRLC_F3_MSG_UNLOCK();
  
  entry_ptr->f3_msg_id = f3_msg_id;
  entry_ptr->gas_id    = gas_id;
  entry_ptr->arg1      = arg1;
  entry_ptr->arg2      = arg2;
  entry_ptr->arg3      = arg3;

  return;
}
#endif /* FEATURE_GRLC_STORE_F3_MSG */
