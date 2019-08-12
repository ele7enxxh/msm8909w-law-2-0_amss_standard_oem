#ifndef TDSMACULHSLOG_H__
#define TDSMACULHSLOG_H__

/*===========================================================================
                             M A C H S L O G . H

DESCRIPTION
   This file contains MAC HS layer log packet structure definitions,
   prototypes for MAC HS logging functions, any constant definitions that
   are needed, and any external variable declarations needed for MAC HS logging.

Copyright (c) 2011-2012 by Qualcomm Technologies, Incorporated.  All Rights Reserved.

============================================================================*/


/*==========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/tdscdma/mac/inc/tdsmaculhslog.h#1 $ $DateTime: 2016/12/13 07:58:19 $ $Author: mplcsds1 $

when        who     what, where, why
--------    ---     --------------------------------------------------------
10/24/10    guo     Initial release Upmerged WCDMA to TDS

============================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "tdscdma_variation.h"
#include "comdef.h"
#include "log_codes_tdscdma.h"
#ifdef CUST_H
#include "customer.h"
#endif

#if defined  (FEATURE_TDSCDMA_HSUPA) 

#include "tdsuecomdef.h"
#include "log.h"
#include "tdsmacrrcif.h"
#include "tdsmacul.h"
#include "tdsmaculhs.h"

#ifdef T_WINNT
  #error code not present
#endif


/*!  \brief  This macro is used for filling the bit fields in EUL uplink log packet*/
#define TDSMAC_HS_UL_LOG_PKT_SET_FIELD(buf, field1, field2, value) \
  (buf) = (((buf) & (~MAC_HS_UL_##field1##_##field2##_BMASK)) | \
           (((value) << MAC_HS_UL_##field1##_##field2##_SHIFT) & \
           MAC_HS_UL_##field1##_##field2##_BMASK))
/*!  \brief 
 These are valid only in case of TTI_TYPE is 2ms 
 status log packet will be logged every 80ms
*/

/*!  \brief  Status log packet logged every 200ms in case of 10ms TTI */
#define TDSMAC_HS_UL_STATUS_LOG_TIMER_VAL_10MS_TTI                             200

/*!  \brief 
 If 2ms TTI - Number of status reports to collect are 40 
 If 10ms TTI - Number of status reports to collect are 20 
*/
#define TDSMAC_HS_UL_NUM_REPORTS_IN_STATUS_LOG_PKT                             20

#define TDSMAC_HS_UL_NUM_MAC_ES_HEADERS_IN_LOG_PKT                            16

#define TDSMAC_HS_UL_NUM_MAC_E_HDRS                                           16
/*===========================================================================

                      DATA STRUCTURES FOR LOG PACKETS

===========================================================================*/
	
typedef PACK(struct)
{
    /*!  \brief  Non-scheduled grant value. Range: 0..19982 */
  uint16    non_sched_tx_grant;
	
  /*!  \brief E-DCH MAC-d flow identifier. Range: 0..31 */
  uint8     mac_d_flow_id;

  /*!  \brief  E-DCH MAC-d flow power offset. Range: 0..7*/
  uint8     power_offset;

  /*!  \brief  E-DCH MAC-d flow maximum number of retransmissions. Range: 0..15*/
  uint8     max_retx;

  /*!  \brief     1   Indicates which of the other Mac-d flows are allowed to be multiplexed
  along with this Mac-d flow. For example: Bit 0 (LSB) whether Mac-d flow 0 is
  allowed to be multiplexed with this Mac-d flow.Bit value = 1 means that the
  Mac-d flow corresponding to this bit position is allowed to be multiplexed with
  the current Mac-d flow.*/
  uint8     mux_list;

  /*!  \brief Transmission grant type for this Mac-d flow 0: SCHEDULED 1: NON_SCHEDULED*/
  uint8     grant_type;

  /*!  \brief  Number of logical channels mapped onto this MAC-d flow. Range: 1..15 */
  uint8     num_logical_ch;



  /*!  \brief NOTE: Field is only valid only if GRANT_TYPE indicates NON_SCHEDULED */
  /*!  \brief     1   Non-scheduled TX HARQ process allocation (2ms TTI only) for this Mac-d flow.
  Bit 0 (LSB) corresponds to HARQ process 0, bit 1 corresponds to HARQ process 1, …etc.
  0: Process is disabled for this Mac-d flow
  1: Process is enabled for this Mac-d flow */
  uint8     non_sched_harq_alloc;

  uint8 reserved; /*!< \brief Reserved bits for aligning*/
  uint16 reserved1; /*!< \brief Reserved bits for aligning*/

}tdsmac_hs_ul_log_mac_d_flow_info_type;


typedef PACK(struct)
{

  uint8                           ddi; /*!< \brief  Data Description Identifier (6 bits)0..62 (63 is reserved) */
  uint8 reserved;
  uint16                          rlc_pdu_size_in_bits; /*!< \brief  RLC PDU size 16..5000 (size in bits) */

} tdsmac_hs_ul_log_rlc_size_ddi_info_s_type;


/*!  \brief 
   Configuration Parameters of UL Dedicated logical channel mapped to E-DCH                                                             
  */

typedef PACK(struct)
{

  
  uint8  rb_id;/*!< \brief  Radio Bearer Identifier (0-31) */

  
  uint8   rlc_id;/*!< \brief  RLC logical channel buffer identifier (0-20) */

  uint8   priority; /*!< \brief  MAC logical channel priority (1-8) */

  uint8  chan_type;  /*!< \brief  Logical channel type (DCCH or DTCH) */


  uint8  rlc_mode;  /*!< \brief  RLC mode (UM or AM) */

 
  boolean  include_in_scheduling_info; /*!< \brief  if set to false then dont consider this 
                              mac_id while sending scheduling information*/
  uint8 reserved;
  uint8                     num_rlc_size_ddi_info; /*!< \brief  number of valid fields in rlc_size_ddi_info[]  0..32*/
 
 
  tdsmac_hs_ul_log_rlc_size_ddi_info_s_type     rlc_size_ddi_info[MAX_RLC_PDU_SIZE_PER_LOGCHAN]; /*!< \brief  Mapping of rlc-pdu-size  to ddi. 
                                                                                                                Valid number of fields in following array is  equal to 'num_rlc_size_ddi_info' */

} tdsmac_hs_ul_log_logch_info_s_type ;

/*!  \brief 
 Data structure for Logical Channel configuration info
 */

typedef PACK(struct)
{


  uint8   e_mac_d_flow_id; /*!< \brief E-DCH MAC-d flow identified   0..7 */

  uint8   e_mac_d_flow_power_offset; /*!< \brief  E-DCH MAC-d flow power offset 0..6 (unit in dB) */

  uint8   e_mac_d_flow_max_num_of_retx; /*!< \brief  E-DCH MAC-d flow maximum number of  retransmissions 0..15 */

  uint8   e_mac_d_flow_max_retran_tmr;   /*!< \brief  E-DCH MAC-d flow max retransmission timer */
  uint8   e_mac_d_flow_mux_list; /*!< \brief  E-DCH MAC-d flow multiplexing list:
                                                                     Bit 0 is for MAC-d flow 0, Bit 1 is for MAC-d flow 1, …
                                                                     Value '1' for a bit means multiplexing is allowed.
                                                                    Bit 0 is Most Significant Bit(Left most bit) in the bitstring*/

  uint8   e_tx_grant_info;  /*!< \brief  transmission grant type */

  uint8 reserved;
  uint8   num_logch; /*!< \brief   number of logical channels  in e_ul_logch_info 0..15  */
  
  tdsmac_hs_ul_log_logch_info_s_type    e_ul_logch_info[MAX_DED_LOGCHAN_PER_TRCH];  /*!< \brief  number of fields valid in following array is equal to 'num_logch' */

}tdsmac_hs_ul_log_mac_d_flow_info_s_type;


/*===========================================================================

                     CONFIG  LOG PACKET DEFINITION

===========================================================================*/

/*! TDSCDMA_MAC_HS_UL_CONFIG_LOG_PACKET */
typedef PACK(struct)
{
  log_hdr_type   hdr;                     /*!< \brief Log header */
  uint8 version; /*!< \brief Inidcates the version*/

  uint8 mac_eul_action; /*!< \brief EUL Action
                                            0-  START EUL
                                            1-  RECONFIG
                                            2-  STOP   */

  uint8 mac_e_reset_ind; /*!< \brief     Indicates if MAC has to reset the TSNs
                                            0-FALSE
                                            1-TRUE   */

  uint8 mac_e_tsn_action; /*!< \brief  
                                            Inidicates the Action f the TSN have to be reverted or Backed up
                                            0-  Backup TSN
                                            1-  Revert back to previous TSN
                                            */
  
  uint16 t_rucch_timer; /*!< \brief Gives the value of T-RUCCH timer 
                                             Range (40.. 320)
                                              */
  uint16 ernti;/*!< \brief ERNTI value*/
   
  uint16  si_alone_max_retran_tmr;/*!< \brief 
                                                              Retransmission timer when SI sent alone
                                                              Range (10.. 560 )ms*/
  uint16 wait_tmr;       /*!< \brief   Wait timer
                                                  Range (5..2000)ms*/
  uint8 nrucch;   /*!< \brief   Gives value of N-RUCCH 
                                       Range 0-7*/
  uint8 mmax;    /*!< \brief   The max number of synchronisation attempts allowed.
                                       Range  1.. 32*/

 
  uint8 si_alone_max_retan;/*!< \brief Maximum number of retransmissions when SI sent alone
                                                Range 0-15  */
  uint8 non_scded_peridic_si_tmr;  /*!< \brief   T-SI-NST timer
                                                                  Range (5..200)ms*/
  uint8 periodic_si_tmr;  /*!< \brief   T-SI timer
                                              Range (5..200)ms*/
  uint8 extended_est_tmr; /*!< \brief   Extended Estimation Timer
                                                  Range (0..5)   */

  uint8 serv_cell_change;  /*!< \brief   Indicates if there is a Seving cell or Working Frequency change
                                                  0 -- True
                                                  1-FALSE*/
  uint8 nasc;   /*!< \brief     Number of available access service classes Range 0-7  
                                       Inidcates size of pval    */

  uint32 pval[MAX_ASC]; /*!< \brief Persistence value  for each ASC. Number of ASC  indicated by nasc*/

  uint16 reserved;
  uint8 ernti_change;     /*!< \brief Indicates if there is a ERNTI change
                                                  0 -- True
                                                  1-FALSE*/

  uint8     num_macd_flows;   /*!< \brief  Number of MAC-d flows configured. Range: 0..7 */
  
  tdsmac_hs_ul_log_mac_d_flow_info_s_type    e_mac_d_flow_info[TDSMAX_E_DCH_MAC_D_FLOW];/*!< \brief  E-DCH MAC-d flow */
  
}TDSCDMA_MAC_HS_UL_CONFIG_LOG_PACKET_type;

/*!  \brief 
 MAC HS CONFIG LOG PACKET SIZE
 */


/*!  \brief 
MAC HS CONFIG function call
*/
void tdsmac_hs_ul_send_config_log_packet
(
  const tdsmac_e_config_s_type   *hs_ul_config_ptr
);

/*=========================================================================

  MAC HS HEADER STTATUS LOG PACKET - LOG CODE 4322

========================================================================*/


/*!  \brief 
Bit masks for the 1st 3 bytes of the header log packet
*/
#define TDSMAC_HS_UL_HDR_LOG_NUM_SAMPLES_BMASK     0x3F
#define TDSMAC_HS_UL_HDR_LOG_NUM_SAMPLES_SHIFT     0

#define TDSMAC_HS_UL_HDR_LOG_TTI_BMASK             0x40
#define TDSMAC_HS_UL_HDR_LOG_TTI_SHIFT             6

#define TDSMAC_HS_UL_HDR_LOG_ETFCI_TABLE_BMASK     0x80
#define TDSMAC_HS_UL_HDR_LOG_ETFCI_TABLE_SHIFT     7

#define TDSMAC_HS_UL_HDR_LOG_START_FN_BMASK        0xFFF
#define TDSMAC_HS_UL_HDR_LOG_START_FN_SHIFT        0

#define TDSMAC_HS_UL_HDR_LOG_HARQ_ID_BMASK         0x70
#define TDSMAC_HS_UL_HDR_LOG_HARQ_ID_SHIFT         4

/*!  \brief 
Bit masks for happy bit and tx type
*/
#define TDSMAC_HS_UL_HDR_LOG_TX_TYPE_BMASK         0x3
#define TDSMAC_HS_UL_HDR_LOG_TX_TYPE_SHIFT         0

#define TDSMAC_HS_UL_HDR_LOG_HB_BMASK              0x4
#define TDSMAC_HS_UL_HDR_LOG_HB_SHIFT              2

/*!  \brief 
Bit masks for number of mac-es-headers and the si present field
*/
#define TDSMAC_HS_UL_HDR_LOG_MAC_ES_BMASK          0x3F
#define TDSMAC_HS_UL_HDR_LOG_MAC_ES_SHIFT          0

#define TDSMAC_HS_UL_HDR_LOG_SI_PRESENT_BMASK      0x40
#define TDSMAC_HS_UL_HDR_LOG_SI_PRESENT_SHIFT      6

/*!  \brief 
Bit masks for number of MAC-e header portion
*/
#define TDSMAC_HS_UL_HDR_LOG_HLID_BMASK            0xF
#define TDSMAC_HS_UL_HDR_LOG_HLID_SHIFT            0

#define TDSMAC_HS_UL_HDR_LOG_HLBS_BMASK            0xF0
#define TDSMAC_HS_UL_HDR_LOG_HLBS_SHIFT            4

#define TDSMAC_HS_UL_HDR_LOG_TEBS_BMASK            0x1F
#define TDSMAC_HS_UL_HDR_LOG_TEBS_SHIFT            0

#define TDSMAC_HS_UL_HDR_LOG_SI_TRIG_BMASK         0x20
#define TDSMAC_HS_UL_HDR_LOG_SI_TRIG_SHIFT         5

#define TDSMAC_HS_UL_HDR_LOG_SI_PAD_BMASK          0x40
#define TDSMAC_HS_UL_HDR_LOG_SI_PAD_SHIFT          6

#define TDSMAC_HS_UL_HDR_LOG_UPH_BMASK             0x1F
#define TDSMAC_HS_UL_HDR_LOG_UPH_SHIFT             0


/*!  \brief 
EUL Action
*/
#define TDSMAC_HS_UL_ACT_LOG_START               0
#define TDSMAC_HS_UL_ACT_LOG_RECONFIG            1
#define TDSMAC_HS_UL_ACT_LOG_STOP                2
#define TDSMAC_HS_UL_ACT_LOG_NOOP                3

/*!  \brief 
TSN Action
*/
#define TDSMAC_HS_UL_TSN_ACT_LOG_BACKUP         0
#define TDSMAC_HS_UL_TSN_ACT_LOG_REVERT         1

/*!  \brief 
 si cause 
 0-	TEBS became  More than Zero
1-	Serving Cell\Working Frequency Change
2-	ERNTI change
3-	Higher priority Data arrived
4-	Wait timer expiry
5-	Periodic SI timer expiry
6-	Quantisation of Data

*/
#define TDSMAC_HS_SI_LOG_TEBS_NOW_ABVE_ZERO       0
#define TDSMAC_HS_SI_LOG_SRVCELL_CHANGE          1
#define TDSMAC_HS_SI_LOG_ERNTI_CHANGE             2
#define TDSMAC_HS_SI_HIGH_PRIO_LOG_CHAN       3
#define TDSMAC_HS_SI_LOG_WAIT_TMR_EXPIRY          4
#define TDSMAC_HS_SI_LOG_PERIODIC_TMR_EXP         5
#define TDSMAC_HS_SI_LOG_QUANTISATION_OF_DATA     6
#define TDSMAC_HS_SI_LOG_HARQ_RETRANSMIT          7

/*!  \brief 
EUL log tmr length
*/
#define TDSMAC_HSUPA_STATUS_LOG_TIMER_BASE  100
/*!  \brief 
SI trigger reason
*/
#define TDSMAC_SI_EVENT_TRIGGER      0x1  /*!< \brief  SI was triggered due to either buffer status or periodic trigger */
#define TDSMAC_SI_PAD_TRIGGER        0x80 /*!< \brief  SI was appended due to available                                 */

/*!  \brief 
EUL log version
*/
#define TDSMAC_HS_LOG_VERSION                   1



/*!  \brief 
 MAC HS Header Type
 */
#ifdef __GNUC__
typedef PACKED struct PACKED_POST
#else /* __GNUC__*/
typedef PACKED struct
#endif /* __GNUC__ */
{
  /*!  \brief Data Description Indicator. Range: 0..62*/
  uint8 ddi;

  /*!  \brief  Transmit Sequence Number. Range: 0..63*/
  uint8 tsn;

  /*!  \brief  N = number of MAC-d PDUs in this MAC-es PDU. Range: 0..63 */
  uint16 npdus;

}tdsmac_hs_ul_log_hdr_type;


typedef PACK(struct)
{
  /*!  \brief Indicates the type of transmission 0: new transmission 1: retransmission
  2: no transmission */
  uint8         tx_type;

  /*!  \brief  ETFCI used this tti */
  uint8         etfci;

  /*!  \brief Whether mobile is happy (1) or unhappy (0) with the current SG*/
  uint8         happy_bit;

  
  /*!  \brief  Total E-DCH buffer status (see [S19, section 9.2.5.3.2] for details); range is 0 to 31 */
  uint8      tebs;
  /*!  \brief  UE power headroom (see [S19, section 9.2.5.3.2] for details); range is 0 to 31 */
  uint8      uph;
  /*!  \brief  Highest priority logical channel ID; range is 0 to 15 */
  uint8      hlid;
  /*!  \brief  Highest priority logical channel buffer status (see [S19, section 9.2.5.3.2] for details); range is 0 to 15 */
  uint8      hlbs;
  /*!  \brief  Indicates whether the next fields related to SI are present
  0: SI not present1: SI present */
  boolean       si_present;

  /*!  \brief Whether SI was triggered by buffer status or periodic trigger
  0: SI was not triggered by buffer status or periodic trigger
  1: SI was triggered due to either buffer status or periodic trigger*/
  boolean       si_triggered;

  /*!  \brief Whether SI was added due to available padding 0: SI was not appended
  due to available 1: SI was appended due to available */
  boolean       si_padding;

  uint16 reserved; 
}tdsmac_hs_ul_log_hdr_ctrl_info_type;

/*!  \brief 
 Header info per TTI
 */
typedef PACK(struct)
{
  /*!  \brief Number of MAC-es headers in this MAC-e PDU. Range: 0..62*/
  uint8                                 num_mac_es_hdrs;

  uint8 reserved; 
  uint16 reserved1; 
  /*!  \brief information related to si */
  tdsmac_hs_ul_log_hdr_ctrl_info_type      ctrl_info;

  /*!  \brief  Header data */
  tdsmac_hs_ul_log_hdr_type                hdr_data[TDSMAC_HS_UL_NUM_MAC_ES_HEADERS_IN_LOG_PKT];

}tdsmac_hs_ul_log_header_info_per_tti_type;

/*!  \brief 
 Header info per TTI
 */
typedef PACK(struct)
{
  /*!  \brief  NUM_SAMP        Number of MAC-e headers in this log packet. Range: 0..50*/
  uint8         num_samples;

  /*!  \brief  TTI 0 - 10ms, 1 - 2ms */
  uint8         tti;


  /*!  \brief  harq_id 1-8 */
  uint8         harq_id;

  /*!  \brief  ETFCI Table IDx */
  uint8         etfci_table_idx;

  /*!  \brief  Starting EUL Frame number or Subframe number for E-DPCH.
  Range: 0..255. Frame number if TTI = 10ms Range: 0..1279. Subframe number if TTI = 2ms*/
  /*!  \brief  Indicates which HARQ process ID corresponds to the
  first frame or subframe number in this log packet. Range: 0..7 */
  uint16        start_fn;
  /*!  \brief  dirty_bit is uesd to make sure that there will be no over writting of the log samples */
  boolean                                       dirty_bit;

  
  uint8 reserved; 
  /*!  \brief  Complete header information for all samples*/
  tdsmac_hs_ul_log_header_info_per_tti_type    hdr_info[TDSMAC_HS_UL_NUM_REPORTS_IN_STATUS_LOG_PKT];

}tdsmac_hs_ul_log_header_uncompressed_pkt_type;

/*!  \brief 
 Header info per TTI
 */

typedef PACK(struct)
{

  /*!  \brief  Indicates if New transmission, retransmission, EUL spacing or no grant*/
  uint8 tx_type; 
  
  /*! \brief Chosen this TTi,range 0-127 */
  uint8     etfci;

  /*! \brief NUM_MAC_HDRS  Number of MAC-es headers in this MAC-e PDU. Range: 0..62*/

  uint8     num_mac_hdrs;
  
  /*! \brief SI_PRESENT        
  Indicates whether the next fields related to SI are present
  0: SI not present 
  1: SI present
  */
  uint8     si_present;



  /*!  \brief  MAC e- headers ddi, TSN, N in that order*/
  uint8     mac_e_hdr[TDSMAC_HS_UL_NUM_MAC_E_HDRS][3];
  



  /*!  \brief HLID        Highest priority logical channel ID. Range: 0..15
  TEBS      Total E-DCH Buffer Status (see 25.321 section 9.2.5.3.2). Range: 0..31
  HLBS      Highest priority Logical channel Buffer Status (see 25.321 section 9.2.5.3.2). Range: 0..15
  UPH       UE Power Headroom (see 25.321 section 9.2.5.3.2). Range: 0..31
  SNPL   SNPL value
  SI_TRIG       Whether SI was triggered by buffer status or periodic trigger
  0: SI was not triggered by buffer status or periodic trigger
  1: SI was triggered due to either buffer status or periodic trigger
  SI_PAD        Whether SI was added due to available padding
  0: SI was not appended due to available
  1: SI was appended due to available
  */
  
  uint8     si_content[4];

}tdsmac_hs_ul_log_header_status_pkt_type;


/*!  \brief 
Log packet for reporting MAC HS header status log packet.
*/
typedef PACK(struct)
{
  
 
 
  log_hdr_type   hdr;                     /*!< \brief Log header */
  uint8  nsamp_tti_etfci_tbl;/*!< \brief         Number of MAC-e headers in this log packet. Range: 0..50 */

  uint8  start_fn;  /*!< \brief Starting EUL Frame number or Subframe number for E-DPCH. Rnage 0-255 */
  uint8 start_harq_id_start_fn;/*!< \brief START_HARQ_ID     Indicates which HARQ process ID corresponds)*/

  
  uint8 reserved; /*!< \brief reserved bits for alignment*/
  tdsmac_hs_ul_log_header_status_pkt_type  hdr_pkt_info[TDSMAC_HS_UL_NUM_REPORTS_IN_STATUS_LOG_PKT];

}TDSCDMA_MAC_HS_UL_HEADER_STATUS_LOG_PACKET_type;





/*!  \brief 
E-TFCI Info per tti
*/
typedef PACK(struct)
{

  uint8 etfci; /*!< \brief ETFCI value, range 0-127 */
  uint8 timeslot; /*!< \brief number of timeslot */
  uint8 min_etfci;  /*!< \brief Min ETFCI allowed, range 0-127 */
  uint8 max_etfci; /*!< \brief Max ETFCI allowed, range 0-127 */
  uint8 dflow_mux_list; /*!< \brief 
 					 Indicates which of the other Mac-d flows are allowed to be multiplexed along with
 					 the MAC DFLOW mapped to highest prio Logical channel. e.g., bit 0 (LSB) whether 
 					 Mac-d flow 0 is allowed to be multiplexed with this Mac-d flow.
  Bit value = 1 means that the Mac-d flow corresponding to this bit position is allowed to be multiplexed with the current Mac-d flow.
  */
  uint8 high_log_chan_id; /*!< \brief Highest Logical channel having data,range 1-16 */
  uint8 dummy_wrd;      /* !< \brief This is a Dummy word added to solve misaligned buffer exception !*/
  uint8 num_log_chan_having_data; /*!< \brief Number of Logical channel having data*/
  uint32 high_log_chan_buffer; /*!< \brief Highest logical channel buffer*/
  
}tdsmac_hs_ul_log_etfci_status_pkt_per_tti_type;





typedef PACK(struct)

{
 uint8 tx_type; /*!< \brief Indicates 
                          0 - New transmission
                          1 - retransmission
                          2 - EUL spacing
                          3 - no grant  */
 uint8 etfci_ret_status; /*!< \brief  	 Indicates the ETFCI return status 
							 0-EUL_MAC_E_PDU_ON_EPUCH,
							 1-EUL_SI_ALONE_ON_EPUCH,
							 2-EUL_GRANT_SI_ON_ERUCCH,
							 3-EUL_NON_GRANT_SI_ON_ERUCCH,
							 4-EUL_ERUCCH_GRANT_SI_AND_RTX_EPUCH,
							 5-EUL_NO_MAC_E_PDU
							 */

    uint8 si_cause;  /*!< \brief si cause
					  0-  TEBS became  More than Zero
					  1-  Serving Cell\Working Frequency Change
					  2-  ERNTI change
					  3-  Higher priority Data arrived
					  4-  Wait timer expiry
					  5-  Periodic SI timer expiry
					  6-  Quantisation of Data*/
/*!  \brief 
					  If ETFCI_RETURN_STATUS for this MAC-e header indicates that this is
						   EUL_GRANT_SI_ON_ERUCCH or
						   EUL_NON_GRANT_SI_ON_ERUCCH  or
					   EUL_ERUCCH_GRANT_SI_AND_RTX_EPUCH
					  then log ASC Info
*/
  uint8 asc_info;/*!< \brief   ASC Index chosen
					  Range (0..7) */
  uint32    sched_buf_status; /*!< \brief Indicates the current scheuled buffer status*/
  uint32    non_sched_buf_status; /*!< \brief Indicated the current Non-Scheduled buffer status*/

tdsmac_hs_ul_log_etfci_status_pkt_per_tti_type   etfci_status_info; /*!< \brief Only when data is sent on MAC-E PDU*/
}tdsmac_hs_ul_log_etfci_status_pkt_type;

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

/*!  \brief 
Log packet for reporting MAC HS etfci status log packet.
*/
typedef PACK(struct)
{

  log_hdr_type   hdr;                     /*!< \brief Log header */
  /*!  \brief         Number of MAC-e headers in this log packet. Range: 0..50 */
  uint8                                nsamp_tti_etfci_tbl;

  /*!  \brief  Starting EUL Frame number or Subframe number for E-DPCH.
                Range: 0..255. */
  uint8                                start_fn;
  /*!  \brief  Indicates which HARQ process ID corresponds
                to the first frame or subframe number in this log packet. Range: 0..7*/
  uint8                                start_harq_id_start_fn;

  uint8   reserved;/*!< \brief Reserved for alignment*/
  tdsmac_hs_ul_log_etfci_status_pkt_type  etfci_status_pkt_info[TDSMAC_HS_UL_NUM_REPORTS_IN_STATUS_LOG_PKT];
}TDSCDMA_MAC_HS_UL_ETFCI_STATUS_LOG_PACKET_type;


extern TDSCDMA_MAC_HS_UL_ETFCI_STATUS_LOG_PACKET_type   tdscdma_mac_hs_ul_etfci_sts_log;
extern TDSCDMA_MAC_HS_UL_HEADER_STATUS_LOG_PACKET_type  tdscdma_mac_hs_ul_hdr_sts_log;
extern rex_timer_type tdsmac_hsupa_tmr_log_blk;
extern void tdscdma_mac_hs_ul_etfci_status_log_packet( void);
extern void tdscdma_mac_hs_ul_header_status_log_packet( void);
extern void tdscdma_mac_hs_ul_config_log_packet(tdsmac_ul_config_type *cfgptr);
extern void tdsmac_log_fill_hs_ul_hdr_sts(
   tdsl1_eul_edch_ctrl_info_struct_type *tdsmac_eul_etfci_return_info_ptr,
   tdsl1_eul_sg_upd_info_struct_type    *sg_info_ptr);
extern void tdsmac_log_fill_hs_ul_etfci_sts(
   tdsl1_eul_edch_ctrl_info_struct_type *tdsmac_eul_etfci_return_info,
   tdsl1_eul_sg_upd_info_struct_type    *sg_info);
extern void tdsmac_log_fill_si_in_hdr_sts(uint8 hlid,uint8 tebs,uint8 hlbs,uint8 uph, uint8 snpl);


#ifdef T_WINNT
  #error code not present
#endif



/*===========================================================================

                      PUBLIC DECLARATIONS

===========================================================================*/

#endif /* FEATURE_TDSCDMA_MAC_HS_QXDM_LOGGING */
#endif /* TDSMACHSLOG_H__ */





