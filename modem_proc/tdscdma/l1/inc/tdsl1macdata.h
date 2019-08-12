#ifndef TDSL1_MAC_DATA_H
#define TDSL1_MAC_DATA_H

/*============================================================================
                            L1 DATA INTERFACE 


GENERAL DESCRIPTION
  This files contains all necessary control and data definitions for 
  interactions between L1 and MAC. It defines the format for packing uplink 
  transport channel data and control information for transmission. It also 
  defines transport block formats that aid in decoding downlink physical 
  channels. Interfaces dealing with data flow between MAC and L1 are also 
  defined.
  
EXTERNALIZED FUNCTIONS

INTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS  
  

Copyright (c) 2000 - 2011 by Qualcomm Technologies Incorporated.  All Rights Reserved.

============================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/tdscdma/l1/inc/tdsl1macdata.h#1 $ 
$DateTime: 2016/12/13 07:58:19 $ 
$Author: mplcsds1 $

when        who     what, where, why
--------    ---     -------------------------------------------------------- 
04/25/12    mr      Fixed Compiler warning
02/29/12    owen    Change FEATURE_HSDPA_MAC_EHS to FEATURE_TDSCDMA_HSDPA_MAC_EHS 
03/03/11    cdf     Change TDS_L1_UL_MAC_MAX_HDR_SIZE to 6 from 5
02/02/11    ps      Added the interface for MAC/L1 for HSUPA
10/13/00    tds     file created.
                    
===========================================================================*/

/*===========================================================================

                      INCLUDE FILES

===========================================================================*/

#include "tdscdma_variation.h"
#include "comdef.h"
#include "customer.h"
#include "dsmutil.h"
#include "rex.h"
#ifdef FEATURE_TDSCDMA_A2_HW_DL
#include "a2_common.h"
#endif
#include "tdsl1std.h"
#include "tdsl1sapcommon.h"
#include "tdsl1macif.h"

/*===========================================================================

                        FEATURE DEPENDENCIES

===========================================================================*/

/* FEATURE_MCAL_TDSCDMA has to be enabled for FEATURE_TDSCDMA_DL_DATA_PATH_OPT */
#if defined(FEATURE_TDSCDMA_DL_DATA_PATH_OPT) && (!defined(FEATURE_MCAL_TDSCDMA))
#error "FEATURE_TDSCDMA_DL_DATA_PATH_OPT defined without FEATURE_MCAL_TDSCDMA"
#endif

//#if defined(FEATURE_WCDMA_UL_DATA_PATH_OPT) && (!defined(FEATURE_TDSCDMA_HSUPA))
/* FEATURE_TDSCDMA_HSUPA has to be enabled for FEATURE_WCDMA_UL_DATA_PATH_OPT */
//#error "FEATURE_WCDMA_UL_DATA_PATH_OPT defined without FEATURE_TDSCDMA_HSUPA"
//#endif


/*===========================================================================

                      DOWNLINK DATA DECLARATIONS

===========================================================================*/
/* -----------------------------------------------------------------------
** Constant / Define Declarations
** ----------------------------------------------------------------------- */

/* This is the maximum number of transport block set for which the transport
   block headers can be reported by L1 to L2 for decoding
   Note that size defined is based on the maximum amout that can be
  reported by mDSP. SW has to process that and should have that much capacity.
  This is not as per UE capability specified in standard */
#define TDS_L1_DEC_MAX_TB_SET_HDR  32

/* 32 R99 Max TB ina frame for all TTI ending at this frame (No double buffering
   Max 70 PDU per sub frame Number of sub frames to cover are 5 (in 10 ms) +
   2 (To cover interrupt latency
   Total transport block (MAC-d PDUs) will be 32 + 7*70 = 524 */
#define TDS_L1_DEC_MAX_TB          524

/* Number of word 32 data reads required for transport block header */
#define TDS_L1_DEC_TB_HDR_SIZE_W32 3
/* Number of word 32 data reads required for HS-DSCH MAC-d PDU */
#define TDS_L1_DEC_HS_MAC_D_HDR_SIZE_W32 1

#ifdef FEATURE_TDSCDMA_PWR_TFC_ELMN
#define TDS_PWR_TFC_ELMN_X        15
#define TDS_PWR_TFC_ELMN_Y        30  /* shall be multiples of 15 */
#define TDS_PWR_TFC_ELMN_Y_FRAME   2  /* Y/15 */
#endif

/* DOB Extension buffer size based on which MAC would indicate
   appropriate number of P-to-Cs to be formed. */
#define TDSL1_DEC_DOB_EXTN_BUF_SIZE MCALTDS_DEC_DOB_EXTN_BUF_SIZE

/* Number of P-to-Cs that MAC could indicate L1 to concatenate in one shot. */
//#define L1_DEC_MAX_COMPLETE_PDU_FROM_PREV_TTI MCALTDS_DEC_MAX_COMPLETE_PDU_FROM_PREV_TTI

#ifdef FEATURE_TDSCDMA_DL_DATA_PATH_OPT
/* Number of TSN info buffers. This value is exported for MAC to use. */
#define TDS_UE_NUM_TSN_INFO_BUF MCALTDS_DEC_MAX_TSN_INFO_BUF
#endif /* FEATURE_TDSCDMA_DL_DATA_PATH_OPT */

/* -----------------------------------------------------------------------
** Type Declarations
** ----------------------------------------------------------------------- */

typedef enum
{
  TDSL1_MAC_SUSPEND,
  TDSL1_MAC_RESUME
} tdsl1_mac_suspend_resume_enum_type;

#if 0
#ifdef FEATURE_MCAL_TDSCDMA
typedef mcaltds_dec_tb_tm_cipher_state_enum_type  tdsl1_tb_tm_cipher_state_enum_type;
#else
typedef enum
{
  TDSL1_TB_TM_CIPHER_NOT_APPLICABLE,
  TDSL1_TB_TM_CIPHER_START,
  TDSL1_TB_TM_CIPHER_CONT,
  TDSL1_TB_TM_CIPHER_STOP,
  TDSL1_TB_TM_CIPHER_SINGLE
} tdsl1_tb_tm_cipher_state_enum_type;
#endif
#endif

