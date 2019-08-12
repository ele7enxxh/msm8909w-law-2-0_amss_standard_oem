#ifndef RLCLOG_H
#define RLCLOG_H

/*===============================================================================================
                             R L C L O G . H

DESCRIPTION
This file contains RLC layer log packet structure definitions, 
prototypes for RLC logging functions, any constant definitions that are needed, and any external
variable declarations needed for RLC logging.

Copyright (c) 2001 by Qualcomm Technologies, Inc.  All Rights Reserved.
Copyright (c) 2002-2009 Qualcomm Technologies, Inc.  All Rights Reserved.
================================================================================================*/


/*================================================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/wcdma/rlc/inc/rlclog.h#1 $ $DateTime: 2016/12/13 08:00:12 $ $Author: mplcsds1 $

when        who     what, where, why
--------    ---     ---------------------------------------------------------
06/09/15   aa      Replaced RLC UM/AM/TM config log packets with new packets
01/14/15   aa      Changes for new ULC log packet(0x41C5)
05/23/13   geg     WSW decoupling (except for component of QCHAT)
08/23/12   mrg     MAC-iis: Special HE functionality with LI 7 configured and 
                   RLC PDU size greater than 126 octets
07/24/12    geg      Upmerge WCDMA.MPSS.1.0 to WCDMA.MPSS.2.0
06/18/12    mrg     L2 - MAC-I and HS-RACH code changes
02/01/12    grk     Feature cleanup
09/28/10    grk     Increased MAX STATUS PDU bytes that can be logged to 256.
09/22/09    sup     Removed log code assignments and placed them in
                    log_codes_wcdma_int_v.h
05/11/09    ssg     Updated Copyright Information
10/17/08    grk     Merged taxis changes to mainline.
03/16/07    vg      added num_of_entity in the structure in logging header structure for AM/UM.
02/13/07    vg      changed AM/UM logging buffer length calculation MACRO to
                    implement dynamic PDU logging.
01/11/07    vg      added structure for UM PDu logging
11/23/06    vg      added UM DL/UL log packets under feature FEATURE_UM_PDU_LOGGING
10/09/06    ttl     Put increased number of user plane UL/DL AM data pdu 
                    logging under FEATURE_RLC_LOG_MORE_AM_PDUS.
09/22/06    ssg     Increased number of user plane UL/DL AM data logging pdus.
                    Changed log codes for WCDMA_UL_RLC_AM_USR_PLANE_PDU_LOG_PACKET
                    and WCDMA_DL_RLC_AM_USR_PLANE_PDU_LOG_PACKET
12/09/05    aw      Obsoleted the RLC ul v1 am statistics log packet.
09/07/05    aw      Added a new log code WCDMA_UL_RLC_V2_AM_STAT_LOG_PACKET
                    for uplink statistics which includes duplicate 
                    retransmissions.
09/07/05    aw      Added a new macro definition RLC_UL_V2_AM_STAT_PACKET_LEN
                    to calculate the length of 
                    WCDMA_UL_RLC_V2_AM_STAT_LOG_PACKET.
05/31/05    mv      Removed the define for AM_DATA_START_LC_ID.
08/25/04    mv      Separated the RLC PDU logging for the user plane and 
                    signaling plane logical channels.
08/19/04    mv      Changed some logging codes and their names.
06/02/04    mv      Fixed lint errors and warnings.
12/06/03    ttl     Added structures and definitions for PDU ciphering 
                    parameters logging.
11/05/02    ttl     Added logging for ciphering config.
08/13/02    ttl     Changed the log code for WCDMA_UL_RLC_CIPH_CFG_LOG_PACKET and
                    WCDMA_DL_RLC_CIPH_CFG_LOG_PACKET.
                    Added ciph_act_sn to rlc_log_ul_ciphering_type and
                    rlc_log_dl_ciphering_type.
06/12/02    ttl     Defined MAX_UL_RLC_LOG_PDUS, MAX_DL_RLC_LOG_PDUS and 
                    MAX_RLC_LOG_PDUS_SIZE.
                    Restructured the logging structures.
                    Added DL AM PDU logging, UL NAK PDU logging and DL NAK PDU logging.
04/24/02    ttl     Rewrote the statistic log code.
02/04/02    sk      Added data structures, packet definitions to log AM 
                    statistics. Added structures rlc_log_ul_am_stat_type, 
                     rlc_log_ul_am_pdu_data_type.
12/16/01    ttl     Changed naming convention of PU to PDU for Jun. spec.
09/14/01    rc      Changed log codes offsets from 0xXX to 0x1XX. 
08/24/01    rc      Added customer.h
08/20/01    rc      Added defines for Logical Channel State Information. 
                    Modified data structures for Acknowledged Mode Log Packet.
04/23/01    rc      Fixed comment to reflect the correct  range of logical 
                    channel IDs on the Uplink. 
03/26/01    rc      Initial Release

================================================================================================*/

#include "wcdma_variation.h"
#include "customer.h"
#include "comdef.h"
#include "uecomdef.h"
#include "log.h"
#include "rlcrrcif_v.h"
#include "log_codes_wcdma_int.h"
#ifdef T_WINNT
  #error code not present
#endif

#define RLC_UL_NULL_STATE         0
#define TM_UL_DATA_TRANSFER_READY 1
#define UM_UL_DATA_TRANSFER_READY 2
#define UM_UL_WAITING_TO_SUSPEND  3
#define UM_UL_SUSPENDED           4
#define AM_UL_DATA_TRANSFER_READY 5
#define AM_UL_WAITING_TO_SUSPEND  6
#define AM_UL_SUSPENDED           7
#define AM_UL_RESET_PENDING       8
#define AM_UL_CONFIG_PENDING      9

#define RLC_DL_NULL_STATE         0
#define TM_DL_DATA_TRANSFER_READY 1
#define UM_DL_DATA_TRANSFER_READY 2
#define AM_DL_DATA_TRANSFER_READY 3
#define AM_DL_RESET_PENDING       4
       
#define RLC_BCCH 0
#define RLC_PCCH 1
#define RLC_CCCH 2                      
#define RLC_DCCH 3                     
#define RLC_CTCH 4                     
#define RLC_DTCH 5     
                
#define RLC_DISCARD_MODE_TBE 0
#define RLC_DISCARD_MODE_MAXDAT 1
#define RLC_DISCARD_MODE_NO_DISCARD 2

#ifdef FEATURE_RLC_LOG_MORE_AM_PDUS
  #define MAX_UL_RLC_LOG_PDUS 42
#if ((defined (FEATURE_HSDPA_MAC_EHS)) && (defined (FEATURE_WCDMA_REL7)))
  #define MAX_DL_RLC_LOG_PDUS 26
#else
  #define MAX_DL_RLC_LOG_PDUS 42
#endif
#else
  #define MAX_UL_RLC_LOG_PDUS 16
  #define MAX_DL_RLC_LOG_PDUS 16
#endif // FEATURE_RLC_LOG_MORE_AM_PDUS

#define MAX_RLC_LOG_PDUS_SIZE 256

#define MAX_CIPHER_PDU 24

#ifdef FEATURE_WCDMA_UL_COMPR
#define MAX_ULC_LOG_SAMPLES 50
#endif

#ifndef LOG_WCDMA_BASE_C 
#define LOG_WCDMA_BASE_C 0x4000
#endif
/*---------------------------------------------------------------------------
  Data Structure for Unacknowledged and Acknowledged Mode Uplink Ciphering.
---------------------------------------------------------------------------*/
#ifdef __GNUC__
typedef PACKED struct PACKED_POST
#else /* __GNUC__*/
typedef PACKED struct 
#endif /* __GNUC__ */
{
  uint8    rlc_ul_id;           /* UL logical channel id(0..18)            */
  uint16   rb_id;               /* Radio bearer id (0..31)                 */
  boolean  ciph_mode;           /* Ciphering enabled - 0: NO, 1: Yes       */
  uint32   ciph_id;             /* Ciphering key Id                        */
                                /* 00 - 0, 01 - 1, 10 - 2, 11 - 3.         */
  uint32   hfn_am_um;           /* Hyperframe Number (20 bits)             */
  uint16   ciph_act_sn;         /* the SN to start the ciphering           */
}rlc_log_ul_ciphering_type;

/*---------------------------------------------------------------------------
  Data Structure for Unacknowledged and Acknowledged Mode Downlink Ciphering.
---------------------------------------------------------------------------*/
#ifdef __GNUC__
typedef PACKED struct PACKED_POST
#else /* __GNUC__*/
typedef PACKED struct 
#endif /* __GNUC__ */ 
{
  uint8    rlc_dl_id;           /* DL logical channel id (0..18)           */
  uint16   rb_id;               /* Radio bearer id (0..31)                 */
  boolean  ciph_mode;           /* Ciphering enabled - 0: NO, 1: Yes       */
  uint32   ciph_id;             /* Ciphering key Id                        */
                                /* 00 - 0, 01 - 1, 10 - 2, 11 - 3.         */
  uint32   hfn_am_um;           /* Hyperframe Number (20 bits)             */
  uint16   ciph_act_sn;         /* the SN to start the ciphering           */
}rlc_log_dl_ciphering_type;

