#ifndef MAC_UL_HS_H
#define MAC_UL_HS_H

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

              U P L I N K   M A C   H S U P A   H E A D E R    F I L E

DESCRIPTION
   MAC HSUPA specific type declarations.


Copyright (c) 2001, 2002, 2003 by Qualcomm Technologies, Inc.  All Rights Reserved.
Copyright (c) 2006-2009 by Qualcomm Technologies, Inc.  All Rights Reserved.

===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/wcdma/mac/inc/maculhs.h#1 $ $DateTime: 2016/12/13 08:00:12 $ $Author: mplcsds1 $

when       who      what, where, why
--------   -------- --------------------------------------------------------
03/12/15   kv       MAC to pass valid PO for ReTx if SG or HARQ is de-activated after First TX
12/15/14   as       In FACH state MAC should send CMAC_STATUS_IND only for TM CCCH UL data.
10/21/14   ts       SW enhancement to avoid reporting of event 6D due to persistent higher ETFCIs selected on both the carriers
07/30/14   ts       16QAM: Updated the validation check for the ETFCI table passed by RRC, in the config handler function
07/02/14   ts       Fix to STOP SI timers on Secondary Carrier Deactivation per spec CR 25.321_CR0624
05/15/14   kc       Changes for enabling RxD for FACH Signaling when HS-RACH is configured
05/13/14   ymu      Feature name FEATURE_WCDMA_DC_HSUPA_FRAME replacement with FEATURE_WCDMA_DC_HSUPA 
03/19/14   kc       Changing FEATURE for heap corruption issue.
03/18/14   ts       Fix for heap memory corruption issue, to allocate and free heap memory fo rlog buffers in l2ultask itself
02/24/14   ts       Changes for 0x430E log packet redesign
02/05/13   ts       DC-HSUPA: Fix for incorrect transmission of preambles on Primary Carrier when Secondary alone is capable of clearing data available
12/13/13   kc       Changes for Happy Bit computation code redesign
11/07/13   ts       DC-HSUPA: Changes for SI timer redesign by keeping a single base timer and reducing signals
11/06/13   as       Fix to consider all the restrictions in EUL new data transfer irrespective of SI trigger 
                    condition and should not trigger immediately when we 
06/12/13   as       HSRACH - Fix to clear TM mode partial info at MAC as well during all scenarios
                    while flushing RLC TM water mark and partial info.
05/28/13   kc       Change to save PO computed in Non-Sched PO API & use it during ETFCI selection
05/22/13   kc       Added DC-HSUPA Changes
05/03/13   kc       Remove CM interleaving workaround
04/26/13   as       MAC-I: MAC-I: MAC not to error fatal during RRC config if there is HS UL type transition 
                    without MAC-es/e reset indicator flag set to TRUE.
12/03/12   mrg      HSRACH - When EDCH resource is rleased - commit the 0x430E log packet with available log samples
10/12/12   kc       HS_RACH MAC F3's moved under debug variable and x4314 log packet issue
07/16/12   mrg      MAC_I & HS-RACH Compilation Warnings fixed
08/22/12   kc       New log packet 0x4314 for HS RACH MAC timer info
08/13/12   mrg      HS-RACH & MAC-I Feature updated code check-in.
07/24/12   geg      Upmerge WCDMA.MPSS.1.0 to WCDMA.MPSS.2.0
06/05/12   mrg      HS-RACH & MAC-I Feature code integrated
01/02/12   grk      Feature cleanup
10/22/11   sk       NikeL UL CM workaround under feature FEATURE_WCDMA_UL_CM_INTLV_WORKAROUND
12/17/10   mrg      Changes for LOW GRANT support when the UE is in DTX Call and 
                    SRB delay condition added in L1 data query interface function 
12/06/10   mrg      Reverting the CR-265483 changes 
12/03/10   mrg      LOW_GRANT and SRB_OMIT conditions are added to the L1 data query funciton
06/01/10   mrg      IRQ split changes
10/12/09   mrg      MAC_HS_UL_MAX_NUM_RLC_PDUS_PER_LC define is added
05/07/09   ssg      Updated Copyright Information
04/29/09   av       Fixed compiler warnings in L2 
04/15/09   mrg      Removing extra prototype of the funciton 
                    l1_phy_edch_get_mac_hs_ul_mac_d_si_log_info()
10/17/08   grk      Merged taxis changes to mainline.
06/03/08   kvk      Added support for new reason fields
05/02/08   sk       If the SG bits is in the range of 0 and (min_sched_rlc_pdu_size+hdr) treat it as 0
                    for purposes of quantization.
03/13/08   sk       Added support to serve equally 2 lcs with the same priority in a round robin fashion
                    across tti's
03/03/08   sk       Moved some F3's under Debug flag. Also changed the array size for
                    the prev_tx_info to 8
02/19/08   sk       Chnaged the MAC_HS_UL_MAX_HARQ_ID to 8
10/07/07   sk       Added min_rlc_pdu_size to mac_hs_ul_si_happybit_parameters_config_type
02/04/07   sk       Added support for the 0x4311 Log packet
12/20/06   am       Changed return type of mac_d_si_log_info func
12/06/06   sk       Made changes to the 4309 log packet. Added the enum fo rthe etfci retsrtcion reason
11/15/06   sk       Added HHO support
01/02/06   sk       Initial revision
09/07/06   sk       Merged all IOT fixes
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "wcdma_variation.h"
#include "comdef.h"

#ifdef CUST_H
#include "customer.h"
#endif

#ifdef FEATURE_WCDMA_HSUPA

#include "macrrcif_v.h"
#include "macinternal.h"
#include "l1macdata.h"
#include "rlcul.h"

/*===========================================================================

                     DEFINES AND MACROS FOR MODULE

===========================================================================*/
/*--------------------------------------------------------------------------
Number of MAC-d flows configured
--------------------------------------------------------------------------*/
#define MAC_HS_UL_MIN_MAC_D_FLOWS                  1
#define MAC_HS_UL_MAX_MAC_D_FLOWS                  MAX_E_DCH_MAC_D_FLOW

/*--------------------------------------------------------------------------
Ranges for MAC-d flow Id
--------------------------------------------------------------------------*/
#define MAC_HS_UL_MIN_MAC_D_FLOW_ID                0
#define MAC_HS_UL_MAX_MAC_D_FLOW_ID                (MAX_E_DCH_MAC_D_FLOW - 1)

/*--------------------------------------------------------------------------
Max TSN  value
--------------------------------------------------------------------------*/
#define MAC_HS_UL_MAX_TSN                          63

/*--------------------------------------------------------------------------
E-DCH MAC-d flow mpower offset range
--------------------------------------------------------------------------*/
#define MAC_HS_UL_MIN_POWER_OFFSET      0
#define MAC_HS_UL_MAX_POWER_OFFSET      6

/*--------------------------------------------------------------------------
E-DCH MAC-d flow maximum number of  retransmissions
--------------------------------------------------------------------------*/
#define MAC_HS_UL_MAC_D_FLOW_MAX_NUM_RETX          15

/*--------------------------------------------------------------------------
Max MAC-e PDU contents size: max number of non scheduled data
     in bits that can be part of MAC-e PDU
--------------------------------------------------------------------------*/
#define MAC_HS_UL_MAX_NON_SCHED_DATA_SIZE          19982