#if 0
#ifdef FEATURE_TDSCDMA_DL_DATA_PATH_OPT
typedef mcaltds_dec_tsn_trace_enum_type tdsl1_dec_tsn_trace_enum_type;
#endif
#endif

/* This strcuture defines the multisegmented TB buffer to be
   defined by MAC */
typedef struct
{
  /* Number of TB buffer segment */
  uint8 num_seg;
  
  /* Size of each TB segment */
  uint16 seg_size;
  
  /* Pointer to the array of pointer of Tb buffer segment */
  uint8 **buf;
} tdsl1_dl_tb_buf_multi_seg;

/* Indication whether data being delivered to L2 belongs to
   current TTI or previous TTI or both. */
//typedef mcaltds_dec_mac_hs_tti_type tdsl1_dl_hs_tti_enum_type;

#if 0
/* This structure defines the ciphering parameters for a transport block
   header information of a transport block. This is included in transport
   block info structure (tdsl1_dl_tb_hdr_struct_type). */
#ifdef FEATURE_MCAL_TDSCDMA
typedef mcaltds_dec_ciphering_param_struct_type  tdsl1_dl_ciphering_param_struct_type;
#else
typedef struct
{
  /* Ciphering status returned by L2 for this transport block. */
  boolean ciphering_control;
  
  /* Ciphering bit offset returned by L2 of each transport block. */
  uint8 ciphering_bit_offset;
  
  /* Ciphering key id returned by L2 of this transport block. */
  uint8 ciphering_key_id;
  
  /* Ciphering count (CFN/SFN) returned by L2 of this transport block. */
  uint32 ciphering_count;
  
  /* Ciphering radio bearer id returned by L2 for this transport block. */
  uint8 ciphering_radio_bearerid;
} tdsl1_dl_ciphering_param_struct_type;
#endif
#endif

#if 0
/* This structure defines the transport block header information for a
   transport block. This is included in transport block set structure
   (tdsl1_dl_tb_set_hdr_struct_type) that accommodates all transport blocks */
#ifdef FEATURE_MCAL_TDSCDMA
typedef mcaltds_dec_tb_hdr_struct_type  tdsl1_dl_tb_hdr_struct_type;
#else
typedef struct
{
  /* TB Id. This is set and maintained by MAC */
  uint16  tb_id;
  
  /* Valid flag for this TB. If MAC finds any problem with this TB
    it will set this TB valid flag to FALSE otherwise it will be TRUE */
  boolean valid;
  
  /* TB index of this TB */
  uint8 tb_idx; 
  
  /* Bit offset from the starting DOB location of the transport block.
    The offset specified in in the number of bits. The L2 function that
    decode the header, reports back the offset to read the transport
    block data in this strcuture member. */
  uint8 num_bit_offset_trblk;
  
  /* ciphering parameters for this transport block */
  tdsl1_dl_ciphering_param_struct_type ciphering_param;
  
  /* Pointer to the transport block header */
  uint8 tb_header[TDS_L1_DEC_TB_HDR_SIZE_W32 * 4];
} tdsl1_dl_tb_hdr_struct_type;
#endif
#endif

/* Access macros for featurized structure member.
   It provides access to non_hs members access based on feature definition */

#ifdef __GNUC__

 #define TDSL1_DL_TB_SET_HDR(hdr_struct_ptr,member) \
  hdr_struct_ptr->info.non_hs.member

#else /* __GNUC__ */

 #define TDSL1_DL_TB_SET_HDR(hdr_struct_ptr,member) \
  hdr_struct_ptr->info.non_hs.##member

#endif /* __GNUC__ */

#if 0
/* This structure defines the information of all transport block headers
   information for all transport block sets for which the headers need to be
   decoded and information is required to get the data from the hardware. */
#ifdef FEATURE_MCAL_TDSCDMA
typedef mcaltds_dec_tb_hdr_decode_struct_type  tdsl1_dl_tb_hdr_decode_struct_type;
#else
typedef struct
{
  /* Number of transport block set for which the TrBlk header is to be
    decoded for all transport blocks in them. This is the number of
    decoder status FIFO size. There is also the TrCh and TF Ids for
    each FIFO entry. */
  uint8 num_tb_set;
  
  /* transport block set information for all the transport block headers
    in each transport block set */
  tdsl1_dl_tb_set_hdr_struct_type tb_set_hdr[TDS_L1_DEC_MAX_TB_SET_HDR];
} tdsl1_dl_tb_hdr_decode_struct_type;
#endif
#endif

#if 0
/* This structure defines the data element for a transport block in a 
   transport block set. This is included in the structure
   (tdsl1_dl_tb_set_data_struct_type) */
#ifdef FEATURE_MCAL_TDSCDMA
typedef  mcaltds_dec_tb_data_in_tbset_struct_type  tdsl1_dl_tb_data_in_tbset_struct_type;
#else
typedef struct
{
  /* TB Id. This is set and maintained by MAC */
  uint16  tb_id;
  
  /* RLC PDU size of the data in this TB in bits.
     In case of loopback mode 2 for the DCH TrCh the data size will
     include the CRC length also */
  uint16 data_size;
  
  /* data buffer size is bytes used to accomodate the RLC PDU bits */
  uint16 buffer_size_bytes;
  
  /* TB data buffer for various memory allocation scheme.
    In case the TB buffer is allocated by MAC the buffer pointer
    is not reported by TB_DATA_IND */
  
  dsm_item_type *tb_data_buffer;
} tdsl1_dl_tb_data_in_tbset_struct_type;
#endif
#endif

#if 0
/* This structure defines the data and status element for a transport block
   set. This is included in the structure (tdsl1_dl_tb_data_struct_type) which 
   is used for sending the data and status information for all transport
   block sets to L2. */
#ifdef FEATURE_MCAL_TDSCDMA
typedef mcaltds_dec_tb_set_data_struct_type  tdsl1_dl_tb_set_data_struct_type;
#else
typedef struct
{
  /* TrCh reference type for the transport block set */
  tdsl1_dl_trch_ref_type_enum_type trch_ref_type;

  /* Number of transport blocks in this transport block set. */
  uint8 num_tb;
  
  /* transport block size for the transport block of this trblk set */
  uint16 trblk_size;
  
  /* points to the array of the TB info structure */
  tdsl1_dl_tb_data_in_tbset_struct_type *tb_data_info;
} tdsl1_dl_tb_set_data_struct_type;
#endif
#endif