/*---------------------------------------------------------------------------
  Data Structure for RLC UL states for Transparent, Unacknowledged and 
  Acknowledged Modes.
---------------------------------------------------------------------------*/
#ifdef __GNUC__
typedef PACKED struct PACKED_POST
#else /* __GNUC__*/
typedef PACKED struct
#endif /* __GNUC__ */
{
  uint8    rlc_ul_id;           /* UL logical channel id (0..18)           */
  uint8    rlc_state;           /* State of the RLC State Machine          */
                                /* 0000 - UL_NULL_STATE                    */
                                /* 0001 - TM_UL_DATA_TRANSFER_READY        */
                                /* 0010 - UM_UL_DATA_TRANSFER_READY        */
                                /* 0011 - UM_UL_WAITING_TO_SUSPEND         */
                                /* 0100 - UM_UL_SUSPENDED                  */
                                /* 0101 - AM_UL_DATA_TRANSFER_READY        */
                                /* 0110 - AM_UL_WAITING_TO_SUSPEND         */
                                /* 0111 - UM_UL_SUSPENDED                  */
                                /* 1000 - AM_UL_RESET_PENDING              */
} rlc_log_ul_state_type;

/*---------------------------------------------------------------------------
  Data Structure for RLC DL states for Transparent, Unacknowledged and 
  Acknowledged Modes.
---------------------------------------------------------------------------*/
#ifdef __GNUC__
typedef PACKED struct PACKED_POST
#else /* __GNUC__*/
typedef PACKED struct
#endif /* __GNUC__ */
{
  uint8    rlc_dl_id;           /* DL logical channel id (0..18)           */
  uint8    rlc_state;           /* State of the RLC State Machine          */
                                /* 0000 - DL_NULL_STATE                    */
                                /* 0001 - TM_DL_DATA_TRANSFER_READY        */
                                /* 0010 - UM_DL_DATA_TRANSFER_READY        */
                                /* 0011 - AM_DL_DATA_TRANSFER_READY        */
                                /* 0100 - AM_DL_RESET_PENDING              */
} rlc_log_dl_state_type;

/*---------------------------------------------------------------------------
  Data structure for RLC Uplink Transparent Mode.
---------------------------------------------------------------------------*/
#ifdef __GNUC__
typedef PACKED struct PACKED_POST
#else /* __GNUC__*/
typedef PACKED struct 
#endif /* __GNUC__ */ 
{
  uint8 log_chan_type;          /* Type of TM UL Logical Channel           */
                                /* RLC_CCCH                                */
                                /* RLC_DCCH                                */
                                /* RLC_DTCH                                */
  uint8 rb_id;                  /* UL Logical Channel RB ID            */
  uint8 log_chan_ul_rlc_id;     /* UL Logical Channel RLC ID(0..18)        */
  uint8 time_disc;              /* Elapsed time in msec before an SDU is   */
                                /* discarded at the transmitter.Values:    */
                                /* 10, 20, 30, 40, 50, 60, 70, 80, 90, 100 */
  boolean segm;                 /* Segmentation is allowed or not.         */
                                /* 0 - FALSE                               */
                                /* 1 - TRUE                                */
}rlc_log_ul_tm_type;


/*---------------------------------------------------------------------------
  Data structure for RLC Downlink Transparent Mode.
---------------------------------------------------------------------------*/
#ifdef __GNUC__
typedef PACKED struct PACKED_POST
#else /* __GNUC__*/
typedef PACKED struct 
#endif /* __GNUC__ */ 
{
  uint8 log_chan_type;          /* Type of TM DL Logical Channel           */
                                /* RLC_BCCH                                */
                                /* RLC_PCCH                                */
                                /* RLC_DCCH                                */
                                /* RLC_DTCH                                */
  uint8 rb_id;                  /* DL Logical Channel RB ID            */
  uint8 log_chan_dl_rlc_id;     /* DL Logical Channel RLC ID(0..18)        */
  boolean segm;                 /* Segmentation is allowed or not.         */
                                /* 0 - FALSE                               */
                                /* 1 - TRUE                                */
}rlc_log_dl_tm_type;


/*---------------------------------------------------------------------------
  Data structure for RLC Uplink Unacknowledged Mode.
---------------------------------------------------------------------------*/
#ifdef __GNUC__
typedef PACKED struct PACKED_POST
#else /* __GNUC__*/
typedef PACKED struct 
#endif /* __GNUC__ */ 
{
  uint8  log_chan_type;         /* Type of UM UL Logical Channel           */
                                /* RLC_DCCH                                */
                                /* RLC_DTCH                                */
  uint8  rb_id;                 /* UL Logical Channel RB ID                                       */ 
  uint8  log_chan_ul_rlc_id;    /* UL Logical Channel RLC ID(0..18)        */
  uint8  time_disc;             /* Elapsed time in msec before an SDU      */
                                /* is discarded at the transmitter. Values:*/
                                /* 10, 20, 30, 40, 50, 60, 70, 80, 90, 100 */
  uint8  li_length;             /* Size of the length indicator in UMD     */
                                /* PDU - 7 or 15 bits.                     */
  uint32 hfn;                   /* Hyperframe Number - 20 bits             */
  boolean est_or_reest;         /* If Est/Re-est took place or not         */
}rlc_log_ul_um_type;


/*---------------------------------------------------------------------------
  Data structure for RLC Downlink Unacknowledged Mode.
---------------------------------------------------------------------------*/
#ifdef __GNUC__
typedef PACKED struct PACKED_POST
#else /* __GNUC__*/
typedef PACKED struct 
#endif /* __GNUC__ */ 
{
  uint8  log_chan_type;         /* Type of UM DL Logical Channel           */
                                /* RLC_CCCH                                */
                                /* RLC_DCCH                                */
                                /* RLC_CTCH                                */
                                /* RLC_DTCH                                */
  uint8 rb_id;                  /* DL Logical Channel RB ID            */
  uint8  log_chan_dl_rlc_id;    /* DL Logical Channel RLC ID(0..18)        */
  uint8  li_length;             /* Size of the length indicator in UMD     */
                                /* PDU - 7 or 15 bits.                     */
  uint32 hfn;                   /* Hyperframe Number - 20 bits             */
  boolean est_or_reest;         /* If Est/Re-est took place or not         */
}rlc_log_dl_um_type;

