#ifndef MACULHSLOG_H__
#define MACULHSLOG_H__

/*===========================================================================
                             M A C H S L O G . H

DESCRIPTION
   This file contains MAC HS layer log packet structure definitions,
   prototypes for MAC HS logging functions, any constant definitions that
   are needed, and any external variable declarations needed for MAC HS logging.

Copyright (c) 2001 - 2004 by Qualcomm Technologies, Inc.  All Rights Reserved.
Copyright (c) 2006 - 2009 by Qualcomm Technologies, Inc.  All Rights Reserved.
============================================================================*/


/*==========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/wcdma/mac/inc/maculhslog.h#1 $ $DateTime: 2016/12/13 08:00:12 $ $Author: mplcsds1 $

when        who     what, where, why
--------    ---     --------------------------------------------------------
11/06/14    aa      Replaced 0x4321 log packet with enhanced version 0x41C2
05/13/14    ymu     Feature name FEATURE_WCDMA_DC_HSUPA_FRAME replacement with FEATURE_WCDMA_DC_HSUPA 
02/24/14    ts      Changes for 0x430E log packet redesign
05/22/13    kc      Added DC-HSUPA Changes
05/23/13    geg     WSW decoupling (except for component of QCHAT)
08/22/12    kc      New log packet 0x4314 for HS RACH MAC timer info
07/24/12    geg     Upmerge WCDMA.MPSS.1.0 to WCDMA.MPSS.2.0
06/05/12    mrg     HS-RACH & MAC-I Feature code integrated
03/15/11    ssg     Mainlining FEATURE_WCDMA_HSUPA_2MS_TTI.
05/11/10    mrg     MAC_HS_UL_NUM_MAC_ES_HEADERS_IN_LOG_PKT and 
                    MAC_HS_UL_NUM_MAC_E_HDRS define values are updated to 16
03/29/10    mrg     reverting double buffer log commit changes
02/19/10    mrg     log_commit both the buffers when buffers had valid log samples (ex: at reconifg times)
09/22/09    sup     Removed log code assignments and placed them in 
                    log_codes_wcdma_int_v.h
05/29/09    mrg     Due to overwritting of log smaples in ISR ,there is a mismatch 
                    of the log smaple values before log_commit.So dirt_bit concept is added.
05/07/09    ssg     Updated Copyright Information
10/17/08    grk     Merged taxis changes to mainline.
05/27/08    mrg     Log packets 0x4322 and 0x4323 are modified to work in 2ms TTI
02/10/07    sk      Logging support for the internal log packet - 0x4323
01/31/07    sk      Logging support for log code 0x4321, 0x4322
05/12/06    sk      Initial Release

============================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "wcdma_variation.h"
#include "comdef.h"
#include "log_codes_wcdma_int.h"
#ifdef CUST_H
#include "customer.h"
#endif

#if (defined  (FEATURE_WCDMA_HSUPA) && defined (FEATURE_MAC_HS_UL_QXDM_LOGGING))

#include "uecomdef.h"
#include "log.h"
#include "macrrcif_v.h"
#include "macul.h"
#include "maculhs.h"


#ifdef T_WINNT
  #error code not present
#endif


#ifndef LOG_WCDMA_BASE_C
#define LOG_WCDMA_BASE_C 0x4000
#endif

/* This macro is used for filling the bit fields in EUL uplink log packet*/
#define MAC_HS_UL_LOG_PKT_SET_FIELD(buf, field1, field2, value) \
  (buf) = (((buf) & (~MAC_HS_UL_##field1##_##field2##_BMASK)) | \
           (((value) << MAC_HS_UL_##field1##_##field2##_SHIFT) & \
           MAC_HS_UL_##field1##_##field2##_BMASK))
/*------------------------------------------------------------------------
 These are valid only in case of TTI_TYPE is 2ms 
 status log packet will be logged every 80ms
------------------------------------------------------------------------*/
#define MAC_HS_UL_STATUS_LOG_TIMER_VAL_2MS_TTI                              80

// Status log packet logged every 200ms in case of 10ms TTI
#define MAC_HS_UL_STATUS_LOG_TIMER_VAL_10MS_TTI                             200

/*------------------------------------------------------------------------
 If 2ms TTI - Number of status reports to collect are 40 
 If 10ms TTI - Number of status reports to collect are 20 
------------------------------------------------------------------------*/
#define MAC_HS_UL_NUM_REPORTS_IN_STATUS_LOG_PKT                             40
#define MAC_HS_UL_NUM_REPORTS_IN_STATUS_LOG_PKT_10MS_TTI                    20

#define MAC_HS_UL_NUM_MAC_ES_HEADERS_IN_LOG_PKT                            16

#define MAC_HS_UL_NUM_MAC_E_HDRS                                           16

#define MAC_HS_UL_NUM_MACI_IS_STATUS_LOG_PKT                                40

#define MAC_HS_UL_CHECK_LOG_STATUS_AFTER_N_CFN_COUNT                          25

#ifdef FEATURE_WCDMA_HS_RACH
#define MAC_HS_RACH_MAX_SAMPLES_MAC_TIMER_LOG_PKT                     40
#endif

/*===========================================================================

                      DATA STRUCTURES FOR LOG PACKETS

===========================================================================*/
#ifdef __GNUC__
typedef PACKED struct PACKED_POST
#else /* __GNUC__*/
typedef PACKED struct
#endif /* __GNUC__ */
{
  /*E-DCH MAC-d flow identifier. Range: 0..31 */
  uint8     mac_d_flow_id;

  /* E-DCH MAC-d flow power offset. Range: 0..7*/
  uint8     power_offset;

  /* E-DCH MAC-d flow maximum number of retransmissions. Range: 0..15*/
  uint8     max_retx;

  /*    1   Indicates which of the other Mac-d flows are allowed to be multiplexed
  along with this Mac-d flow. For example: Bit 0 (LSB) whether Mac-d flow 0 is
  allowed to be multiplexed with this Mac-d flow.Bit value = 1 means that the
  Mac-d flow corresponding to this bit position is allowed to be multiplexed with
  the current Mac-d flow.*/
  uint8     mux_list;

  /*Transmission grant type for this Mac-d flow 0: SCHEDULED 1: NON_SCHEDULED*/
  uint8     grant_type;

  /* Number of logical channels mapped onto this MAC-d flow. Range: 1..15 */
  uint8     num_logical_ch;

  /* Non-scheduled grant value. Range: 0..19982 */
  uint16    non_sched_tx_grant;

  /*NOTE: Field is only valid only if GRANT_TYPE indicates NON_SCHEDULED */
  /*    1   Non-scheduled TX HARQ process allocation (2ms TTI only) for this Mac-d flow.
  Bit 0 (LSB) corresponds to HARQ process 0, bit 1 corresponds to HARQ process 1, …etc.
  0: Process is disabled for this Mac-d flow
  1: Process is enabled for this Mac-d flow */
  uint8     non_sched_harq_alloc;

}mac_hs_ul_log_mac_d_flow_info_type;

#ifdef __GNUC__
typedef PACKED struct PACKED_POST
#else /* __GNUC__*/
typedef PACKED struct
#endif /* __GNUC__ */
{
  /* Data Description Identifier (6 bits), it identifies the logical channel,
     MAC-d flow and size of the MAC-d PDUs concatenated into
     the associated MAC-es PDU */
  uint8                           ddi; /* 0..62 (63 is reserved) */

    /* Since there is no header for MAC-D pdus in EUL, so rlc_pdu_size is
     same as mac_d_pdu_size */
  uint16                          rlc_pdu_size_in_bits; /* 16..5000 (size in bits) */

} mac_hs_ul_log_rlc_size_ddi_info_s_type;

#ifdef __GNUC__
typedef PACKED struct PACKED_POST
#else /* __GNUC__*/
typedef PACKED struct
#endif /* __GNUC__ */
{
  uint16                          min_pdu_size; /* 16..12000 (size in bits) */
  uint16                          max_pdu_size; /* 16..12000 (size in bits) */

} mac_hs_ul_log_rlc_flex_pdu_info_type;



/*--------------------------------------------------------------------------------------*/
/* Configuration Parameters of UL Dedicated logical channel mapped to E-DCH                                                             */
/*--------------------------------------------------------------------------------------*/
#ifdef __GNUC__
typedef PACKED struct PACKED_POST
#else /* __GNUC__*/
typedef PACKED struct
#endif /* __GNUC__ */
{

  /* Radio Bearer Identifier (0-31) */
  uint8                     rb_id;

  /* LC ID as configured by NW */
  uint8                     lc_id;
   
  /* RLC logical channel buffer identifier (0-20) */
  uint8                     rlc_id;

    /* MAC logical channel priority (1-8) */
  uint8                     priority;

  /* Logical channel type (DCCH or DTCH) */
  uint8                     chan_type;

  /* RLC mode (UM or AM) */
  uint8                     rlc_mode; /* UM or AM */

  /* if set to false then dont consider this mac_id while sending scheduling
     information*/
  boolean                   include_in_scheduling_info;

  /*0: Fixed PDU
    1: Flexible PDU*/
  uint8                     pdu_type;

  mac_hs_ul_log_rlc_flex_pdu_info_type flex_pdu_info;

  /* number of valid fields in rlc_size_ddi_info[] */
  uint8                     num_rlc_size_ddi_info; /* 0..32, 0 is valid only when action is NOOP or STOP */

  /* Mapping of rlc-pdu-size  to ddi. Valid number of fields in following array is
     equal to 'num_rlc_size_ddi_info' */
  mac_hs_ul_log_rlc_size_ddi_info_s_type     rlc_size_ddi_info[MAX_RLC_PDU_SIZE_PER_LOGCHAN];

} mac_hs_ul_log_logch_info_s_type ;

/*--------------------------------------------------------------------------
 Data structure for Logical Channel configuration info
 --------------------------------------------------------------------------*/
#ifdef __GNUC__
typedef PACKED struct PACKED_POST
#else /* __GNUC__*/
typedef PACKED struct
#endif /* __GNUC__ */
{

  /* E-DCH MAC-d flow identified */
  uint8                                  e_mac_d_flow_id; /* 0..7 */

  /* E-DCH MAC-d flow power offset */
  uint8                                  e_mac_d_flow_power_offset; /* 0..6 (unit in dB) */

  /* E-DCH MAC-d flow maximum number of  retransmissions */
  uint8                                  e_mac_d_flow_max_num_of_retx; /* 0..15 */

  /* E-DCH MAC-d flow multiplexing list:
     Indicates, if this is the first MAC-d flow for which PDU's are placed in
     the MAC-e PDU, the other MAC-d flows from which MAC-d PDU's
     are allowed to be included in the same MAC-e PDU.
     Bit 0 is for MAC-d flow 0, Bit 1 is for MAC-d flow 1, …
     Value '1' for a bit means multiplexing is allowed.
     Bit 0 is Most Significant Bit(Left most bit) in the bitstring
     NOTE: The bit that corresponds to the MAC-d flow itself is ignored */
  uint8                                  e_mac_d_flow_mux_list; /* BITSTRING */

  /* transmission grant type */
  uint8                                  e_tx_grant_info;

  /* number of logical channels */
  uint8                                  num_logch; /* 0..15 */

  /* Max MAC-e PDU contents size: max number of non scheduled data
     in bits that can be part of MAC-e PDU */
  uint16                                max_mac_e_pdu_size_in_bits; /* 1..19982 */

  /* MAC-d PDU's are only allowed to be transmitted in those processes
     for which the bit is set to "1". Bit 0 corresponds to HARQ process
     0, bit 1 corresponds to HARQ process 1. This is valid for 2ms TTI only.
     Bit 0 is Most Significant Bit(Left Most bit) of the BITSTRING */
  uint8                           non_sched_tx_grant_harq_allocation_2ms; /* BITSTRING */

  /* number of fields valid in following array is equal to 'num_logch' */
  mac_hs_ul_log_logch_info_s_type             e_ul_logch_info[MAX_DED_LOGCHAN_PER_TRCH];

}mac_hs_ul_log_mac_d_flow_info_s_type;

/*===========================================================================

                     CONFIG  LOG PACKET DEFINITION

===========================================================================*/

/*--------------------------------------------------------------------------
Log packet for reporting MAC HS Configuration log packet.
---------------------------------------------------------------------------*/

LOG_RECORD_DEFINE(WCDMA_MAC_HS_UL_CONFIG_LOG_PACKET_V1)

  /*Version Number*/
  uint8 ver;

  /* 0 = 10ms TTI, 1= 2msTTI */
  uint8     tti;
  /*Indicates which standardized E-TFCI TB size table shall be used*/
  uint8     etfci_table_idx;

  /*Happy bit delay condition over which to evaluate the current
  serving grant. Value is reported in milliseconds. The following values are possible:
  2ms,10ms,20ms,50ms,100ms,200ms,500ms,1000ms */
  uint16    happy_bit_delay;

  /* Number of MAC-d flows configured. Range: 0..7 */
  uint8     num_macd_flows;

    /* E-DCH minimum set E-TFCI.
    0xFF indicates that minimum set E-TFCI is not provided by NW.*/
  uint8     min_etfci;

  /*Periodicity for Scheduling Info when no grant is available. The range is 0..1000
  is mapped as follows: 0: no periodic reporting configured 1: every TTI 4: 4ms 10: 10ms
  20: 20ms 50: 50ms 100: 100ms 200: 200ms 500: 500ms 1000: 1000ms */
  uint16    si_period_no_grant;

  /* Periodicity for Scheduling Info when grant is available. Same range as above.*/
  uint16    si_period_grant;

  /*Power offset in case SI only is transmitted. Range: 0..6 corresponding to 0dB..6dB*/
  uint8     si_power_offset;

  /*MAC-E or MAC-I*/ 
  uint8 mac_ul_pdu_type;

  /*MAC_E RESET Indicator(TRUE/FALSE)*/
  boolean mac_e_reset_indicator;

  /*6 bits (Single Carrier is configured)
    14 bits (DC-HSUPA is configured)*/
  uint8 tsn_size;

  /* E-DCH MAC-d flow */
  mac_hs_ul_log_mac_d_flow_info_s_type    e_mac_d_flow_info[MAX_E_DCH_MAC_D_FLOW];

LOG_RECORD_END

/*--------------------------------------------------------------------------
 MAC HS CONFIG LOG PACKET SIZE
 --------------------------------------------------------------------------*/
#define MAC_HS_UL_CHAN_CFG_INFO_LOG_PKT_V1_SIZE(num_macd_flow, num_log_ch, num_ddi, num_flex_ch, num_fixed_ch) (\
    FPOS(WCDMA_MAC_HS_UL_CONFIG_LOG_PACKET_V1_type, e_mac_d_flow_info) + \
    ((num_macd_flow)*(7*sizeof(uint8)+sizeof(uint16))) + ((num_log_ch)*(8*sizeof(uint8))) + \
    (num_flex_ch * 2 * sizeof(uint16)) + (num_fixed_ch * sizeof(uint8)) + \
    (num_ddi * (sizeof(uint8) + sizeof(uint16))))

/*--------------------------------------------------------------------------
MAC HS CONFIG function call
--------------------------------------------------------------------------*/
void mac_hs_ul_send_config_log_packet
(
  const mac_e_config_s_type   *hs_ul_config_ptr,
  boolean mac_e_reset_indicator
);

/*=========================================================================

  MAC HS HEADER STTATUS LOG PACKET - LOG CODE 4322

========================================================================*/


/*===========================================================================
Bit masks for the 1st 3 bytes of the header log packet
===========================================================================*/
#define MAC_HS_UL_HDR_LOG_NUM_SAMPLES_BMASK     0x3F
#define MAC_HS_UL_HDR_LOG_NUM_SAMPLES_SHIFT     0

#define MAC_HS_UL_HDR_LOG_TTI_BMASK             0x40
#define MAC_HS_UL_HDR_LOG_TTI_SHIFT             6

#define MAC_HS_UL_HDR_LOG_ETFCI_TABLE_BMASK     0x80
#define MAC_HS_UL_HDR_LOG_ETFCI_TABLE_SHIFT     7

#define MAC_HS_UL_HDR_LOG_START_FN_BMASK        0xFFF
#define MAC_HS_UL_HDR_LOG_START_FN_SHIFT        0

#define MAC_HS_UL_HDR_LOG_HARQ_ID_BMASK         0x70
#define MAC_HS_UL_HDR_LOG_HARQ_ID_SHIFT         4

/*===========================================================================
Bit masks for happy bit and tx type
===========================================================================*/
#define MAC_HS_UL_HDR_LOG_TX_TYPE_BMASK         0x3
#define MAC_HS_UL_HDR_LOG_TX_TYPE_SHIFT         0

#define MAC_HS_UL_HDR_LOG_HB_BMASK              0x4
#define MAC_HS_UL_HDR_LOG_HB_SHIFT              2

/*===========================================================================
Bit masks for number of mac-es-headers and the si present field
===========================================================================*/
#define MAC_HS_UL_HDR_LOG_MAC_ES_BMASK          0x3F
#define MAC_HS_UL_HDR_LOG_MAC_ES_SHIFT          0

#define MAC_HS_UL_HDR_LOG_SI_PRESENT_BMASK      0x40
#define MAC_HS_UL_HDR_LOG_SI_PRESENT_SHIFT      6

/*===========================================================================
Bit masks for number of MAC-e header portion
===========================================================================*/
#define MAC_HS_UL_HDR_LOG_HLID_BMASK            0xF
#define MAC_HS_UL_HDR_LOG_HLID_SHIFT            0

#define MAC_HS_UL_HDR_LOG_HLBS_BMASK            0xF0
#define MAC_HS_UL_HDR_LOG_HLBS_SHIFT            4

#define MAC_HS_UL_HDR_LOG_TEBS_BMASK            0x1F
#define MAC_HS_UL_HDR_LOG_TEBS_SHIFT            0

#define MAC_HS_UL_HDR_LOG_SI_TRIG_BMASK         0x20
#define MAC_HS_UL_HDR_LOG_SI_TRIG_SHIFT         5

#define MAC_HS_UL_HDR_LOG_SI_PAD_BMASK          0x40
#define MAC_HS_UL_HDR_LOG_SI_PAD_SHIFT          6

#define MAC_HS_UL_HDR_LOG_UPH_BMASK             0x1F
#define MAC_HS_UL_HDR_LOG_UPH_SHIFT             0

#ifdef FEATURE_WCDMA_DC_HSUPA
#define   MAC_HS_UL_DC_CARRIER_CONFIG_MASK  0x3

#define   MAC_HS_UL_DC_CARRIER_INDEX_MASK   0x3
#define   MAC_HS_UL_DC_CARRIER_INDEX_SHIFT  2

#define   MAC_HS_UL_DC_CARRIER_TX_TYPE_MASK  0x3
#define   MAC_HS_UL_DC_CARRIER_TX_TYPE_SHIFT 4

#define   MAC_HS_UL_DC_CARRIER_HB_MASK  0x1
#define   MAC_HS_UL_DC_CARRIER_HB_SHIFT 6

#endif /* FEATURE_WCDMA_DC_HSUPA */

/*--------------------------------------------------------------------------
 MAC HS Header Type
 --------------------------------------------------------------------------*/
#ifdef __GNUC__
typedef PACKED struct PACKED_POST
#else /* __GNUC__*/
typedef PACKED struct
#endif /* __GNUC__ */
{
  /*Data Description Indicator. Range: 0..62*/
  uint8 ddi;

  /* Transmit Sequence Number. Range: 0..63*/
  uint8 tsn;

  /* N = number of MAC-d PDUs in this MAC-es PDU. Range: 0..63 */
  uint16 npdus;

}mac_hs_ul_log_hdr_type;


#ifdef __GNUC__
typedef PACKED struct PACKED_POST
#else /* __GNUC__*/
typedef PACKED struct
#endif /* __GNUC__ */
{
  /*Indicates the type of transmission 0: new transmission 1: retransmission
  2: no transmission */
  uint8         tx_type;

  /* ETFCI used this tti */
  uint8         etfci;

  /*Whether mobile is happy (1) or unhappy (0) with the current SG*/
  uint8         happy_bit;

  /* Indicates whether the next fields related to SI are present
  0: SI not present1: SI present */
  boolean       si_present;

  /*Whether SI was triggered by buffer status or periodic trigger
  0: SI was not triggered by buffer status or periodic trigger
  1: SI was triggered due to either buffer status or periodic trigger*/
  boolean       si_triggered;

  /*Whether SI was added due to available padding 0: SI was not appended
  due to available 1: SI was appended due to available */
  boolean       si_padding;

  /* HLID, TEBS, HEBS,.. */
  uint8      tebs;
  uint8      uph;
  uint8      hlid;
  uint8      hlbs;

}mac_hs_ul_log_hdr_ctrl_info_type;

/*--------------------------------------------------------------------------
 Header info per TTI
 --------------------------------------------------------------------------*/
#ifdef __GNUC__
typedef PACKED struct PACKED_POST
#else /* __GNUC__*/
typedef PACKED struct
#endif /* __GNUC__ */
{

  /*Number of MAC-es headers in this MAC-e PDU. Range: 0..62*/
  uint8                                 num_mac_es_hdrs;

  /*information related to si */
  mac_hs_ul_log_hdr_ctrl_info_type      ctrl_info;

  /* Header data */
  mac_hs_ul_log_hdr_type                hdr_data[MAC_HS_UL_NUM_MAC_ES_HEADERS_IN_LOG_PKT];

}mac_hs_ul_log_header_info_per_tti_type;

/*--------------------------------------------------------------------------
 Header info per TTI
 --------------------------------------------------------------------------*/
#ifdef __GNUC__
typedef PACKED struct PACKED_POST
#else /* __GNUC__*/
typedef PACKED struct
#endif /* __GNUC__ */
{
  /*NUM_SAMP        Number of MAC-e headers in this log packet. Range: 0..50*/
  uint8         num_samples;

  /* TTI 0 - 10ms, 1 - 2ms */
  uint8         tti;

  /*START_FN        Starting EUL Frame number or Subframe number for E-DPCH.
  Range: 0..255. Frame number if TTI = 10ms Range: 0..1279. Subframe number if TTI = 2ms*/
  /*START_HARQ_ID       Indicates which HARQ process ID corresponds to the
  first frame or subframe number in this log packet. Range: 0..7 */
  uint16        start_fn;

  uint8         harq_id;

  /* ETFCI Table IDx */
  uint8         etfci_table_idx;

  /* dirty_bit is uesd to make sure that there will be no over writting of the log samples */
  boolean                                       dirty_bit;

  /* Complete header information for all samples*/
  mac_hs_ul_log_header_info_per_tti_type    hdr_info[MAC_HS_UL_NUM_REPORTS_IN_STATUS_LOG_PKT];

}mac_hs_ul_log_header_uncompressed_pkt_type;

/*--------------------------------------------------------------------------
 Header info per TTI
 --------------------------------------------------------------------------*/
#ifdef __GNUC__
typedef PACKED struct PACKED_POST
#else /* __GNUC__*/
typedef PACKED struct
#endif /* __GNUC__ */
{
  /*TX_TYPE     Indicates the type of transmission
  0: new transmission 1: retransmission 2: no transmission
  HAPPY_BIT     Whether mobile is happy (1) or unhappy (0) with the current SG*/
  uint8     happy_bit_tx_type;

  /* ETFCI chosen this TTi */
  uint8     etfci;

  /*NUM_MAC_HDRS  Number of MAC-es headers in this MAC-e PDU. Range: 0..62
  SI_PRESENT        Indicates whether the next fields related to SI are present
  0: SI not present 1: SI present*/
  uint8     num_mac_hdrs_si_present;

  /*HLID        Highest priority logical channel ID. Range: 0..15
  TEBS      Total E-DCH Buffer Status (see 25.321 section 9.2.5.3.2). Range: 0..31
  HLBS      Highest priority Logical channel Buffer Status (see 25.321 section 9.2.5.3.2). Range: 0..15
  UPH       UE Power Headroom (see 25.321 section 9.2.5.3.2). Range: 0..31
  SI_TRIG       Whether SI was triggered by buffer status or periodic trigger
  0: SI was not triggered by buffer status or periodic trigger
  1: SI was triggered due to either buffer status or periodic trigger
  SI_PAD        Whether SI was added due to available padding
  0: SI was not appended due to available
  1: SI was appended due to available */
  uint8     si_content[3];

  /* MAC e- headers ddi, TSN, N in that order*/
  uint8     mac_e_hdr[MAC_HS_UL_NUM_MAC_E_HDRS][3];

}mac_hs_ul_log_header_status_pkt_type;


#ifdef FEATURE_MAC_I

#ifdef __GNUC__
typedef PACKED struct PACKED_POST
#else /* __GNUC__*/
typedef PACKED struct
#endif /* __GNUC__ */
{
  //Size of Mac-is SDU in the Mac-is PDU (choosen PDU size)
  uint16        maci_sdu_size;
  
  /* 0000 to 1101 : Logical Channel ID 1 to 14
    * 1110         : Identification of CCCH (SRB0)
    *1111         : Identification of E-RNTI being included.
    */
   uint8        logch_id;
  
  /* Number of consecutive Mac-is SDU’s of given SDU_SIZE of Mac-is PDU(range is 0 to 63) */ 
  uint8 n_sdu;
  
  /* Flag bit to indicate if any more fields are present in Mac-i header.
    * 0 : Followed by another Mac-i header
    * 1 : Followed by Mac-is PDU
  */
  boolean       flag_bit;
  
  /*Segmentation Type: 0- PC, 1 - PS, 2-PM, 3-PE */
  uint8    ss;
  
  /* Transmit Sequence Number. Range: 0..63*/
  #ifdef FEATURE_WCDMA_DC_HSUPA
  uint16         tsn;
  #else
  uint8         tsn;
  #endif
  
  /* 0 : Primary ERNTI (P), 1 : Secondary ERNTI (S) */
  uint8         ernti;
  
}mac_hs_ul_maci_hdr_logch_type;


#ifdef __GNUC__
typedef PACKED struct PACKED_POST
#else /* __GNUC__*/
typedef PACKED struct
#endif /* __GNUC__ */
{
  
  /*Number of MAC-es headers in this MAC-e PDU. Range: 0..62*/
  uint8                               num_mac_i_hdrs;
  
  /*information related to si */
  mac_hs_ul_log_hdr_ctrl_info_type    ctrl_info;
  
  /* Available payload in bits summed over all logical channels configured on scheduled Mac-d flows.
   * The value 65535 means available  payload is greater or equal to 65536 bits.
   */
  uint16    sched_buf_status;
  
  /* Available payload in bits summed over all logical   channels configured on non-scheduled Mac-d flows.
   * The value 65535 means   available payload is greater or equal to 65536 bits.
   */
  uint16    non_sched_buf_status;
  
  /*Final sum of non-scheduled payloads after E-TFC selection. Range: 0..20000 */
  uint16    ns_payload;
  
  /*Final sum of scheduled payloads after E-TFC selection. Range: 0..20000 */
  uint16    sched_payload;
  
  #ifdef FEATURE_WCDMA_DC_HSUPA
  uint8     carrier_index;
  uint8     carrier_config_type;
  uint16    l1_sup_payload;
  uint16    l1_sg_payload;
  #endif
  
  uint8     etfci_restricted_reason;
  
}mac_hs_ul_maci_pkt_per_tti_type;


#ifdef __GNUC__
typedef PACKED struct PACKED_POST
#else /* __GNUC__*/
typedef PACKED struct
#endif /* __GNUC__ */
{
  /*Number of MAC-I headers in this log packet. Range: 0..50*/
  uint8         log_pkt_num_samples;

  /* Total number of Cont case samples */
  uint8         num_continue;
  
  /* TTI 0 - 10ms, 1 - 2ms */
  uint8         tti;
  
  /* Starting EUL Frame number or Subframe number for E-DPCH.
   *  Range: 0..255. Frame number if TTI = 10ms
   * Range: 0..1279. Subframe number if TTI = 2ms
   */
  uint16        start_frame_num;
  
  /* Indicates which HARQ process ID corresponds to the  first frame 
   * or subframe number in this log packet. Range: 0..7 
   */
  uint8         harq_id;
  
  /* ETFCI Table IDx */
  uint8         etfci_table_idx;
  
  /* RRC State Indicator- 0 : CELL_DCH, 1 : CELL_FACH,2 : IDLE,3 : N/A */
  uint8         rrc_state;
  
  uint8         log_pakt_version;
  
  /* dirty_bit is uesd to make sure that there will be no over writting of the log samples */
  boolean       dirty_bit;
  
  uint8   num_tti_logged;
  
  mac_hs_ul_maci_hdr_logch_type      maci_logch_hdr_info[MAC_HS_UL_NUM_MACI_IS_STATUS_LOG_PKT + 1];
  
  mac_hs_ul_maci_pkt_per_tti_type    maci_per_tti_info[MAC_HS_UL_NUM_MACI_IS_STATUS_LOG_PKT];
  
}mac_hs_ul_maci_logpkt_uncompressed_pkt_type;


#ifdef __GNUC__
typedef PACKED struct PACKED_POST
#else /* __GNUC__*/
typedef PACKED struct
#endif /* __GNUC__ */
{
#ifdef FEATURE_WCDMA_DC_HSUPA
  uint8     hb_tx_type_carrier_index_conf;
  uint16    l1_sup_payload;
  uint16    l1_sg_payload;
  uint16    ss_tsn;
#else
  uint8     ss_tsn;
#endif
  uint8     etfci_reason_hb_tx_type;
  uint8     etfci_index;
  uint16    sched_buf_status;
  uint16    non_sched_buf_status;
  uint16    sched_payload;
  uint16    ns_payload;
  uint8     uph_si_type;
  uint8     hlbs_hlid;
  uint8     tebs;
  uint8     sdu_size;
  uint8     ernti_lcid_sdu_size;
  uint8     fbit_n_sdu;
}mac_hs_ul_maci_is_per_tti_info_type;

typedef struct
{
  boolean dcupa_active;

  /* Indicates which HARQ process ID corresponds to the  first frame 
   * or subframe number in this log packet. Range: 0..7 
   */
  uint8         harq_id;

  mac_hs_ul_maci_pkt_per_tti_type maci_per_tti_info_record;

  /* Total number of Cont case samples */
  /* uint8         num_continue;       */

  mac_hs_ul_maci_hdr_logch_type   maci_hdr_info_record[MAC_HS_UL_MAX_NUM_RLC_PDUS_PER_LC];
  
}mac_hs_ul_maci_log_pkt_record_per_carrier_type;

/*--------------------------------------------------------------------------
Log packet for reporting MAC HS header status log packet.
---------------------------------------------------------------------------*/

LOG_RECORD_DEFINE(WCDMA_MAC_HS_UL_MAC_I_LOG_PACKET)
  /*        Number of MAC-i-is headers in this log packet. Range: 0..40 */
  
  /*        E-TFC table index: 0 or 1
  uintSTART_FN      Starting EUL Frame number or Subframe number for E-DPCH.
  Range: 0..255. Frame number if TTI = 10ms
  Range: 0..1279. Subframe number if TTI = 2ms
  START_HARQ_ID     Indicates which HARQ process ID corresponds
  to the first frame or subframe number in this log packet. Range: 0..7*/
  
  uint8                                  version;
  uint8                                  num_sample_rrc_state;
  uint8                                  start_fn;
  uint8                                  tti_start_harq_id_start_fn;

    /* Number of samples that are accumulated */
  mac_hs_ul_maci_is_per_tti_info_type    maci_is_log_samples[MAC_HS_UL_NUM_MACI_IS_STATUS_LOG_PKT];

LOG_RECORD_END

#if 0
  /* MAC HS UL MAC-I-IS  LOG PACKET SIZE  */
#define MAC_HS_UL_MACI_IS_INFO_LOG_PKT_SIZE(sum_new_re_tx_count, num_continue_tx, num_dtx) (\
  FPOS(WCDMA_MAC_HS_UL_MAC_I_LOG_PACKET_type, maci_is_log_samples) + \
  (((sum_new_re_tx_count)*(17*sizeof(uint8))) + (num_continue_tx * (4*sizeof(uint8))) + (num_dtx * (4*sizeof(uint8))))
#endif

#ifdef FEATURE_WCDMA_DC_HSUPA

#define MAC_HS_UL_DC_MACI_IS_INFO_LOG_PKT_SIZE(new_tx_count, retx_count, num_continue_tx, num_dtx) (\
  FPOS(WCDMA_MAC_HS_UL_MAC_I_LOG_PACKET_type, maci_is_log_samples) + (num_continue_tx * (6 * sizeof(uint8))) +\
    ((num_dtx + retx_count) *(10 * sizeof(uint8))) + ((new_tx_count) *(23 * sizeof(uint8))) + 92)

#else

#define MAC_HS_UL_MACI_IS_INFO_LOG_PKT_SIZE(sum_new_re_tx_count, num_continue_tx, num_dtx) (\
  FPOS(WCDMA_MAC_HS_UL_MAC_I_LOG_PACKET_type, maci_is_log_samples) + (40*(17*sizeof(uint8))))
  
#endif /*  FEATURE_WCDMA_DC_HSUPA  */



#endif /*  FEATURE_MAC_I  */

/*--------------------------------------------------------------------------
Log packet for reporting MAC HS header status log packet.
---------------------------------------------------------------------------*/

LOG_RECORD_DEFINE(WCDMA_MAC_HS_UL_HEADER_STATUS_LOG_PACKET)
  /*        Number of MAC-e headers in this log packet. Range: 0..50 */
  /*        Transmission time interval 0: 10ms 1: 2ms*/
  /*        E-TFC table index: 0 or 1
  uintSTART_FN      Starting EUL Frame number or Subframe number for E-DPCH.
  Range: 0..255. Frame number if TTI = 10ms
  Range: 0..1279. Subframe number if TTI = 2ms
  START_HARQ_ID     Indicates which HARQ process ID corresponds
  to the first frame or subframe number in this log packet. Range: 0..7*/
  uint8                                nsamp_tti_etfci_tbl;

  uint8                                start_fn;
  uint8                                start_harq_id_start_fn;

    /* Number of samples that are accumulated */
  mac_hs_ul_log_header_status_pkt_type  hdr_pkt_info[MAC_HS_UL_NUM_REPORTS_IN_STATUS_LOG_PKT];

LOG_RECORD_END

/*--------------------------------------------------------------------------
 MAC HS UL HEADER  LOG PACKET SIZE
 --------------------------------------------------------------------------*/
#define MAC_HS_UL_HDR_INFO_LOG_PKT_SIZE(num_samples, num_mac_hdrs, num_si, num_new_tx) (\
    FPOS(WCDMA_MAC_HS_UL_HEADER_STATUS_LOG_PACKET_type, hdr_pkt_info) + \
    ((num_samples)*(sizeof(uint8))+ (num_new_tx * (2 * sizeof(uint8))) + (num_mac_hdrs)*(3*sizeof(uint8)) + \
    (num_si) *(3*sizeof(uint8))))

/*=========================================================================

  MAC HS PDU STTATUS LOG PACKET - LOG CODE 4323

========================================================================*/


#ifdef __GNUC__
typedef PACKED struct PACKED_POST
#else /* __GNUC__*/
typedef PACKED struct
#endif /* __GNUC__ */
{
  /*SI_TRIG     Whether SI was triggered by buffer status or periodic trigger*/
  /* 0: SI was not triggered by buffer status or periodic trigger
  1: SI was triggered due to either buffer status or periodic trigger*/
  uint8     si_trigger;

  /*SI_PAD      Whether SI was added due to available padding
  0: SI was not appended due to available
  1: SI was appended due to available */
  uint8     si_pad;

  /*HAPPY_BIT       Whether mobile is happy (1) or unhappy (0) with the current SG*/
  boolean   happy_bit;

  /*TX_TYPE     Indicates the type of transmission
  0: new transmission
  1: retransmission
  2: no transmission */
  uint8     tx_type;

  /*ETFCI       Final E-TFCI selected. Range: 0..127 */
  uint8     etfci;

    /*  S_BUF_STATUS  2   Available payload in bits summed over all logical
  channels configured on scheduled Mac-d flows. The value 65535 means available
  payload is greater or equal to 65536 bits */
  uint16    sched_buf_status;

  /*NS_BUF_STATUS   2   Available payload in bits summed over all logical
  channels configured on non-scheduled Mac-d flows. The value 65535 means
  available payload is greater or equal to 65536 bits.*/
  uint16    non_sched_buf_status;

}mac_hs_ul_log_etfci_status_ctrl_info_per_tti_type;


/*--------------------------------------------------------------------------
E-TFCI Info per tti
--------------------------------------------------------------------------*/
#ifdef __GNUC__
typedef PACKED struct PACKED_POST
#else /* __GNUC__*/
typedef PACKED struct
#endif /* __GNUC__ */
{

  /*L1_SUP_PAYLOAD  2   Max supported payload per L1 E-TFC restriction
  calculated in L1. Range: 0..20000 */
  uint16    l1_sup_payload;

  /*    2   SG payload calculated in L1. Range: 0..20000 */
  uint16    l1_sg_payload;

  /*NS_PAYLOAD  2   Final sum of non-scheduled payloads after
  E-TFC selection. Range: 0..20000 */
  uint16    ns_payload;

  /*S_PAYLOAD   2   Final sum of scheduled payloads after E-TFC
  selection. Range: 0..20000 */
  uint16    sched_payload;

}mac_hs_ul_log_etfci_status_pkt_per_tti_type;

#ifdef __GNUC__
typedef PACKED struct PACKED_POST
#else /* __GNUC__*/
typedef PACKED struct
#endif /* __GNUC__ */
{
  uint16                                        num_samples;
  /* TTI 0 - 10ms, 1 - 2ms */
  uint8                                         tti;

  /*START_FN        Starting EUL Frame number or Subframe number for E-DPCH.
  Range: 0..255. Frame number if TTI = 10ms Range: 0..1279. Subframe number if TTI = 2ms*/
  /*START_HARQ_ID       Indicates which HARQ process ID corresponds to the
  first frame or subframe number in this log packet. Range: 0..7 */
  uint16                                        start_fn;

  uint8                                         harq_id;

/* dirty_bit is uesd to make sure that there will be no over writting of the log samples */
  boolean                                       dirty_bit;

  /* ETFCI Table IDx */
  uint8                                         etfci_table_idx;

  mac_hs_ul_log_etfci_status_ctrl_info_per_tti_type    ctrl_info[MAC_HS_UL_NUM_REPORTS_IN_STATUS_LOG_PKT];

  mac_hs_ul_log_etfci_status_pkt_per_tti_type   etfci_status_info[MAC_HS_UL_NUM_REPORTS_IN_STATUS_LOG_PKT];

} mac_hs_ul_log_etfci_status_uncompressed_pkt_type;

#ifdef __GNUC__
typedef PACKED struct PACKED_POST
#else /* __GNUC__*/
typedef PACKED struct
#endif /* __GNUC__ */
{
  uint8     si_happy_bit_tx_type;
  uint8     etfci;
  uint16    sched_buf_status;
  uint16    non_sched_buf_status;
  mac_hs_ul_log_etfci_status_pkt_per_tti_type   etfci_status_info;
}mac_hs_ul_log_etfci_status_pkt_type;

/*===========================================================================

                      DATA STRUCTURES FOR EVENTS

===========================================================================*/
/*===========================================================================
FUNCTION: mac_hs_ul_populate_header_status_log_packet

DESCRIPTION:


DEPENDENCIES:
  None

RETURN VALUE:
  None

SIDE EFFECTS:

===========================================================================*/

/*--------------------------------------------------------------------------
Log packet for reporting MAC HS etfci status log packet.
---------------------------------------------------------------------------*/

LOG_RECORD_DEFINE(WCDMA_MAC_HS_UL_ETFCI_STATUS_LOG_PACKET)

  /*        Number of MAC-e headers in this log packet. Range: 0..50 */
  /*        Transmission time interval 0: 10ms 1: 2ms*/
  /*        E-TFC table index: 0 or 1
  uintSTART_FN      Starting EUL Frame number or Subframe number for E-DPCH.
  Range: 0..255. Frame number if TTI = 10ms
  Range: 0..1279. Subframe number if TTI = 2ms
  START_HARQ_ID     Indicates which HARQ process ID corresponds
  to the first frame or subframe number in this log packet. Range: 0..7*/
  uint8                                nsamp_tti_etfci_tbl;
  uint8                                start_fn;
  uint8                                start_harq_id_start_fn;
  mac_hs_ul_log_etfci_status_pkt_type  etfci_status_pkt_info[MAC_HS_UL_NUM_REPORTS_IN_STATUS_LOG_PKT];

LOG_RECORD_END

/*--------------------------------------------------------------------------
 MAC HS UL HEADER  LOG PACKET SIZE
 --------------------------------------------------------------------------*/
#define MAC_HS_UL_ETFCI_STATUS_INFO_LOG_PKT_SIZE(num_samples, num_new_tx) (\
    FPOS(WCDMA_MAC_HS_UL_ETFCI_STATUS_LOG_PACKET_type, etfci_status_pkt_info) + \
    ((num_samples)*(2*sizeof(uint8) + 2*sizeof(uint16))+ (num_new_tx * (sizeof(mac_hs_ul_log_etfci_status_pkt_per_tti_type)))))