#if 0
/* This structure defines the transport block information that is used to get
   the transport block headers decoded and update the transport block
   information to the L2 buffers */
#ifdef FEATURE_MCAL_TDSCDMA
typedef mcaltds_dec_tb_data_struct_type  tdsl1_dl_tb_data_struct_type;
#else
typedef struct
{
  /* Number of transport block set for which the data and status is to be
    transferred to L2. */
  uint8 num_tb_set;
  
  /* Data and status for all transport blocks sets */
  tdsl1_dl_tb_set_data_struct_type tb_set_data[TDS_L1_DEC_MAX_TB_SET_HDR];

} tdsl1_dl_tb_data_struct_type;
#endif
#endif

#ifdef FEATURE_TDSCDMA_DL_DATA_PATH_OPT
/* This structure defines the HS transport block information passed to
   MAC and L2 during Data Read Event handling. */
typedef tdsl1_dec_tsn_info_type tdsl1_dec_hs_tsn_info_type;

#ifdef FEATURE_TDSCDMA_HSDPA_MAC_EHS
//typedef mcaltds_dec_concatenated_pdu_tsn_type tdsl1_dec_concatenated_pdu_tsn_type;

/* Move-in task source and destination descriptors */
/* MAC will populate the source descriptor list
 * which spans across all PDUs for the 10ms TTI */
extern uint32
  dec_src_descriptor_move_in[2*TDSL1_DEC_MAX_DMOV_SOURCE_DESCRIPTOR_LEN_MOVE_IN] __attribute__((aligned(8)));


/* L1 will determine the destination address and
 * populate the destination descriptor list */
extern uint32
  dec_dst_descriptor_move_in[2*TDSL1_DEC_MAX_DMOV_DESTINATION_DESCRIPTOR_LEN_MOVE_IN] __attribute__((aligned(8)));
#endif /* FEATURE_TDSCDMA_HSDPA_MAC_EHS */

#endif /* FEATURE_TDSCDMA_DL_DATA_PATH_OPT */

/*===========================================================================

                      UPLINK DATA DECLARATIONS

===========================================================================*/


  /* maximum # of TrCHs in a 10 ms frame */
#define TDSL1_UL_MAX_TRCH         TDSUE_MAX_TRCH

  /* MAC header size in bytes */
#define TDS_L1_UL_MAC_MAX_HDR_SIZE 6

/*+ Updated by uplink  2010-11-31*/
/* MDSP return values corresponding these enums. So, the order is important */
typedef enum
{
  TDSL1_PRACH_NO_ACK,   /* FPACH ACK was not received */
  TDSL1_PRACH_ACK,      /* FPACH ACK was received */
  TDSL1_PRACH_ABORT,
  TDSL1_PRACH_MSG_DONE, /* RACH MSG TX done */
  TDSL1_PRACH_DELAY
} tdsl1_prach_status_enum_type;
/*- Updated by uplink  2010-11-31*/

/* uplink transport block format (control + data) */
typedef struct {
     /* pointer to transport block data */
#ifdef L1_UL_USE_DSM
  dsm_item_type *data_ptr;
    /* MAC header data */
  uint8          mac_hdr[TDS_L1_UL_MAC_MAX_HDR_SIZE];
#else
    /* for test purposes only */
  uint8         *data_ptr;
    /* MAC header data */
  uint8         *mac_hdr;
#endif
    /* size of MAC header in bits (0..39)*/
  uint8          mac_hdr_size;
    /* ciphering flag for this block */
  boolean        ciphering_on;
  /*#ifdef FEATURE_WCDMA_L1_TM_CIPHER_SUPPORTED*/
  /* TM ciphering state */
  tdsl1_tb_tm_cipher_state_enum_type tm_cipher_state;
  /*#endif*/
    /* radio bearer id */
  uint8          radio_bearer_id;
    /* ciphering key */
  uint8          ciphering_key_id;
    /* combination of hyper frame num and RLC seq num */
  uint32         count;
    /* offset in transport block from where ciphering should start */
  uint8          offset;
 /* Ciphering Algorithm: Kasumi or Snow3G*/
  uint32 cipher_algo;

} tdsl1_ul_tb_data_type;

/* transport channel data */
typedef struct {
    /* number of transport blocks - also indicates no TrCH if zero */
  uint8              num_tb;
    /* transport block size in bits (0..5120) */
  uint16             tb_size;
    /* transport channel data */
  tdsl1_ul_tb_data_type trch_data[TDSL1_UL_MAX_NUM_TB];
} tdsl1_ul_trch_data_type;

/* Tx data (for a 10ms frame) + control information */
typedef struct {
    /* number of transport channels */
  uint8                num_trch;
    /* flag to indicate if MAC is busy writing this buffer */
  boolean              mac_busy;
    /* TFCI used for this 10ms frame of Tx data */
  uint16               TFCI;
    /* data of all transport channels in the 10ms frame */
  tdsl1_ul_trch_data_type tx_data[TDSL1_UL_MAX_TRCH];
    /* ASC index valid for PRACH only */
  uint8                asc_num;
#ifdef FEATURE_TDSCDMA_DSDA
    /* SRB data present or not in this TTI- TRUE means SRB data present */
  boolean              srb_data_present;
 
    /* Indicates number of TTI in which SRB data would go */
  tdsl1_tti_enum_type     srb_data_num_tti;
#endif
  /* flag to indicate if this is new rach tx or rach re-tx */
  boolean              new_rach_tx;
} tdsl1_ul_tx_data_type;