/*---------------------------------------------------------------------------
  Data structure for RLC Uplink Acknowledged Mode.
---------------------------------------------------------------------------*/
#ifdef __GNUC__
typedef PACKED struct PACKED_POST
#else /* __GNUC__*/
typedef PACKED struct 
#endif /* __GNUC__ */ 
{
  uint8  log_data_ul_rlc_id;    /* Data Logical Channel UL RLC ID (0..18)  */
  uint8  log_cont_ul_rlc_id;    /* Control Logical Channel UL RLC ID(0..18)*/
  uint16 rb_id;                 /* Radio Bearer ID                         */
  uint8  log_chan_type;         /* Logical Channel Type                    */
                                /* RLC_DCCH                                */
                                /* RLC_DTCH                                */
  uint8  li_length;             /* Size of the length indicator field      */
                                /* in the AMD PDU, 7 bits or 15 bits.      */
  uint16 trans_wind_size;       /* Max number of RLC PUs permitted to      */
                                /* be sent without receiving an ACK,       */
                                /* Max. value 4095 , but only 15 values.   */
  uint16 tmr_rst;               /* Timer for detecting loss of RESET       */
                                /* ACK PDU. Max. value 1000, but only 16   */
                                /* values.                                 */
  uint32 hfn;                   /* Hyperframe Number - 20 bits.            */
  uint8  max_rst;               /* Maximum number of Retransmissions of    */
                                /* RESET PDU. Values: 1, 4, 6, 8, 12, 16,  */
                                /* 24, 32                                  */
  rlc_discard_mode_e_type 
        discard_mode;           /* SDU_DISCARD: RLC_DISCARD_MODE_TBE       */
                                /*              RLC_DISCARD_MODE_MAXDAT    */
                                /*              RLC_DISCARD_MODE_NO_DISCARD*/
  uint32 tmr_discard;           /* Elapsed time in ms before an SDU is     */
                                /* discarded. Max value 7500, but only 16  */
                                /* values.                                 */
  uint16 tmr_mrw;               /* Used to trigger the retransmission of a */
                                /* status PDU containing an MRW SUFI field.*/ 
                                /* Max value 1000, but only 16 values.     */
  uint8 max_dat;                /* Number of retx. of a PU before the SDU  */
                                /* to which it belongs is discarded.Max    */
                                /* value is 40, but only 16 values.        */
  uint8 max_mrw;                /* Max number of retx. of the MRW command, */
                                /* Values - 1, 4, 6, 8, 12, 24, 32         */
  uint16 tmr_poll_proh;         /* Minimum time between polls in ms(0      */
                                /* disabled).                              */
  uint16 tmr_poll;              /* Poll retx timeout in ms.                */
                                /* Max value 1000, but only 17 values      */
  uint8 poll_pdu;                /* Interval between polls in units of PUs  */
                                /* (0= POLL_PU disabled)                   */
                                /* Values - 0, 1, 2, 4, 8, 16, 32, 64, 128 */
  uint8 poll_sdu;               /* POLL_SDU - Interval between polls       */
                                /* in units of SDUs                        */
                                /* 0 - POll_SDU is disabled, 1, 4, 16, 64  */
  boolean last_tx_poll;         /* Last_tx_poll [3]- Indicates if a poll   */
                                /* should be sent when the last PU in the  */
                                /* tx buffer is transmitted.               */
                                /*    0 - Off                              */
                                /*    1 - On                               */
  boolean last_retx_poll;       /* Last_retx_poll [4]- Indicates if a poll */
                                /* should be sent when the last PU in the  */
                                /* retx buffer is transmitted.             */
                                /*    0 - Off                              */
                                /*    1 - On                               */                    
  uint8 poll_win;               /* poll_win : When a poll_win % of the     */
                                /* Tx window is reached, a poll should be  */
                                /* transmitted. Values: 0, 50, 60, 70, 80, */
                                /* 85, 90, 95, 100                         */
  uint16 timer_poll_periodic;   /* Interval between polls  in ms for       */
                                /* periodic polling. Values: 0, 100, 200,  */
                                /* 300, 400, 500, 750, 1000, 2000          */
  boolean est_or_reest;         /* If Est/Re-est took place or not         */
  boolean sp_he;                /* Special HE is configured or not         */
  boolean one_sided_reest;      /* One sided Re-establishment flag         */
} rlc_log_ul_am_type;


/*---------------------------------------------------------------------------
  Data structure for RLC Downlink Acknowledged Mode.
---------------------------------------------------------------------------*/
#ifdef __GNUC__
typedef PACKED struct PACKED_POST
#else /* __GNUC__*/
typedef PACKED struct 
#endif /* __GNUC__ */ 
{
  uint8  log_data_dl_rlc_id;    /* Data Logical Channel DL RLC ID(0..18)   */
  uint8  log_cont_dl_rlc_id;    /* Control Logical Channel DL RLC ID(0..18)*/
  uint16 rb_id;                 /* Radio Bearer ID                         */
  uint8  log_chan_type;         /* Logical Channel Type                    */
                                /* RLC_DCCH                                */
                                /* RLC_DTCH                                */
  uint8  li_length;             /* Size of the length indicator field      */
                                /* in the AMD PDU, 7 bits or 7 bits        */
  uint16 rcv_wind_size;         /* Max number of RLC PUs allowed to        */
                                /* be received without receiving an ACK,   */
                                /* integer max 4095 - 14 values            */
  uint16 tmr_epc;               /* Timer to wait in ms before the estimated*/ 
                                /* PDU counter starts to decrement.        */
                                /* Max value - 1000, 17 values             */
  uint32 hfn;                   /* Hyperframe Number - 20 bits             */
  uint16 tmr_period;            /* Time between STATUS reports in ms for   */
                                /* periodic status reporting.              */
                                /* Max - 1200, 8 values.                   */
  boolean inseq_delivery;       /* Indicates if RLC should                 */
                                /* preserve the order of higher layer PDUs.*/
                                /*   0 - False                             */
                                /*   1 - True                              */
  uint16 status_prohibit;       /* Minimum time between STATUS reports     */
                                /* in ms. Values: 0, 160, 320, 640, 1280.  */
  boolean missing_pdu_ind;      /* Indicates if the UE should send a       */
                                /* STATUS report for each missing PU that  */
                                /* is detected.                            */
                                /*   0 - False                             */
                                /*   1 - true                              */
  boolean est_or_reest;         /* If Est/Re-est took place or not         */
  boolean sp_he;                /* Special HE is configured or not         */

} rlc_log_dl_am_type;

/*---------------------------------------------------------------------------
  Data structure for logging RLC Uplink Acknowledged Mode Statistics
---------------------------------------------------------------------------*/
#ifdef __GNUC__
typedef PACKED struct PACKED_POST
#else /* __GNUC__*/
typedef PACKED struct 
#endif /* __GNUC__ */ 
{
  uint8  log_data_ul_rlc_id;      /* Data Logic Channel UL RLC ID(0..18)   */
  uint8  log_ctl_ul_rlc_id;       /* Control Logic Channel UL RLC ID(0..18)*/
  uint8  log_chan_type;           /* Logical Channel Type (-1..5)          */
  uint16 tot_num_rlc_resets;      /* Total number of rlc resets            */
  uint32 tot_num_pdu_byte_txd;    /* Total number of PDU bytes Txd         */
  uint32 tot_num_sdu_bytes_txd;   /* Total number of SDU bytes Rxd from    */
                                  /* upper layer                           */
  uint16 tot_num_data_pdu_txd;    /* total number of AMD PDUs transmitted  */
  uint16 tot_num_data_pdu_retxd;  /* Total number of AMD PDUs retransmitted*/
  uint16 tot_num_pdu_nak;         /* Total number of AMD PDUs nakd to NW   */
  uint16 tot_num_ctl_pdu_txd;     /* Total number of control PDUs Txd      */
  uint16 sn_last_pdu_txd;         /* Sequence number of last PDU Txd. VT_S */
                                  /* 0..4096                               */
  uint16 sn_last_inseq_pdu_ackd;  /* Sequence number of the last insequence*/
                                  /* pdu acked by the peer entity.  VT_A   */
                                  /* 0..4096                               */
  uint16 tx_win_size;             /* Transmission window size. VT_MS       */
                                  /* 1..4095                               */
  uint16 ul_am_buf_status;        /* Snap shot of the AM buffer occupancy  */
  uint16 tot_num_pdus_with_two_retrans; /* Total number of retrans for     */
  uint16 tot_num_pdus_with_three_retrans; /* pdus which have more than 2   */
  uint16 tot_num_pdus_with_four_retrans;  /* dat counts. Number of PDUs    */
  uint16 tot_num_pdus_with_five_retrans; /* with 2, 3, 4, or >=6 dat counts*/
  uint16 tot_num_pdus_with_six_or_more_retrans; 
} rlc_log_ul_am_stat_type;

/*---------------------------------------------------------------------------
  Data structure for logging RLC Downlink Acknowledged Mode Statistics
---------------------------------------------------------------------------*/
#ifdef __GNUC__
typedef PACKED struct PACKED_POST
#else /* __GNUC__*/
typedef PACKED struct 
#endif /* __GNUC__ */ 
{
  uint8  log_data_dl_rlc_id;      /* Data Logic Channel DL RLC ID(0..18)   */
  uint8  log_ctl_dl_rlc_id;       /* Control Logic Channel DL RLC ID (0-18)*/
  uint8  log_chan_type;           /* Logical Channel Type (-1..5)          */
  uint32 tot_num_pdu_byte_rxd;    /* Total number of PDU bytes received    */
  uint32 tot_num_sdu_byte_rxd;    /* Total number of SDU bytes sends to    */
                                  /* the upper layer.                      */
  uint16 tot_num_error_pdu_rxd;   /* Erroneous PDUs Rx'd                   */
  uint16 tot_num_data_pdu_rxd;    /* Total number of AMD PDUs received     */
  uint16 tot_num_ctl_pdu_rxd;     /* Total number of Control PDUs rx'd     */
  uint16 tot_num_pdu_nak;         /* Total number of AMD PDUs NAK by the   */
                                  /* peer entity.                          */
  uint16 sn_last_inseq_pdu_rxd;   /* Sequence number of last in sequence   */
                                  /* PDU received. VR(R)                   */
                                  /* 0..4096                               */
  uint16 highest_sn_pdu_rxd;      /* Highest seq number PDU Rxd VR_H       */
                                  /* 0..4096                               */
} rlc_log_dl_am_stat_type;

/*---------------------------------------------------------------------------
  Data structure for logging UL RLC PDU information
---------------------------------------------------------------------------*/