/*--------------------------------------------------------------------------
Since there is no header for MAC-D pdus in EUL, so rlc_pdu_size is
same as mac_d_pdu_size. The pdu size should be between 16 and 5000 bits
--------------------------------------------------------------------------*/
#define MAC_HS_UL_MIN_MAC_D_PDU_SIZE              16
#define MAC_HS_UL_MAX_MAC_D_PDU_SIZE              5000

/*--------------------------------------------------------------------------
The size of the Si is 18bits.
--------------------------------------------------------------------------*/
#define MAC_HS_UL_SI_SIZE                           18

/*--------------------------------------------------------------------------
Data Description Identifier (6 bits), it identifies the logical channel,
MAC-d flow and size of the MAC-d PDUs concatenated into the associated
MAC-es PDU. 0-62 is the valid range. 63 is reserved
--------------------------------------------------------------------------*/
#define MAC_HS_UL_MAX_DDI_VALUE                   63

/*--------------------------------------------------------------------------
Number of valid fields in mapping of rlc pdu size to ddi values. 32 is the
max
--------------------------------------------------------------------------*/
#define MAC_HS_UL_NUM_RLC_PDU_SIZE_DDI            32

/*--------------------------------------------------------------------------
Maximum Number of RLC PDUS allowed in one TTI
--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------
There are 32 entries in the TEBS value table.
--------------------------------------------------------------------------*/
#define MAC_HS_UL_MAX_TEBS_VALUES                 32

/*--------------------------------------------------------------------------
There are 15 entries in the HLBS value table.
--------------------------------------------------------------------------*/
#define MAC_HS_UL_MAX_HLBS_VALUES                 16

/*--------------------------------------------------------------------------
MAximum value for the HARQ_ID
--------------------------------------------------------------------------*/
#define MAC_HS_UL_MAX_HARQ_ID                     7

/*--------------------------------------------------------------------------
Max number of HARQ processes in case of 2ms TTI configured by RRC
--------------------------------------------------------------------------*/
#define MAC_HS_UL_MAX_NUM_HARQ_PROCESSES_2MS      8

#define MAC_HS_UL_BYTES_TO_BITS_COVERT             8

/*--------------------------------------------------------------------------
Max number of HARQ processes in case of 10ms TTI configured by RRC
--------------------------------------------------------------------------*/
#define MAC_HS_UL_MAX_NUM_HARQ_PROCESSES_10MS     4

/*--------------------------------------------------------------------------
Sec 11.8.1.4 specifies that the UE use maximum number of
HARQ transmissions of 8 for a HARQ profile for 'Control-only' transmissions
--------------------------------------------------------------------------*/
#define MAC_HS_UL_MAX_NUM_RETX_CONTROL_ONLY       7

/*--------------------------------------------------------------------------
Is there is room after E-TFCI selection adn as a result of quantization,
we can attach a special header consisting of all 1s, as in 111111
--------------------------------------------------------------------------*/
#define MAC_HS_UL_SI_SPL_HEADER                   0x3F

/*--------------------------------------------------------------------------
Each uint32 can contain the following
DDI1*N1*DDI2*N2*DDI3*N3-partial
Hence it can completely contain 2 sets of DDI,N and one complete ddi
--------------------------------------------------------------------------*/
#define MAC_HS_UL_MAX_HDR_SIZE                    32

/*  If Grant is not enough to build even 1 PDU,SI will be sent and timer will be started to re-send the SI */
#define MAC_HS_UL_LOW_GRANT_TIMER_VALUE 100

#ifdef FEATURE_WCDMA_DC_HSUPA
/* DC-HSUPA uses extended TSN field of 14 bits instead of 6 bits */
#define   MAC_HS_UL_DC_TSN_EXTRA_ETXN_BITS   8
#define   MAC_HS_UL_MAX_NUM_CARRIER_ALLOWED  2

#ifdef FEATURE_WCDMA_EVT6D_ENH
#define MAC_EV_6D_OPT_TSHLD_TABLE0                   100
#define MAC_EV_6D_OPT_TSHLD_TABLE1                   60
#define MAC_EV_6D_OPT_TSHLD_TABLE2                   87
#define MAC_EV_6D_OPT_TSHLD_TABLE3                   50
#define MAC_EV_6D_OPT_TSHLD_TABLE4                   81
#define MAC_EUL_MAX_E_TFCI_ARRAY_INDEX               5
#endif

#else
#define   MAC_HS_UL_MAX_NUM_CARRIER_ALLOWED  1
#endif

/* Table 9.2.5.3.2.1 TEBS values (ceil in bits) */

#define MAC_E_TEBS_INDEX_0 0

#define MAC_E_TEBS_INDEX_1 10<<3

#define MAC_E_TEBS_INDEX_2 14<<3

#define MAC_E_TEBS_INDEX_3 18<<3

#define MAC_E_TEBS_INDEX_4 24<<3

#define MAC_E_TEBS_INDEX_5 32<<3

#define MAC_E_TEBS_INDEX_6 42<<3

#define MAC_E_TEBS_INDEX_7 55<<3

#define MAC_E_TEBS_INDEX_8 73<<3

#define MAC_E_TEBS_INDEX_9 97<<3

#define MAC_E_TEBS_INDEX_10 129<<3

#define MAC_E_TEBS_INDEX_11 171<<3

#define MAC_E_TEBS_INDEX_12 228<<3

#define MAC_E_TEBS_INDEX_13 302<<3

#define MAC_E_TEBS_INDEX_14 401<<3

#define MAC_E_TEBS_INDEX_15 533<<3

#define MAC_E_TEBS_INDEX_16 708<<3

#define MAC_E_TEBS_INDEX_17 940<<3

#define MAC_E_TEBS_INDEX_18 1248<<3

#define MAC_E_TEBS_INDEX_19 1658<<3

#define MAC_E_TEBS_INDEX_20 2202<<3

#define MAC_E_TEBS_INDEX_21 2925<<3

#define MAC_E_TEBS_INDEX_22 3884<<3

#define MAC_E_TEBS_INDEX_23 5160<<3

#define MAC_E_TEBS_INDEX_24 6853<<3

#define MAC_E_TEBS_INDEX_25 9103<<3

#define MAC_E_TEBS_INDEX_26 12092<<3

#define MAC_E_TEBS_INDEX_27 16062<<3

#define MAC_E_TEBS_INDEX_28 21335<<3

#define MAC_E_TEBS_INDEX_29 28339<<3

#define MAC_E_TEBS_INDEX_30 37642<<3

#define MAC_E_TEBS_INDEX_31 50000<<3


/*===========================================================================

                     KEY MACROS

===========================================================================*/
#define HARQ_MASK   0xFF

/*===========================================================================

                     KEY MACROS

===========================================================================*/
#define HARQ_MASK   0xFF

/*--------------------------------------------------------------------------
harq_restriction is 8 bit bit-string. The LSB of this bit string corresponds
to the harq_id 0, the MSB corresponds to harq_id 7
Harq_id's can range from 0-7
--------------------------------------------------------------------------*/
#define MAC_HS_UL_MAC_D_FLOW_IS_ALLOWED_ON_HARQ(harq_id, harq_restriction) \
                 ((1 << (MAC_HS_UL_MAX_HARQ_ID-harq_id)) & harq_restriction)

/*--------------------------------------------------------------------------

--------------------------------------------------------------------------*/
#define MAC_HS_UL_MAC_D_FLOW_IS_ALLOWED_BY_MUX_LIST(flow_id, mux_list) \
                 ((128 >> flow_id) & mux_list)