#ifdef FEATURE_TDSCDMA_HSUPA
typedef struct
{
  /*----------------------------------------------------------------
  False indicates MAC doesn't need to do anything PERHAPS happy bit
  computation.Edch build frame indication will not be called in case
  of FALSE
  ----------------------------------------------------------------*/
  boolean                   new_transmission;

  /*----------------------------------------------------------------
  The value as number of bits indicates the maximum payload that can
  carry traffic from scheduled MAC-d flows. Values in range 0-20000
  0 indicates no serving grant is available
  ----------------------------------------------------------------*/
  uint16                    srv_grant_payload_bits;


  /*----------------------------------------------------------------
  The value as E-TFCI indicates the maximum payload that can carry
  traffic from both scheduled, non-scheduled MAC-d flows. This also
  includes the Scheduling information. Values in range 0-127
  0xFF indicates no power is available to transmit any E-DCH data
  ----------------------------------------------------------------*/
  uint8                     ue_tx_pwr_max_payload_idx;


  /*----------------------------------------------------------------
  This indicates the HARQ process ID that will be used for the
  following transmission.
  ----------------------------------------------------------------*/
  uint8                     harq_id;


  /*----------------------------------------------------------------
  TRUE indicates that for the DCH a non-empty TFCI was selected
  ----------------------------------------------------------------*/
  boolean                   dpdch_present;

  /*----------------------------------------------------------------
  Maximum E-TFCI allowed by current configuration from UTRAN
  ----------------------------------------------------------------*/
  uint8                     max_allowed_etfci;

  /*----------------------------------------------------------------
  This bit mask is only for logging purposes. This bit mask indicates whether the binary
  search in the R_combo table ended up in overflow because of SG or MP. This means
  that the SG or MP can support more number of bits than the configuration can.
  #define L1_SGP_OVERFLOW_IND_BMSK 0x1
  #define L1_MP_OVERFLOW_IND_BMSK 0x2
  ----------------------------------------------------------------*/

  #if defined( FEATURE_TDSCDMA_L1_LOGGING_ENABLED )
  uint8 sg_mp_overflow_ind_bmsk; 
  #endif

} tdsl1_eul_serving_grant_update_info_type;

/*------------------------------------------------------------------
This structure gives the HARQ profile corresponding to the HARQ ID
of the upcoming transmission.
------------------------------------------------------------------*/
typedef struct
{
  /*----------------------------------------------------------------
  Indicates the power offset of the highest priority logical
  channel carrying data. Value in the range 0-6
  ----------------------------------------------------------------*/
  uint8     harq_power_offset;

  /*----------------------------------------------------------------
  Maximum number of retransmissions allowed in this HARQ Id
  value in the ramge 0-15
  ----------------------------------------------------------------*/
  uint8     max_number_of_retrans;

}tdsl1_eul_harq_profile_type;


typedef struct
{
  /*----------------------------------------------------------------
  Chosen e-tfci for this tti. Value in the range 0-127. 0xFF indicates
  no TFCI was selected
  ----------------------------------------------------------------*/
  uint32                     etfci;

  /*----------------------------------------------------------------
  Scheduled number of bits corresponding to the scheduled payload
  that was transmitted in this TTI
  Case 1 if SG was 0 then Sched_num_bits would be set to 0.
  Case 2 If SG > 0 but was not enough to build
  even one MAC-d pdu from the highest priority logical channel, then
  MAC will pretend that it sent the scheduling grant number of bits
  ----------------------------------------------------------------*/
  uint32                     sched_num_bits;

  /*----------------------------------------------------------------
  In case of a e-tfci was chosen, then the harq_profile gives
  the power offset and the max number of retransmissions allowed
  using this HARQ id.
  ----------------------------------------------------------------*/
  tdsl1_eul_harq_profile_type  harq_profile;

}tdsl1_eul_etfci_info_type;

typedef struct
{
  /*----------------------------------------------------------------
  TRUE indicates UE was happy with the grant.
  ----------------------------------------------------------------*/
  boolean                   happy_bit_indicator;

  /*----------------------------------------------------------------
  Information regarding the chosen TFCI adn the corresponsding
  harq profile.
  ----------------------------------------------------------------*/
  tdsl1_eul_etfci_info_type    eul_etfci_info;

  #ifdef  FEATURE_TDSCDMA_PLT
  /*----------------------------------------------------------------
  WPLT module overrides RSN for testing only.
  0xFF indicates that WPLT does NOT want to override RSN.
  ----------------------------------------------------------------*/
  uint8                     rsn;

  /*----------------------------------------------------------------
  TRUE indicates that WPLT wants L1 to force a UL E-DCH CRC error
  ----------------------------------------------------------------*/
  boolean                   force_crc_error;
  #endif /*  FEATURE_TDSCDMA_PLT */

} tdsl1_eul_edch_control_info_type;

typedef struct
{
  /* num of bits in unit */
  uint32 data_unit_len;

  /* pointer to data payload */
  uint32* data_unit_ptr;

  /* Is this unit the last unit in this TTI or not */
  boolean last_unit_of_tti;

  /* cipher enabled for this unit or not */
  boolean cipher_enabled;

  /* Below control parameters are valid only if cipher_enabled=TRUE */

  /* L1 will subtract 1 for HW */
  uint32 radio_bearer_id;

  /* cipher key index */
  uint32 cipher_key_index;

  /* offset in bits to start ciphering */
  uint32 cipher_offset;

  /* counter for ciphering */
  uint32 cipher_count_c;

} tdsl1_eul_edch_data_unit_struct_type;

/* Maximum possible number of E-TFC's in a E-TFC table */
#define L1_UPA_UL_MAX_NUM_ETFC_PER_TAB  128

/* 2 E-TFC tables for 10 ms TTI and 2 ms TTI respectively */
#define L1_EUL_NUM_ETFC_TAB_PER_TTI  2

/* struct type for one E-TFC table */
typedef struct
{
  /* Number of E-TFC in this table, up to L1_UPA_UL_MAX_NUM_ETFC_PER_TAB */
  uint32  num_etfc;

  /* E-TFC table of TB size */
  uint16  etfc_tb_size_table[L1_UPA_UL_MAX_NUM_ETFC_PER_TAB];
} tdsl1_upa_ul_etfc_table_struct_type;