#ifdef __GNUC__
typedef PACKED struct PACKED_POST
#else /* __GNUC__*/
typedef PACKED struct 
#endif /* __GNUC__ */
{
  uint8  log_data_ul_rlc_id;      /* Data Logical Channel UL RLC ID(0..18) */
  uint16 num_pdu;                 /* Number of PDUs logged                 */
  uint16 pdu_size;                /* PDU size in octets of each PDU        */
  uint32 log_data[MAX_UL_RLC_LOG_PDUS];
                                  /* The first four bytes of the pdu       */
} rlc_log_ul_am_pdu_data_type;

/*---------------------------------------------------------------------------
  Data structure for logging DL RLC PDU information
---------------------------------------------------------------------------*/

#ifdef __GNUC__
typedef PACKED struct PACKED_POST
#else /* __GNUC__*/
typedef PACKED struct 
#endif /* __GNUC__ */
{
  uint8  log_data_dl_rlc_id;      /* Data Logical Channel DL RLC ID(0..18) */
  uint16 num_pdu;                 /* Number of PDUs logged                 */
  uint16 pdu_size;                /* PDU size in octets of each PDU        */
  uint32 log_data[MAX_DL_RLC_LOG_PDUS];
                                  /* The first four bytes of the pdu       */
} rlc_log_dl_am_pdu_data_type;

#if ((defined (FEATURE_HSDPA_MAC_EHS)) && (defined (FEATURE_WCDMA_REL7)))
/*---------------------------------------------------------------------------
  Data structure for logging Flexible DL RLC PDU logging and Size
---------------------------------------------------------------------------*/
#ifdef __GNUC__
typedef PACKED struct PACKED_POST
#else /* __GNUC__*/
typedef PACKED struct 
#endif /* __GNUC__ */
{
  uint16 pdu_size;                /* PDU size in octets of each PDU        */
  uint32 log_data;                /* The first four bytes of the pdu       */
} rlc_log_flex_dl_am_pdu_info_type;

/*---------------------------------------------------------------------------
  Data structure for logging Flexible DL RLC PDU information
---------------------------------------------------------------------------*/

#ifdef __GNUC__
typedef PACKED struct PACKED_POST
#else /* __GNUC__*/
typedef PACKED struct 
#endif /* __GNUC__ */
{
  uint8  log_data_dl_rlc_id;      /* Data Logical Channel DL RLC ID        */
  uint8  num_pdu;                 /* Number of PDUs logged                 */
  uint8 li_length;                /* Length of the length indicator        */
                                  /* 1 - 7 bits, 2 - 15 bits               */
} rlc_log_flex_dl_am_pdu_data_type;
#endif /* ((defined (FEATURE_HSDPA_MAC_EHS)) && (defined (FEATURE_WCDMA_REL7))) */

#ifdef FEATURE_MAC_I
/*---------------------------------------------------------------------------
  Data structure for logging Flexible DL RLC PDU logging and Size
---------------------------------------------------------------------------*/
#ifdef __GNUC__
typedef PACKED struct PACKED_POST
#else /* __GNUC__*/
typedef PACKED struct 
#endif /* __GNUC__ */
{
  uint16 pdu_size;                /* PDU size in octets of each PDU        */
  uint32 log_data;                /* The first four bytes of the pdu       */
  uint32  count_c_rlc_log;        /* Count - Ciphering HFN + RLC      */
} rlc_log_flex_ul_pdu_info_type;

/*---------------------------------------------------------------------------
  Data structure for logging Flexible DL RLC PDU information
---------------------------------------------------------------------------*/

#ifdef __GNUC__
typedef PACKED struct PACKED_POST
#else /* __GNUC__*/
typedef PACKED struct 
#endif /* __GNUC__ */
{
  uint8  log_data_ul_rlc_id;      /* Data Logical Channel DL RLC ID        */
  uint8  num_pdu_li_length;       /* num_pdu: 6 bits, li_length: 2 bits    */
  uint8 ciph_mode_ID_algo_rlc_mode;
  uint16  act_sn;                      /* SN to start ciphering            */
  uint8 new_tx_bo[3];
  uint8 re_tx_bo[3];
  uint16 ctrl_data_bo;
  uint8 num_sdus;
} rlc_log_flex_ul_pdu_data_type;
#endif 

/*---------------------------------------------------------------------------
  Data structure for logging UL RLC AM NAK PDU information
---------------------------------------------------------------------------*/

#ifdef __GNUC__
typedef PACKED struct PACKED_POST
#else /* __GNUC__*/
typedef PACKED struct 
#endif /* __GNUC__ */
{
  uint8  log_ctl_ul_rlc_id;       /* Data Logical Channel UL RLC ID(0..18) */
  uint16 pdu_size;                /* PDU size in octets of each PDU        */
  uint8  log_data[MAX_RLC_LOG_PDUS_SIZE];
                                  /* Dump of the status PDU                */
} rlc_log_ul_am_nak_pdu_type;

/*---------------------------------------------------------------------------
  Data structure for logging UL RLC AM NAK PDU information
---------------------------------------------------------------------------*/
#ifdef __GNUC__
typedef PACKED struct PACKED_POST
#else /* __GNUC__*/
typedef PACKED struct 
#endif /* __GNUC__ */
{
  uint8  log_ctl_dl_rlc_id;       /* Data Logical Channel UL RLC ID(0..18) */
  uint16 pdu_size;                /* PDU size in octets of each PDU        */
  uint8  log_data[MAX_RLC_LOG_PDUS_SIZE];
                                  /* Dump of the status PDU                */
} rlc_log_dl_am_nak_pdu_type;

/*---------------------------------------------------------------------------
  Data structure for logging UL RLC PDU Ciphering information
---------------------------------------------------------------------------*/
#ifdef __GNUC__
typedef PACKED struct PACKED_POST
#else /* __GNUC__*/
typedef PACKED struct 
#endif /* __GNUC__ */
{
  uint8  log_ul_rlc_id;           /* Data Logical Channel UL RLC ID(0..18) */
  uint32 cipher_key_idx;
  rlc_cipher_algo_e_type cipher_algo;
  uint32 count_c;                 /* PDU size in octets of each PDU        */
                                  /* Dump of the status PDU                */
} rlc_log_ul_pdu_cipher_info_type;

/*---------------------------------------------------------------------------
  Data structure for logging DL RLC PDU Ciphering information
---------------------------------------------------------------------------*/
#ifdef __GNUC__
typedef PACKED struct PACKED_POST
#else /* __GNUC__*/
typedef PACKED struct 
#endif /* __GNUC__ */
{
  uint8  log_dl_rlc_id;           /* Data Logical Channel UL RLC ID(0..18) */
  uint32 cipher_key_idx;
  rlc_cipher_algo_e_type cipher_algo;
  uint32 b4_cipher;               /* The 1st 4 bytes of the PDU before     */
                                  /* decipher.                             */
  uint32 count_c;                 /* PDU size in octets of each PDU        */
                                  /* Dump of the status PDU                */
} rlc_log_dl_pdu_cipher_info_type;

#ifdef FEATURE_UM_PDU_LOGGING
/*---------------------------------------------------------------------------
  Data structure for logging DL RLC UM PDU 
---------------------------------------------------------------------------*/
#ifdef __GNUC__
typedef PACKED struct PACKED_POST
#else /* __GNUC__*/
typedef PACKED struct 
#endif /* __GNUC__ */
{
  uint8  log_data_dl_rlc_id;      /* Data Logical Channel DL RLC ID(0..18) */
  uint8 num_pdu;                 /* Number of PDUs logged                 */
  uint16 pdu_size;                /* PDU size in octets of each PDU        */
  uint32 log_data[MAX_DL_RLC_LOG_PDUS];
                                  /* The first four bytes of the pdu       */
} rlc_log_dl_um_pdu_data_type;

/*---------------------------------------------------------------------------
  Data structure for logging UL UM RLC PDU 
---------------------------------------------------------------------------*/
#ifdef __GNUC__
typedef PACKED struct PACKED_POST
#else /* __GNUC__*/
typedef PACKED struct 
#endif /* __GNUC__ */ 
{
  uint8  log_data_ul_rlc_id;      /* Data Logical Channel UL RLC ID(0..18) */
  uint8 num_pdu;                 /* Number of PDUs logged                 */
  uint16 pdu_size;                /* PDU size in octets of each PDU        */
  uint32 log_data[MAX_UL_RLC_LOG_PDUS];
                                  /* The first four bytes of the pdu       */
} rlc_log_ul_um_pdu_data_type;