/*===========================================================================

                     GENERAL PURPOSE MACROS

===========================================================================*/

/*--------------------------------------------------------------------------
Check for the validity of the TSN
--------------------------------------------------------------------------*/
#define MAC_HS_UL_TSN_IS_INVALID(tsn)\
      (tsn > MAC_HS_UL_MAX_TSN)

/*--------------------------------------------------------------------------
Check on the number of MAC-d flows configured
--------------------------------------------------------------------------*/
#define MAC_HS_UL_NUM_MAC_D_FLOW_IS_INVALID(num_flows)\
      ((num_flows < MAC_HS_UL_MIN_MAC_D_FLOWS) || \
       (num_flows > MAC_HS_UL_MAX_MAC_D_FLOWS))

/*--------------------------------------------------------------------------
Check on the range of the MAC-d flow id
--------------------------------------------------------------------------*/
#define MAC_HS_UL_MAC_D_FLOW_ID_IS_INVALID(flow_id)\
       (flow_id > MAC_HS_UL_MAX_MAC_D_FLOW_ID)

/*--------------------------------------------------------------------------
Check on the range for the max number of retransmissions
--------------------------------------------------------------------------*/
#define MAC_HS_UL_MAX_NUM_OF_RETX_IS_INVALID(num_retx)\
       (num_retx > MAC_HS_UL_MAC_D_FLOW_MAX_NUM_RETX)

/*--------------------------------------------------------------------------
Check on the MAC-d flow power offset values
--------------------------------------------------------------------------*/
#define MAC_HS_UL_MAC_D_FLOW_POWER_OFFSET_IS_INVALID(pwr_offset)\
       (pwr_offset > MAC_HS_UL_MAX_POWER_OFFSET)

/*--------------------------------------------------------------------------
Check to see if the configured number of bits for non-sched data is within
the valid range
--------------------------------------------------------------------------*/
#define MAC_HS_UL_NON_SCHED_DATA_SIZE_IS_INVALID(size)\
       (size > MAC_HS_UL_MAX_NON_SCHED_DATA_SIZE)

/*--------------------------------------------------------------------------
Check to see if the PDU size lies in the valid range.
--------------------------------------------------------------------------*/
#define MAC_HS_UL_MAC_D_PDU_SIZE_IS_INVALID(size)\
        ((size > MAC_HS_UL_MAX_MAC_D_PDU_SIZE) &&\
         (size < MAC_HS_UL_MIN_MAC_D_PDU_SIZE))

/*--------------------------------------------------------------------------
DDI value can't exceed 63.
--------------------------------------------------------------------------*/
#define MAC_HS_UL_DDI_VALUE_IS_INVALID(ddi_val)\
        (ddi_val > MAC_HS_UL_MAX_DDI_VALUE)

/*--------------------------------------------------------------------------
Check on the number of RLC PDU sizes to the DDI values dont exceed 32.
--------------------------------------------------------------------------*/
#define MAC_HS_UL_NUM_RLC_PDU_SIZE_DDI_IS_INVALID(size)\
        (size > MAC_HS_UL_NUM_RLC_PDU_SIZE_DDI)

/*--------------------------------------------------------------------------
Ensure that the E-TFCI table index is either 0 or 1
--------------------------------------------------------------------------*/
#ifdef FEATURE_WCDMA_16_QAM
#define MAC_HS_UL_E_TFCI_TABLE_INDEX_IS_INVALID(index) \
        ((mac_hs_ul_tti_type == E_TTI_2MS)      ?      \
         (index >= L1_EUL_NUM_ETFC_TAB_FOR_2MS) :      \
         (index >= L1_EUL_NUM_ETFC_TAB_PER_TTI))
#else
#define MAC_HS_UL_E_TFCI_TABLE_INDEX_IS_INVALID(index)\
        (index >= L1_EUL_NUM_ETFC_TAB_PER_TTI)
#endif /* FEATURE_WCDMA_16_QAM */

/*--------------------------------------------------------------------------
Ensure that the E-TFCI is in the range of 0 to ntfc
--------------------------------------------------------------------------*/
#define MAC_HS_UL_E_TFC_INDEX_IS_INVALID(etfc_index, ntfc)\
        (etfc_index >= ntfc)

/*--------------------------------------------------------------------------
Ensure that the HARQ Id is between 1-8
--------------------------------------------------------------------------*/
#define MAC_HS_UL_HARQ_ID_IS_INVALID(harq_id)\
        (harq_id > MAC_HS_UL_MAX_HARQ_ID)

#ifdef  FEATURE_MAC_I
/* enhanced logging mechanism for extended debugging */
extern uint32 mac_ul_hs_mac_i_debug_trace_level;

#define MAC_I_CHECK_SDU_SEG_NOT_PRESENT(mac_hs_ul_logical_ch_mac_d_flow_mapping, rlc_id) (FALSE == mac_hs_ul_logical_ch_mac_d_flow_mapping[rlc_id].rlc_ul_partial_info.mac_is_sdu_seg_present)
#define MAC_I_MSG_ERROR   0x01
#define MAC_I_MSG_MED     0x02
#define MAC_I_MSG_LOW     0x04
#define MAC_I_MSG_ERR_FATAL 0x08
#define MSG_HIGH_MAC_I  MSG_HIGH

#define MSG_ERR_FATAL_MAC_I   (((mac_ul_hs_mac_i_debug_trace_level & MAC_I_MSG_ERR_FATAL) > 0) ? MSG_ERROR : ERR_FATAL);
#define MSG_ERROR_MAC_I   (((mac_ul_hs_mac_i_debug_trace_level & MAC_I_MSG_ERROR) > 0) ? ERR_FATAL : MSG_ERROR);
#define MSG_MED_MAC_I   (((mac_ul_hs_mac_i_debug_trace_level & MAC_I_MSG_MED) > 0) ? MSG_HIGH : MSG_MED);
#define MSG_LOW_MAC_I   (((mac_ul_hs_mac_i_debug_trace_level & MAC_I_MSG_LOW) > 0) ? MSG_HIGH : MSG_LOW);
#endif /*   FEATURE_MAC_I */

/*===========================================================================

                     TYPE DEFINITIONS FOR MODULE

===========================================================================*/

typedef enum
{
  /* Applicable when the LC's mapped to Sched or NS flows are limited by power(MP)*/
  ETFCI_LTD_BY_TX_POWER=0,
  /* Applicable when the LC's mapped to Sched or NS flows are limited by grant 
     (either sched or NS grant) (SG)*/
  ETFCI_LTD_BY_GRANT,
  /* Applicable when the LC's mapped to Sched or NS flows are limited by buffer status(BO)*/
  ETFCI_LTD_BY_BUF_STATUS,
  /*Applicable when the LC's mapped to sched or NS flows are limited by RRC "HARQAlloclist"
     (HQR)*/
  ETFCI_LTD_BY_HARQ_RESTRICTION,
  /*Applicable when the LC's mapped to the sched or NS flows are limited by HARQ deactivation*/
  ETFCI_LTD_BY_HARQ_DEACTIVATED,
  /*Applicable when the ETFCI is restrcited by IRQ flowcontrol(LFC)*/  
  ETFCI_LTD_BY_IRQ_FC,
  /*Applicable when the ETFCI is restricted by MUX list(MUX)*/
  ETFCI_LTD_BY_MUX_RESTRICT,
  /*Applicable when ETFCI is restricted by RLC window stall(RWS)*/
  ETFCI_LTD_BY_RLC_WINDOW_STALL,
  /*Applicable when ETFCI is restricted by Max SF configuration(MSF)*/
  ETFCI_LTD_BY_MAX_SF_CFG,
  /*Applicable when ETFCI is restricted by Minimum set(MIS)*/
  ETFCI_LTD_BY_MIN_SET,
  /* Not applicable when we have both scheduled and non-sched data in the same TTI*/
  ETFCI_LTD_RE_TX,

  ETFCI_LTD_NA
}mac_hs_ul_etfci_restriction_reason_e_type;