#ifdef T_WINNT
  #error code not present
#endif


#ifdef FEATURE_WCDMA_HS_RACH

#ifdef __GNUC__
typedef PACKED struct PACKED_POST
#else /* __GNUC__*/
typedef PACKED struct
#endif /* __GNUC__ */
{
 /* CFN/SuFN value of each sample */
  uint16        cfn_sfn;

  /* TEBS buffer value for CCCH (in bytes) */
  uint8          ccch_tebs_val;

  /*  Bit 0     : Indicates if EDCH resource is acquired by UE or not
       Bit 1-7  : Indicates the countdown timer value for Maximum
                      EDCH resource allocation for CCCH data (in TTIs)  */
  uint8          rsrc_avail_ccch_timer_val;

  /* Indicates the countdown timer value for Maximum period 
      for Collision Resolution (in TTIs) */
  uint8          agch_coll_resol_timer_val;

  /* indicates the countdown timer value for TB timer (in TTIs) */
  uint8          tb_timer_val;

  /* TEBS buffer value for DXCH (in bytes) */
  uint8          dxch_tebs_val;
  
  /*  Bit 0    :  indicates if SI0 is padded or independent
       Bit 1    :  indicates if AGCH is decoded within Maximum period for Collision Resolution
       Bit 2    :  indicates if MAC-EHS data is decoded on the DL or not, if yes then reset the TB timer 
       Bit 3    :  if EDCH data transmission is ongoing or not
       Bit 4-7 : indicates the cause of EDCH resource release */       
  uint8          misc_vals;
}mac_hs_ul_hsrach_mac_timer_info_type;