/*---------------------------------------------------------------------------
  Adding this structure for dynamic UM PDU logging. we will give the size of this tructure to the 
  allocate memeory on DIAG side.
---------------------------------------------------------------------------*/
#ifdef __GNUC__
typedef PACKED struct PACKED_POST
#else /* __GNUC__*/
typedef PACKED struct 
#endif /* __GNUC__ */ 
{
  uint8 num_um_entity;
  uint8  log_data_rlc_id;      /* Data Logical Channel UL RLC ID(0..18) */
  uint8 num_pdu;                 /* Number of PDUs logged                 */
  uint16 pdu_size;                /* PDU size in octets of each PDU        */
}rlc_um_log_header_type ;
#endif

/*---------------------------------------------------------------------------
  Adding this structure for dynamic AM PDU logging. we will give the size of this tructure to the 
  allocate memeory on DIAG side.
---------------------------------------------------------------------------*/
#ifdef __GNUC__
typedef PACKED struct PACKED_POST
#else /* __GNUC__*/
typedef PACKED struct 
#endif /* __GNUC__ */
{
  uint8  num_am_entity;
  uint8  log_data_rlc_id;      /* Data Logical Channel UL RLC ID(0..18) */
  uint16 num_pdu;                 /* Number of PDUs logged                 */
  uint16 pdu_size;                /* PDU size in octets of each PDU        */
}rlc_am_log_header_type ;

#ifdef FEATURE_WCDMA_UL_COMPR
/*---------------------------------------------------------------------------
  Datastructure for logging ULC log packet (per sample elements)
---------------------------------------------------------------------------*/
#ifdef __GNUC__
typedef PACKED struct PACKED_POST
#else /* __GNUC__*/
typedef PACKED struct 
#endif /* __GNUC__ */
{
  uint8 cfn;                 /*CFN when ISR fired*/
  uint8 comp_isr_late;       /*No of BPGs compression ISR arrived late */
  uint16 bytes_comp;         /* No of bytes compressed during current ISR */
  uint16 bytes_uncomp;       /* No of uncompressed bytes used */
  uint8 num_sdu_comp_uncomp; /* No of higher layer SDUs compressed in current ISR(3 bits)
                                                       No of uncompressed SDUs pumped in current ISR (5 bits)*/
  uint16 ticks_bo_limits;    /* Ticks usage per compression ISR(6 bits)
                                                       Cumulative compression Rate(7 bits) 
                                                       ISR exited due to limits 0-None, 1-COMP/UNCOMP limit, 2-MAX BPG limit,
                                                       3-DSM limit, 4-FC limit(3 bits)*/
  uint32 wm_byte_cnt;        /* No of bytes of outstanding data in WM Q*/  
  uint16 comp_q_byte_cnt;    /* No of bytes of outstanding data in UL COMP Q*/  
}rlc_ul_comp_log_info_type ;
#endif

/*---------------------------------------------------------------------------
  Logging parameters for RLC PDU logging. RLCLOG_SIG_PDU indicates
  indicates the signaling plane PDUs, RLCLOG_USR _PDU indicates the
  user plane PDUs and RLCLOG_MAX indicates the size of array for storing
  the logging parameters.
---------------------------------------------------------------------------*/
typedef enum
{
    RLCLOG_SIG_PDU = 0,
    RLCLOG_USR_PDU = 1,
    RLCLOG_MAX
}rlc_log_pdu_enum_type;


#define RLCLOG_TYPE_IDX 0
#define RLCLOG_CH_IDX   1


/*---------------------------------------------------------------------------
                   LOG PACKET: WCDMA_UL_RLC_V1_CIPH_CFG_LOG_PACKET
 --------------------------------------------------------------------------*/

LOG_RECORD_DEFINE(WCDMA_UL_RLC_V1_CIPH_CFG_LOG_PACKET)
    
  /*-------------------------------------------------------------------------
    Number of UM and AM uplink logical channels to configure for ciphering. 
  -------------------------------------------------------------------------*/
  uint8 num_ciphlog_chan_ul;

  /*-------------------------------------------------------------------------
    Ciphering Information for each UM and AM logical channel. 
  -------------------------------------------------------------------------*/
  rlc_log_ul_ciphering_type ul_ciphering[UE_MAX_UL_UM_CHANNEL + UE_MAX_AM_ENTITY];
LOG_RECORD_END

/*---------------------------------------------------------------------------
                   LOG PACKET: WCDMA_UL_RLC_STATE_LOG_PACKET
---------------------------------------------------------------------------*/

LOG_RECORD_DEFINE(WCDMA_UL_RLC_STATE_LOG_PACKET)
    
  /*-------------------------------------------------------------------------
    Number of Uplink logical channels. 
  -------------------------------------------------------------------------*/
  uint8 num_log_chan_ul;

  /*-------------------------------------------------------------------------
   State Information for each Uplink logical channel. 
  -------------------------------------------------------------------------*/
  rlc_log_ul_state_type ul_state[UE_MAX_UL_LOGICAL_CHANNEL];                                                 

LOG_RECORD_END

/*---------------------------------------------------------------------------
                   LOG PACKET: WCDMA_UL_RLC_TM_LOG_PACKET
---------------------------------------------------------------------------*/

LOG_RECORD_DEFINE(WCDMA_UL_RLC_TM_LOG_PACKET)
    
  uint8 version;

  /*-------------------------------------------------------------------------
    Number of Uplink Transparent Mode logical channels(1..8).
  -------------------------------------------------------------------------*/
  uint8 num_log_chan_tm_ul;

  /*-------------------------------------------------------------------------
   Information for each Uplink Transparent Mode logical channel. 
  -------------------------------------------------------------------------*/
  rlc_log_ul_tm_type rlc_ul_tm[UE_MAX_UL_TM_CHANNEL];                                                

LOG_RECORD_END

/*---------------------------------------------------------------------------
                   LOG PACKET: WCDMA_UL_RLC_UM_LOG_PACKET
---------------------------------------------------------------------------*/

LOG_RECORD_DEFINE(WCDMA_UL_RLC_UM_LOG_PACKET)
    
  uint8 version;
  /*-------------------------------------------------------------------------
    Number of Uplink Unacknowledged Mode logical channels(1..3).
  -------------------------------------------------------------------------*/
  uint8 num_log_chan_um_ul;

  /*-------------------------------------------------------------------------
    Information for each Uplink Unacknowledged Mode logical channel. 
  -------------------------------------------------------------------------*/
  rlc_log_ul_um_type rlc_ul_um[UE_MAX_UL_UM_CHANNEL];                                                

LOG_RECORD_END

/*---------------------------------------------------------------------------
                   LOG PACKET: WCDMA_UL_RLC_V1_AM_LOG_PACKET
---------------------------------------------------------------------------*/

LOG_RECORD_DEFINE(WCDMA_UL_RLC_V1_AM_LOG_PACKET)
    
  uint8 version;
  /*-------------------------------------------------------------------------
    Number of Uplink Acknowledged Mode entities (1..4).
  -------------------------------------------------------------------------*/
  uint8 num_am_entity_ul;

  /*-------------------------------------------------------------------------
    For each RLC AM Uplink entity 
  -------------------------------------------------------------------------*/
  rlc_log_ul_am_type rlc_ul_am[UE_MAX_AM_ENTITY];

LOG_RECORD_END

/*---------------------------------------------------------------------------
                   LOG PACKET: WCDMA_DL_RLC_V1_CIPH_CFG_LOG_PACKET
---------------------------------------------------------------------------*/


LOG_RECORD_DEFINE(WCDMA_DL_RLC_V1_CIPH_CFG_LOG_PACKET)
    
  /*-------------------------------------------------------------------------
    Number of UM and AM downlink logical channels to configure for
    ciphering. 
  -------------------------------------------------------------------------*/
  uint8 num_ciphlog_chan_dl;

  /*-------------------------------------------------------------------------
    Ciphering Information for each UM and AM downlink logical channel. 
  -------------------------------------------------------------------------*/
  rlc_log_dl_ciphering_type dl_ciphering[UE_MAX_DL_UM_CHANNEL + UE_MAX_AM_ENTITY];

LOG_RECORD_END

/*---------------------------------------------------------------------------
                   LOG PACKET: WCDMA_DL_RLC_STATE_LOG_PACKET
 --------------------------------------------------------------------------*/

LOG_RECORD_DEFINE(WCDMA_DL_RLC_STATE_LOG_PACKET)
    
  /*-------------------------------------------------------------------------
    Number of Downlink logical channels. 
  -------------------------------------------------------------------------*/
  uint8 num_log_chan_dl;

  /*-------------------------------------------------------------------------
    State Information for each Downlink logical channel. 
  -------------------------------------------------------------------------*/
  rlc_log_dl_state_type dl_state[UE_MAX_DL_LOGICAL_CHANNEL];                                                 

LOG_RECORD_END

/*---------------------------------------------------------------------------
                   LOG PACKET: WCDMA_DL_RLC_TM_LOG_PACKET
---------------------------------------------------------------------------*/