#ifdef FEATURE_MAC_I

typedef enum
{
  MAC_NO_TRANSITION = 0,
  MAC_E_TO_I,
  MAC_I_TO_E,
  MAC_E_INVALID_TRANSITION
} mac_e_transition_e_type;

typedef struct
{
  /* 4 Pc of X and 1 Pc of Y Size 
   *          - num_complete_pdu = 4+1 ---> Total = 5, (X,4), (Y, 1)
   * 4 Pc of X and 1 Ps of Z bytes from Y bytes PDU 
   *          - num_complete_pdu = 4+1 ---> Total = 5, (X,4), (Y, 1), partial_present = TRUE, partial_size = Z 
   */
  uint32 num_total_pdu;
  uint32 num_ctrl_pdu;
  uint32 num_retx_pdu;
  uint32 num_new_data_pdu;
  uint32 complete_pdu_idx;
  uint16 num_complete_pdu[MAC_HS_UL_MAX_NUM_RLC_PDUS_PER_LC];
  uint16 complete_pdu_size[MAC_HS_UL_MAX_NUM_RLC_PDUS_PER_LC];
  uint16 retx_sn[MAC_HS_UL_MAX_NUM_RLC_PDUS_PER_LC];
  boolean partial_present;
  uint32 partial_size;
  uint32 total_lc_pdu_size;
} mac_i_flex_pdu_size_chosen;

#endif  /* FEATURE_MAC_I  */

#ifdef FEATURE_WCDMA_DC_HSUPA

typedef   enum
{
  SINGLE_CARRIER_CONFIG_TYPE =0,
  DUAL_CARRIER_CONFIG_TYPE
}mac_ul_hs_carrier_config_type;
#endif /*  FEATURE_WCDMA_DC_HSUPA  */


typedef struct
{
/* Number of HARQ processes that are active. MAC will need this for triggering SI */
  uint8     num_harq_active;
  
/*  This identifies the state of the HARQ process used for the upcoming transmission  */
  boolean   current_harq_active;
  
/* This identifies the Active and deactive HARQ process.MAC will need this for HappyBit computation */
  uint8   active_harq_bit_mask;
}mac_hs_ul_harq_active_info_type;


/*-----------------------------------------------------------------------------------------------------
There will be 3 levels of checking. Level 1 takes the highest priority over 2 and 3.
All reasons are grouped and then checked across each group as follows:

1st Level: 
  If any of the following holds true then reason will be set to that 
  given that SG is enough to build at least 1 scheduled data PDU.
     HQR
     HQD
     MUX
     RWS
2nd Level:
     Min(BO, MP, SG)  // BO takes priority over SG

3rd level:
     Min(MP, MSF, MIS, LFC) // These are listed in priority order in case more than one reason applies
---------------------------------------------------------------------------------------------------------*/
typedef struct
{
  boolean        level1_etfci_reason_flag;
  boolean        level2_etfci_reason_flag;
  
  mac_hs_ul_etfci_restriction_reason_e_type   reason;
  mac_hs_ul_etfci_restriction_reason_e_type   level1_etfci_reason;
  mac_hs_ul_etfci_restriction_reason_e_type   level2_etfci_reason;
}log_etfci_restriction_reason_info_type;

/*----------------------------------------------------------------------------
This structure gives a mapping between the priority levels and the number of
logical channels mapped at that priority level and their rlc_id's
----------------------------------------------------------------------------*/

typedef struct
{
  /* Number of logical channels at this priority level */
  uint8                     num_logical_channels;

  /* Start index for the round robin scheduling*/
  uint8                     start_idx;

  /* RLC logical channel buffer identifier (0-20) */
  rlc_lc_id_type            rlc_id[MAX_DED_LOGCHAN_PER_TRCH];

}mac_hs_ul_priority_lc_mapping_info_type;

/*----------------------------------------------------------------------------
This structure gives all the information about a MAC-d flow ID.
----------------------------------------------------------------------------*/

typedef struct
{
  /* TRUE indicates that this MAC-d flow is enabled, FALSE, otherwise */
  boolean                                mac_d_flow_enabled;

  /* E-DCH MAC-d flow power offset */
  uint8                                  power_offset; /* 0..6 (unit in dB) */

  /* E-DCH MAC-d flow maximum number of  retransmissions */
  uint8                                  max_num_of_retx; /* 0..15 */

  /* E-DCH MAC-d flow multiplexing list:
     Indicates, if this is the first MAC-d flow for which PDU's are placed in
     the MAC-e PDU, the other MAC-d flows from which MAC-d PDU's
     are allowed to be included in the same MAC-e PDU.
     Bit 0 is for MAC-d flow 0, Bit 1 is for MAC-d flow 1, …
     Value '1' for a bit means multiplexing is allowed.
     Bit 0 is Most Significant Bit(Left most bit) in the bitstring
     NOTE: The bit that corresponds to the MAC-d flow itself is ignored */
  uint8                                  mux_list; /* BITSTRING */

  /* transmission grant type */
  mac_e_tx_grant_info_e_type             tx_grant_info;

  /* below field is valid only if mac_e_tx_grant_info_e_type
     indicates NON_SCHEDULED */
  mac_non_sched_tx_grant_info_s_type     non_sched_tx_grant;

  /* Harq process restriction valied for only 2ms TTI case */
  uint8                                  harq_restriction;

}mac_hs_ul_mac_d_flow_info_type;

/*----------------------------------------------------------------------------
This structure gives the mapping between the logical channel ID and MAC-d flow
id.
----------------------------------------------------------------------------*/
typedef struct
{
  /* TRUE Indicates if this logical channel is enabled or not */
  boolean                               logical_ch_enabled;

  /* logical channel id sent by the NW - to be used in SI */
  uint8                                 logical_ch_id;

  /* E-DCH MAC-d flow identified */
  uint8                                  mac_d_flow_id; /* 0..7 */

  /* Radio Bearer Identifier (0-31) */
  rb_id_type                             rb_id;

  /* RLC mode (UM or AM) */
  uecomdef_logch_mode_e_type             rlc_mode; /* UM or AM */

  /* Logical channel type (DCCH or DTCH) */
  uecomdef_logchan_e_type                chan_type;

  /* number of valid fields in rlc_size_ddi_info[] */
  /* 0..32, 0 is valid only when action is NOOP or STOP */
  uint8                                  num_rlc_size_ddi_info;

  /* RLC Hdr size = 8 bits for UM and 16 bits for AM */
  uint8                                  rlc_hdr_length;

  /* Indicates if this channel has to be looked into for computing
  scheduling info */
  boolean                                include_in_sched_info;

  /* Priority */
  uint8                                  priority;

#ifdef FEATURE_MAC_I
  rlc_pdu_size_type  rlc_pdu_type;         /* Flexible or Fixed PDU type */
  rlc_li_e_type      ul_li_size;           /* LI size, only for AM       */
  uint16             ul_flex_min_pdu_size; /* Minimum PDU size           */
  uint16             ul_flex_max_pdu_size; /* Minimum PDU size           */
  rlc_ul_partial_info_type              rlc_ul_partial_info;
  
#endif    /* FEATURE_MAC_I */

  /* Mapping of rlc-pdu-size  to ddi. Valid number of fields in following array is
  equal to 'num_rlc_size_ddi_info' */
  mac_e_rlc_size_ddi_info_s_type         rlc_size_ddi_info[MAX_RLC_PDU_SIZE_PER_LOGCHAN];

}mac_hs_ul_logical_ch_mac_d_flow_info_type;