/*struct type for power offset req*/
typedef struct
{
  /*----------------------------------------------------------------
  Indicates the harq id used for the upcoming tx
  ----------------------------------------------------------------*/
  uint8     harq_id;

  /*----------------------------------------------------------------
  Indicates if the serving grant was 0 or not
  ----------------------------------------------------------------*/
  boolean   sg_available;

  /*----------------------------------------------------------------
  Identifies the number of HARQ processes that are active.
  MAC will need this for triggering scheduling information
  ----------------------------------------------------------------*/
  uint8     num_harq_active;

  //#ifdef FEATURE_WCDMA_HSUPA_2MS_TTI
  /*----------------------------------------------------------------
  Bitmask value of the active HARQ process.
  The bit position in the active_harq_bitmask corresponds to the active HARQ process
  ----------------------------------------------------------------*/
  uint8      active_harq_bit_mask;
  //#endif

  /*----------------------------------------------------------------
  This identifies the state of the HARQ process used for the
  upcoming transmission.
  ----------------------------------------------------------------*/
  boolean   per_harq_active;

  /*----------------------------------------------------------------
  TRUE indicates ACK was received from the serving RLS. In case FALSE
  MAC will generate a SI depending on if the SI was sent alone
  or along with data.
  ----------------------------------------------------------------*/
  boolean   ack_recvd_srv_rls;
  
  /* TRUE indicates that the NACK was received from the serving RLS
     for max tx times      */
   boolean   nack_recvd_srv_rls_max_tx_times;

  /* SRB0~SRB4 data should not be transmitted during SRB delay */
  boolean   srb_omit;

  /*----------------------------------------------------------------
  False indicates it is a re transmission and TRUE indicates it is a
  new transmission.
  ----------------------------------------------------------------*/
  boolean new_tx;
  
  //#ifdef FEATURE_WCDMA_HSUPA_2MS_TTI 
  /* Flag to indicate if the CM is present or not */
  boolean dtx_flag;
 // #endif
  
} tdsl1_eul_pwr_offset_req_struct_type;


#if defined( FEATURE_TDSCDMA_L1_LOGGING_ENABLED )

typedef PACKED struct PACKED_POST {
     /*NUM_POW_LIMITED:Number of TTIs transmission is limited by power*/
 uint8 num_tx_limited_by_pwr;

 /*NUM_SG_LIMITED:Number of TTIs transmission is limited by serving grant*/
 uint8 num_tx_limited_by_sg;

 /*NUM_BUF_LIMITED:Number of TTIs transmission is limited by available buffer
   status*/
 uint8 num_buf_limited;

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
 uint16 sum_sched_buf_bytes;

 /*SUM_NS_BUF:SUM of non schedule buffer bytes status for newtx  divided by 64 is 
   sent to diag and Mean non-scheduled buffer status in bytes (total over all NS flows)
   is displayed*/
 uint16 sum_non_sched_buf_bytes;

 /* NUM_SI:Number of SIs sent */
 uint8  num_si_sent;
}tdseul_log_l1_mac_stats_mac_info_struct_type;

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
uint16    sum_sched_bits_txed;

/*Sum of the number of transmitted non-scheduled bits per TTI, divided by 64, counting only new transmissions*/
uint16    sum_non_sched_bits_txed;

/*Sum of the number of bits that could have been transmitted per TTI based on available buffer status (scheduled flows only) divided by 64,
counting only new transmissions NOTE While summing up over all TTIs: For all TTIs for which the buffer status is greater than the max TB size,
use the max TB size instead of the actual buffer status in the summation.*/
uint16   sum_buf_bits;

/*mac continuous data as in the 4311 logpkt*/
tdseul_log_l1_mac_stats_mac_info_struct_type continuous_mac_info;

}tdsl1_eul_mac_stats_struct_type;

/* Bit mask to indicate to MAC when the number of bits supported by SG is greater than
** that supported by MAX_CFG*/
#define L1_SGP_OVERFLOW_IND_BMSK 0x1
/* Bit mask to indicate to MAC when the number of bits supported by MP (Power) is greater than
** that supported by MAX_CFG*/
#define L1_MP_OVERFLOW_IND_BMSK 0x2

#endif/*FEATURE_TDSCDMA_L1_LOGGING_ENABLED*/

#endif /* FEATURE_TDSCDMA_HSUPA */

/* Macros to access important l1_ux_data_type elements */
/* Ciphering flag */
#define TDS_L1_TX_BUF_CIPH_FLAG(trch_ptr, tb_id) \
        ((trch_ptr)->trch_data[tb_id].ciphering_on)
/* MAC header size */
#define TDS_L1_TX_BUF_MAC_HDR_SIZE(trch_ptr, tb_id) \
        ((trch_ptr)->trch_data[tb_id].mac_hdr_size)
/* MAC header data pointer */
#define TDS_L1_TX_BUF_MAC_HDR_DATA(trch_ptr, tb_id, offset) \
        (*(uint32 *)&((trch_ptr)->trch_data[tb_id].mac_hdr[offset << 2]))

#ifndef L1_UL_USE_DSM
/* MAC data pointer */
#define TDS_L1_TX_BUF_DATA(trch_ptr, tb_id, offset) \
        (*(uint32 *)&((trch_ptr)->trch_data[tb_id].data_ptr[offset << 2]))
#else
/* MAC DSM data pointer */
#define TDS_L1_TX_BUF_DSM_DATA_PTR(trch_ptr, tb_id) \
        ((trch_ptr->trch_data[tb_id].data_ptr))
/* MAC DSM data */
#define TDS_L1_TX_BUF_DSM_DATA(dsm_item_ptr, offset) \
        (*(uint32 *)&(dsm_item_ptr->data_ptr[offset]))
#endif


/*===========================================================================

                      DOWNLINK FUNCTIONS

===========================================================================*/

/*===========================================================================
FUNCTION tdsl1_phy_dl_trblk_header_ind

DESCRIPTION
  This function exists in the L2 and is used by L1 to update the TrBlk header
  information to MAC/RLC and get the TrBlk data offset and ciphering parameters
  (if the TrBlks are ciphered).

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  The members of the structure pointed in the function argument, get the
  update to the values after L2 decodes the header information.
===========================================================================*/

extern void tdsl1_phy_dl_trblk_header_ind
(
  tdsl1_dl_tb_hdr_decode_struct_type *tb_hdr_decode_info 
    /* TrBlk header information to get it decoded */
);

/*===========================================================================
FUNCTION tdsl1_phy_dl_trblk_data_ind

DESCRIPTION
  This function exists in the L2 and is used by L1 to update the TrBlk data
  information to MAC/RLC.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

extern void tdsl1_phy_dl_trblk_data_ind
(
  tdsl1_dl_tb_data_struct_type *tb_data_info /* TrBlk data information */
);