LOG_RECORD_DEFINE(WCDMA_DL_RLC_TM_LOG_PACKET)
    
  uint8 version;
  /*-------------------------------------------------------------------------
    Number of Downlink Transparent Mode logical channels(1..8).
  -------------------------------------------------------------------------*/
  uint8 num_log_chan_tm_dl;

  /*-------------------------------------------------------------------------
    Information for each Downlink Transparent Mode logical channel. 
  -------------------------------------------------------------------------*/
  rlc_log_dl_tm_type rlc_dl_tm[UE_MAX_DL_TM_CHANNEL];                                                

LOG_RECORD_END

/*---------------------------------------------------------------------------
                   LOG PACKET: WCDMA_DL_RLC_UM_LOG_PACKET
 --------------------------------------------------------------------------*/
LOG_RECORD_DEFINE(WCDMA_DL_RLC_UM_LOG_PACKET)
    
  uint8 version;    
  /*-------------------------------------------------------------------------
    Number of Downlink Unacknowledged Mode logical channels(1..3).
  -------------------------------------------------------------------------*/
  uint8 num_log_chan_um_dl;

  /*-------------------------------------------------------------------------
    Information for each Downlink Unacknowledged Mode logical channel. 
  -------------------------------------------------------------------------*/
  rlc_log_dl_um_type rlc_dl_um[UE_MAX_DL_UM_CHANNEL];                                                

LOG_RECORD_END

/*---------------------------------------------------------------------------
                   LOG PACKET: WCDMA_DL_RLC_V1_AM_LOG_PACKET
 --------------------------------------------------------------------------*/

LOG_RECORD_DEFINE(WCDMA_DL_RLC_V1_AM_LOG_PACKET)
    
  uint8 version;
  /*-------------------------------------------------------------------------
    Number of Downlink Acknowledged Mode entities (1..4).
  -------------------------------------------------------------------------*/
  uint8 num_am_entity_dl;

  /*-------------------------------------------------------------------------
    For each RLC AM Downlink entity. 
  -------------------------------------------------------------------------*/
  rlc_log_dl_am_type rlc_dl_am[UE_MAX_AM_ENTITY];

LOG_RECORD_END

/*---------------------------------------------------------------------------
                   LOG PACKET: WCDMA_UL_RLC_V2_AM_STAT_LOG_PACKET
 --------------------------------------------------------------------------*/

LOG_RECORD_DEFINE(WCDMA_UL_RLC_V2_AM_STAT_LOG_PACKET)
    
  /*-------------------------------------------------------------------------
    Number of Uplink Acknowledged Mode entities (1..4).
  -------------------------------------------------------------------------*/
  uint8 num_am_entity_ul;

  /*-------------------------------------------------------------------------
    For each RLC AM Uplink entity. 
  -------------------------------------------------------------------------*/
  rlc_log_ul_am_stat_type     rlc_ul_am_stat[UE_MAX_AM_ENTITY];

LOG_RECORD_END

/*---------------------------------------------------------------------------
                   LOG PACKET: WCDMA_DL_RLC_AM_STAT_LOG_PACKET
 --------------------------------------------------------------------------*/

LOG_RECORD_DEFINE(WCDMA_DL_RLC_AM_STAT_LOG_PACKET)
    
  /*-------------------------------------------------------------------------
    Number of Downlink Acknowledged Mode entities (1..4).
  -------------------------------------------------------------------------*/
  uint8 num_am_entity_dl;

  /*-------------------------------------------------------------------------
    For each RLC AM Downlink entity. 
  -------------------------------------------------------------------------*/
  rlc_log_dl_am_stat_type rlc_dl_am_stat[UE_MAX_AM_ENTITY];

LOG_RECORD_END

/*---------------------------------------------------------------------------
  Log Packets 0x4144 and 0x414B used for logging RLC PDUs are now obsolete.
  Two new packets have replaced these log packets 
  RLC_AM_USR_PLANE_PDU_LOG_PACKET for logging user plane PDUs
  and RLC_AM_SIG_PLAN_PDU_LOG_PACKET for logging signalling plane RLC
  PDUs.
---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------
                   LOG PACKET: WCDMA_UL_RLC_AM_USR_PLANE_PDU_LOG_PACKET
 --------------------------------------------------------------------------*/

#ifndef WCDMA_UL_RLC_AM_USR_PLANE_PDU_LOG_PACKET
#ifdef FEATURE_RLC_LOG_MORE_AM_PDUS
  #define WCDMA_UL_RLC_AM_USR_PLANE_PDU_LOG_PACKET   WCDMA_UL_RLC_AM_USR_PLANE_PDU_0x415B_LOG_PACKET 
#else
  #define WCDMA_UL_RLC_AM_USR_PLANE_PDU_LOG_PACKET   WCDMA_UL_RLC_AM_USR_PLANE_PDU_0x413B_LOG_PACKET 
#endif //FEATURE_RLC_LOG_MORE_AM_PDUS
#else
#error redefinition of LOG command code: WCDMA_UL_RLC_AM_USR_PLANE_PDU_LOG_PACKET
#endif

LOG_RECORD_DEFINE(WCDMA_UL_RLC_AM_USR_PLANE_PDU_LOG_PACKET)
    
  /*-------------------------------------------------------------------------
    Number of Uplink Acknowledged Mode entities (1..4).
  -------------------------------------------------------------------------*/
  uint8 num_am_entity_ul;

  /*-------------------------------------------------------------------------
    For each RLC AM Uplink entity. 
  -------------------------------------------------------------------------*/
  rlc_log_ul_am_pdu_data_type ul_am_pdu_data[UE_MAX_AM_ENTITY];

LOG_RECORD_END

#ifdef FEATURE_UM_PDU_LOGGING


LOG_RECORD_DEFINE(WCDMA_UL_RLC_UM_USR_PLANE_PDU_LOG_PACKET)

  /*-------------------------------------------------------------------------
    Number of Uplink Acknowledged Mode entities (1..4).
  -------------------------------------------------------------------------*/
  uint8 num_um_entity_ul;

  /*-------------------------------------------------------------------------
    For each RLC AM Uplink entity.
  -------------------------------------------------------------------------*/
  rlc_log_ul_um_pdu_data_type ul_um_pdu_data[UE_MAX_UL_UM_CHANNEL];

LOG_RECORD_END
#endif

/*---------------------------------------------------------------------------
             LOG PACKET: WCDMA_DL_RLC_AM_USR_PLANE_PDU_LOG_PACKET
 --------------------------------------------------------------------------*/

#ifndef WCDMA_DL_RLC_AM_USR_PLANE_PDU_LOG_PACKET
#ifdef FEATURE_RLC_LOG_MORE_AM_PDUS
  #define WCDMA_DL_RLC_AM_USR_PLANE_PDU_LOG_PACKET   WCDMA_DL_RLC_AM_USR_PLANE_PDU_0x4154_LOG_PACKET
#else
  #define WCDMA_DL_RLC_AM_USR_PLANE_PDU_LOG_PACKET   WCDMA_DL_RLC_AM_USR_PLANE_PDU_0x4134_LOG_PACKET
#endif //FEATURE_RLC_LOG_MORE_AM_PDUS
#else
#error redefinition of LOG command code: WCDMA_DL_RLC_AM_USR_PLANE_PDU_LOG_PACKET
#endif

LOG_RECORD_DEFINE(WCDMA_DL_RLC_AM_USR_PLANE_PDU_LOG_PACKET)
    
  /*-------------------------------------------------------------------------
    Number of Downlink Acknowledged Mode entities (1..4).
  -------------------------------------------------------------------------*/
  uint8 num_am_entity_dl;

  /*-------------------------------------------------------------------------
    For each RLC AM Uplink entity. 
  -------------------------------------------------------------------------*/
  rlc_log_dl_am_pdu_data_type dl_am_pdu_data[UE_MAX_AM_ENTITY];

LOG_RECORD_END

#if ((defined (FEATURE_HSDPA_MAC_EHS)) && (defined (FEATURE_WCDMA_REL7)))

LOG_RECORD_DEFINE(WCDMA_DL_FLEXIBLE_RLC_AM_PDU_LOG_PACKET_V1)

  /* Version number */
  uint8 version_num;

  /*-------------------------------------------------------------------------
    Number of Downlink Acknowledged Mode entities (1..4).
  -------------------------------------------------------------------------*/
  uint8 num_am_entity_dl;

  /*-------------------------------------------------------------------------
    For each RLC AM Uplink entity. 
  -------------------------------------------------------------------------*/
  rlc_log_flex_dl_am_pdu_data_type dl_am_flex_pdu_data[UE_MAX_AM_ENTITY];

  /*-------------------------------------------------------------------------
    For each PDU received - SIZE and DATA information. 
  -------------------------------------------------------------------------*/
  rlc_log_flex_dl_am_pdu_info_type pdu_info[MAX_DL_RLC_LOG_PDUS];