/*----------------------------------------------------------------------------
This structure gives the config parameters for computing the happy bit
and the scheduling information.
----------------------------------------------------------------------------*/
typedef struct
{
  /* Happy bit delay condition */
  uint16                           happy_bit_delay_in_ms; /* in ms */

  /* Periodicity for Scheduling Info - no grant: uint16.
     It can have following values:
               0: indicates that no report(scheduling information) needs to be sent,
       otherwise: periodically send scheduling information as per its value */
  uint16                           no_grant_periodicity_of_si; /* in ms */

  /* Periodicity for Scheduling Info - grant.
     It can have following values:
               0: indicates that no report(scheduling information) needs to be sent,
       otherwise: periodically send scheduling information as per its value */
  uint16                           grant_periodicity_of_si; /* in ms */

  uint8                            power_offset_of_si; /* 0..6 (in dB) */

  uint32                           min_rlc_pdu_size;

  /* Number of logical channels for which reporting has been requested */
  uint16                           num_log_ch;

  /* Identifies the logical channels Id's for which reporting has been requested*/
  rlc_lc_id_type                   rlc_id[MAX_DED_LOGCHAN_PER_TRCH];

}mac_hs_ul_si_happybit_parameters_config_type;

/*----------------------------------------------------------------------------
This structure gives the config parameters related to E-TFCI
----------------------------------------------------------------------------*/
typedef struct
{
  /* Index into the E-TFCI table */
  uint8                             e_tfci_index;

  /* This means E-TFCI in the range 0 - e_tfci_min_set_index is allowed*/
  uint8                             e_tfci_min_set_index;

  /* number of E-TFCI */
  uint32                            ntfc;

  /* The E-TFCI table pointer for the current TTI */
  const  uint16                            *etfci_ptr;

}mac_hs_ul_e_tfci_info_type;

typedef enum
{
  NON_SCHED_GRANT_LC,
  SCHED_GRANT_LC,
  BOTH_NSG_SG_LC
} mac_dchsupa_lc_type_po_e_type;

/*----------------------------------------------------------------------------
This structure stores the various parameters as a result of the power offset
computation. These results will be needed for E-TFCI selection and determining
if SI needs to be sent or not.
----------------------------------------------------------------------------*/
typedef struct
{
  /* Bitstring which identifies the MAC-d flows whose data may be present
  in the current MAC-e PDU */
  uint8                     mux_list;
  
  /* Indicates the power offset that will be sent to L1 after E-TFCI selection */
  uint8                     power_offset;
}mac_hs_ul_pwr_offset_profile;


typedef struct
{
 /* index1-PO Secondary , Index2- PO Primary, Index3-PO NSG */
 mac_hs_ul_pwr_offset_profile   po_profile[3];
 
 /* Total Scheduled Buffer Status*/
 uint32                    sched_buf_status;
 
 /* Total Non-Scheduled Buffer Status*/
 uint32                    non_sched_buf_status;
 
 /* Gives the highest priority scheduled flow's PDU size */
 uint32                    min_sched_rlc_pdu_size;

}mac_dchsupa_po_compn_result_type;


typedef struct
{

  #ifdef FEATURE_WCDMA_DC_HSUPA
  /* TRUE : DC_HSUPA is active at Layer 1; FALSE: DC_HSUPA is configured, not active*/
  boolean   is_dc_hsupa_active;
  #endif
  
  /* TRUE indicates if SI needs to be sent or not */
  boolean   si_send_status;
  
  /*True Indicates new Tx, False indicates Re-Tx */
  boolean new_tx;
  
  /* TRUE indicates if the SI is sent alone without higher layer data */
  boolean   si_sent_alone;
  
  /* Bitstring which identifies the MAC-d flows whose data may be present
  in the current MAC-e PDU */
  uint8                     mux_list;
  
  /* Indicates the power offset that will be sent to L1 after E-TFCI selection */
  uint8                     power_offset;
  
  /* Scheduled Buffer Status after considering include_in_si flag - In BITS */
  uint32                    sched_buf_status;

  /* Total Scheduled Buffer Status - In BITS */
  uint32                    total_sched_buf_status;

  /* Total Non-Scheduled Buffer Status - In BITS */
  uint32                    non_sched_buf_status;

  /* Gives the highest priority scheduled flow's PDU size */
  uint32                    min_sched_rlc_pdu_size;
  
  mac_hs_ul_harq_active_info_type            harq_l1_info;
  
  /*reason for selecting the PO as 0xFF*/
  mac_hs_ul_etfci_restriction_reason_e_type  mac_hs_ul_pwr_offset_reason;

}mac_hs_ul_pwr_offset_compn_result_type;

/*----------------------------------------------------------------------------
This structure stores the SI timer blocks and a boolean to indicate
if any timer is active or not.
----------------------------------------------------------------------------*/
typedef struct
{
  /* Timer to report SI in case of NO Grant/Low Grant/Periodic Grant condition */
  rex_timer_type                        mac_hs_ul_si_timer_blk;  

}mac_hs_ul_si_tmr_info_type;

/*----------------------------------------------------------------------------
This enum will identify what state the UE is in currently. This information
will be needed so that we dont keep triggering SI when we are on zero sg
and no timers are configured.
----------------------------------------------------------------------------*/
typedef enum
{
  MAC_HS_UL_SI_STATE_INVALID=0,
  /* There is no initial serving grant */
  MAC_HS_UL_SI_STATE_ZERO_SG,
  /* There is initial serving grant */
  MAC_HS_UL_SI_STATE_NON_ZERO_SG

}mac_hs_ul_si_state_e_type;

typedef struct
{
  /* State in the previous TTI */
  mac_hs_ul_si_state_e_type prev_state;
  /* State in the curr TTI */
  mac_hs_ul_si_state_e_type curr_state;

}mac_hs_ul_si_state_type;

typedef struct
{
  /* To start the Timer with rex_set_timer()*/
  boolean       start_timer;
  /* To keep track of timer if it is active or not */
  boolean       timer_active;
  /* To keep track if the timer is expired */
  boolean       timer_expired;
  /* Configured timer value stored here */
  uint16        config_count;
}mac_hsupa_si_tmr_count_info_type;

typedef struct
{
  boolean                             si_triggered;
  boolean                             si_padded;
  boolean                             si_sent_alone;
  /* Highest priority data that is obtained from RLC in the previous transmission */
  uint8                               prev_priority_lc_data;
  /* Indicates If in this TTI data from a high priority LC arrived - valid only in NSG state */
  boolean                             higher_priority_data_arrived;

  mac_hs_ul_si_state_type             si_state;
  mac_hs_ul_si_tmr_info_type          si_timer_blk;
  
  /* TSI_G */  
  mac_hsupa_si_tmr_count_info_type    si_grant_timer;
  /* TSI_NG */
  mac_hsupa_si_tmr_count_info_type      si_no_grant_timer;
  /* TSI_LG */
  mac_hsupa_si_tmr_count_info_type      si_low_grant_timer;
}mac_hsupa_si_tmr_info_type;