#ifdef FEATURE_TDSCDMA_DL_MODEM_ACC
/*===========================================================================
FUNCTION tdsl1_phy_dl_trblk_hs_data_ind

DESCRIPTION
  This function exists in the L2 and is used by L1 to update the HSDPA 
  TrBlk data information to MAC/RLC.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void tdsl1_phy_dl_trblk_hs_data_ind(tdsl1_dl_hs_tti_enum_type tti_type);

#ifdef FEATURE_TDSCDMA_A2_HW_DL
/*===========================================================================
FUNCTION  l1_dl_phy_hspa_gather_and_decipher

DESCRIPTION
  This function is invoked by Mac-hs in its task context to perform
  P-to-C operation. MAC provides a list of P-to-Cs and the IOVEC information
  which are passed to A2 driver by L1.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

extern void tdsl1_dl_phy_hspa_gather_and_decipher
(
  /* P-to-C information for each partial-to-complete PDU */
#ifdef FEATURE_TDSCDMA_HSDPA_MAC_EHS
  tdsl1_dec_rlc_pdu_concatenate_info_struct_type *mac_p_to_c_ptr,
#endif
  /* IOVEC information for each partial PDU */
  a2_iovec_t a2_dec_src_iovec_list[]
);
#endif /* FEATURE_TDSCDMA_A2_HW_DL */

#endif /* FEATURE_TDSCDMA_DL_MODEM_ACC */

/*===========================================================================
FUNCTION l1_phy_sfn_decode_ind

DESCRIPTION
  This function exists in the L3 (RRC) and is used to get the SFN decoded
  for the neighbor BCH TrBlk.

DEPENDENCIES
  None

RETURN VALUE
  Decoded SFN for the BCH TrBlk.

SIDE EFFECTS
  None
===========================================================================*/

extern word tdsl1_phy_sfn_decode_ind
(
  byte *bch_trblk   /* TrBlk data information */
);

/*===========================================================================

                      UPLINK FUNCTIONS

===========================================================================*/

#if 0
/*==============================================================================================
 FUNCTION NAME  void mac_update_hfn_on_rollover

 DESCRIPTION : This function is called 2 times by layer 1. Once at the time when Layer 1 is
 suspended and there is a W->G handover. This function is called again when Layer 1 is 
 resumed (G->W this may happen if the reconfiguration fails or...).  
 
 cfn : indicates the current value of the CFN
 num_roll_overs : The number of roll-overs that occureed during the time L1 was suspended
 to the time the Layer 1 resumed .
 
 If the reconfiguration took 
   a) < 2.5 s ==> the variable num_roll_overs = 0
   b) 2.5s <= reconfig_time < 5.12s ===> num_roll_overs = 1
      ....
MAC based on the number of roll overs that ocurred would increment
its hfn count.

===============================================================================================*/
#if 0
void mac_update_hfn_on_rollover
(
  uint8 cfn, 
  uint8 num_roll_overs,
  tdsl1_mac_suspend_resume_enum_type l1_suspend_state
); 
#endif 
#endif

/*===========================================================================
FUNCTION tdsl1_phy_ul_build_frame_ind

DESCRIPTION
  This function is defined by L2 and builds Tx data for the specified TTI and
  action time. The data is filled by L2 in a transmnit buffer owned by L1.

DEPENDENCIES
  None

RETURN VALUE
  TRUE  Build Frame indication accepted by MAC
  FALSE Build Frame indication rejected by MAC

SIDE EFFECTS
  None
===========================================================================*/
extern boolean tdsl1_phy_ul_build_frame_ind
(
    /* current tti */
  tdsl1_tti_enum_type     tti,        
    /* next next frame's CFN */
  uint8                cfn,        

  boolean        ul_tx_disabled,     

  #ifdef FEATURE_TDSCDMA_PWR_TFC_ELMN
  /* array of TFCI states for power based TFC elimination */
  const tdsl1_ul_tfc_pwr_state_enum_type  *tfc_excs_pwr_state_ptr,
  #endif

#if defined(FEATURE_TDSCDMA_IDLEINTERVAL_INFO_SUPPORT) || defined(FEATURE_TDSCDMA_DMO_SUPPORT) || defined(FEATURE_TDSCDMA_DSDS_QTA) 
	boolean *allowed_tfci_list,
#endif

  boolean need_srb_status,
  
  /* pointer to Tx data buffer */
  tdsl1_ul_tx_data_type  *tx_buf_ptr  
);

/*===========================================================================
FUNCTION tdsl1_phy_ul_trch_status_ind

DESCRIPTION
  This function is defined by L2 and processes the transmit status of a
  PRACH transmission.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void tdsl1_phy_ul_trch_status_ind
(
    /* PRACH transmit status as informed by mDSP */
  tdsl1_prach_status_enum_type   status  
);

/*===========================================================================
FUNCTION  L1_PHY_UL_CFN_MISSED_IND

DESCRIPTION
  This function notifies MAC that CFN update interrupt was missed due to
  long period of TDS_INTLOCK().

DEPENDENCIES
  DCH state when ENC detects missed CFN update interrupt.

RETURN VALUE
  None

SIDE EFFECTS
  MAC should sync-up or reset.
===========================================================================*/
extern void tdsl1_phy_ul_cfn_missed_ind
(
  uint8 curr_cfn  
);

#ifdef FEATURE_TDSCDMA_HSUPA



/*===========================================================================
FUNCTION  L1_EDCH_QUERY_MAC_PWR_OFFSET_INFO

DESCRIPTION
This function will return the power offset corresponding to the highest
priority logical channel carrying data mapped onto non-scheduled MAC-d flows
incase when SG_available is FALSE and the PO of  overall highest priority
logical channel carrying data if SG_Available is TRUE


DEPENDENCIES

Serving grant function should have been complete before calling
this function

RETURN VALUE

  Power OFfset.

  Boolean to indicate if SI was sent alone

SIDE EFFECTS

  The returned poweroffset will be used for the mapping of T/P to payload.

===========================================================================*/
extern uint8 tdsl1_edch_query_mac_pwr_offset_info
(
  tdsl1_eul_pwr_offset_req_struct_type mac_pwr_offset_info
);

/*===========================================================================
FUNCTION    L1_EDCH_REQ_CONTROL_INFO

DESCRIPTION
L1 calls the function with serving grant information as the input parameter.
MAC will perform the E-TFCI selection based on this information.


DEPENDENCIES

Mapping of T/P to payload should have been done prior to this function call.

RETURN VALUE
  Chosen E-TFCI and the HARQ profile information

SIDE EFFECTS

===========================================================================*/
extern tdsl1_eul_edch_control_info_type* tdsl1_edch_req_control_info
(
  tdsl1_eul_serving_grant_update_info_type *sg_info_type
);