LOG_RECORD_END

#endif /* ((defined (FEATURE_HSDPA_MAC_EHS)) && (defined (FEATURE_WCDMA_REL7))) */

#ifdef FEATURE_MAC_I

LOG_RECORD_DEFINE(WCDMA_UL_FLEXIBLE_RLC_PDU_LOG_PACKET_V1)

  /* Version number */
  uint8 version_num;

  /*-------------------------------------------------------------------------
    Number of Downlink Acknowledged Mode entities (1..4).
  -------------------------------------------------------------------------*/
  uint8 num_entity_ul;

  /*-------------------------------------------------------------------------
    For each Uplink RLC entity. 
  -------------------------------------------------------------------------*/
  rlc_log_flex_ul_pdu_data_type ul_flex_pdu_data[UE_MAX_AM_ENTITY];

  /*-------------------------------------------------------------------------
    For each PDU received - SIZE and DATA information. 
  -------------------------------------------------------------------------*/
  rlc_log_flex_ul_pdu_info_type pdu_info[MAX_UL_RLC_LOG_PDUS];

LOG_RECORD_END

#endif 

#ifdef FEATURE_UM_PDU_LOGGING


LOG_RECORD_DEFINE(WCDMA_DL_RLC_UM_USR_PLANE_PDU_LOG_PACKET)

  /*-------------------------------------------------------------------------
    Number of Downlink Acknowledged Mode entities (1..4).
  -------------------------------------------------------------------------*/
  uint8 num_um_entity_dl;

  /*-------------------------------------------------------------------------
    For each RLC AM Uplink entity.
  -------------------------------------------------------------------------*/
  rlc_log_dl_um_pdu_data_type dl_um_pdu_data[UE_MAX_DL_UM_CHANNEL];

LOG_RECORD_END

#endif
/*---------------------------------------------------------------------------
                   LOG PACKET: WCDMA_UL_RLC_AM_NAK_PDU_LOG_PACKET
 --------------------------------------------------------------------------*/


LOG_RECORD_DEFINE(WCDMA_UL_RLC_AM_NAK_PDU_LOG_PACKET)
    
  /*-------------------------------------------------------------------------
    NAK PDU information.
  -------------------------------------------------------------------------*/
  rlc_log_ul_am_nak_pdu_type ul_am_nak_pdu;

LOG_RECORD_END

/*---------------------------------------------------------------------------
                   LOG PACKET: WCDMA_DL_RLC_AM_NAK_PDU_LOG_PACKET
 --------------------------------------------------------------------------*/


LOG_RECORD_DEFINE(WCDMA_DL_RLC_AM_NAK_PDU_LOG_PACKET)
    
  /*-------------------------------------------------------------------------
    NAK PDU information.
  -------------------------------------------------------------------------*/
  rlc_log_dl_am_nak_pdu_type dl_am_nak_pdu;
  
LOG_RECORD_END

/*---------------------------------------------------------------------------
              LOG PACKET: WCDMA_UL_RLC_AM_SIG_PLANE_PDU_LOG_PACKET
 --------------------------------------------------------------------------*/


LOG_RECORD_DEFINE(WCDMA_UL_RLC_AM_SIG_PLANE_PDU_LOG_PACKET)

  /*-------------------------------------------------------------------------
                   Signalling PDU information
  -------------------------------------------------------------------------*/

  uint8 num_am_entity_ul;

  rlc_log_ul_am_pdu_data_type ul_am_pdu_data[UE_MAX_SRB_AM_EMTITIES_3];

LOG_RECORD_END

  /*-------------------------------------------------------------------------
               LOG PACKET: WCDMA_DL_RLC_AM_SIG_PLANE_PDU_LOG_PACKET
  -------------------------------------------------------------------------*/

 LOG_RECORD_DEFINE(WCDMA_DL_RLC_AM_SIG_PLANE_PDU_LOG_PACKET)

  /*-------------------------------------------------------------------------
                     Signalling PDU information
  -------------------------------------------------------------------------*/

  uint8 num_am_entity_dl;
  
  rlc_log_dl_am_pdu_data_type dl_am_pdu_data[UE_MAX_SRB_AM_EMTITIES_3];


LOG_RECORD_END


/*---------------------------------------------------------------------------
                   LOG PACKET: WCDMA_UL_RLC_V1_PDU_CIPHER_LOG_PACKET
 --------------------------------------------------------------------------*/

LOG_RECORD_DEFINE(WCDMA_UL_RLC_V1_PDU_CIPHER_LOG_PACKET)
    
  /*-------------------------------------------------------------------------
    Number of Uplink Channels.
  -------------------------------------------------------------------------*/
  uint16 num_pdu;

  /*-------------------------------------------------------------------------
    For each RLC AM Uplink entity. 
  -------------------------------------------------------------------------*/
  rlc_log_ul_pdu_cipher_info_type ul_pdu_cipher_info[MAX_CIPHER_PDU];

LOG_RECORD_END

/*---------------------------------------------------------------------------
                   LOG PACKET: WCDMA_DL_RLC_PDU_CIPHER_LOG_PACKET
 --------------------------------------------------------------------------*/

LOG_RECORD_DEFINE(WCDMA_DL_RLC_V1_PDU_CIPHER_LOG_PACKET)
    
  /*-------------------------------------------------------------------------
    Number of Downlink Channels.
  -------------------------------------------------------------------------*/
  uint16 num_pdu;

  /*-------------------------------------------------------------------------
    For each RLC AM Uplink entity. 
  -------------------------------------------------------------------------*/
  rlc_log_dl_pdu_cipher_info_type dl_pdu_cipher_info[MAX_CIPHER_PDU];

LOG_RECORD_END

#ifdef FEATURE_WCDMA_UL_COMPR
/*---------------------------------------------------------------------------
                   LOG PACKET: WCDMA_UL_COMPRESSION_LOG_PACKET
 --------------------------------------------------------------------------*/

LOG_RECORD_DEFINE(WCDMA_RLC_UL_COMPRESSION_LOG_PACKET)

  uint8  version;
  uint8  num_samples;
  uint32 total_isr_misses;
  
  /* For each sample */
  rlc_ul_comp_log_info_type comp_log_info[MAX_ULC_LOG_SAMPLES];

LOG_RECORD_END
#endif

/*---------------------------------------------------------------------------
  Length of Uplink Transparent Mode Configuration Log Packet.
---------------------------------------------------------------------------*/
#define RLC_UL_TM_PACKET_LEN(num_tm) ( \
  FPOS(WCDMA_UL_RLC_TM_LOG_PACKET_type, rlc_ul_tm)+ \
      (num_tm * sizeof(rlc_log_ul_tm_type)) )
      
/*---------------------------------------------------------------------------
  Length of Uplink Unacknowledged Mode Configuration Log Packet.
---------------------------------------------------------------------------*/
#define RLC_UL_UM_PACKET_LEN(num_um) ( \
  FPOS(WCDMA_UL_RLC_UM_LOG_PACKET_type, rlc_ul_um)+ \
      (num_um * sizeof(rlc_log_ul_um_type)) )
      
/*---------------------------------------------------------------------------
  Length of Uplink Acknowledged Mode Configuration Log Packet.
---------------------------------------------------------------------------*/
#define RLC_UL_AM_PACKET_LEN(num_am) ( \
  FPOS(WCDMA_UL_RLC_V1_AM_LOG_PACKET_type, rlc_ul_am)+ \
      (num_am * sizeof(rlc_log_ul_am_type)) )

/*---------------------------------------------------------------------------
  Length of Downlink Transparent Mode Configuration Log Packet.
---------------------------------------------------------------------------*/
#define RLC_DL_TM_PACKET_LEN(num_tm) ( \
  FPOS(WCDMA_DL_RLC_TM_LOG_PACKET_type, rlc_dl_tm)+ \
      (num_tm * sizeof(rlc_log_dl_tm_type)) )
      
/*---------------------------------------------------------------------------
  Length of Downlink Unacknowledged Mode Configuration Log Packet.
---------------------------------------------------------------------------*/
#define RLC_DL_UM_PACKET_LEN(num_um) ( \
  FPOS(WCDMA_DL_RLC_UM_LOG_PACKET_type, rlc_dl_um)+ \
      (num_um * sizeof(rlc_log_dl_um_type)) )
      
/*---------------------------------------------------------------------------
  Length of Downlink Acknowledged Mode Configuration Log Packet.
---------------------------------------------------------------------------*/
#define RLC_DL_AM_PACKET_LEN(num_am) ( \
  FPOS(WCDMA_DL_RLC_V1_AM_LOG_PACKET_type, rlc_dl_am)+ \
      (num_am * sizeof(rlc_log_dl_am_type)) )