/*----------------------------------------------------------------------------
This structure stores the E-TFC payload in bits, Power Offset and the
serving grant that was used in the current TTI.
----------------------------------------------------------------------------*/
typedef struct
{
  /* Indicates the power offset that was used in the current transmission */
  uint8         power_offset;

  /* Indicates the maximum number of retransmissions */
  uint8         max_num_retx;

  /* Indicates the E-TFCI selected for this TTI */
  uint8         etfci;

  /* Serving grant available in the current tti*/
  uint32        serving_grant_payload_txed;

  /* Total payload that can be transmitted based on the available UE Tx power */
  uint32        total_ue_tx_pwr_payload;

  /* This was the serving grant available from L1 */
  uint32        serving_grant_bits;

  /* TRUE indicates SI was sent with some higher layer data */
  boolean       si_sent_with_data;

}mac_hs_ul_prev_harq_tx_info_type;

/*----------------------------------------------------------------------------
This structure stores results of the E-TFCI selection.
----------------------------------------------------------------------------*/
typedef struct
{
  /* TRUE indicates if SI needs to be sent or not */
  boolean   si_send_status;
  
  /* TRUE indicates if SI is padded in a TB */
  boolean   si_padded;
  
  /* TRUE indicates if the special SI header has to be included */
  boolean   special_si_header;

  /* TRUE indicates if the SI is sent alone without higher layer data */
  boolean   si_sent_alone;

  /* TRUE indicates that start the no grant periodicity timer. This
  timer is actually started when the E-TFCI selection is done. */
  boolean   si_start_no_grant_tmr;

  /* TRUE indicates that start the grant periodicity timer. This
  timer is actually started when the E-TFCI selection is done. */
  boolean   si_start_grant_tmr;

  /* Indicates the amount of bits to be padded */
  uint32    padding;

}mac_hs_ul_etfci_seln_results_type;

typedef struct
{
  boolean   cur_harq_active;
  mac_hs_ul_si_state_e_type   si_curr_state;
  uint8     cur_rrc_sched_harq_restriction;
  uint8     cur_active_harq_bit_mask;
  uint8     etfc_max_sf_index;
  uint32    cur_serving_grant;
  
  #ifdef  FEATURE_WCDMA_DC_HSUPA
  uint8     prim_car_rrc_harq_restriction;
  uint8     prim_car_active_harq_bit_mask;
  uint32    prim_car_serving_grant;
  
  uint8     sec_car_rrc_harq_restriction;
  uint8     sec_car_active_harq_bit_mask;
  uint32    sec_car_serving_grant;
  #endif
  
  /* Actual amount of data transmitted this tti */
  uint32    scheduled_data_txed;
  
  /* E-TFCI selected this tti. In case of re-transmission this will
  refer to the E-TFCI chosen for the previous transmission for the
  same HARQ process */
  uint32    etfci_bits;
  
  /* Maximum bits that can be Tx this TTI based on available UE tx power */
  uint32    ue_total_tx_pwr_payload;
}mac_hs_ul_happy_bit_params_type;

/*----------------------------------------------------------------------------
This structure stores information about the DDI and the corresponding
RLC PDU size and the number of PDUs that has to be built for each
logical channel.
----------------------------------------------------------------------------*/
typedef struct
{
#ifdef FEATURE_MAC_I 
  /* This field indicates the length of the mac_e_hdr_info. 
   * This is needed in case of MAC-I, as multiple (LC,L) combinations possible for same LC
   * This is NOT needed in case of MAC-e, as only one (DDI,N) combination exists for a given LC
   */
  uint16                    mac_e_hdr_info_len;
#endif
  /* Indicates the DDI that has to be attached to the MAC header */
  uint8                     mac_e_hdr_info[MAX_MAC_E_HDR_SIZE_BYTES];
  /* Information about the number of PDUs and the PDU size for each logical channel */
  rlc_ul_frame_format_type  rlc_pdu_info;

}mac_hs_ul_rlc_pdu_ddi_info_type;

#ifdef FEATURE_WCDMA_DC_HSUPA
typedef struct
{
  boolean                      sec_carrier_active;
  boolean                      tsn_field_extn_flag;
  uint8                        e_tfci_min_set;
  uint8                        harq_restriction;
  l1_eul_carr_idx_enum_type    l1_carrier_indx;
  mac_sec_eul_action_e_type    eul_action;
  uint32                       non_sched_buf_status;
  uint32                       total_non_sched_grant;
  /* Holds PO for Non-sched mac-d flows when BOTH carriers are active */
  uint8                        non_sched_harq_po;
  /* Used to save HB computation params on secondary carrier */
  mac_hs_ul_happy_bit_params_type  hb_params;
  /*Holds Happy Bit on Sec carrier -  If predetermined to be TRUE during
    secondary carrier ETFCI selection */
  boolean                        hb_on_sec_carr;
  uint32                         hb_total_buf_status;
  uint32                         hb_pdu_size;
}mac_hs_ul_sec_carrier_info_type;
#endif /*  FEATURE_WCDMA_DC_HSUPA  */

/*----------------------------------------------------------------------------
This structure will contain the loop limits based on the configuration
that MAC gets
----------------------------------------------------------------------------*/

typedef struct
{
  /* Loop start index when looping over priority*/
  uint16    min_mlp;

  /* Loop end index when looping over priority*/
  uint16    max_mlp;

  /* Loop start index when looping over logical channels*/
  uint16    min_rlc_id;

  /* Loop end index when looping over logical channels*/
  uint16    max_rlc_id;

}mac_hs_ul_loop_limits_cfg;

typedef  struct
{
  /* Bitstring : To identifies MAC-d flows whose data presentin in current MAC-e PDU */
  uint8                     mux_list;
  
  /* Indicates the power offset that will be sent to L1 after E-TFCI selection */
  uint8                     power_offset;
  
  /* Total Scheduled Buffer Status - In BITS */
  uint32                    sched_buf_status;
  
  /* Gives the highest priority scheduled flow's PDU size */
  uint32                    min_sched_rlc_pdu_size;
  
   /* Reason for selecting Invalid PO - 0xFF */
   mac_hs_ul_etfci_restriction_reason_e_type  pwr_offset_reason;
}mac_hs_ul_store_po_result_info_type;

typedef struct
{
  uint8    carrier_index;
  uint8    harq_id;
  uint8    etfc_idx;
  uint8    etfc_min_set_index;
  uint8    etfc_max_sf_index;
  uint32   remaining_avl_payload;
  uint32   scheduled_bits_available;
  uint32   total_ns_payload;
  uint32   size_of_si;
  #ifdef FEATURE_HSUPA_NEW_ETFCI_ALGORITHM
  uint32   quantization_loss;
  #endif
  uint32   mac_e_pdu_size;
  mac_hs_ul_etfci_seln_results_type      etfci_si_result_info;
  mac_hs_ul_store_po_result_info_type    po_result_info;
  mac_hs_ul_happy_bit_params_type        happybit_param_info;
  l1_eul_serving_grant_update_info_type  *l1_sg_info_ptr;
  mac_hs_ul_rlc_pdu_ddi_info_type        *bfi_rlc_info_ptr;
  mac_hsupa_si_tmr_info_type             *si_timer_info_ptr;
}mac_hs_ul_etfc_common_info_type;