/*===========================================================================
FUNCTION        L1_EDCH_SERVING_CELL_CHANGE_IND

DESCRIPTION
This function is called by L1 after L1 gets reconfigured due to an Active
set update command. TRUE indicates that serving cell has changed.

DEPENDENCIES

This function will be called only if the serving cell change is detected
by L1 based on the ASET message recvd frm the nw.

RETURN VALUE
  None

SIDE EFFECTS

===========================================================================*/
extern void tdsl1_edch_serving_cell_change_ind
(
  boolean   srv_cell_changed
);

/*===========================================================================
FUNCTION  L1_EUL_GET_SRV_CELL_CHANGE_IND

DESCRIPTION
  This function is called by MAC and is needed for SI triggering

  Quote from spec
    11.8.1.6.2 Report Triggering when SG <> 'Zero_Grant' and at least one 
               process is activated

      If an E-DCH serving cell change occurs and if the new E-DCH serving 
      cell was not part of the previous Serving E-DCH RLS, the transmission 
      of a Scheduling Information shall be triggered.

 In the following scenarios L1 returns TRUE

 1: Simple SHO/Serving Cell Change with Change of Serving RLS
 2: HHO: Irrespective of the RGComb Index for the previous and the new Serving 
    Cell (even if they are the same) SI should be triggered
 3: SHO/SCC when SRGCH is not configured before or after the Serving Cell 
    Change:
      This is the E// configuration <this is the case that has been discussed
      in this thread  look for Anils response>
 4: SHO/SCC where NS_RGCH is de-configured but RG Combination index is re-used
    for S_RGCH (we don't expect to see this practically, but our 
    implementation should address this):


  NOTE: (4) above is not TRUE currently because of the complications involved.
 
DEPENDENCIES

RETURN VALUE
  Boolean TRUE indicates that serving cell change occurred

SIDE EFFECTS
  none
===========================================================================*/


/*===========================================================================
FUNCTION        L1_EDCH_WRITE_ERAM_DATA_UNIT

DESCRIPTION
  After L1 calls tdsl1_phy_edch_build_frame_ind(), MAC and RLC will call this
  function to write TX data to EUL ERAM bank in HW.

DEPENDENCIES
  After L1 calls tdsl1_phy_edch_build_frame_ind(), MAC and RLC will call this
  function to write TX data to EUL ERAM bank in HW.
  The first call of this function in a TTI must be from MAC for MAC-es header
  only so that L1 can save the MAC-es header len.
  The last call of this function in a TTI must set the last cipher unit bit.

RETURN VALUE
  None

SIDE EFFECTS
  E-DCH TX data will be writen to HW
===========================================================================*/
/*
 *  REV1: L1 will concatenate all units into one E-DCH TB on the fly with
 *        book keeping of unit ending bit position
 *  REV2: write directly to HW
 */
void tdsl1_edch_write_eram_data_unit
(
  tdsl1_eul_edch_data_unit_struct_type *edch_data_unit_ptr
);

/*===========================================================================
FUNCTION        L1_PHY_EDCH_BUILD_FRAME_IND

DESCRIPTION
  This function is called by L1 to trigger MAC/RLC to build E-DCH MAC-e PDU.

DEPENDENCIES
  This function is called by L1 to trigger MAC/RLC to build E-DCH MAC-e PDU.

RETURN VALUE
  none

SIDE EFFECTS
  E-DCH TX data will be writen to HW via L1 wrappers during this function
===========================================================================*/
void tdsl1_phy_edch_build_frame_ind
(
  uint32 harq_id,
  uint32 etfci,
  uint32 frame_number, // cfn for 10 ms TTI; cfn*5+sub_frame_number for 2 ms TTI
  uint32 uph, // UE power headroom value
  //#ifdef FEATURE_WCDMA_UL_DATA_PATH_OPT
  tdsl1_l2_data_type *data_buf
  //#endif /* FEATURE_WCDMA_UL_DATA_PATH_OPT */
);


#if defined( FEATURE_TDSCDMA_L1_LOGGING_ENABLED )
/*===========================================================================
FUNCTION        L1_PHY_EDCH_GET_MAC_HS_UL_MAC_D_SI_LOG_INFO

DESCRIPTION

This function is called by L1 whenever they need to send the status log packet
to diag services.

DEPENDENCIES



RETURN VALUE
MAC returns the following which is populated in the L1 structure
Log packet 4309
mac-d present   - For each Mac-d flow (max 8) whether it was present in this TTI or not.
   For example, bit 0 (LSB) indicates whether MAC-d flow 0 is present in this transport block.
0: this Mac-d flow is absent
1: this Mac-d flow was present
Si_present  - Indicates if scheduling information was present  in this TTI

Returns SI present field.

SIDE EFFECTS

===========================================================================*/
extern uint8 tdsl1_phy_edch_get_mac_hs_ul_mac_d_si_log_info
(
  uint8     *mux_list,
  uint8     *etfci_reason
);

/*==========================================================================
FUNCTION: L1_EDCH_UPDATE_MAC_LOG_PACKETS_DURING_RECONFIG

DESCRIPTION:
This function will be called by L1 whenever there is a reconfig
happening. In seam less reconfig scenarios MAC does not get configured
and the build frame also doesnt come for certain frames.
For logging purposes MAC needs to know that there has been a gap in the
accumulation of log packets. Hence now whenever log code 0x4309 gets flushed
L1 will call MAC function so that the MAC log packets also gets flushed

DEPENDENCIES:
None

RETURN VALUE:
None

SIDE EFFECTS:
The log packet will be flushed to diag. There may be less than 20 samples

==========================================================================*/

extern void tdsl1_edch_update_mac_log_packets_during_reconfig(void);

#endif /*#if defined( FEATURE_TDSCDMA_L1_LOGGING_ENABLED ) && defined( FEATURE_TDSCDMA_HSUPA_L1_LOGGING_ENABLED)*/