/*---------------------------------------------------------------------------
  Length of Uplink State Log Packet.
---------------------------------------------------------------------------*/
#define RLC_UL_STATE_PACKET_LEN(num_chan) ( \
  FPOS(WCDMA_UL_RLC_STATE_LOG_PACKET_type, ul_state)+ \
      (num_chan * sizeof(rlc_log_ul_state_type)) )

/*---------------------------------------------------------------------------
  Length of Downlink State Log Packet.
---------------------------------------------------------------------------*/
#define RLC_DL_STATE_PACKET_LEN(num_chan) ( \
  FPOS(WCDMA_DL_RLC_STATE_LOG_PACKET_type, dl_state)+ \
      (num_chan * sizeof(rlc_log_dl_state_type)) )
      
/*---------------------------------------------------------------------------
  Length of Downlink AM Statistics packet.
---------------------------------------------------------------------------*/
#define RLC_DL_AM_STAT_PACKET_LEN(num_chan) ( \
  FPOS(WCDMA_DL_RLC_AM_STAT_LOG_PACKET_type, rlc_dl_am_stat)+ \
      (num_chan * sizeof(rlc_log_dl_am_stat_type)) )

/*---------------------------------------------------------------------------
  Length of Uplink AM Statistics packet V2.
---------------------------------------------------------------------------*/
#define RLC_UL_V2_AM_STAT_PACKET_LEN ( \
  FPOS(WCDMA_UL_RLC_V2_AM_STAT_LOG_PACKET_type, rlc_ul_am_stat)+ \
   (UE_MAX_AM_ENTITY * sizeof(rlc_log_ul_am_stat_type)) );
   
/*---------------------------------------------------------------------------
  Length of UL AM User Plane PDU log packet.
---------------------------------------------------------------------------*/
#define RLC_UL_AM_USR_PLANE_PDU_LOG_PACKET_LEN(num_of_pdu) ( \
  FPOS(WCDMA_UL_RLC_AM_USR_PLANE_PDU_LOG_PACKET_type, ul_am_pdu_data)+ \
      (sizeof(rlc_am_log_header_type) + (num_of_pdu * 4)) )

#ifdef FEATURE_UM_PDU_LOGGING
#define RLC_UL_UM_USR_PLANE_PDU_LOG_PACKET_LEN(num_of_pdu) ( \
  FPOS(WCDMA_UL_RLC_UM_USR_PLANE_PDU_LOG_PACKET_type, ul_um_pdu_data)+ \
      (sizeof(rlc_um_log_header_type) + (num_of_pdu * 4)) )
#endif

/*---------------------------------------------------------------------------
  Length of UL AM Signaling Plane PDU log packet.
---------------------------------------------------------------------------*/
      
#define RLC_UL_AM_SIG_PLANE_PDU_LOG_PACKET_LEN(num_of_pdu) ( \
  FPOS(WCDMA_UL_RLC_AM_SIG_PLANE_PDU_LOG_PACKET_type, ul_am_pdu_data)+ \
      (sizeof(rlc_am_log_header_type) + (num_of_pdu * 4)) )
                                                
/*---------------------------------------------------------------------------
  Length of DL AM User Plane PDU log packet.
---------------------------------------------------------------------------*/
#define RLC_DL_AM_USR_PLANE_PDU_LOG_PACKET_LEN(num_of_pdu) ( \
  FPOS(WCDMA_DL_RLC_AM_USR_PLANE_PDU_LOG_PACKET_type, dl_am_pdu_data)+ \
      (sizeof(rlc_am_log_header_type) + (num_of_pdu * 4)) )
    
#ifdef FEATURE_UM_PDU_LOGGING
#define RLC_DL_UM_USR_PLANE_PDU_LOG_PACKET_LEN(num_of_pdu) ( \
  FPOS(WCDMA_DL_RLC_UM_USR_PLANE_PDU_LOG_PACKET_type, dl_um_pdu_data)+ \
      (sizeof(rlc_um_log_header_type) + (num_of_pdu * 4)) )
#endif

/*---------------------------------------------------------------------------
  Length of DL AM Signaling Plane PDU log packet.
---------------------------------------------------------------------------*/
#define RLC_DL_AM_SIG_PLANE_PDU_LOG_PACKET_LEN(num_of_pdu) ( \
  FPOS(WCDMA_DL_RLC_AM_SIG_PLANE_PDU_LOG_PACKET_type, dl_am_pdu_data)+ \
      (sizeof(rlc_am_log_header_type) + (num_of_pdu * 4)) )
                                                
#if ((defined (FEATURE_HSDPA_MAC_EHS)) && (defined (FEATURE_WCDMA_REL7)))
/*---------------------------------------------------------------------------
  Length of DL AM User Plane PDU log packet.
---------------------------------------------------------------------------*/
#define RLC_DL_AM_FLEXIBLE_PDU_LOG_PACKET_LEN(num_entity, num_of_pdu) ( \
  FPOS(WCDMA_DL_FLEXIBLE_RLC_AM_PDU_LOG_PACKET_V1_type, dl_am_flex_pdu_data)+ \
      (sizeof(rlc_log_flex_dl_am_pdu_data_type) * num_entity) +  \
      (sizeof(rlc_log_flex_dl_am_pdu_info_type) * num_of_pdu))
#endif

#ifdef FEATURE_MAC_I
#define RLC_UL_FLEXIBLE_PDU_LOG_PACKET_LEN(num_entity, num_of_pdu) ( \
  FPOS(WCDMA_UL_FLEXIBLE_RLC_PDU_LOG_PACKET_V1_type, ul_flex_pdu_data)+ \
      (sizeof(rlc_log_flex_ul_pdu_data_type) * num_entity) +  \
      (sizeof(rlc_log_flex_ul_pdu_info_type) * num_of_pdu))
#endif

/*---------------------------------------------------------------------------
  Length of UL AM NAK PDU log packet.
---------------------------------------------------------------------------*/
#define RLC_UL_AM_NAK_PDU_LOG_PACKET_LEN() ( \
  FPOS(WCDMA_UL_RLC_AM_NAK_PDU_LOG_PACKET_type, ul_am_nak_pdu)+ \
      sizeof(rlc_log_ul_am_nak_pdu_type) )
                                                
/*---------------------------------------------------------------------------
  Length of DL AM NAK PDU log packet.
---------------------------------------------------------------------------*/
#define RLC_DL_AM_NAK_PDU_LOG_PACKET_LEN() ( \
  FPOS(WCDMA_DL_RLC_AM_NAK_PDU_LOG_PACKET_type, dl_am_nak_pdu)+ \
      sizeof(rlc_log_dl_am_nak_pdu_type) )
                                                
/*---------------------------------------------------------------------------
  Length of Uplink Ciphering Configuration Log Packet.
---------------------------------------------------------------------------*/
#define RLC_UL_CIPH_PACKET_LEN(num_chan) ( \
  FPOS(WCDMA_UL_RLC_V1_CIPH_CFG_LOG_PACKET_type, ul_ciphering)+ \
      (num_chan * sizeof(rlc_log_ul_ciphering_type)) )
      
/*---------------------------------------------------------------------------
  Length of Downlink Ciphering Configuration Log Packet.
---------------------------------------------------------------------------*/
#define RLC_DL_CIPH_PACKET_LEN(num_chan) ( \
  FPOS(WCDMA_DL_RLC_V1_CIPH_CFG_LOG_PACKET_type, dl_ciphering)+ \
      (num_chan * sizeof(rlc_log_dl_ciphering_type)) )
      
#define RLC_UL_PDU_CIPH_INFO_PACKET_LEN(num_pdu) ( \
  FPOS(WCDMA_UL_RLC_V1_PDU_CIPHER_LOG_PACKET_type, ul_pdu_cipher_info)+ \
      (num_pdu * sizeof(rlc_log_ul_pdu_cipher_info_type)) )

#define RLC_DL_PDU_CIPH_INFO_PACKET_LEN(num_pdu) ( \
  FPOS(WCDMA_DL_RLC_V1_PDU_CIPHER_LOG_PACKET_type, dl_pdu_cipher_info)+ \
      (num_pdu * sizeof(rlc_log_dl_pdu_cipher_info_type)) )

#ifdef FEATURE_WCDMA_UL_COMPR
#define RLC_UL_COMPRESSION_LOG_PACKET_LEN(num_samples)( \
  FPOS(WCDMA_RLC_UL_COMPRESSION_LOG_PACKET_type, comp_log_info)+ \
      (num_samples * sizeof(rlc_ul_comp_log_info_type)) ) 
#endif

#ifdef T_WINNT
  #error code not present
#endif

#endif