#ifdef __GNUC__
typedef PACKED struct PACKED_POST
#else /* __GNUC__*/
typedef PACKED struct
#endif /* __GNUC__ */
{
 uint8          log_pkt_version;
 
 /*Number of MAC timer samples in this log packet. Range: 0..40*/
 uint8         log_pkt_num_samples;
 
 /* Starting EUL Frame number or Subframe number.
  *  Range: 0..255. Frame number if TTI = 10ms
  * Range: 0..1279. Subframe number if TTI = 2ms
  */
 uint16        start_frame_num;
 
 mac_hs_ul_hsrach_mac_timer_info_type hsrach_mac_timer_info[MAC_HS_RACH_MAX_SAMPLES_MAC_TIMER_LOG_PKT];

}mac_hs_ul_hsrach_mac_timer_log_pkt_type;


/*--------------------------------------------------------------------------
Log packet for HS RACH MAC timer info.
---------------------------------------------------------------------------*/
LOG_RECORD_DEFINE(WCDMA_HS_RACH_MAC_TIMERS_LOG_PACKET)

uint8          log_pkt_version;
 
 /*Number of MAC timer samples in this log packet. Range: 0..40*/
 uint8         log_pkt_num_samples;
 
 /* Starting EUL Frame number or Subframe number.
  *  Range: 0..255. Frame number if TTI = 10ms
  * Range: 0..1279. Subframe number if TTI = 2ms
  */
 uint16        start_frame_num;
 
 mac_hs_ul_hsrach_mac_timer_info_type  hsrach_mac_timer_info[MAC_HS_RACH_MAX_SAMPLES_MAC_TIMER_LOG_PKT];

LOG_RECORD_END

/*--------------------------------------------------------------------------
 MAC HS RACH TIMER INFO LOG PACKET SIZE
 --------------------------------------------------------------------------*/
#define WCDMA_HS_RACH_MAC_TIMERS_LOG_PACKET_SIZE(num_samples) (\
     FPOS(WCDMA_HS_RACH_MAC_TIMERS_LOG_PACKET_type, hsrach_mac_timer_info) + \
       ((num_samples)*(sizeof(mac_hs_ul_hsrach_mac_timer_info_type))))


#endif /* FEATURE_WCDMA_HS_RACH */

/*===========================================================================

                      PUBLIC DECLARATIONS

===========================================================================*/

#endif /* FEATURE_HSDPA && FEATURE_MAC_HS_QXDM_LOGGING */
#endif /* MACHSLOG_H__ */