typedef struct
{
  boolean    pdu_built_flag;
  boolean    tmp_log_harq_r;
  boolean    tmp_log_harq_d;
  boolean    tmp_log_harq_mux;
  boolean    tmp_log_rws;
} mac_hs_ul_etfci_reason_info_type;

#if defined( FEATURE_L1_LOGGING_ENABLED ) && defined( FEATURE_HSUPA_L1_LOGGING_ENABLED)

#ifdef __GNUC__
typedef PACKED struct PACKED_POST
#else /* __GNUC__*/
typedef PACKED struct 
#endif /* __GNUC__ */
{
     /*NUM_POW_LIMITED:Number of TTIs transmission is limited by power*/
 uint8 num_tx_limited_by_pwr;

 /*NUM_SG_LIMITED:Number of TTIs transmission is limited by serving grant*/
 uint8 num_tx_limited_by_sg;

 /*NUM_BUF_LIMITED:Number of TTIs transmission is limited by available buffer
   status*/
 uint8 num_buf_limited;

 /*NUM_HARQ_RESTRICTION: Number of TTIs transmission is limited by HARQ restriction*/
 uint8  num_harq_restriction;

 /*NUM_HARQ_DEACT: Number of TTIs transmission is limited by HARQ Deactivated*/
 uint8  num_harq_deactivated;

 /*NUM_IRQ_FC: Number of TTIs transmission is limited by IRQ FC*/
 uint8  num_irq_fc;

 /*NUM_MUX_RESTRICTION: Number of TTIs transmission is limited by MUX restriction*/
 uint8 num_mux_restriction;

 /*NUM_RLC_WIN_STALL: Number of TTIs transmission is limited by RLC window stall*/
 uint8  num_rlc_win_stall;

 /*NUM_MAX_SF_CFG: Number of TTIs transmission is limited by MAX SF cfg*/
 uint8 num_max_sf_cfg;

 /*NUM_MIN_SET: Number of TTIs transmission is limited by MIN set*/
 uint8 num_min_set;

 /*NUM_BUF_EMPTY:Number of TTIs both sched + non-sched buffers were empty*/
 uint8 num_buf_empty;

 /* NUM_S_BUF_EMPTY:Number of TTIs total scheduled buffer was empty*/
 uint8 num_sched_buf_empty;

 /*NUM_NS_BUF_EMPTY:Number of TTIs total non-scheduled buffer was empty*/
 uint8 num_non_sched_buf_empty;

 /*SUM_S_BUF:SUM of schedule buffer bytes status for newtx  divided by 64 is sent
   to diag and Mean scheduled buffer status in bytes (value of 0xFFFF means
   more than 65535 bytes) is displayed
 */
 uint32 sum_sched_buf_bytes;

 /*SUM_NS_BUF:SUM of non schedule buffer bytes status for newtx  divided by 64 is
   sent to diag and Mean non-scheduled buffer status in bytes (total over all NS flows)
   is displayed*/
 uint32 sum_non_sched_buf_bytes;

 /* NUM_SI:Number of SIs sent */
 uint8  num_si_sent;
}mac_hs_ul_l1_mac_stats_info_struct_type;

/*----------------------------------------------------------------------------
This structure stores the values needed for logging the 0x4311 Log packet.
The accumulated log packet is logged at L1. MAC provides all the following
information to L1.
----------------------------------------------------------------------------*/

typedef struct
{
/*MAC_RESETS    1   Number of MAC-e resets*/
uint8    num_mac_e_resets;

/*Sum of the number of transmitted scheduled bits per TTI, divided by 64, counting only new transmissions*/
uint32    sum_sched_bits_txed;

/*Sum of the number of transmitted non-scheduled bits per TTI, divided by 64, counting only new transmissions*/
uint32    sum_non_sched_bits_txed;

/*Sum of the number of bits that could have been transmitted per TTI based on available buffer status (scheduled flows only) divided by 64,
counting only new transmissions NOTE While summing up over all TTIs: For all TTIs for which the buffer status is greater than the max TB size,
use the max TB size instead of the actual buffer status in the summation.*/
uint32   sum_buf_bits;

/*mac continuous data as in the 4311 logpkt*/
mac_hs_ul_l1_mac_stats_info_struct_type continuous_mac_info;

}mac_hs_ul_log_mac_stats_struct_type;

#endif

/*===========================================================================

                         GLOBAL FUNCTIONS

===========================================================================*/

/*===========================================================================
FUNCTION: MAC_HS_UL_CONFIG_REQ_HANDLER

DESCRIPTION:
This function is called when some HSUPA related configuration is given by
RRC to MAC.  The uplink configuration is absolute.  Both HSUPA and Non-HS
configuration is obtained in the same command.  RRC will specify if HS
configuration has changed or not.  If HS configuration has changed then
this function will be called to apply the configuration. By defualt whenever
MAC receives the CMAC_UL_CONFIG_REQ it will apply the new confirguration
for Non-HS channels.

DEPENDENCIES:
  If at this point any validations fail, MAC err_fatals so that these
  can be corrected.

RETURN VALUE:
  None

SIDE EFFECTS:

  MAC internal tables are populated.
  Hence if any configuration validation fails MAC HSUPA config will not
  be reliable. Its upto RRC to make MAC come to a sane state.
===========================================================================*/
void mac_hs_ul_config_req_handler
(
  mac_ul_config_type    *cfgptr
);

/*===========================================================================
FUNCTION: mac_hs_ul_init

DESCRIPTION:
  This function performs the intialization of some global data and creates
  timers for the triggers needed for scheduling information. This function is
  called when the mac ul task is created

DEPENDENCIES:
  None

RETURN VALUE:
  None

SIDE EFFECTS:
  All internal variables maintained by MAC are initialized
===========================================================================*/
void mac_hs_ul_perform_init(boolean is_power_up);

#ifdef FEATURE_MAC_HS_UL_QXDM_LOGGING
/*===========================================================================
FUNCTION: mac_eul_allocate_mem_log_records

DESCRIPTION:
This function dynamically allocates memory for EUL 0x430E log record pointer
===========================================================================*/
void mac_eul_allocate_mem_log_records(void);
#endif

 /*===========================================================================
 FUNCTION mac_hs_ul_rlc_pdu_size_sort_comp_fn
 
 DESCRIPTION
   This function is the comparison function for the bubble sort utility in L1
   math utils. It takes void pointer, casts them to mac_e_rlc_size_ddi_info_s_type 
   struct type and compares the rlc_pdu_sizes.
 
 DEPENDENCIES
   None
 
 RETURN VALUE
   1 - if argument1 pdu size > argument2 pdu size
   0 - otherwise
 
 SIDE EFFECTS
   None
 ===========================================================================*/
 
int8 mac_hs_ul_rlc_pdu_size_sort_comp_fn(
  /* The two rlc_size_ddi_info to be compared */
  const void *rlc_size_ddi_info1, const void *rlc_size_ddi_info2);

 boolean    mac_hs_ul_query_si_triggered( l1_eul_pwr_offset_req_struct_type  pwr_offset_info,
                                          mac_hsupa_si_tmr_info_type            *si_state_info_ptr,
                                          mac_hs_ul_prev_harq_tx_info_type      *prev_harq_tx_info_ptr,
                                          uint32                            sched_buf_status,
                                          boolean                           trig_si_tmr_valid);