//#ifdef FEATURE_WCDMA_HSUPA_2MS_TTI
/*===========================================================================
FUNCTION      L1_GET_CUR_EUL_SUBFN
 
DESCRIPTION   This function returns the value of the OTA EUL sub fn number
 
DEPENDENCIES  None
 
RETURN VALUE  eul_fig current subfn value if eul is active and TTI is 2msec
              else returns invalid subfn
 
SIDE EFFECTS  None
===========================================================================*/
extern uint16 tdseulsg_l1_get_cur_eul_subfn(void);

//#endif /* FEATURE_WCDMA_HSUPA_2MS_TTI */

/*==========================================================================
FUNCTION: L1_EDCH_RESET_MAC_TSN

DESCRIPTION:
L1 Indicate to the mac through a function call at the activation time even
before processing the cphy_setup req (when mac-es/e reset indicator present)
so that mac will be ready with new TSNs . But this has disadvantage that
the cphy_setup is not processed at L1 and mac will be resetting the TSNs
and will have old cfg till L1 signals the mac to process the new cfg. And
this also involves design change.


The various scenarios and how the UE will handle it are listed below.
In all the following scenarios the MAC_E_RESET_INDICATOR is set to TRUE

++ No config change in MAC or L1
Reset TSN's and build frame can go on as usual


++ MAC config change, L1 has NO change
Reset the TSN's (option 1)
MAC will determine if config has changed
MAC will DTX until it received the signal from L1 to apply new config
L1 will continue build frame indication


++ MAC no config change, L1 has config change
Reset TSN's (option 1)
L1 will not send Build Frame indication


++ MAC config changes, L1 config also changes
 Reset TSN
L1 will not call Build Frame indication

DEPENDENCIES:
  None

RETURN VALUE:
  TRUE indicates that mac config has changed and FALSE indicates
  that no MAC config has really changed.
  based on this L1 can evaluate if it needs to suspend the BFI
  during this time frame.

SIDE EFFECTS:


==========================================================================*/
extern boolean tdsl1_edch_reset_mac_tsn(void);

#endif /* FEATURE_TDSCDMA_HSUPA */

/* +TDD HSUPA  changes */
#ifdef FEATURE_TDSCDMA_HSUPA



/*Max number of Harq process */
#define TDSMAX_HARQ_PROCESS 8


// L1 and MAC HARQ process status update

typedef struct
{
 uint8 num_harq_proc_status_to_report;
 uint8 failed_harq_id[TDSMAX_HARQ_PROCESS];
} tdsl1_eul_harq_status_struct_type;


// L1 and MAC ETFCI selection
typedef enum
{
   EUL_NEW_TRANSMISSION,
   EUL_RETRANSMISSION,
   EUL_NO_GRANT,
   EUL_SPACING_TTI,
   EUL_NUM_TX_STATUS
} tdsl1_eul_tx_status_enum_type;

// L1 indicate MAC E-RUCCH transmission status


typedef struct
{
  uint8 harq_id;
  tdsl1_eul_tx_status_enum_type  tx_status;
  uint8 timeslot_available;
} tdsl1_eul_sg_upd_info_struct_type;





typedef struct
{
  uint8 power_offset;
  uint8 max_num_retransmit;
  uint16 harq_retransmit_timer;
} tdsl1_eul_harq_profile_struct_type;




typedef enum
{
    EUL_ERUCCH_NO_ACK,   /* No FPACH ACK from network */
    EUL_ERUCCH_ABORT,    /* E-RUCCH is aborted */
    EUL_ERUCCH_MSG_DONE, /* E-RUCCH is sent over the air */
    EUL_NUM_ERUCCH_STATUS
} tdsl1_eul_erucch_status_enum_type;


typedef enum
{
  EUL_MAC_E_PDU_ON_EPUCH,
  EUL_SI_ALONE_ON_EPUCH,
  EUL_GRANT_SI_ON_ERUCCH,
  EUL_NON_GRANT_SI_ON_ERUCCH,
  EUL_ERUCCH_GRANT_SI_AND_RTX_EPUCH,
  EUL_NO_MAC_E_PDU 
} tdsl1_eul_etfci_enum_type;


typedef struct
{
    uint8 etfci;
    tdsl1_eul_etfci_enum_type  etfci_status;
    uint8 asc_index;
    tdsl1_eul_harq_profile_struct_type harq_profile;
    boolean SI_included;
    uint32 buf_status;  /*  EUL buffer size in byte  */
} tdsl1_eul_edch_ctrl_info_struct_type;



// L1 indicate MAC to build PDU

typedef struct
{
  uint8 *data_seg_list_ptr;
  uint32 length; 
} tdsl1_eul_data_seg_list_struct_type;

typedef struct
{
  tdsl1_eul_data_seg_list_struct_type *cur_ul_buf_ptr; 
  uint32 num_src_desc;
#ifdef FEATURE_TDSCDMA_DSDA
  /* Flag to indicate whether SRB data is present or not */
  boolean srb_data_present;
#endif

} tdseul_l1_l2_data_struct_type;

typedef struct
{
     uint8 harq_id;
     uint8 etfci;
     uint8 current_uph;
     uint8 snpl;
     uint8 *erucch_data_pointer;
     tdseul_l1_l2_data_struct_type *data_buf;
 
} tdseul_l1_build_frame_struct_type;





void     tdsmac_eul_harq_status_update
(
 tdsl1_eul_harq_status_struct_type * harq_status
);



tdsl1_eul_edch_ctrl_info_struct_type *
tdsmac_eul_etfci_selection
(
  tdsl1_eul_sg_upd_info_struct_type * sg_info
);




void 
tdsmac_eul_phy_erucch_status_ind
(
tdsl1_eul_erucch_status_enum_type erucch_status
);




void tdsmac_eul_phy_edch_build_frame_ind
(
	tdseul_l1_build_frame_struct_type * build_frame_info
);

// MAC send power offset to L1

typedef struct
{
    uint8 min_etfci_to_select;
    uint8 max_etfci_to_select;
} tdsl1_eul_supported_etfci_struct_type;

tdsl1_eul_supported_etfci_struct_type  tdsl1_eul_calc_supported_etfci_based_on_power_offset
(
	uint8 pwr_offset
);

uint16* 
tdsl1_eul_get_etfci_tab_ptr
(
  uint8 num_timeslot
);

#endif
/* -TDD HSUPA  changes */

#endif /* L1_MAC_DATA_H */