/*===============================================================================================
 FUNCTION NAME  : mac_hs_ul_ignore_ul_srbs

 DESCRIPTION  : This function clears the buffer status of UL Signal Radio Bearers

 RETURN VALUE:
  Num of Non-schd bytes mapped on SRBs at present TTI
===============================================================================================*/
uint32    mac_hs_ul_ignore_ul_srbs
(
  /* Uplink Buffer Status Pointer */
  rlc_ul_buf_status_type        *rlc_ul_buf_status
);

/*===============================================================================================
 FUNCTION NAME  : mac_hs_ul_logpacket_info

 DESCRIPTION  : This function 
===============================================================================================*/
#ifdef FEATURE_MAC_HS_UL_QXDM_LOGGING
void mac_hs_ul_logpacket_info
(
  uint8   harq_id
);

void  mac_hs_ul_mace_etfci_log_buf_change(uint16  num_samples);
void  mac_hs_ul_mace_hdr_log_buf_change(uint16  num_samples);

#endif

/*===============================================================================================
 FUNCTION NAME  : buffer_status_info_logging

 DESCRIPTION  : This function 
===============================================================================================*/

void buffer_status_info_logging(void);

#ifdef FEATURE_WCDMA_HS_RACH
rlc_ul_lc_info_type *get_mac_hs_ul_lc_info_ptr(void);
mac_hs_ul_logical_ch_mac_d_flow_info_type *get_mac_hs_logch_macd_flow_mapping_ptr(void);

#ifdef FEATURE_MAC_HS_UL_QXDM_LOGGING
  /*===========================================================================
  FUNCTION mac_hsrach_submit_timer_log_pkt
  
  DESCRIPTION This function enqueues a command to L2 UL task 
                command queue to send mac timer log info to Diag buffers.
  ===========================================================================*/
void mac_hsrach_submit_timer_log_pkt(void);
/*==========================================================================
FUNCTION: mac_update_hs_rach_log_pkt_info

DESCRIPTION: Fields of HS RACH log packet are updated here.
==========================================================================*/
void mac_update_hs_rach_log_pkt_info 
( 
  boolean rsrc_avail, 
  l1_mac_hs_rach_edch_rsrc_rel_cause_enum_type rel_cause 
);

#endif /* FEATURE_MAC_HS_UL_QXDM_LOGGING */

#endif /* FEATURE_WCDMA_HS_RACH */

void mac_hs_ul_eul_action_stop_handler
(
#ifdef FEATURE_WCDMA_HS_RACH 
  mac_hs_rach_action_e_type   hs_rach_action
#else
  void
#endif
);

void   mac_hs_ul_init_si_timers_powerup( mac_hsupa_si_tmr_info_type*  si_timer_info_ptr);
void   mac_hs_ul_init_sec_car_si_timers_powerup( mac_hsupa_si_tmr_info_type*  si_timer_info_ptr);

void   mac_hs_ul_init_si_timers( mac_hsupa_si_tmr_info_type*  si_timer_info_ptr, boolean stop_si_timer);
void   mac_hs_ul_set_si_timer_conifg_values( uint16 t_si_grant,
                                             uint16 t_si_no_grant,
                                             mac_hsupa_si_tmr_info_type*  si_timer_info_ptr);
  
void   mac_hs_ul_initialize_globals_before_etfci_selection( l1_eul_serving_grant_update_info_type *sg_info_ptr ,
                                                   mac_hs_ul_etfc_common_info_type       *etfc_common_info_ptr,
                                                   l1_eul_etfci_info_type                 *eul_etfci_info_ptr,
                                                   mac_hs_ul_pwr_offset_compn_result_type  *po_compn_result_ptr,
                                                   uint8                                  carrier_index);

#ifdef FEATURE_MAC_I 

/* Clear all the partial related information for each logical channel
 * Useful during MAC-I to MAC-E transition
 */
void mac_hs_ul_clear_mac_i_partial_info( void );

void   mac_hs_ul_maci_log_buf_change(void);

/*===========================================================================
FUNCTION      mac_hs_ul_mac_i_config_handler

DESCRIPTION   Following actions are completed
              1. If the current config is RECONFIG
                  - Update the mac_hs_ul_transition_type based on current  
                    and reconfig pdu type.
                  - Check MAC HS UL Reset as the queues need to be flushed
                    (25.331 - 8.6.6.37 E-DCH Info)
                     
              2. Update the Current pdu type
===========================================================================*/
void mac_hs_ul_mac_i_config_handler(mac_ul_config_type    *cfgptr);

/*-------------------------------------------------------------------
FUNCTION MAC_HS_UL_CLEAR_RLC_ID_PARTIAL_INFO

DESCRIPTION
  API to clear stored partial info for the passed RLC - ID
  
DEPENDENCIES
  
RETURN VALUE
  none

SIDE EFFECTS
  
--------------------------------------------------------------------*/
void mac_hs_ul_clear_rlc_id_partial_info(rlc_lc_id_type rlc_id);

#endif /* FEATURE_MAC_I */

#ifdef FEATURE_WCDMA_HS_RACH
void mac_hsrach_reset_tsn_during_rsr_release(void);

rlc_ul_lc_info_type *get_mac_hs_ul_lc_info_ptr(void);

mac_hs_ul_logical_ch_mac_d_flow_info_type *get_mac_hs_logch_macd_flow_mapping_ptr(void);

void mac_hsrach_config_timers_init(uint8 add_e_dch_tx_backoff, uint8 max_collision_resol,
                                   uint8 max_e_dch_res_alloc_ccch ,
                                   l1_e_dch_tx_cont_backoff_enum_type   l1_e_dch_tx_cont_backoff );

void mac_hsrach_config_timers_stop(void);

void l1_mac_hsrach_get_mac_config_params(l1_mac_hs_rach_config_params *config_params);


void mac_hsrach_phychan_estblish_failure_handler(void);

void mac_hsrach_edch_res_release_cleanup(void);

void mac_hs_ul_fill_ernti_special_mac_i_hdr(mac_hs_ul_rlc_pdu_ddi_info_type  *bfi_rlc_info);

//Tebs timer count is zero and enough SG to include SI0 with data
void mac_hsrach_tebs_timer_expiry_during_build_pdu(void);

boolean mac_hsrach_tebs0_si_trrigger_query(void);

/* Buffer status computation and also determines the LC_TYPE in HS_RACH */
boolean  mac_hsrach_buffer_status(boolean *data_present_on_srb);

void   mac_hsrach_tebs_timer_update_during_po(uecomdef_logchan_e_type   chan_type,rlc_lc_id_type  rlc_id);

void mac_hsrach_edch_tx_backoff_timer_update(l1_mac_hsrach_eul_timer_status_enum status);

uint32 mac_hsrach_si0_content(uint32    uph);

#endif /* FEATURE_WCDMA_HS_RACH */
void mac_hs_ul_hb_computation
(
  uint8 index,
  l1_eul_edch_control_info_type *mac_control_info_ptr,
  mac_hs_ul_happy_bit_params_type  *hapy_bit_params_ptr
);
#endif /* FEATURE_WCDMA_HSUPA */
#endif /* MAC_HS_UL_H */
